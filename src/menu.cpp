#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include "menu.h"

#include "promo_outline.h"

// For the breakout board, you can use any 2 or 3 pins.
// These pins will also work for the 1.8" TFT shield.
#define TFT_CS 8
#define TFT_RST 6 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC 7


static void menu_draw_main_screen(void);
static void menu_ligne_test(void);
static void draw_image_centered(uint8_t x0, uint8_t y0, const uint8_t *image, uint8_t image_width, uint8_t image_height);
static void draw_image(uint8_t x0, uint8_t y0, const uint8_t *image, uint8_t image_width, uint8_t image_height);

static Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

extern void menu_init(void) {
    //! Size XxY: 160x128
	tft.initR(INITR_GREENTAB); // Init ST7735S chip, green tab
	Serial.println(F("Initialized"));
	tft.setRotation(3);

	uint16_t time = millis();
	tft.fillScreen(ST77XX_BLACK);
	time = millis() - time;
	Serial.println(time, DEC);
	// delay(500);

    menu_draw_main_screen();
    // menu_ligne_test();
}


static void menu_draw_main_screen(void) {
    tft.fillScreen(ST77XX_BLACK);

    tft.drawRect(0, 0, tft.width(), 13, CYAN);
    tft.setTextSize(1);
    tft.setCursor(3, 3);
    tft.print("Promo Remote");

    draw_image_centered(90, 70, promo_outline_array, PROMO_OUTLINE_WIDTH, PROMO_OUTLINE_HEIGHT);
}

static void draw_image_centered(uint8_t x0, uint8_t y0, const uint8_t *image, uint8_t image_width, uint8_t image_height) {
    draw_image(x0 - image_width / 2, y0 - image_height / 2, image, image_width, image_height);
}

static void draw_image(uint8_t x0, uint8_t y0, const uint8_t *image, uint8_t image_width, uint8_t image_height) {

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

