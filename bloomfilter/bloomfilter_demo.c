#include "bloomfilter.h"
#include <stdio.h>
#include <assert.h>
#include <sys/time.h>


/* 以查询单词是否在 wordlist.txt 中为例，可以和使用 Hashmap 作对比 */

long long get_ustime_sec(void)
{
    struct timeval tv;
    long long ust;

    gettimeofday(&tv, NULL);
    ust = ((long long)tv.tv_sec)*1000000;
    ust += tv.tv_usec;
    return ust;
}

int main(int argc, char **argv)
{
    bloomfilter *filter;

    /* 单词大概 11000 个，这里设置错误率为 0.001% */
    filter = bloomfilter_new(11000, 0.00001);
    assert(filter != NULL);

    int i = 0;
    long long start, stop;
    char word[30];

    FILE *f = fopen("../_test/wordlist.txt", "r");
    assert(f != NULL);

    start = get_ustime_sec();
    while (!feof(f)) {
        i++;
        fscanf(f, "%s\n", word);
        bloomfilter_add(filter, word);
    }
    stop = get_ustime_sec();
    fclose(f);
    
    printf("put %d word in bloomfilter cost time %lldμs\n", i, stop-start);

    char test1[30] = "comically";
    char test2[30] = "serholiu";
    int r;
    r = bloomfilter_check(filter, test1);
    if (r) {
        printf("word [%s] in wordlist.txt\n", test1);
    } else {
        printf("word [%s] not in wordlist.txt\n", test1);
    }

    r = bloomfilter_check(filter, test2);
    if (r) {
        printf("word [%s] in wordlist.txt\n", test2);
    } else {
        printf("word [%s] not in wordlist.txt\n", test2);
    }

    bloomfilter_free(filter);

    return 0;
}