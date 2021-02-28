#ifndef MENU_H
#define MENU_H

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735

#include "global.h"

#define COLOR(r, g, b)  ((((b) >> 3) << (3+8)) | (((g) >> 2) << 5) | ((r) >> 3))
#define BLACK   0x0000  //COLOR(0, 0, 0)
#define BLUE    COLOR(0, 0, 255)
#define RED     COLOR(255, 0, 0)
#define GREEN   COLOR(0, 255, 0)
#define CYAN    COLOR(0, 255, 255)
#define MAGENTA COLOR(255, 255, 0)
#define YELLOW  COLOR(255, 255, 0)
#define WHITE   0xFFFF  //COLOR(255, 255, 255)

#define MAX_CHILD   8

// ---------------------------------------------------------------

extern Adafruit_ST7735 tft;

// ---------------------------------------------------------------

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    ENTER
} NAVIGATE_OPTIONS;

typedef enum {
    SUBMENU,
    UINT,
    LABEL,
    STRING,
    CUSTOM
} MENU_TYPE;


typedef struct menu_submenu_t {
    struct menu_item_t *children[MAX_CHILD];
    uint8_t number_of_children;
    uint8_t selected;
} menu_submenu_t;


typedef struct menu_uint_t {
    uint16_t value;
    void (*on_change)(uint16_t);
} menu_uint_t;


typedef struct menu_item_t {
    struct menu_item_t *parent;
    MENU_TYPE type;
    char label[20];
    union {
        menu_submenu_t submenu;
        menu_uint_t uint;
    };
} menu_item_t;

// ---------------------------------------------------------------

extern void menu_init(void);

extern void item_uint_init(menu_item_t *menu, menu_item_t *parent, char *label, uint16_t value);
extern void item_label_init(menu_item_t *menu, menu_item_t *parent, char *label);
extern void item_submenu_init(menu_item_t *menu, menu_item_t *parent, char *label);
extern uint8_t item_submenu_add_child(menu_item_t *menu, menu_item_t *child);

extern void menu_set(menu_item_t *menu);
extern void menu_draw_gui(void);
extern void menu_navigate(NAVIGATE_OPTIONS action);

extern void draw_image_centered(uint8_t x0, uint8_t y0, const uint8_t *image, uint8_t image_width, uint8_t image_height);
extern void draw_image(uint8_t x0, uint8_t y0, const uint8_t *image, uint8_t image_width, uint8_t image_height);

#endif