#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hashmap.h"
#include "ast_data.h"
#include "ast_eval.h"

///////////////////////////////////////////////////////////////////////////////

#define MAX_DATA 16384

struct stack
{
    ast_data data[MAX_DATA];
    int count;
};

static struct stack stack;

void wind_data(ast_data *data, int vars)
{
    if (vars == 0)
    {
        return;
    }
    if (stack.count + vars > MAX_DATA)
    {
        fprintf(stderr, "Stack overflow\n");
        exit(EXIT_FAILURE);
    }
    memcpy(stack.data + stack.count, data, sizeof(*data) * (size_t)vars);
    stack.count += vars;
}

void unwind_data(ast_data *data, int vars)
{
    if (vars == 0)
    {
        return;
    }
    if (vars > stack.count)
    {
        fprintf(stderr, "Stack underflow\n");
        exit(EXIT_FAILURE);
    }
    stack.count -= vars;
    memcpy(data, stack.data + stack.count, sizeof(*data) * (size_t)vars);
}

int push_data(ast_data data)
{
    if (stack.count == MAX_DATA)
    {
        fprintf(stderr, "Stack overflow\n");
        exit(EXIT_FAILURE);
    }
    stack.data[stack.count++] = data;
    return 1;
}

ast_data pop_data(void)
{
    if (stack.count == 0)
    {
        fprintf(stderr, "Stack underflow\n");
        exit(EXIT_FAILURE);
    }
    return stack.data[--stack.count];    
}

ast_data *peek_data(void)
{
    if (stack.count == 0)
    {
        fprintf(stderr, "Empty stack\n");
        exit(EXIT_FAILURE);
    }
    return &stack.data[stack.count - 1];    
}

ast_data *sync_data(int count)
{
    if ((count == 0) || (count > stack.count))
    {
        fprintf(stderr, "Stack underflow\n");
        exit(EXIT_FAILURE);
    }
    stack.count -= count - 1;
    return &stack.data[stack.count - 1];    
}

///////////////////////////////////////////////////////////////////////////////

static hashmap *map;

