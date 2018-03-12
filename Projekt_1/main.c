/*
Jako argumenty przekaż liczbę elementów tablicy, rozmiar bloku, sposób alokacji pamięci oraz spis wykonywanych operacji. 
Zakładamy, że możliwe jest zlecenie trzech operacji 
( 
    stworzenie tablicy, 
    wyszukanie elementu,
    usunięcie i dodanie zadanej liczby bloków 

albo 
    stworzenie tablicy,
    usunięcie i dodanie zadanej liczby bloków, 
    naprzemienne usunięcie i dodanie zadanej liczby bloków
).

Program powinien stworzyć tablice bloków o zadanej liczbie elementów i rozmiarze bloków. Dane można wygenerować na stronach typu generatedata.com 
albo użyć danych losowych.
W programie zmierz, wypisz na konsolę i zapisz  do pliku z raportem  czasy realizacji podstawowych operacji:
- stworzenie tablicy z zadaną liczbą bloków o zdanym rozmiarze i przy pomocy wybranej funkcji alokującej,
- wyszukanie najbardziej podobnego elementu z punktu widzenia sumy znaków do elementu zadanego jako argument
- usunięcie kolejno zadanej liczby bloków a następnie dodanie na ich miejsce nowych bloków
- na przemian usunięcie i dodanie zadanej liczby bloków 
Mierząc czasy poszczególnych operacji zapisz trzy wartości: czas rzeczywisty, czas użytkownika i czas systemowy. 
Rezultaty umieść pliku raport2.txt i dołącz do archiwum zadania.
*/

#include "lib1.h"

#include <time.h>
#include <sys/times.h>

char *create_value(int max_size);
void add_remove_blocks(int count, Table *tab);
void add_remove_blocks_cross(int count, Table *tab);
void print_times(struct tms *ts, struct tms *te);

int main(int argc, char **argv)
{
    struct tms *ts = calloc(1, sizeof(struct tms));
    struct tms *te = calloc(1, sizeof(struct tms));

    if (argc < 4)
    {
        printf("program require at least 4 arguments\narr_size, block_size, static|dynamic , [operations]");
        exit(EXIT_FAILURE);
    }

    FILE *fp;

    int arr_size, block_size;
    bool is_static;

    arr_size = atoi(argv[0]);
    block_size = atoi(argv[1]);
    if (strcmp("static", argv[2]) == 0)
        is_static = true;
    else if (strcmp("dynamic", argv[2]) == 0)
        is_static = false;
    else
    {
        printf("static or dynamic ONLY");
        exit(EXIT_FAILURE);
    }

    Table *table;

    if (strcmp(argv[3], "c") == 0)
    {
        times(ts);
        table = create_table(arr_size, block_size, is_static);
        times(te);
        void print_times(struct tms * ts, struct tms * te);
    }
    else
    {
        printf("You must create a table ");
        exit(EXIT_FAILURE);
    }

    for (int i = 3; i < argc; i++)
    {

        if (strcmp(argv[i], "f") == 0)
        {
            find_block(rand() % table->current_last, table);
        }
        else if (strcmp(argv[i], "ar") == 0)
        {
            int count;

            if (i + 1 < argc)
                count = atoi(argv[i + 1]);
            else
            {
                printf("unvalid operations");
                exit(EXIT_FAILURE);
            }
            if (count > table->height)
                exit(EXIT_FAILURE);
            if (table->height - table->current_last < count)
                exit(EXIT_FAILURE);

            times(ts);
            add_remove_blocks(count, table);
            times(te);
            void print_times(struct tms * ts, struct tms * te);
        }
        else if (strcmp(argv[i], "arc") == 0)
        {
            int count;

            if (i + 1 < argc)
                count = atoi(argv[i + 1]);
            else
            {
                printf("unvalid operations");
                exit(EXIT_FAILURE);
            }
            if (count > table->height)
                exit(EXIT_FAILURE);
            if (table->height - table->current_last < count)
                exit(EXIT_FAILURE);

            times(ts);
            add_remove_blocks_cross(count, table);
            times(te);
            void print_times(struct tms * ts, struct tms * te);
        }
    }

    return 0;
}

void print_times(struct tms *ts, struct tms *te)
{
    printf("%ld", te->tms_utime + te->tms_stime - ts->tms_utime + ts->tms_stime);
    printf("%ld", te->tms_utime - ts->tms_utime);
    printf("%ld", te->tms_stime - ts->tms_stime);
}

char *create_value(int max_size)
{
    srand(time(NULL));
    int size = rand() % max_size;
    char *value = calloc(size, sizeof(char));
    char sub_vals[49] = "abcdefghijklmnopqrstuwyzABCDEFGHIJKLMNOPQRSTUWYZ";
    while (size > 1)
    {
        value[size - 1] = sub_vals[rand() % 50];
    }

    return value;
}

void add_remove_blocks(int quantity, Table *tab)
{
    for (int i = 0; i < quantity; i++)
        create_block(tab, create_value(tab->width));

    for (int i = 0; i < quantity; i++)
        delete_block(rand() % tab->height, tab);
}

void add_remove_blocks_cross(int quantity, Table *tab)
{
    for (int i = 0; i < quantity; i++)
    {
        create_block(tab, create_value(tab->width));
        delete_block(rand() % tab->height, tab);
    }
}