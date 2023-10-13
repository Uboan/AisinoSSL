
#include <stdint.h>
#define ENTROPY_POOL_SIZE 64

#define MAX_RAND_VALUE 255
#define OPTION_RDRAND 1
#define OPTION_TIME 2
#define OPTION_OTHTER 3
unsigned int drng_features;


union ENTROPY_POOL_t{
    uint8_t u8[64];
    uint16_t u16[32];
    uint32_t u32[16];
    uint64_t u64[8];
} entropy_pool;



void initialize_entropy_pool();

void update_entropy_pool();