#include  <EEPROM.h>

#include "storage.h"


// typedef struct {
//     uint16_t magic_word;
//     user_storage_t user;
//     // uint16_t crc;
// } storage_t;


extern void storage_init(user_storage_t *user) {
    uint16_t magic_word;
    EEPROM.get(0, magic_word);
    if(magic_word != 0xAA55) {
        //! not init, write data
        magic_word = 0xAA55;
        EEPROM.put(0, magic_word);
        EEPROM.put(2, *user);
    }
    else {
        EEPROM.get(2, *user);
    }
}

extern void storage_sync(user_storage_t *user) {
    EEPROM.put(2, *user);
}