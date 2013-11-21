#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

long long get_ustime_sec(void)
{
    struct timeval tv;
    long long ust;

    gettimeofday(&tv, NULL);
    ust = ((long long)tv.tv_sec)*1000000;
    ust += tv.tv_usec;
    return ust;
}

typedef struct {
    int index;
    char word[30];
} value;

int compare_word(const void *desc, const void *src)
{
    return strcmp(desc, src);
}

int main(int argc, char **argv)
{
    hashmap *map = hashmap_new(0, compare_word);
    FILE *f = fopen("test/wordlist.txt", "r");
    long long start, stop;
    int i = 0;
    
    start = get_ustime_sec();
    while (!feof(f)) {
        i++;
        value *v = (value *)malloc(sizeof(value));
        fscanf(f, "%s\n", v->word);
        v->index = i;
        hashmap_put(map, v->word, v);
    }
    stop = get_ustime_sec();
    printf("cost time %lld\n", stop-start);
    char test[30] = "comically";
    value *t = hashmap_get(map, test);
    printf("%p\n", t);
    printf("%d %s\n", t->index, t->word);

    hashmap_delete(map, test);

    t = hashmap_get(map, test);
    printf("%p\n", t);
    //printf("%d %s\n", t->index, t->word);
    value *v = (value *)malloc(sizeof(value));
    v->index = 88;
    strcpy(v->word, test);
    hashmap_put(map, test, v);

    t = hashmap_get(map, test);
    printf("%p\n", t);
    printf("%d %s\n", t->index, t->word);
    return 0;
}