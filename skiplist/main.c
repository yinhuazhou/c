#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "skiplist.h"

char *words[] = {
	"A","A's","AA's","AB's","ABM's","AC's","ACTH's","AI's","AIDS's","AM's","AOL","AOL's","ASCII's","ASL's","ATM's","ATP's","AWOL's","AZ's","AZT's","Aachen","Aaliyah","Aaliyah's","Aaron","Abbas","Abbasid","Abbott","Abbott's","Abby","Abby's","Abdul","Abdul's","Abe","Abe's","Abel","Abel's","Abelard","Abelson","Abelson's","Aberdeen","Aberdeen's","Abernathy","Abernathy's","Abidjan","Abidjan's","Abigail","Abilene","Abner","Abner's","Abraham","Abraham's","Abram","Abram's","Abrams","Absalom","Abuja","Abyssinia","Abyssinia's","Abyssinian","Abyssinian's","Ac","Ac's","Acadia","Acadia's","Acapulco","Acapulco's","Accenture","Accenture's","Accra","Accra's","Acevedo","Acevedo's","Achaean","Achaean's","Achebe","Achebe's","Achernar","Acheson","Acheson's","Achilles","Achilles's","Aconcagua","Aconcagua's","Acosta","Acosta's","Acropolis","Acrux","Acrux's","Actaeon","Acton","Acts","Acuff","Acuff's","Ada","Ada's","Adam","Adam's","Adams","Adan","Adan's",
	"Adana","Adar","Adar's","Addams","Adderley","Adderley's","Addie","Addie's","Addison","Adela","Adela's","Adelaide","Adelaide's","Adele","Adele's","Adeline","Adeline's","Aden","Aden's","Adenauer","Adhara","Adhara's","Adidas","Adidas's","Adirondack","Adirondack's","Adirondacks","Adirondacks's","Adkins","Adkins's","Adler","Adolf","Adolf's","Adolfo","Adolfo's","Adolph","Adolph's","Adonis","Adonis's","Adonises","Adrian","Adriana","Adriana's","Adriatic","Adriatic's","Adrienne","Adrienne's","Advent","Advent's","Adventist","Adventist's","Advents","Advil","Advil's","Aegean","Aegean's","Aelfric","Aelfric's","Aeneas","Aeneas's","Aeneid","Aeneid's","Aeolus","Aeolus's","Aeroflot","Aeroflot's","Aeschylus","Aeschylus's","Aesculapius","Aesculapius's","Aesop","Aesop's","Afghan","Afghan's","Afghanistan","Afghanistan's","Afghans","Africa","Africa's","African","African's","Africans","Afrikaans","Afrikaans's","Afrikaner","Afrikaner's","Afrikaners","Afro","Afro's","Afrocentrism","Afros","Ag","Ag's","Agamemnon","Agamemnon's","Agassi","Agassi's","Agassiz","Agassiz's","Agatha",
	"Agatha's","Aggie","Aglaia","Agnes","Agnew","Agnew's","Agni","Agni's","Agra","Agricola","Agrippa","Agrippa's","Agrippina","Agrippina's","Aguilar","Aguilar's","Aguinaldo","Aguinaldo's","Aguirre","Aguirre's","Agustin","Agustin's","Ahab","Ahab's","Ahmad","Ahmad's","Ahmadabad","Ahmadinejad","Ahmadinejad's","Ahmed","Ahmed's","Ahriman","Ahriman's","Aida","Aida's","Aiken","Aileen","Aileen's","Aimee","Aimee's","Ainu","Airedale","Airedale's","Airedales","Aisha","Ajax","Ajax's","Akbar","Akhmatova","Akhmatova's","Akihito","Akita","Akiva","Akiva's","Akkad","Akron","Akron's","Al","Al's","Ala","Alabama","Alabama's","Alabaman","Alabaman's","Alabamans","Alabamian","Alabamian's","Alabamians","Aladdin","Aladdin's","Alamo","Alamo's","Alamogordo","Alamogordo's","Alan","Alan's","Alana","Alana's","Alar","Alar's","Alaric","Alaska","Alaska's","Alaskan","Alaskan's","Alaskans","Alba","Albania","Albania's","Albanian","Albanian's","Albanians","Albany","Albany's","Albee","Alberio","Alberio's","Albert","Alberta","Alberta's",
	"Alberto","Alberto's","Albigensian","Albion","Albireo","Albireo's","Albuquerque","Albuquerque's","Alcatraz","Alcestis","Alcibiades","Alcindor","Alcindor's","Alcmena","Alcmena's","Alcoa","Alcoa's","Alcott","Alcuin","Alcyone","Aldan","Aldebaran","Aldebaran's","Alden","Alderamin","Alderamin's","Aldo","Aldo's","Aldrin","Aldrin's","Alec","Alec's","Aleichem","Aleichem's","Alejandra","Alejandra's","Alejandro","Alejandro's","Alembert","Alembert's","Aleppo","Aleppo's","Aleut","Aleut's","Aleutian","Aleutian's","Alex","Alex's","Alexander","Alexander's","Alexandra","Alexandria","Alexandria's","Alexei","Alexei's","Alexis","Alfonso","Alfonso's","Alfonzo","Alfonzo's","Alford","Alford's","Alfred","Alfreda","Alfreda's","Alfredo","Algenib","Algenib's","Alger","Algeria","Algeria's","Algerian","Algerian's","Algerians","Algieba","Algieba's","Algiers","Algiers's","Algol","Algol's","Algonquian","Algonquian's","Algonquians","Algonquin","Algonquin's","Alhambra","Alhena","Alhena's","Ali","Alice","Alice's","Alicia","Alicia's","Alighieri","Alighieri's","Aline","Aline's","Alioth","Alioth's","Alisa",
	"Alisa's","Alisha","Alisha's","Alison","Alison's","Alissa","Alissa's","Alistair","Alistair's","Alkaid","Alkaid's","Allah","Allah's","Allahabad","Allan","Allan's","Alleghenies","Alleghenies's","Allegheny","Allegra","Allegra's","Allen","Allende","Allende's","Allentown","Allie","Allie's","Allison","Allison's","Allstate","Allstate's","Allyson","Allyson's","Alma","Alma's","Almach","Almach's","Almaty","Almaty's","Almighty","Almighty's","Almohad","Almohad's","Almoravid","Almoravid's","Alnilam","Alnilam's","Alnitak","Alnitak's","Alonzo","Alonzo's","Alpert","Alpert's","Alphard","Alphard's","Alphecca","Alphecca's","Alpheratz","Alpheratz's","Alphonse","Alphonse's","Alphonso","Alphonso's","Alpine","Alpo","Alpo's","Alps","Alps's","Alsace","Alsace's","Alsatian","Alsatian's","Alsop","Alsop's","Alston","Alston's","Alta","Altai","Altai's","Altaic","Altaic's","Altair","Altamira","Althea","Althea's","Altiplano","Altiplano's","Altman","Altman's","Altoids","Altoids's","Alton","Alton's","Aludra","Aludra's","Alva","Alvarado","Alvarado's","Alvarez","Alvarez's",
	"Alvaro","Alvaro's","Alvin","Alvin's","Alyce","Alyce's","Alyson","Alyson's","Alyssa","Alyssa's","Alzheimer","Alzheimer's","Am","Am's","Amadeus","Amadeus's","Amado","Amado's","Amalia","Amalia's","Amanda","Amanda's","Amarillo","Amaru","Amaru's","Amaterasu","Amaterasu's","Amati","Amazon","Amazon's","Amazons","Amber","Amber's","Amelia","Amelia's","Amenhotep","Amenhotep's","Amerasian","America","America's","American","American's","Americana","Americana's","Americanism","Americanism's","Americanisms","Americanization","Americanization's","Americanizations","Americanize","Americanized","Americanizes","Americanizing","Americans","Americas","Amerind","Amerind's","Amerindian","Amerindian's","Amerindians","Amerinds","Ameslan","Ameslan's","Amharic","Amharic's","Amherst","Amie","Amie's","Amiga","Amiga's","Amish","Amish's","Amman","Amman's","Amoco","Amoco's","Amos","Amparo","Amparo's","Ampere","Ampere's","Amritsar","Amsterdam","Amsterdam's","Amtrak","Amundsen","Amur","Amur's","Amway","Amway's","Amy","Amy's","Ana","Ana's","Anabaptist","Anabaptist's","Anabel","Anabel's","Anacin",
	"Anacin's","Anacreon","Anaheim","Analects","Analects's","Ananias","Ananias's","Anasazi","Anastasia","Anatole","Anatole's","Anatolia","Anatolia's","Anatolian","Anatolian's","Anaxagoras","Anchorage","Anchorage's","Andalusia","Andalusia's","Andalusian","Andaman","Andean","Andersen","Andersen's","Anderson","Andes","Andes's","Andorra","Andorra's","Andre","Andre's","Andrea","Andrea's","Andrei","Andrei's","Andres","Andretti","Andretti's","Andrew","Andrew's","Andrews","Andrianampoinimerina","Andrianampoinimerina's","Android","Android's","Andromache","Andromeda","Andromeda's","Andropov","Andy","Andy's","Angara","Angel","Angel's","Angela","Angela's","Angelia","Angelia's","Angelica","Angelico","Angelina","Angelina's","Angeline","Angeline's","Angelique","Angelique's","Angelita","Angelita's","Angelo","Angelo's","Angelou","Angevin","Angevin's","Angie","Angie's","Angkor","Anglia","Anglican","Anglican's","Anglicanism","Anglicanism's","Anglicanisms","Anglicans","Anglicize","Anglo","Anglophile","Angola","Angola's","Angolan","Angolan's","Angolans","Angora","Angora's","Angoras","Anguilla","Anguilla's","Angus","Angus's","Aniakchak",
	"Aniakchak's","Anibal","Anibal's","Anita","Anita's","Ankara","Ankara's","Ann","Ann's","Anna","Annabel","Annabel's","Annabelle","Annabelle's","Annam","Annam's","Annapolis","Annapolis's","Annapurna","Annapurna's","Anne","Annette","Annette's","Annie","Annie's","Annmarie","Annmarie's","Anouilh","Anselm","Anselmo","Anselmo's","Anshan","Anshan's","Antaeus","Antananarivo","Antananarivo's","Antarctic","Antarctic's","Antarctica","Antarctica's","Antares","Antares's","Anthony","Antichrist","Antichrist's","Antichrists","Antietam","Antigone","Antigone's","Antigua","Antigua's","Antilles","Antilles's","Antioch","Antipas","Antipas's","Antofagasta","Antofagasta's","Antoine","Antoine's","Antoinette","Anton","Anton's","Antone","Antone's","Antonia","Antonia's","Antoninus","Antonio","Antonio's","Antonius","Antony","Antony's","Antwan","Antwan's","Antwerp","Antwerp's","Anubis","Anubis's","Anzac","Apache","Apache's","Apaches","Apalachicola","Apalachicola's","Apennines","Aphrodite","Aphrodite's","Apia","Apocrypha","Apocrypha's","Apollinaire","Apollo","Apollo's","Apollonian","Apollos","Appalachia","Appalachia's","Appalachian","Appalachians",
	"Appaloosa","Appaloosa's","Apple","Apple's","Appleseed","Appleton","Appomattox","Apr","Apr's","April","April's","Aprils","Apuleius","Aquafresh","Aquafresh's","Aquarius","Aquarius's","Aquariuses","Aquila","Aquinas","Aquinas's","Aquino","Aquino's","Aquitaine","Aquitaine's","Ar","Ar's","Ara","Ara's","Arab","Arab's","Arabia","Arabia's","Arabian","Arabian's","Arabians","Arabic","Arabic's","Arabs","Araby","Araceli","Araceli's","Arafat","Arafat's","Araguaya","Araguaya's","Aral","Aral's","Aramaic","Aramaic's","Aramco","Aramco's","Arapaho","Arapaho's","Ararat","Ararat's","Araucanian","Arawak","Arawak's","Arawakan","Arawakan's","Arbitron","Arbitron's","Arcadia","Arcadia's","Arcadian","Arcadian's","Archean","Archibald","Archibald's","Archie","Archie's","Archimedes","Arctic","Arctic's","Arcturus","Arcturus's","Arden","Arequipa","Ares","Argentina","Argentina's","Argentine","Argentinian","Argentinian's","Argentinians","Argo","Argonaut","Argonaut's","Argonne","Argonne's","Argos","Argus","Argus's","Ariadne","Ariadne's","Arianism","Arianism's","Ariel","Aries",
	"Aries's","Arieses","Ariosto","Aristarchus","Aristarchus's","Aristides","Aristophanes","Aristophanes's","Aristotelian","Aristotelian's","Aristotle","Aristotle's","Arius","Arius's","Ariz","Arizona","Arizona's","Arizonan","Arizonan's","Arizonans","Arizonian","Arizonian's","Arizonians","Arjuna","Ark","Arkansan","Arkansan's","Arkansas","Arkansas's","Arkhangelsk","Arkwright","Arlene","Arlene's","Arline","Arline's","Arlington","Armageddon","Armageddon's","Armageddons","Armagnac","Armagnac's","Armand","Armand's","Armando","Armando's","Armani","Armani's","Armenia","Armenia's","Armenian","Armenian's","Armenians","Arminius","Arminius's","Armonk","Armonk's","Armour","Armour's","Armstrong","Armstrong's","Arneb","Arneb's","Arnhem","Arno","Arno's","Arnold","Arnulfo","Arnulfo's","Aron","Aron's","Arrhenius","Arron","Arron's","Art","Art's","Artaxerxes","Artaxerxes's","Artemis","Artemis's","Arthur","Arthur's","Arthurian","Artie","Artie's","Arturo","Arturo's","Aruba","Aruba's","Aryan","Aryan's","Aryans","As","Asama","Asama's","Ascella","Ascella's","Asgard","Ashanti","Ashcroft","Ashe",
	"Ashikaga","Ashikaga's",
};

