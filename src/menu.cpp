#include "menu.h"

// For the breakout board, you can use any 2 or 3 pins.
// These pins will also work for the 1.8" TFT shield.
#define TFT_CS 8
#define TFT_RST 6 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC 7

#define MENU_LINE   10
#define MENU_COLUMN 26


Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);


//! -- STATIC Functions -------------------------
static void menu_set_cursor(int8_t line, int8_t column) {
    if (line < 0) { line = MENU_LINE + line; }
    if (column < 0) { column = MENU_COLUMN + column; }
    tft.setCursor((uint8_t)column * 6 + 3, (uint8_t)line * 12 + 3);
}

static void menu_clear_char(int8_t line, int8_t column, uint8_t n_char) {
    if (line < 0) { line = MENU_LINE + line; }
    if (column < 0) { column = MENU_COLUMN + column; }
    tft.fillRect((uint8_t)column * 6 + 3, (uint8_t)line * 12 + 3, n_char * 6, 12, BLACK);
}

static void menu_draw_rect(uint8_t line, uint16_t rect_color) {
    tft.drawRect(0, line * 12, tft.width(), 13, rect_color);
}

static void item_base_init(menu_item_t *menu, menu_item_t *parent, MENU_TYPE type, char* label) {
    memset(menu, 0, sizeof(menu_item_t));
    if(parent != NULL) {
        menu->parent = parent;
    }
    menu->type = type;
    strcpy(menu->label, label);
}

//! -- INIT Functions -------------------------
extern void menu_init(void) {
    //! Size XxY: 160x128
    tft.initR(INITR_GREENTAB); // Init ST7735S chip, green tab
    Serial.println(F("Initialized"));
    tft.setRotation(3);

    uint16_t time = millis();
    tft.fillScreen(BLACK);
    time = millis() - time;
    Serial.println(time, DEC);
    // delay(500);
}

extern void item_uint_init(menu_item_t *menu, menu_item_t *parent, char* label, uint16_t value) {
    item_base_init(menu, parent, UINT, label);
    menu->uint.value = value;
}

extern void item_label_init(menu_item_t *menu, menu_item_t *parent, char* label) {
    item_base_init(menu, parent, LABEL, label);
}

extern void item_submenu_init(menu_item_t *menu, menu_item_t *parent, char* label) {
    item_base_init(menu, parent, SUBMENU, label);
}

extern uint8_t item_submenu_add_child(menu_item_t *menu, menu_item_t *child) {
    if(menu->type != SUBMENU) { return FALSE; }
    if(menu->submenu.number_of_children >= MAX_CHILD) { return FALSE; }
    menu->submenu.children[menu->submenu.number_of_children++] = child;
    menu->submenu.selected = 0;
    return TRUE;
}

//! -- DRAW/NAVIGATE Functions -------------------------
static menu_item_t* current_menu = NULL;
static bool need_redraw = TRUE;

extern void menu_set(menu_item_t *menu) {
    current_menu = menu;
}

extern void menu_draw_gui(void) {
    if(need_redraw == FALSE) { return; }
    need_redraw = FALSE;
    tft.fillScreen(BLACK);

    //! if selected not submenu, take parent
    menu_item_t *menu = current_menu;
    if(menu->type != SUBMENU) { menu = menu->parent; }

    menu_set_cursor(0, 0);
    tft.print(menu->label);
    menu_draw_rect(0, CYAN);

    if(menu->type == SUBMENU) {

        menu_set_cursor(1, 0);
        if(menu->submenu.selected == 0) {
            tft.print("> "); 
        }
        else { menu_set_cursor(1, 2); }        
        tft.print("...");

        menu_item_t *child;
        for(uint8_t i = 0; i < menu->submenu.number_of_children; i++) {
            child = menu->submenu.children[i];
            
            if(menu->submenu.selected == i + 1) {
                menu_set_cursor(i + 2, 0);
                tft.print("> "); 
            }
            else { menu_set_cursor(i + 2, 2); }
            tft.print(child->label);

            if(child->type == SUBMENU) {
                menu_set_cursor(i+2, -2);
                tft.print("->");
            }
            else if(child->type == UINT) {
                menu_set_cursor(i+2, -4);
                // tft.print(*(uint16_t *)child->uint.value);
                tft.print(child->uint.value);
            }
        }
    }
}


