#include <stdio.h>
#include <stdlib.h>
#include "entropy.h"
#include <stdint.h>
#include "rand.h"

int main(){

    void *ctx;
    int arr[5];
    memset(arr, 0, sizeof(arr));
    aisino_rand_init(&ctx);
    aisino_rand_seed(ctx, NULL, 0);
    aisino_rand_list(ctx, arr, 5);
    aisino_rand_free(ctx);
    return 0;
}