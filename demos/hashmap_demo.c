#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>

/*
 * Hashmap 使用，其中 Key 和 Value 均为指针类型，Key 定义为 const 类型，
 * 使用时需要自己管理指针指向的内容：下面将 wordlist 文件中的单词加入 Hashmap
 * 中
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
static int newvalue = 2;

/* 定义对 Key 比较的函数 */
int compare_word(const char *desc, const char *src)
{
    return strcmp(desc, src);
}

/* map 函数，其中可以对 Value 值进行修改 */
void map_func(const char *key, void **value, const void *other)
{
    *value = &newvalue;
}

int main(int argc, char **argv)
{
    /* 新建一个使用默认初始大小的 Hashmap */
    hashmap *map = hashmap_new(0, compare_word);
    assert(map != NULL);

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
        hashmap_put(map, key, &value);
    }
    stop = get_ustime_sec();

    printf("put %d word in hashmap cost time %lldμs\n", i, stop-start);
    
    /* 查询 */
    char test[30] = "comically";
    int *t = hashmap_get(map, test);
    if (t) {
        printf("%s in wordlist and value = %d\n", test, *t);
    } else {
        printf("%s not in wordlist\n", test);
    }

    /* 改变 value 值 */
    hashmap_map(map, map_func, NULL);
    
    t = hashmap_get(map, test);
    if (t) {
        printf("%s in wordlist and value = %d\n", test, *t);
    } else {
        printf("%s not in wordlist\n", test);
    }

    hashmap_delete(map, test);

    t = hashmap_get(map, test);
    if (t) {
        printf("%s in wordlist\n", test);
    } else {
        printf("%s not in wordlist\n", test);
    }

    /**
     * 在删除 Hashmap 前，需要自己管理 Key, Value 指向的内存
     * 这里并没有释放 Key 内存
     */
    hashmap_free(map);
    return 0;
}