struct data {
    int key;
    char *value;
};

static int comp_key(const void *a, const void *b)
{
    const struct data *pa = a;
    const struct data *pb = b;

    return (pa->key < pb->key) ? -1 : (pa->key != pb->key);
}

static int comp_range(const void *a, const void *b)
{
    const struct data *data = a;
    const int *range = b;

    return (range[0] > data->key) ? -1 : !((data->key >= range[0]) && (data->key <= range[1]));
}

static void walk(skiplist *list)
{
    struct cursor cursor = {NULL};
    struct data *data;
    int count = 0;
 
    printf("First 5 records:\n");
    while ((data = skiplist_fetch(list, &cursor))) {
        printf("%d %s fetched\n", data->key, data->value);
        if (++count > 4) {
            break;
        }
    }
    printf("\n");
}

static void filter(skiplist *list, int min, int max)
{
    struct cursor cursor = {.comp = comp_range, .data = (int []){min, max}};
    struct data *data;

    printf("Filtering records from %d to %d\n", min, max);
    while ((data = skiplist_fetch(list, &cursor))) {
        printf("%d %s fetched\n", data->key, data->value);
    }
    printf("\n");
}

int main(void)
{
    int iter, size = sizeof words / sizeof *words;
    skiplist *list;
    struct data *data;
    struct data temp;

    list = skiplist_create(comp_key);
    if (list == NULL) {
        perror("skiplist_create");
        exit(EXIT_FAILURE);
    }
    for (iter = 0; iter < size; iter++) {
        data = malloc(sizeof *data);
        if (data == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        data->key = iter;
        data->value = words[iter];
        data = skiplist_insert(list, data);
        if (data == NULL) {
            perror("skiplist_insert");
            exit(EXIT_FAILURE);
        }
    }
    walk(list);
    printf("Deleting first 5 records\n");
    for (iter = 0; iter < 5; iter++) {
        temp.key = iter;
        data = skiplist_delete(list, &temp);
        if (data != NULL) {
            printf("%d %s deleted\n", data->key, data->value);
            free(data);
        }
    }
    printf("\nSearching record %d\n", size / 2);
    temp.key = size / 2;
    data = skiplist_search(list, &temp);
    if (data != NULL) {
        printf("%d %s founded\n", data->key, data->value);
    }
    printf("\n");
    filter(list, size - 6, size - 1);
    skiplist_destroy(list, free);
    return 0;
}