extern void menu_navigate(NAVIGATE_OPTIONS action) {
    Serial.print(current_menu->label); Serial.print(" - ");
    if(current_menu->type == SUBMENU) {
        
        // menu_set_cursor(current_menu->submenu.selected + 1, 0);
        // tft.print(" ");
        uint8_t old_selected = current_menu->submenu.selected;
        // menu_clear_char(current_menu->submenu.selected + 1, 0, 1);
        switch(action) {
            case UP:
                if(current_menu->submenu.selected > 0) { current_menu->submenu.selected--; }
                break;
            case DOWN:
                if(current_menu->submenu.selected < current_menu->submenu.number_of_children) { current_menu->submenu.selected++; }
                break;
            case LEFT:
                if(current_menu->parent != NULL) { 
                    current_menu = current_menu->parent;
                    need_redraw = TRUE;
                }
                break;
            case RIGHT:
            case ENTER:
                if(current_menu->submenu.selected == 0) {
                    if(current_menu->parent != NULL) { 
                        current_menu = current_menu->parent;
                        need_redraw = TRUE;
                    }
                }
                else if(current_menu->submenu.children[current_menu->submenu.selected - 1]->type == SUBMENU) {
                    current_menu = current_menu->submenu.children[current_menu->submenu.selected - 1];
                    need_redraw = TRUE;
                }
                break;
            default:
                break;
        }
        Serial.print(current_menu->submenu.selected);
        if(!need_redraw) {
            menu_clear_char(old_selected + 1, 0, 1);
            menu_set_cursor(current_menu->submenu.selected + 1, 0);
            tft.print(">");
        }
    }
    Serial.println(" ");
}


//! -- HELPER Functions ---------------------------------

extern void draw_image_centered(uint8_t x0, uint8_t y0, const uint8_t *image, uint8_t image_width, uint8_t image_height) {
    draw_image(x0 - image_width / 2, y0 - image_height / 2, image, image_width, image_height);
}

extern void draw_image(uint8_t x0, uint8_t y0, const uint8_t *image, uint8_t image_width, uint8_t image_height) {

    uint16_t time = millis();

    for(uint8_t y = 0; y < image_height; y++) {
        for(uint8_t x = 0; x < image_width; x++) {
            uint8_t color8 = (uint16_t)pgm_read_byte(&image[y * image_width + x]);
            if(color8 == 0) { continue; }
            uint16_t color = COLOR( ((color8 >> 4) << 6) & 0xff, ((color8 >> 2) << 6) & 0xff, (color8 << 6) & 0xff);
            tft.drawPixel(x0 + x, y0 + y, color);
        }
    }

    time = millis() - time;
    tft.setCursor(3, 15);
    tft.print(time); tft.print("ms");
}

static void menu_ligne_test(void) {
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextWrap(false);
    tft.setTextSize(2);

    // char base_char = '0';
    // char ligne_str[13];
    // for (uint8_t i = 0; i < 6; i++) {
    //     tft.drawRect(0, i * 21, tft.width(), 22, CYAN);
    //     tft.setCursor(3, i * 21 + 3);
    //     for (uint8_t j = 0; j < 13; j++) {
    //         ligne_str[j] = base_char++;
    //     }
    //     tft.print(ligne_str);
    // }

    tft.setTextSize(1);
    char base_char = '0';
    char ligne_str[26];
    for (uint8_t i = 0; i < 11; i++) {
        tft.drawRect(0, i * 12, tft.width(), 13, CYAN);
        tft.setCursor(3, i * 12 + 3);
        for (uint8_t j = 0; j < 26; j++) {
            ligne_str[j] = base_char++;
        }
        tft.print(ligne_str);
    }
}

