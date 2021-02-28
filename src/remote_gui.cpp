#include "remote_gui.h"
#include "menu.h"

#include "promo_outline.h"

menu_item_t root_menu;
menu_item_t menus[10];

void on_uint_change(uint16_t new_value) {
    Serial.print("New value: "); Serial.println(new_value);
}


extern void remote_gui_init(void) {

    menu_init();
    // menu_promo_image_screen();
    // menu_ligne_test();

    item_submenu_init(&root_menu, NULL, (char*)"Remote v0.1");
    item_label_init(&menus[0], &root_menu, (char *)"menu_101");
    item_submenu_add_child(&root_menu, &menus[0]);
    item_submenu_init(&menus[1], &root_menu, (char *)"menu_102");
    item_submenu_add_child(&root_menu, &menus[1]);
    item_label_init(&menus[2], &root_menu, (char *)"menu_103");
    item_submenu_add_child(&root_menu, &menus[2]);
    item_uint_init(&menus[3], &root_menu, (char *)"menu_104", 10);
    menus[3].uint.on_change = &on_uint_change;
    item_submenu_add_child(&root_menu, &menus[3]);
    item_label_init(&menus[4], &root_menu, (char *)"menu_105");
    item_submenu_add_child(&root_menu, &menus[4]);
    item_label_init(&menus[5], &root_menu, (char *)"menu_106");
    item_submenu_add_child(&root_menu, &menus[5]);
    item_label_init(&menus[6], &root_menu, (char *)"menu_107");
    item_submenu_add_child(&root_menu, &menus[6]);
    item_label_init(&menus[7], &root_menu, (char *)"menu_108");
    item_submenu_add_child(&root_menu, &menus[7]);

    menu_set(&root_menu);
    menu_draw_gui();
}



static void menu_promo_image_screen(void) {
    tft.fillScreen(BLACK);

    tft.drawRect(0, 0, tft.width(), 13, CYAN);
    tft.setTextSize(1);
    tft.setCursor(3, 3);
    tft.print("Promo Remote");

    draw_image_centered(90, 70, promo_outline_array, PROMO_OUTLINE_WIDTH, PROMO_OUTLINE_HEIGHT);

    tft.setCursor(40, 35);
    tft.print("10");

    tft.setCursor(40, 95);
    tft.print("20");

    tft.setCursor(30, 65);
    tft.print("30");

    tft.setCursor(140, 65);
    tft.print("40");
}