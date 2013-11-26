#include "skiplist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>

/*
 * SkipList 使用，其中 Key 和 Value 均为指针类型，Key 定义为 const 类型，
 * 使用时需要自己管理指针指向的内容。
 *
 * 以查找 wordlist 中单词为例 
 */

long long get_ustime_sec(void)
{
    struct timeval tv;
    long long ust;

    gettimeofday(&tv, NULL);
    ust = ((long long)tv.tv_sec)*1000000;
    ust += tv.tv_usec;
    return ust;
}

static int value = 1;

/* 定义对 Key 比较的函数 */
int compare_word(const char *desc, const char *src)
{
    return strcmp(desc, src);
}

int main(int argc, char **argv)
{
    skiplist *list = skiplist_new(compare_word);
    assert(list != NULL);
    

    FILE *f = fopen("wordlist.txt", "r");
    assert(f != NULL);

    char *key;
    int i = 0;
    long long start, stop;

    start = get_ustime_sec();
    while (!feof(f)) {
        i++;
        key = (char *)malloc(sizeof(char) * 30);
        assert(key != NULL);

        fscanf(f, "%s\n", key);
        skiplist_insert(list, key, &value);
    }
    stop = get_ustime_sec();
    fclose(f);
    
    printf("put %d word in skiplist cost time %lldμs\n", i, stop-start);
    
    /* 查询 */
    char test[30] = "comically";
    int *t = skiplist_search(list, test);
    if (t) {
        printf("%s in wordlist and value = %d\n", test, *t);
    } else {
        printf("%s not in wordlist\n", test);
    }


    skiplist_delete(list, test);

    t = skiplist_search(list, test);
    if (t) {
        printf("%s in wordlist\n", test);
    } else {
        printf("%s not in wordlist\n", test);
    }

    /**
     * 在删除 Hashmap 前，需要自己管理 Key, Value 指向的内存
     * 这里并没有释放 Key 内存
     */
    skiplist_free(list);
    return 0;
}