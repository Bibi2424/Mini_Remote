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

#define VLED_NRF_RX 0
#define VLED_NRF_TX 1

// ---------------------------------------------------------------

extern Adafruit_ST7735 tft;

// ---------------------------------------------------------------

typedef enum {
    NAVIGATE_UP,
    NAVIGATE_DOWN,
    NAVIGATE_LEFT,
    NAVIGATE_RIGHT,
    NAVIGATE_ENTER,
    NAVIGATE_NONE
} NAVIGATE_OPTIONS_t;

typedef enum {
    MENU_NONE,
    SUBMENU,
    UINT,
    LABEL,
    STRING,
    CHECKBOX,
    ACTION,
    CUSTOM
} MENU_TYPE_t;

typedef enum {
    NO_REDRAW,
    PARTIAL_REDRAW,
    FULL_REDRAW,
} REDRAW_TYPE;


typedef struct menu_submenu_t {
    struct menu_item_t *children[MAX_CHILD];
    uint8_t number_of_children;
    uint8_t selected;
} menu_submenu_t;


typedef struct menu_uint_t {
    uint16_t value;
    void (*on_change)(uint16_t);
} menu_uint_t;


typedef struct menu_string_t {
    char str[11];
    void (*on_change)(char*);
} menu_string_t;


typedef struct menu_checkbox_t {
    bool value;
    void (*on_change)(bool);
} menu_checkbox_t;


typedef struct menu_action_t {
    void (*do_action)(void);
} menu_action_t;


typedef struct menu_custom_t {
    void (*draw_call)(Adafruit_ST7735 *screen, menu_item_t *menu);
    menu_item_t *(*navigate_call)(NAVIGATE_OPTIONS_t action, menu_item_t *menu);
} menu_custom_t;


typedef struct menu_item_t {
    struct menu_item_t *parent;
    MENU_TYPE_t type;
    char label[15];
    REDRAW_TYPE redraw;
    union {
        menu_submenu_t submenu;
        menu_uint_t uint;
        menu_string_t str;
        menu_checkbox_t checkbox;
        menu_action_t action;
        menu_custom_t custom;
    };
} menu_item_t;

// ---------------------------------------------------------------

extern void menu_init(void);

extern void item_label_init(menu_item_t *menu, char *label);
extern void item_uint_init(menu_item_t *menu, char *label, uint16_t value, void (*on_change)(uint16_t));
extern void item_string_init(menu_item_t *menu, char *label, const char *new_string, void (*on_change)(char *));
extern void item_checkbox_init(menu_item_t *menu, char* label, bool value, void (*on_change)(bool));
extern void item_action_init(menu_item_t *menu, char* label, void (*do_action)(void));
extern void item_custom_init(menu_item_t *menu, char *label, void (*draw)(Adafruit_ST7735 *, menu_item_t *), menu_item_t *(*navigate)(NAVIGATE_OPTIONS_t, menu_item_t*));
extern void item_submenu_init(menu_item_t *menu, char *label);
extern uint8_t item_submenu_add_child(menu_item_t *menu, menu_item_t *child);

extern void item_set_label(menu_item_t *menu, const char *label);
extern uint16_t item_uint_get_value(menu_item_t *menu);
extern void item_uint_set_value(menu_item_t *menu, const uint16_t value);
extern void item_uint_set_callback(menu_item_t *menu, void (*on_change)(uint16_t));
extern uint8_t item_string_get(menu_item_t *menu, char *new_string);
extern void item_string_set(menu_item_t *menu, const char *new_string);
extern void item_string_set_callback(menu_item_t *menu, void (*on_change)(char *));
extern bool item_checkbox_get_value(menu_item_t *menu);
extern void item_checkbox_set_value(menu_item_t *menu, const bool value);
extern void item_checkbox_set_callback(menu_item_t *menu, void (*on_change)(bool));
extern void item_action_set_callback(menu_item_t *menu, void (*do_action)(void));

extern void menu_set(menu_item_t *menu);
extern void menu_min_redraw(menu_item_t *menu, REDRAW_TYPE redraw);
extern void menu_draw_header(menu_item_t *menu);
extern bool menu_draw_gui(void);
extern void menu_navigate(NAVIGATE_OPTIONS_t action);

extern void menu_vled_set(uint8_t led_id, bool state);

extern void draw_image_centered(uint8_t x0, uint8_t y0, const uint8_t *image, uint8_t image_width, uint8_t image_height);
extern void draw_image(uint8_t x0, uint8_t y0, const uint8_t *image, uint8_t image_width, uint8_t image_height);

extern void debug_menu_item(menu_item_t *menu);

#endif