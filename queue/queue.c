/*! 
 *  \brief     Queue
 *  \author    David Ranieri <davranfor@gmail.com>
 *  \copyright GNU Public License.
 */

#include <stdlib.h>
#include "queue.h"

struct node
{
    void *data;
    struct node *next;
};

struct queue
{
    struct node *head;
    struct node *tail;
    size_t size;
};

queue *queue_create(void)
{
    return calloc(1, sizeof(struct queue));
}

void *queue_push(queue *list, void *data)
{
    if (data == NULL)
    {
        return NULL;
    }

    struct node *node = malloc(sizeof *node);

    if (node == NULL)
    {
        return NULL;
    }
    node->data = data;
    node->next = NULL;
    if (list->head == NULL)
    {
        list->head = node;
    }
    else
    {
        list->tail->next = node;
    }
    list->tail = node;
    list->size++;
    return data;
}

void *queue_pop(queue *list)
{
    void *data = NULL;

    if (list->head != NULL)
    {
        struct node *node = list->head;

        data = node->data;
        list->head = node->next;
        if ((list->head == NULL) || (list->head->next == NULL))
        {
            list->tail = list->head;
        }
        list->size--;
        free(node);
    }
    return data;
}

void *queue_fetch(queue *list, void **iter)
{
    void *data = NULL;

    if (*iter == list)
    {
        if (list->head != NULL)
        {
            data = list->head->data;
        }
        *iter = list->head;
    }
    else
    {
        struct node *node = *iter;

        if (node == NULL)
        {
            return NULL;
        }
        if (node->next != NULL)
        {
            data = node->next->data;
        }
        *iter = node->next;
    }
    return data;
}

void *queue_head(queue *list)
{
    return list->head;
}

void *queue_tail(queue *list)
{
    return list->tail;
}

size_t queue_size(const queue *list)
{
    return list->size;
}

void queue_destroy(queue *list, void (*func)(void *))
{
    struct node *node;

    while ((node = queue_pop(list)))
    {
        if (func != NULL)
        {
            func(node);
        }
    }
    free(list);
}