static ast_data *new_data(ast_type type)
{
    ast_data *data;

    data = malloc(sizeof *data);
    if (data == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    data->type = type;
    data->flags = 0;
    return data;
}

static int comp_data(const void *pa, const void *pb)
{
    const ast_data *a = pa;
    const ast_data *b = pb;

    if (a->type != b->type)
    {
        return 1;
    }
    switch (a->type)
    {
        case TYPE_FUNCTION:
            return strcmp(a->function->name, b->function->name);
        case TYPE_CALLABLE:
            return strcmp(a->callable->name, b->callable->name);
        case TYPE_VARIABLE:
            return (a->variable->function != b->variable->function) ||
                   strcmp(a->variable->name, b->variable->name);
        case TYPE_NUMBER:
            return a->number != b->number;
        case TYPE_STRING:
            return strcmp(a->string, b->string);
        default:
            return 1;
    }
}

static unsigned long hash_data(const void *item)
{
    const ast_data *data = item;

    switch (data->type)
    {
        case TYPE_FUNCTION:
            return hash_str((const unsigned char *)data->function->name);
        case TYPE_CALLABLE:
            return hash_str((const unsigned char *)data->callable->name);
        case TYPE_VARIABLE:
            return hash_str((const unsigned char *)data->variable->name) ^
                   hash_str((const unsigned char *)data->variable->function->name);
        case TYPE_NUMBER:
            return hash_ulong((unsigned long)data->number);
        case TYPE_STRING:
            return hash_str((const unsigned char *)data->string);
        default:
            return 0;
    }
}

static void free_data(void *item)
{
    ast_data *data = item;

    switch (data->type)
    {
        case TYPE_FUNCTION:
            free(data->function->data);
            free(data);
            break;
        case TYPE_VARIABLE:
        case TYPE_NUMBER:
        case TYPE_STRING:
            free(data);
        default:
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////

int is_sequence(int c)
{
    return (c == '\\') || (c == '/') || (c == '"') ||
           (c == 'b')  || (c == 'f') || (c == 'n') || (c == 'r') || (c == 't');
}

int get_sequence(int c)
{
    switch (c)
    {
        case 'b':
            return '\b';
        case 'f':
            return '\f';
        case 'n':
            return '\n';
        case 'r':
            return '\r';
        case 't':
            return '\t';
        // case '"':
        // case '\\':
        // case '/':
        default:
            return c;
    }
}

int valid_name(const char *str)
{
    // First character must be '_'  or [a ... Z]
    if ((*str != '_') && !isalpha((unsigned char)*str))
    {
        return 0;
    }
    str++;
    // The rest can also have digits
    while (*str != '\0')
    {
        if ((*str != '_') && !isalnum((unsigned char)*str))
        {
            return 0;
        }
        str++;
    }
    return 1;
}

///////////////////////////////////////////////////////////////////////////////

#define DEF_OPERATOR(o, ...) [o] =                      \
{                                                       \
    .type = TYPE_OPERATOR,                              \
    .flags = 0,                                         \
    .operator = &(const ast_operator){o, __VA_ARGS__}   \
}

static ast_data operators[] =
{
    DEF_OPERATOR(OPERATOR_PLUS,          1, 13, 'R', "+",   ast_plus),
    DEF_OPERATOR(OPERATOR_MINUS,         1, 13, 'R', "-",   ast_minus),
    DEF_OPERATOR(OPERATOR_NOT,           1, 13, 'R', "!",   ast_not),
    DEF_OPERATOR(OPERATOR_MUL,           2, 12, 'L', "*",   ast_mul),
    DEF_OPERATOR(OPERATOR_DIV,           2, 12, 'L', "/",   ast_div),
    DEF_OPERATOR(OPERATOR_REM,           2, 12, 'L', "%",   ast_rem),
    DEF_OPERATOR(OPERATOR_ADD,           2, 11, 'L', "+",   ast_add),
    DEF_OPERATOR(OPERATOR_SUB,           2, 11, 'L', "-",   ast_sub),
    DEF_OPERATOR(OPERATOR_BIT_LSHIFT,    2, 10, 'L', "<<",  ast_bit_lshift),
    DEF_OPERATOR(OPERATOR_BIT_RSHIFT,    2, 10, 'L', ">>",  ast_bit_rshift),
    DEF_OPERATOR(OPERATOR_LT,            2,  9, 'L', "<",   ast_lt),
    DEF_OPERATOR(OPERATOR_GT,            2,  9, 'L', ">",   ast_gt),
    DEF_OPERATOR(OPERATOR_LT_OR_EQ,      2,  9, 'L', "<=",  ast_lt_or_eq),
    DEF_OPERATOR(OPERATOR_GT_OR_EQ,      2,  9, 'L', ">=",  ast_gt_or_eq),
    DEF_OPERATOR(OPERATOR_IS_EQ,         2,  8, 'L', "==",  ast_is_eq),
    DEF_OPERATOR(OPERATOR_NOT_EQ,        2,  8, 'L', "!=",  ast_not_eq),
    DEF_OPERATOR(OPERATOR_IDENTICAL,     2,  8, 'L', "===", ast_identical),
    DEF_OPERATOR(OPERATOR_NOT_IDENTICAL, 2,  8, 'L', "!==", ast_not_identical),
    DEF_OPERATOR(OPERATOR_BIT_AND,       2,  7, 'L', "&",   ast_bit_and),
    DEF_OPERATOR(OPERATOR_BIT_XOR,       2,  6, 'L', "^",   ast_bit_xor),
    DEF_OPERATOR(OPERATOR_BIT_OR,        2,  5, 'L', "|",   ast_bit_or),
    DEF_OPERATOR(OPERATOR_AND,           2,  4, 'L', "&&",  ast_and),
    DEF_OPERATOR(OPERATOR_XOR,           2,  3, 'L', "^^",  ast_xor),
    DEF_OPERATOR(OPERATOR_OR,            2,  2, 'L', "||",  ast_or),
    DEF_OPERATOR(OPERATOR_EQ,            2,  1, 'R', "=",   ast_eq),
    DEF_OPERATOR(OPERATOR_EQ_ADD,        2,  1, 'R', "+=",  ast_eq_add),
    DEF_OPERATOR(OPERATOR_EQ_SUB,        2,  1, 'R', "-=",  ast_eq_sub),
    DEF_OPERATOR(OPERATOR_EQ_MUL,        2,  1, 'R', "*=",  ast_eq_mul),
    DEF_OPERATOR(OPERATOR_EQ_DIV,        2,  1, 'R', "/=",  ast_eq_div),
    DEF_OPERATOR(OPERATOR_EQ_REM,        2,  1, 'R', "%=",  ast_eq_rem),
    DEF_OPERATOR(OPERATOR_EQ_BIT_AND,    2,  1, 'R', "&=",  ast_eq_bit_and),
    DEF_OPERATOR(OPERATOR_EQ_BIT_XOR,    2,  1, 'R', "^=",  ast_eq_bit_xor),
    DEF_OPERATOR(OPERATOR_EQ_BIT_OR,     2,  1, 'R', "|=",  ast_eq_bit_or),
    DEF_OPERATOR(OPERATOR_EQ_BIT_LSHIFT, 2,  1, 'R', "<<=", ast_eq_bit_lshift),
    DEF_OPERATOR(OPERATOR_EQ_BIT_RSHIFT, 2,  1, 'R', ">>=", ast_eq_bit_rshift),

    DEF_OPERATOR(OPERATOR_LPARENTHS,     0,  0, 'L', "(",   NULL),
    DEF_OPERATOR(OPERATOR_RPARENTHS,     0,  0, 'L', ")" ,  NULL),
    DEF_OPERATOR(OPERATOR_COMMA,         0,  0, 'L', ",",   NULL),
    DEF_OPERATOR(OPERATOR_SEMICOLON,     0,  0, 'L', ";",   NULL),
    DEF_OPERATOR(OPERATOR_EOF,           0,  0, 'L', "",    NULL),
};

int is_assignment(int operator)
{
    switch (operator)
    {
        case OPERATOR_EQ:
        case OPERATOR_EQ_ADD:
        case OPERATOR_EQ_SUB:
        case OPERATOR_EQ_MUL:
        case OPERATOR_EQ_DIV:
        case OPERATOR_EQ_REM:
        case OPERATOR_EQ_BIT_AND:
        case OPERATOR_EQ_BIT_XOR:
        case OPERATOR_EQ_BIT_OR:
        case OPERATOR_EQ_BIT_LSHIFT:
        case OPERATOR_EQ_BIT_RSHIFT:
            return 1;
        default:
            return 0;
    }
}

int is_operator(int operator)
{
    switch (operator)
    {
        case OPERATOR_MUL:
        case OPERATOR_DIV:
        case OPERATOR_REM:
        case OPERATOR_ADD:
        case OPERATOR_SUB:
        case OPERATOR_LT:
        case OPERATOR_GT:
        case OPERATOR_BIT_AND:
        case OPERATOR_BIT_XOR:
        case OPERATOR_BIT_OR:
        case OPERATOR_EQ:
        case OPERATOR_NOT:
        case OPERATOR_LPARENTHS:
        case OPERATOR_RPARENTHS:
        case OPERATOR_COMMA:
        case OPERATOR_SEMICOLON:
            return 1;
        default:
            return 0;
    }
}

static int get_operator(const char **operator)
{
    int result = 0;

    switch (**operator)
    {
        case OPERATOR_LPARENTHS:
        case OPERATOR_RPARENTHS:
        case OPERATOR_COMMA:
        case OPERATOR_SEMICOLON:
            result = **operator;
            (*operator)++;
            break;
        case OPERATOR_MUL:
        case OPERATOR_DIV:
        case OPERATOR_REM:
        case OPERATOR_ADD:
        case OPERATOR_SUB:
            result = **operator;
            (*operator)++;
            if (**operator == '=')
            {
                result ^= 'z';
                (*operator)++;
            }
            break;
        case OPERATOR_EQ:
        case OPERATOR_NOT:
            result = **operator;
            (*operator)++;
            if (**operator == '=')
            {
                result ^= 'z';
                (*operator)++;
            }
            if (**operator == '=')
            {
                result ^= '=';
                (*operator)++;
            }
            break;
        case OPERATOR_BIT_AND:
        case OPERATOR_BIT_XOR:
        case OPERATOR_BIT_OR:
            result = **operator;
            (*operator)++;
            if (**operator == '=')
            {
                result ^= 'z';
                (*operator)++;
            }
            else if (**operator == result)
            {
                result ^= '!';
                (*operator)++;
            }
            break;
        case OPERATOR_LT:
        case OPERATOR_GT:
            result = **operator;
            (*operator)++;
            if (**operator == '=')
            {
                result ^= 'z';
                (*operator)++;
            }
            else if (**operator == result)
            {
                result ^= '!';
                (*operator)++;
                if (**operator == '=')
                {
                    result ^= '=';
                    (*operator)++;
                }
            }
            break;
        default:
            break;
    }
    return result;
}

ast_data *map_operator(const char **operator)
{
    return &operators[get_operator(operator)];
}

int arguments(const ast_data *data)
{
    return data->operator->args;
}

int precedence(const ast_data *pa, const ast_data *pb)
{
    int a = pa->operator->precedence;
    int b = pb->operator->precedence;

    if (a == 0)
    {
        return 0;
    }
    if (a == b)
    {
        return pa->operator->associativity == 'L';
    }
    return a > b;
}

ast_data *unary(ast_data *data)
{
    switch (data->operator->key)
    {
        case OPERATOR_ADD:
            return &operators[OPERATOR_PLUS];
        case OPERATOR_SUB:
            return &operators[OPERATOR_MINUS];
    }
    return data;
}

///////////////////////////////////////////////////////////////////////////////

int is_iterator(const ast_data *data)
{
    switch (data->statement->key)
    {
        case STATEMENT_WHILE:
        case STATEMENT_FOR:
            return 1;
        default:
            return 0;
    }
}

#define DEF_STATEMENT(...)                              \
{                                                       \
    .type = TYPE_STATEMENT,                             \
    .flags = 0,                                         \
    .statement = &(const ast_statement){__VA_ARGS__}    \
}

static ast_data statements[] =
{
    DEF_STATEMENT(STATEMENT_NONE,     0, ""),
    DEF_STATEMENT(STATEMENT_DEF,      0, "def"),
    DEF_STATEMENT(STATEMENT_END,      0, "end"),
    DEF_STATEMENT(STATEMENT_IF,       1, "if"),
    DEF_STATEMENT(STATEMENT_ELIF,     1, "elif"),
    DEF_STATEMENT(STATEMENT_ELSE,     0, "else"),
    DEF_STATEMENT(STATEMENT_WHILE,    1, "while"),
    DEF_STATEMENT(STATEMENT_FOR,      3, "for"),
    DEF_STATEMENT(STATEMENT_CONTINUE, 0, "continue"),
    DEF_STATEMENT(STATEMENT_BREAK,    0, "break"),
    DEF_STATEMENT(STATEMENT_RETURN,   0, "return"),
};

static ast_data branches[] =
{
    DEF_STATEMENT(STATEMENT_IFEL,     0, "if"),
    DEF_STATEMENT(STATEMENT_THEN,     0, "then"),
};

static ast_function *data_def;

ast_data *map_statement(const char *name)
{
    const size_t count = sizeof statements / sizeof *statements;

    for (size_t iter = 1; iter < count; iter++)
    {
        if (strcmp(statements[iter].statement->name, name) == 0)
        {
            return &statements[iter];
        }
    }
    return NULL;
}

ast_data *map_branch(int branch)
{
    return &branches[branch];
}

///////////////////////////////////////////////////////////////////////////////

static ast_data *new_function(void)
{
    struct reference
    {
        ast_data data;
        ast_function function;
    };

    struct reference *reference = calloc(1, sizeof *reference);

    if (reference == NULL)
    {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    reference->data.type = TYPE_FUNCTION;
    reference->data.function = &reference->function;
    return &reference->data;
}

static ast_data *data_fnc;

static ast_data *map_function(const char *name)
{
    ast_data *data;

    data_fnc->function->name = name;
    data = hashmap_insert(map, data_fnc);
    if (data == NULL)
    {
        perror("hashmap_insert");
        exit(EXIT_FAILURE);
    }
    if (data == data_fnc)
    {
        data_fnc = new_function();
    }
    if (data_def == NULL)
    {
        data_def = data->function;
    }
    return data;
}

static void map_functions(void)
{
    data_fnc = new_function();
}

static void unmap_functions(void)
{
    free_data(data_fnc);
}

///////////////////////////////////////////////////////////////////////////////

ast_data *map_callable(const char *name)
{
    const ast_callable callable = {.name = name};
    ast_data temp = {.type = TYPE_CALLABLE, .callable = &callable};
    ast_data *data;

    data = hashmap_search(map, &temp);
    if (data == NULL)
    {
        return map_function(name);
    }
    return data;
}

#define DEF_CALLABLE(...)                           \
{                                                   \
    .type = TYPE_CALLABLE,                          \
    .flags = 0,                                     \
    .callable = &(const ast_callable){__VA_ARGS__}  \
}

static ast_data callable_list[] =
{
    DEF_CALLABLE("abs",    {1,  1}, ast_abs),
    DEF_CALLABLE("ceil",   {1,  1}, ast_ceil),
    DEF_CALLABLE("cos",    {1,  1}, ast_cos),
    DEF_CALLABLE("cosh",   {1,  1}, ast_cosh),
    DEF_CALLABLE("exp",    {1,  1}, ast_exp),
    DEF_CALLABLE("floor",  {1,  1}, ast_floor),
    DEF_CALLABLE("log",    {1,  1}, ast_log),
    DEF_CALLABLE("log10",  {1,  1}, ast_log10),
    DEF_CALLABLE("pow",    {2,  2}, ast_pow),
    DEF_CALLABLE("rand",   {0,  0}, ast_rand),
    DEF_CALLABLE("round",  {1,  1}, ast_round),
    DEF_CALLABLE("sin",    {1,  1}, ast_sin),
    DEF_CALLABLE("sinh",   {1,  1}, ast_sinh),
    DEF_CALLABLE("sqrt",   {1,  1}, ast_sqrt),
    DEF_CALLABLE("tan",    {1,  1}, ast_tan),
    DEF_CALLABLE("tanh",   {1,  1}, ast_tanh),
    DEF_CALLABLE("trunc",  {1,  1}, ast_trunc),

    DEF_CALLABLE("typeof", {1,  1}, ast_typeof),    
    DEF_CALLABLE("cond",   {3,  3}, ast_cond),
    DEF_CALLABLE("print",  {1, 64}, ast_print),
};

static void map_callables(void)
{
    const size_t count = sizeof callable_list / sizeof *callable_list;

    for (size_t iter = 0; iter < count; iter++)
    {
        if (hashmap_insert(map, &callable_list[iter]) == NULL)
        {
            perror("hashmap_insert");
            exit(EXIT_FAILURE);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

static ast_data *new_variable(void)
{
    struct reference
    {
        ast_data data;
        ast_variable variable;
    };

    struct reference *reference = malloc(sizeof *reference);

    if (reference == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    reference->data.type = TYPE_VARIABLE;
    reference->data.flags = 0;
    reference->data.variable = &reference->variable;
    return &reference->data;
}

static ast_data *data_var;

ast_data *map_variable(const char *name)
{
    if (data_def == NULL)
    {
        return NULL;
    }
    data_var->variable->name = name;
    data_var->variable->function = data_def;

    ast_data *data;

    data = hashmap_insert(map, data_var);
    if (data == NULL)
    {
        perror("hashmap_insert");
        exit(EXIT_FAILURE);
    }
    if (data == data_var)
    {
        data->variable->offset = data_def->vars++;
        data_var = new_variable();
    }
    return data;
}

static void map_variables(void)
{
    data_var = new_variable();
}

static void unmap_variables(void)
{
    free(data_var);
}

void map_vars()
{
    if (data_def->vars == 0)
    {
        return;
    }
    data_def->data = calloc((size_t)data_def->vars, sizeof *data_def->data);
    if (data_def->data == NULL)
    {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    data_def = NULL;
}

///////////////////////////////////////////////////////////////////////////////

ast_data *map_boolean(const char *str)
{
    static ast_data booleans[] =
    {
        {.type = TYPE_BOOLEAN, .flags = 0, .number = 0},
        {.type = TYPE_BOOLEAN, .flags = 0, .number = 1},
    };

    if (strcmp(str, "true") == 0)
    {
        return &booleans[1];
    }
    else if (strcmp(str, "false") == 0)
    {
        return &booleans[0];
    }
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////

static ast_data *data_num;

ast_data *map_number(const char *str)
{
    ast_data *data = NULL;
    double number;
    char *ptr;

    number = strtod(str, &ptr);
    if (*ptr == '\0')
    {
        data_num->number = number;
        data = hashmap_insert(map, data_num);
        if (data == NULL)
        {
            perror("hashmap_insert");
            exit(EXIT_FAILURE);
        }
        if (data == data_num)
        {
            data_num = new_data(TYPE_NUMBER);
        }
    }
    return data;
}

static void map_numbers(void)
{
    data_num = new_data(TYPE_NUMBER);
}

static void unmap_numbers(void)
{
    free(data_num);
}

///////////////////////////////////////////////////////////////////////////////

static ast_data *data_str;

ast_data *map_string(const char *str)
{
    ast_data *data;

    data_str->string = str;
    data = hashmap_insert(map, data_str);
    if (data == NULL)
    {
        perror("hashmap_insert");
        exit(EXIT_FAILURE);
    }
    if (data == data_str)
    {
        data_str = new_data(TYPE_STRING);
    }
    return data;
}

static void map_strings(void)
{
    data_str = new_data(TYPE_STRING);
}

static void unmap_strings(void)
{
    free(data_str);
}

///////////////////////////////////////////////////////////////////////////////

ast_data *map_null(const char *str)
{
    static ast_data null = {.type = TYPE_NULL, .flags = 0, .number = 0};

    if (strcmp(str, "null") == 0)
    {
        return &null;
    }
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void map_data(void)
{
    map = hashmap_create(comp_data, hash_data, 10000);
    if (map == NULL)
    {
        perror("hashmap_create");
        exit(EXIT_FAILURE);
    }
    map_functions();
    map_callables();
    map_variables();
    map_numbers();
    map_strings();
}

void unmap_data(void)
{
    hashmap_destroy(map, free_data);
    unmap_functions();
    unmap_variables();
    unmap_numbers();
    unmap_strings();
}

