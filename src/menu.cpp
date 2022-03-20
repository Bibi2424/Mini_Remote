#include "menu.h"

// For the breakout board, you can use any 2 or 3 pins.
// These pins will also work for the 1.8" TFT shield.
#define TFT_CS 8
#define TFT_RST 6 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC 7

#define MENU_LINE   10
#define MENU_COLUMN 26

#define VLED_NUMBER 2


Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

static uint8_t vleds[VLED_NUMBER] = {0, 0};


//! -- STATIC Functions -------------------------
static inline void menu_vled_redraw(uint8_t led_id);
static inline void menu_vled_redraw_all(void);

//! TODO: add menu_to_screen_pos function
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


static void menu_draw_checkbox(int8_t line, int8_t column, bool ticked) {
    if (line < 0) { line = MENU_LINE + line; }
    if (column < 0) { column = MENU_COLUMN + column; }
    if(ticked) {
        menu_set_cursor(line, column);
        tft.print("x");
    }
    tft.drawRect((uint8_t)column * 6 + 1, (uint8_t)line * 12 + 3, 9, 9, WHITE);
}


static void menu_draw_action_icon(int8_t line, int8_t column) {
    if (line < 0) { line = MENU_LINE + line; }
    if (column < 0) { column = MENU_COLUMN + column; }
    tft.drawCircle((uint8_t)column * 6 + 3 + 2, (uint8_t)line * 12 + 3 + 2, 4, WHITE);
}


static void item_base_init(menu_item_t *menu, MENU_TYPE_t type, char* label) {
    memset(menu, 0, sizeof(menu_item_t));
    menu->redraw = NO_REDRAW;
    menu->type = type;
    strncpy(menu->label, label, 15);
    menu->label[15] = '\0';
}


extern void menu_init(void) {
    //! Size XxY: 160x128
    tft.initR(INITR_GREENTAB); // Init ST7735S chip, green tab
    // Serial.println(F("Initialized"));
    tft.setRotation(3);

    // uint16_t time = millis();
    tft.fillScreen(BLACK);
    // time = millis() - time;
    // Serial.println(time, DEC);
    // delay(500);
}


extern void item_label_init(menu_item_t *menu, char* label) {
    item_base_init(menu, LABEL, label);
}


extern void item_uint_init(menu_item_t *menu, char* label, uint16_t value, void (*on_change)(uint16_t)) {
    item_base_init(menu, UINT, label);
    menu->uint.value = value;
    menu->uint.on_change = on_change;
}


extern void item_string_init(menu_item_t *menu, char* label, const char *new_string, void (*on_change)(char*)) {
    item_base_init(menu, STRING, label);
    char str_temp[11];
    strncpy(str_temp, new_string, 10);
    str_temp[11] = '\0';
    sprintf(menu->str.str, "%10s", str_temp);
    menu->str.on_change = on_change;
}


extern void item_checkbox_init(menu_item_t *menu, char* label, bool value, void (*on_change)(bool)) {
    item_base_init(menu, CHECKBOX, label);
    menu->checkbox.value = value;
    menu->checkbox.on_change = on_change;
}


extern void item_action_init(menu_item_t *menu, char* label, void (*do_action)(void)) {
    item_base_init(menu, ACTION, label);
    menu->action.do_action = do_action;
}


extern void item_custom_init(menu_item_t *menu, char *label, void (*draw)(Adafruit_ST7735 *, menu_item_t *), menu_item_t *(*navigate)(NAVIGATE_OPTIONS_t, menu_item_t*)) {
    item_base_init(menu, CUSTOM, label);
    menu->custom.draw_call = draw;
    menu->custom.navigate_call = navigate;
}


extern void item_submenu_init(menu_item_t *menu, char* label) {
    item_base_init(menu, SUBMENU, label);
    menu->submenu.selected = 1;
}


