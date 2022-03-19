#ifndef STORAGE_H
#define STORAGE_H

#include "global.h"

#define MAGIC_WORD  0xAA55

typedef struct {
    uint8_t radio_rx_id;
    uint8_t radio_tx_id;
    uint8_t radio_enable;
} user_storage_t;


extern void storage_init(user_storage_t *user);
extern void storage_sync(user_storage_t *user);
extern void storage_reset(void);

#endif