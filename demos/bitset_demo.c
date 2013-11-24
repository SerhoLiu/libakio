#include "bitset.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* 下面以求两个集合交集为例 */

static int A[20], B[20];

/**
 * 这里为了简便，没有符合集合特性，
 * 如果需要生成唯一元素，也可以借助 bitset 
 */
void init_array()
{
    srand(time(NULL));
    
    int i;
    for (i = 0; i < 20; i++) {
        A[i] = rand() % 30 + 1;
        B[i] = rand() % 40 + 1;
    }
}

void print_array(int array[], int len)
{
    int i;
    for (i = 0; i < len; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    bitset *set;
    set = bitset_new(50);
    if (set == NULL) return 0;

    init_array();
    printf("A: \n");
    print_array(A, 20);
    printf("B: \n");
    print_array(B, 20);

    int i, j, common[20];
    for (i = 0; i < 20; i++) {
        bitset_set(set, A[i], 1);
    }

    j = 0;
    for (i = 0; i < 20; i++) {
        if (bitset_get(set, B[i])) {
            common[j++] = B[i];
        }
    }

    printf("Common: \n");
    print_array(common, j);

    return 0;
}