extern uint8_t item_submenu_add_child(menu_item_t *menu, menu_item_t *child) {
    if(menu->type != SUBMENU) { return FALSE; }
    if(menu->submenu.number_of_children >= MAX_CHILD) { return FALSE; }
    menu->submenu.children[menu->submenu.number_of_children++] = child;
    child->parent = menu;
    return TRUE;
}

//! -- Modify functions -------------------------
extern void item_set_label(menu_item_t *menu, const char *label) {
    strncpy(menu->label, label, 15);
    menu->label[15] = '\0';
    if(menu->parent != NULL) { menu->parent->redraw = FULL_REDRAW; }
}


extern uint16_t item_uint_get_value(menu_item_t *menu) {
    if(menu->type != UINT) { return 0xFFFF; }
    return menu->uint.value;
}


extern void item_uint_set_value(menu_item_t *menu, const uint16_t value) {
    if(menu->type != UINT) { return; }
    menu->uint.value = value;
    menu->redraw = FULL_REDRAW;
    if(menu->parent != NULL) { menu->parent->redraw = PARTIAL_REDRAW; }
}


extern void item_uint_set_callback(menu_item_t *menu, void (*on_change)(uint16_t)) {
    if(menu->type != UINT) { return; }
    menu->uint.on_change = on_change;
}


extern uint8_t item_string_get(menu_item_t *menu, char *new_string) {
    if(menu->type != STRING) { return 0; }
    char *p_str = menu->str.str;
    while(*p_str == ' ' && *p_str != '\0');
    strcpy(new_string, p_str);
    return strlen(new_string);
}


extern void item_string_set(menu_item_t *menu, const char *new_string) {
    if(menu->type != STRING) { return; }
    char str_temp[11];
    strncpy(str_temp, new_string, 10);
    str_temp[11] = '\0';
    sprintf(menu->str.str, "%10s", str_temp);
    menu->redraw = FULL_REDRAW;
    if(menu->parent != NULL) { menu->parent->redraw = PARTIAL_REDRAW; }
}


extern void item_string_set_callback(menu_item_t *menu, void (*on_change)(char*)) {
    if(menu->type != STRING) { return; }
    menu->str.on_change = on_change;
}


extern bool item_checkbox_get_value(menu_item_t *menu) {
    if(menu->type != CHECKBOX) { return false; }
    return menu->uint.value;
}


extern void item_checkbox_set_value(menu_item_t *menu, const bool value) {
    if(menu->type != CHECKBOX) { return; }
    menu->checkbox.value = value;
    menu->redraw = FULL_REDRAW;
    if(menu->parent != NULL) { menu->parent->redraw = PARTIAL_REDRAW; }
}


extern void item_checkbox_set_callback(menu_item_t *menu, void (*on_change)(bool)) {
    if(menu->type != CHECKBOX) { return; }
    menu->checkbox.on_change = on_change;
}


extern void item_action_set_callback(menu_item_t *menu, void (*do_action)(void)) {
    if(menu->type != ACTION) { return; }
    menu->action.do_action = do_action;
}


//! -- DRAW/NAVIGATE Functions -------------------------
static menu_item_t* current_menu = NULL;
static uint8_t current_ligne = 0;
static uint8_t edition_column = -2;


extern void menu_set(menu_item_t *menu) {
    current_menu = menu;
    current_menu->redraw = FULL_REDRAW;
}


extern void menu_redraw(menu_item_t *menu, REDRAW_TYPE type) {
    current_menu->redraw = type;
}


extern void menu_draw_header(menu_item_t *menu) {
    if(menu->redraw != FULL_REDRAW) { return; }

    tft.fillScreen(BLACK);
    menu_set_cursor(0, 0);
    tft.print(menu->label);
    menu_draw_rect(0, CYAN);

    menu_vled_redraw_all();
}


