#include <Arduino.h>

#include <SPI.h>
#include <NRFLite.h>
#include "global.h"
#include "joystick.h"
#include "menu.h"
#include "remote_gui.h"
#include "storage.h"


#define BUTTON_DEBOUNCE_MS 200


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

// const static uint8_t RADIO_ID = 1;			   // Our radio's id.
// const static uint8_t DESTINATION_RADIO_ID = 2; // Id of the radio we will transmit to.
const static uint8_t PIN_RADIO_CE = 9;
const static uint8_t PIN_RADIO_CSN = 10;



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

user_storage_t storage = {
	.radio_rx_id = 1,
	.radio_tx_id = 3,
};


void enter_button(void);
void nrf_interrupt(void);
void nrf_new_radio_rx_id(uint16_t rx_id);
void nrf_new_radio_tx_id(uint16_t tx_id);


void setup() {
	cli();

	pinMode(A0, INPUT_PULLUP);
	pinMode(A1, INPUT_PULLUP);
	pinMode(A2, INPUT_PULLUP);
	pinMode(A3, INPUT_PULLUP);
	//! Table for CPINT correspondance: https://github.com/NicoHood/PinChangeInterrupt?utm_source=platformio&utm_medium=piohome
	PCICR |= _BV(PCIE1);
	PCMSK1 |= _BV(PCINT8);
	PCMSK1 |= _BV(PCINT9);
	PCMSK1 |= _BV(PCINT10);
	PCMSK1 |= _BV(PCINT11);
	pinMode(2, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(2), enter_button, FALLING);

	attachInterrupt(digitalPinToInterrupt(3), nrf_interrupt, FALLING);

	// put your setup code here, to run once:
	Serial.begin(115200);

	storage_init(&storage);
	Serial.print("STORE: ");
	Serial.print(storage.radio_rx_id);
	Serial.print(" / ");
	Serial.println(storage.radio_tx_id);

	if (!_radio.init(storage.radio_rx_id, PIN_RADIO_CE, PIN_RADIO_CSN)) {
        Serial.println("Cannot communicate with radio");
        while (1); // Wait here forever.
	}
	else {
		Serial.println("Init Radio OK");
	}

	// _radioDebug.FromRadioId = RADIO_ID;
	remote_gui_init(&storage);
	item_uint_set_callback(&menus[RADIO_RX_ID], nrf_new_radio_rx_id);
	item_uint_set_callback(&menus[RADIO_TX_ID], nrf_new_radio_tx_id);

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

	// Serial.print("V: ");
	// Serial.print(_radioJoystick.vertical);
	// Serial.print(",H:");
	// Serial.println(_radioJoystick.horizontal);
	//Serial.print();

	// Serial.print("Sending ");
	// Serial.print(_radioDebug.OnTimeMillis);
	// Serial.print(" ms");

	if (_radio.send(storage.radio_tx_id, &_radioJoystick, sizeof(_radioJoystick)), NRFLite::REQUIRE_ACK) {
		// Serial.print("...Success");
		asm("nop");
	}
	else {
		// Serial.print("...Failed");
		_radioDebug.FailedTxCount++;
	}

	delay(200);

	menu_draw_gui();

	/*
    By default, 'send' transmits data and waits for an acknowledgement.
    You can also send without requesting an acknowledgement.
    _radio.send(storage.radio_tx_id, &_radioData, sizeof(_radioData), NRFLite::NO_ACK)
    _radio.send(storage.radio_tx_id, &_radioData, sizeof(_radioData), NRFLite::REQUIRE_ACK) // THE DEFAULT
    */
}

volatile static uint16_t time_pressed = 0;
//! handle pin change interrupt for A0 to A4 here
ISR(PCINT1_vect) {
	if(time_pressed != 0 && (millis() - time_pressed) < BUTTON_DEBOUNCE_MS) { return; }
	time_pressed = 0;
	//! TODO edge detection
	if(digitalRead(A0) == LOW) {
		menu_navigate(LEFT);
		time_pressed = millis();
	}
	else if(digitalRead(A1) == LOW) {
		menu_navigate(RIGHT);
		time_pressed = millis();
	}
	if(digitalRead(A2) == LOW) {
		menu_navigate(DOWN);
		time_pressed = millis();
	}
	else if(digitalRead(A3) == LOW) {
		menu_navigate(UP);
		time_pressed = millis();
	}
}

void enter_button(void) {
	if(time_pressed != 0 && (millis() - time_pressed) < BUTTON_DEBOUNCE_MS) { return; }
	time_pressed = 0;
	menu_navigate(ENTER);
	time_pressed = millis();
}

void nrf_interrupt(void) {
	Serial.println("RNF Interrupt");
}

void nrf_new_radio_rx_id(uint16_t rx_id) {
	storage.radio_rx_id = (uint8_t) rx_id;
	Serial.print("RX ID: "); Serial.println(rx_id);
	//! TODO: Assign to radio
	storage_sync(&storage);
}

void nrf_new_radio_tx_id(uint16_t tx_id) {
	storage.radio_tx_id = (uint8_t) tx_id;
	Serial.print("TX ID: "); Serial.println(tx_id);
	//! TODO: Assign to radio
	storage_sync(&storage);
}
