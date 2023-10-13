#include <stdio.h>
#include <stdlib.h>
#include "entropy.h"
#include <stdint.h>
#include "drng.h"


static uint16_t AiRand(int option){
    
    
    uint16_t *rand;
    if((drng_features & DRNG_HAS_RDRAND)&&(option==OPTION_RDRAND)){
        rdrand16_step(rand);
    }
    
    return rand;


};
void initialize_entropy_pool(){

    drng_features=get_drng_support();//see if drng is supportted

	/* Determine DRNG support */

    for(int i=0;i<ENTROPY_POOL_SIZE;i++){
        entropy_pool.u16[i] = (uint16_t)(AiRand(OPTION_RDRAND) % (MAX_RAND_VALUE + 1));
    }


};

void update_entropy_pool(){

    for (int i = 0; i < ENTROPY_POOL_SIZE; i++) {
        entropy_pool.u16[i] ^= (uint16_t)(AiRand(OPTION_RDRAND) % (MAX_RAND_VALUE + 1));
    }

};


