#include <Arduino.h>

#include <SPI.h>
#include <NRFLite.h>
#include <Adafruit_GFX.h>	 // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include "joystick.h"

#define TRUE 1
#define FALSE 0

joystick_t left_joystick = {
	.vertical_axis = { .pin = A4, .value = 0, .inverted = TRUE },
	.horizontal_axis = { .pin = A5, .value = 0, .inverted = FALSE },
	.output_range = { -100, 100},
	.dead_space = 10
};
joystick_t right_joystick = {
	.vertical_axis = {.pin = A6, .value = 0, .inverted = TRUE},
	.horizontal_axis = {.pin = A7, .value = 0, .inverted = TRUE},
	.output_range = {-100, 100},
	.dead_space = 10
};

const static uint8_t RADIO_ID = 1;			   // Our radio's id.
const static uint8_t DESTINATION_RADIO_ID = 2; // Id of the radio we will transmit to.
const static uint8_t PIN_RADIO_CE = 9;
const static uint8_t PIN_RADIO_CSN = 10;

// For the breakout board, you can use any 2 or 3 pins.
// These pins will also work for the 1.8" TFT shield.
#define TFT_CS 6
#define TFT_RST 7 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC 8

void testdrawtext(char *text, uint16_t color);

struct RadioPacketDebug // Any packet up to 32 bytes can be sent.
{
    uint8_t FromRadioId;
    uint32_t OnTimeMillis;
    uint32_t FailedTxCount;
};


struct RadioPacketJoystick
{
    int8_t vertical;
    int8_t horizontal;
};

NRFLite _radio;
RadioPacketJoystick _radioJoystick;
RadioPacketDebug _radioDebug;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup() {

	pinMode(A0, INPUT_PULLUP); // set pull-up on analog pin 0
	pinMode(A1, INPUT_PULLUP); // set pull-up on analog pin 0
	//! Table for CPINT correspondance: https://github.com/NicoHood/PinChangeInterrupt?utm_source=platformio&utm_medium=piohome
	PCICR |= _BV(PCIE1);
	PCMSK1 |= _BV(PCINT8);
	PCMSK1 |= _BV(PCINT9);

	// put your setup code here, to run once:
	Serial.begin(115200);
        
    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN)) {
        Serial.println("Cannot communicate with radio");
        while (1); // Wait here forever.
	}
	else {
		Serial.println("Init Radio OK");
	}

	// _radioDebug.FromRadioId = RADIO_ID;

	tft.initR(INITR_GREENTAB); // Init ST7735S chip, green tab
	Serial.println(F("Initialized"));

	uint16_t time = millis();
	tft.fillScreen(ST77XX_BLACK);
	time = millis() - time;
	Serial.println(time, DEC);
	// delay(500);

	tft.fillScreen(ST77XX_BLACK);
	testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", ST77XX_WHITE);
	// delay(1000);

	sei();
}

void loop() {
    _radioDebug.OnTimeMillis = millis();

	read_joystick(&left_joystick);
	read_joystick(&right_joystick);

	//! Single joystick mode
	_radioJoystick.vertical = (int8_t)left_joystick.vertical_axis.value;
	_radioJoystick.horizontal = (int8_t)left_joystick.horizontal_axis.value;

	//! Differential mode
	// _radioJoystick.vertical = (int8_t)((left_joystick.vertical_axis.value + right_joystick.vertical_axis.value) / 2);
	// _radioJoystick.horizontal = (int8_t)((right_joystick.vertical_axis.value - left_joystick.vertical_axis.value) * 2);

	Serial.print("V: ");
	Serial.print(_radioJoystick.vertical);
	Serial.print(",H:");
	Serial.println(_radioJoystick.horizontal);
	//Serial.print();

	Serial.print("Sending ");
	Serial.print(_radioDebug.OnTimeMillis);
	Serial.print(" ms");

	if (_radio.send(DESTINATION_RADIO_ID, &_radioJoystick, sizeof(_radioJoystick))) {
		Serial.print("...Success");
	}
	else {
		Serial.print("...Failed");
		_radioDebug.FailedTxCount++;
	}

	delay(100);

	/*
    By default, 'send' transmits data and waits for an acknowledgement.
    You can also send without requesting an acknowledgement.
    _radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData), NRFLite::NO_ACK)
    _radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData), NRFLite::REQUIRE_ACK) // THE DEFAULT
    */
}


void testdrawtext(char *text, uint16_t color) {
	tft.setCursor(0, 0);
	tft.setTextColor(color);
	tft.setTextWrap(true);
	tft.print(text);
}


//! handle pin change interrupt for A0 to A5 here
ISR(PCINT1_vect) {
	//! TODO Debounce
	if(digitalRead(A0) == LOW) {
		Serial.println(" - A0 Press");
	}
	else if(digitalRead(A1) == LOW) {
		Serial.println(" - A1 Press");
	}

	// Serial.println("INT");
}