extern bool menu_draw_gui(void) {
    if(current_menu->redraw == NO_REDRAW) { Serial.print("."); return false; }
    Serial.print("!");

    menu_item_t *menu = current_menu;

    if(menu->type == UINT) {
        menu_clear_char(current_ligne, -6, 5);
        char str_temp[10];
        sprintf(str_temp, "%5u", menu->uint.value);
        menu_set_cursor(current_ligne, -6);
        for(uint8_t i = 0; i < strlen(str_temp); i++) {
            if(i + 5 == edition_column) { tft.setTextColor(BLACK, WHITE); }
            tft.print(str_temp[i]);
            if(i + 5 == edition_column) { tft.setTextColor(WHITE, BLACK); }
        }
        menu->redraw = NO_REDRAW;
    }
    else if(menu->type == STRING) {
        menu_clear_char(current_ligne, -11, 5);
        // char str_temp[11];
        // sprintf(str_temp, "%10s", menu->str.str);
        menu_set_cursor(current_ligne, -11);
        for(uint8_t i = 0; i < strlen(menu->str.str); i++) {
            if(i == edition_column) { tft.setTextColor(BLACK, WHITE); }
            tft.print(menu->str.str[i]);
            if(i == edition_column) { tft.setTextColor(WHITE, BLACK); }
        }
        menu->redraw = NO_REDRAW;
    }
    else if(menu->type == SUBMENU) {
        menu_draw_header(menu);

        if(menu->submenu.selected == 0) {
            menu_set_cursor(1, 0);
            tft.print("> "); 
        }
        else {
            menu_clear_char(1, 0, 1);
            menu_set_cursor(1, 2); 
        }
        tft.print("...");

        menu_item_t *child;
        for(uint8_t i = 0; i < menu->submenu.number_of_children; i++) {
            child = menu->submenu.children[i];
            
            if(menu->submenu.selected == i + 1) {
                menu_set_cursor(i + 2, 0);
                tft.print("> "); 
            }
            else { 
                menu_clear_char(i + 2 , 0, 1);
                menu_set_cursor(i + 2, 2); 
            }
            if (menu->redraw == FULL_REDRAW) { tft.print(child->label); }

            if(child->type == SUBMENU && menu->redraw == FULL_REDRAW) {
                menu_set_cursor(i+2, -2);
                tft.print("->");
            }
            else if(child->type == CUSTOM && menu->redraw == FULL_REDRAW) {
                menu_set_cursor(i+2, -2);
                tft.print("->");
            }
            else if(child->type == UINT && (child->redraw != NO_REDRAW || menu->redraw == FULL_REDRAW)) {
                child->redraw = NO_REDRAW;
                menu_clear_char(i+2, -11, 10);
                menu_set_cursor(i+2, -11);
                char str_temp[11];
                sprintf(str_temp, "%10u", child->uint.value);
                tft.print(str_temp);
            }
            else if(child->type == STRING && (child->redraw != NO_REDRAW || menu->redraw == FULL_REDRAW)) {
                child->redraw = NO_REDRAW;
                menu_clear_char(i+2, -11, 10);
                menu_set_cursor(i+2, -11);
                tft.print(child->str.str);
            }
            else if(child->type == CHECKBOX && (child->redraw != NO_REDRAW || menu->redraw == FULL_REDRAW)) {
                child->redraw = NO_REDRAW;
                menu_clear_char(i+2, -2, 2);
                // menu_set_cursor(i+2, -2);
                menu_draw_checkbox(i+2, -2, child->checkbox.value);
                // if(child->checkbox.value) { tft.print("x"); }
                // else { tft.print("."); }
            }
            else if(child->type == ACTION && menu->redraw == FULL_REDRAW) {
                // menu_set_cursor(i+2, -2);
                // tft.print("#");
                menu_clear_char(i+2, -2, 2);
                menu_draw_action_icon(i+2, -2);
            }
        }
        menu->redraw = NO_REDRAW;
    }
    else if(menu->type == CUSTOM) {
        menu_draw_header(menu);
        menu->custom.draw_call(&tft, menu);
        menu->redraw = NO_REDRAW;
    }

    return true;
}


