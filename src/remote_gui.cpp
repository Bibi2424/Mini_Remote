#include "remote_gui.h"
#include "promo_outline.h"

static void menu_promo_image_draw(Adafruit_ST7735 *screen, menu_item_t *menu);
static menu_item_t* menu_promo_image_navigate(NAVIGATE_OPTIONS action, menu_item_t *menu);

menu_item_t root_menu;
menu_item_t menus[10];


void on_uint_change(uint16_t new_value) {
    Serial.print("New value: "); Serial.println(new_value);
}


extern void remote_gui_init(user_storage_t *user) {

    menu_init();
    // menu_promo_image_screen();
    // menu_ligne_test();

    item_submenu_init(&root_menu, (char*)"Remote v" VERSION);

    // item_label_init(&menus[0], (char *)"menu_101");
    // item_submenu_add_child(&root_menu, &menus[0]);
    // item_submenu_init(&menus[1], (char *)"submenu_102");
    // item_submenu_add_child(&root_menu, &menus[1]);
    // item_label_init(&menus[2], (char *)"menu_103");
    // item_submenu_add_child(&root_menu, &menus[2]);
    // item_uint_init(&menus[3], (char *)"uint_104", 10);
    // menus[3].uint.on_change = &on_uint_change;
    // item_submenu_add_child(&root_menu, &menus[3]);
    // item_label_init(&menus[4], (char *)"menu_105");
    // item_submenu_add_child(&root_menu, &menus[4]);
    // item_string_init(&menus[5], (char *)"string_106", (char *)"s106");
    // item_submenu_add_child(&root_menu, &menus[5]);
    // item_label_init(&menus[6], (char *)"menu_107");
    // item_submenu_add_child(&root_menu, &menus[6]);
    // item_label_init(&menus[7], (char *)"menu_108");
    // item_submenu_add_child(&root_menu, &menus[7]);

    // item_label_init(&menus[0], (char *)"PRoMo image");
    item_custom_init(&menus[MENU_PROMO_IMAGE], (char *)"PRoMo image", &menu_promo_image_draw, &menu_promo_image_navigate);
    item_submenu_add_child(&root_menu, &menus[MENU_PROMO_IMAGE]);

    item_uint_init(&menus[RADIO_RX_ID], (char *)"radio RX ID", user->radio_rx_id, &on_uint_change);
    item_submenu_add_child(&root_menu, &menus[RADIO_RX_ID]);

    item_uint_init(&menus[RADIO_TX_ID], (char *)"radio TX ID", user->radio_tx_id, NULL);
    item_submenu_add_child(&root_menu, &menus[RADIO_TX_ID]);
    // menus[3].uint.on_change = &on_uint_change;

    item_label_init(&menus[MENU_EMPTY_LINE], (char *)" ");
    item_submenu_add_child(&root_menu, &menus[MENU_EMPTY_LINE]);

    item_submenu_init(&menus[MENU_PROMO_STATS], (char *)"PRoMo stats");
    item_submenu_add_child(&root_menu, &menus[MENU_PROMO_STATS]);

    item_uint_init(&menus[MENU_PROMO_STATS_V], (char *)"v:", 0, NULL);
    item_submenu_add_child(&menus[MENU_PROMO_STATS], &menus[MENU_PROMO_STATS_V]);

    item_uint_init(&menus[MENU_PROMO_STATS_T], (char *)"t:", 0, NULL);
    item_submenu_add_child(&menus[MENU_PROMO_STATS], &menus[MENU_PROMO_STATS_T]);

    menu_set(&menus[MENU_PROMO_IMAGE]);
    menu_set(&root_menu);
    menu_draw_gui();
}



static void menu_promo_image_draw(Adafruit_ST7735 *screen, menu_item_t *menu) {
    // screen.fillScreen(BLACK);

    // screen.drawRect(0, 0, screen.width(), 13, CYAN);
    // screen.setTextSize(1);
    // screen.setCursor(3, 3);
    // screen.print("Promo Remote");

    if(menu->redraw == FULL_REDRAW) {
        draw_image_centered(90, 70, promo_outline_array, PROMO_OUTLINE_WIDTH, PROMO_OUTLINE_HEIGHT);
    }

    screen->setCursor(40, 35);
    screen->setTextColor(BLACK, BLACK);
    screen->print("   ");
    screen->setTextColor(WHITE, BLACK);
    screen->setCursor(40, 35);
    screen->print("10");

    screen->setCursor(40, 95);
    screen->setTextColor(BLACK, BLACK);
    screen->print("   ");
    screen->setTextColor(WHITE, BLACK);
    screen->setCursor(40, 95);
    screen->print("20");

    screen->setCursor(40, 65);
    screen->setTextColor(BLACK, BLACK);
    screen->print("   ");
    screen->setTextColor(WHITE, BLACK);
    screen->setCursor(30, 65);
    screen->print("30");

    screen->setCursor(140, 65);
    screen->setTextColor(BLACK, BLACK);
    screen->print("   ");
    screen->setTextColor(WHITE, BLACK);
    screen->setCursor(140, 65);
    screen->print("40");

}

static menu_item_t* menu_promo_image_navigate(NAVIGATE_OPTIONS action, menu_item_t *menu) {
    switch(action) {
        case UP:
            // break;
        case DOWN:
            // break;
        case LEFT:
            // break;
        case RIGHT:
            break;
        case ENTER:
            if(menu->parent != NULL) {
                menu = menu->parent;
                menu->redraw = FULL_REDRAW;
            }
            break;
        default:
            break;
    }
    return menu;
}