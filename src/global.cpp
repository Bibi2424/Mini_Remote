#include "global.h"

uint16_t integer_pow(uint16_t x, uint16_t a) {
    if(a == 0) { return 1; }
    uint16_t result = x;
    while(--a) {
        result *= x;
    }
    return result;
}