extern void menu_navigate(NAVIGATE_OPTIONS_t action) {
    if(NAVIGATE_NONE == action) { return; }

    if(current_menu->type == UINT) {
        switch(action) {
            case NAVIGATE_UP: {
                uint16_t increment = integer_pow(10, 9 - edition_column);
                if(current_menu->uint.value + increment >= current_menu->uint.value) { current_menu->uint.value += increment; }
                current_menu->redraw = PARTIAL_REDRAW;
                break;
            }
            case NAVIGATE_DOWN: {
                uint16_t increment = integer_pow(10, 9 - edition_column);
                if(current_menu->uint.value - increment <= current_menu->uint.value) { current_menu->uint.value -= increment; }
                current_menu->redraw = PARTIAL_REDRAW;
                break;
            }
            case NAVIGATE_LEFT:
                if(edition_column > 5) { edition_column--; current_menu->redraw = PARTIAL_REDRAW; }
                break;
            case NAVIGATE_RIGHT:
                if(edition_column < 10 - 1) { edition_column++; current_menu->redraw = PARTIAL_REDRAW; }
                break;
            case NAVIGATE_ENTER:
                if(current_menu->uint.on_change) { current_menu->uint.on_change(current_menu->uint.value); }
                if(current_menu->parent != NULL) {
                    current_menu->redraw = FULL_REDRAW;
                    current_menu = current_menu->parent;
                    current_menu->redraw = PARTIAL_REDRAW;
                }
                break;
            default:
                break;
        }
    }
    else if(current_menu->type == STRING) {
        switch(action) {
            case NAVIGATE_UP: 
                current_menu->str.str[edition_column]++;
                current_menu->redraw = PARTIAL_REDRAW;
                break;
            case NAVIGATE_DOWN:
                current_menu->str.str[edition_column]--;
                current_menu->redraw = PARTIAL_REDRAW;
                break;
            case NAVIGATE_LEFT:
                if(edition_column > 0) { edition_column--; current_menu->redraw = PARTIAL_REDRAW; }
                break;
            case NAVIGATE_RIGHT:
                if(edition_column < 10 - 1) { edition_column++; current_menu->redraw = PARTIAL_REDRAW; }
                break;
            case NAVIGATE_ENTER:
                if(current_menu->str.on_change) { current_menu->str.on_change(current_menu->str.str); }
                if(current_menu->parent != NULL) {
                    current_menu->redraw = FULL_REDRAW;
                    current_menu = current_menu->parent;
                    current_menu->redraw = FULL_REDRAW;
                }
                break;
            default:
                break;
        }
    }
    else if(current_menu->type == SUBMENU) {
        switch(action) {
            case NAVIGATE_UP:
                if(current_menu->submenu.selected > 0) { 
                    current_menu->submenu.selected--;
                    current_menu->redraw = PARTIAL_REDRAW;
                }
                break;
            case NAVIGATE_DOWN:
                if(current_menu->submenu.selected < current_menu->submenu.number_of_children) { 
                    current_menu->submenu.selected++;
                    current_menu->redraw = PARTIAL_REDRAW;
                }
                break;
            case NAVIGATE_LEFT:
                if(current_menu->parent != NULL) { 
                    current_menu = current_menu->parent;
                    current_menu->redraw = FULL_REDRAW;
                }
                break;
            case NAVIGATE_RIGHT:
            case NAVIGATE_ENTER: {
                if(current_menu->submenu.selected == 0) {
                    if(current_menu->parent != NULL) {
                        current_menu = current_menu->parent;
                        current_menu->redraw = FULL_REDRAW;
                        break;
                    }
                }
                menu_item_t* submenu = current_menu->submenu.children[current_menu->submenu.selected - 1];
                if(submenu->type == SUBMENU) {
                    current_menu = submenu;
                    current_menu->redraw = FULL_REDRAW;
                }
                else if(submenu->type == UINT || submenu->type == STRING) {
                    current_ligne = current_menu->submenu.selected + 1;
                    edition_column = 10 - 1;
                    current_menu = submenu;
                    current_menu->redraw = FULL_REDRAW;
                }
                else if(submenu->type == CHECKBOX) {
                    submenu->checkbox.value ^= 1;
                    current_menu->redraw = FULL_REDRAW;
                    if(submenu->checkbox.on_change != NULL) {
                        submenu->checkbox.on_change(submenu->checkbox.value);
                    }
                }
                else if(submenu->type == ACTION) {
                    if(submenu->action.do_action != NULL) {
                        submenu->action.do_action();
                    }
                    //! Redraw is not necessary, use it for user feedback (screen flicker)
                    current_menu->redraw = FULL_REDRAW;
                }
                else if(submenu->type == CUSTOM) {
                    current_menu = submenu;
                    current_menu->redraw = FULL_REDRAW;
                }
                break;
                }
            default:
                break;
        }
    }
    else if(current_menu->type == CUSTOM) {
        current_menu = current_menu->custom.navigate_call(action, current_menu);
    }
}


//! -- Virtual LED Functions ---------------------------------
static inline void menu_vled_redraw(uint8_t led_id) {
    uint16_t color;
    if(vleds[led_id]) { color = GREEN; }
    else { color = RED; }
    tft.fillCircle(135 + led_id * 15, 6, 5, color);
    tft.drawCircle(135 + led_id * 15, 6, 5, COLOR(128, 128, 128));
}


static inline void menu_vled_redraw_all(void) {
    uint8_t i;
    for(i = 0; i < VLED_NUMBER; i++) {
        menu_vled_redraw(i);
    }
}


extern void menu_vled_set(uint8_t led_id, bool state) {
    if(led_id >= VLED_NUMBER) { return; }
    if(vleds[led_id] == state) { return; }
    vleds[led_id] = state;

    menu_vled_redraw(led_id);
}



//! -- HELPER Functions ---------------------------------
extern void draw_image_centered(uint8_t x0, uint8_t y0, const uint8_t *image, uint8_t image_width, uint8_t image_height) {
    draw_image(x0 - image_width / 2, y0 - image_height / 2, image, image_width, image_height);
}


extern void draw_image(uint8_t x0, uint8_t y0, const uint8_t *image, uint8_t image_width, uint8_t image_height) {

    for(uint8_t y = 0; y < image_height; y++) {
        for(uint8_t x = 0; x < image_width; x++) {
            uint8_t color8 = (uint16_t)pgm_read_byte(&image[y * image_width + x]);
            if(color8 == 0) { continue; }
            uint16_t color = COLOR( ((color8 >> 4) << 6) & 0xff, ((color8 >> 2) << 6) & 0xff, (color8 << 6) & 0xff);
            tft.drawPixel(x0 + x, y0 + y, color);
        }
    }
}


extern void debug_menu_item(menu_item_t *menu) {
    Serial.print("ITEM: ");
    Serial.print(menu->label);
    Serial.print(" - ");
    char *menu_type_str[8] = {(char *)"SUBMENU", (char *)"UINT", (char *)"LABEL", (char *)"STRING", (char *)"CUSTOM"};
    Serial.print(menu_type_str[menu->type + 1]);
    Serial.print(" - r:");
    Serial.print(menu->redraw);
    if(menu->parent != NULL) {
        Serial.print(" - Parent: ");
        Serial.print(menu->parent->label);
    }
    if(menu->type == SUBMENU) {
        Serial.print(" - [");
        for(uint8_t i = 0; i < menu->submenu.number_of_children; i++) {
            Serial.print(menu->submenu.children[i]->label);
            Serial.print(",");
        }
        Serial.print("]");
    }
    Serial.println(" ");
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

