#include <Arduino.h>

#include <SPI.h>
#include <RF24.h>
#include "global.h"
#include "joystick.h"
#include "menu.h"
#include "remote_gui.h"
#include "storage.h"


#define BUTTON_DEBOUNCE_MS 200


typedef struct {
	int8_t vertical;
	int8_t horizontal;
} RadioPacketJoystick_t;


static void enter_button(void);
static void nrf_new_radio_rx_id(uint16_t rx_id);
static void nrf_new_radio_tx_id(uint16_t tx_id);
static void sync_remote_enable(bool enable);


const static uint8_t PIN_RADIO_CE = 9;
const static uint8_t PIN_RADIO_CSN = 10;
const static uint8_t PIN_RADIO_IRQ = 3;

static joystick_t left_joystick = {
	.vertical_axis = { .pin = A4, .value = 0, .inverted = true },
	.horizontal_axis = { .pin = A5, .value = 0, .inverted = false },
	.output_range = { -100, 100},
	.dead_space = 10
};
static joystick_t right_joystick = {
	.vertical_axis = {.pin = A6, .value = 0, .inverted = true},
	.horizontal_axis = {.pin = A7, .value = 0, .inverted = true},
	.output_range = {-100, 100},
	.dead_space = 10
};

static RF24 radio(PIN_RADIO_CE, PIN_RADIO_CSN);
uint8_t address[6] = {"PROM0"};
static RadioPacketJoystick_t _radioJoystick;

volatile static uint16_t time_pressed = 0;
volatile static NAVIGATE_OPTIONS_t next_action = NAVIGATE_NONE;
static uint32_t _lastSendTime;
volatile static uint8_t last_radio_status;
volatile static bool is_sending = false;
volatile static bool need_redraw = false;

user_storage_t storage = {
	.radio_rx_id = 1,
	.radio_tx_id = 2,
	.radio_enable = 1,
};


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
	// attachInterrupt(digitalPinToInterrupt(PIN_RADIO_IRQ), nrf_interrupt, FALLING);

	Serial.begin(230400);

	storage_init(&storage);
	Serial.print("STORED: RX:");
	Serial.print(storage.radio_rx_id);
	Serial.print(" / TX:");
	Serial.println(storage.radio_tx_id);

	// initialize the transceiver on the SPI bus
	if (!radio.begin()) {
		Serial.println(F("radio hardware is not responding!!"));
		while (1);
	}
	else {
		Serial.println("Init Radio OK");
	}
	radio.setPALevel(RF24_PA_HIGH);     	// RF24_PA_MAX is default.
	radio.enableDynamicPayloads();
  	radio.setPayloadSize(sizeof(RadioPacketJoystick_t));
	// radio.enableAckPayload();
	address[4] = storage.radio_tx_id;
	radio.openWritingPipe(address);     	// always uses pipe 0 for tx
	address[4] = storage.radio_rx_id;
	radio.openReadingPipe(1, address);		// using pipe 1 for rx
	radio.stopListening();                  // put radio in TX mode

	remote_gui_init(&storage);
	item_uint_set_callback(&menus[RADIO_RX_ID], nrf_new_radio_rx_id);
	item_uint_set_callback(&menus[RADIO_TX_ID], nrf_new_radio_tx_id);
	item_checkbox_set_callback(&menus[MENU_REMOTE_ON], sync_remote_enable);

	sei();
}


uint32_t last_rx_packet;
void loop() {
	uint32_t now = millis();

	if (now - _lastSendTime > 50 && storage.radio_enable) {
		read_joystick(&left_joystick);
		read_joystick(&right_joystick);

		//! Differential mode
		// _radioJoystick.vertical = (int8_t)((left_joystick.vertical_axis.value + right_joystick.vertical_axis.value) / 2);
		// _radioJoystick.horizontal = (int8_t)((right_joystick.vertical_axis.value - left_joystick.vertical_axis.value) * 2);

		//! Single joystick mode
		_radioJoystick.vertical = (int8_t)left_joystick.vertical_axis.value;
		_radioJoystick.horizontal = (int8_t)left_joystick.horizontal_axis.value;

		// Serial.print("Sending "); Serial.print(_radioDebug.OnTimeMillis); Serial.print(" ms");
		// Serial.print(", V: "); Serial.print(_radioJoystick.vertical); Serial.print(",H:"); Serial.print(_radioJoystick.horizontal);
		// Serial.print(", to:"); Serial.print(storage.radio_tx_id);

		_lastSendTime = millis();

		is_sending = true;
		bool report = radio.write(&_radioJoystick, sizeof(_radioJoystick));    // transmit & save the report
		is_sending = false;

		if(!report) {
			menu_vled_set(VLED_NRF_TX, false);
			Serial.print("!");
		}
		else {
			menu_vled_set(VLED_NRF_TX, true);
			Serial.print("#");
			// uint8_t pipe;
			// if(radio.available(&pipe)) {
			// 	uint8_t size = radio.getDynamicPayloadSize();
			// 	uint8_t rx_buffer[32];
			// 	radio.read(&rx_buffer, size);
			// 	Serial.print("[RX=");
			// 	for(uint8_t i = 0; i < size; i++) {
			// 		Serial.print(rx_buffer[i], HEX);
			// 		Serial.print(":");
			// 	}
			// 	Serial.println("]");
	 	// 		menu_vled_set(VLED_NRF_RX, true);
			// }
			// else {
	 	// 		menu_vled_set(VLED_NRF_RX, false);
			// }
		}
	}
	if((now - last_rx_packet) > 500) {
		menu_vled_set(VLED_NRF_RX, false);
	}

	if(next_action != NAVIGATE_NONE) {
		menu_navigate(next_action);
		Serial.print("NAV:"); Serial.println(next_action);
		next_action = NAVIGATE_NONE;
	}

	if ((need_redraw == true) && (is_sending == false)) {
		Serial.print("#");
		need_redraw = false;
		menu_draw_gui();
	}

	// delay(10);
}

//! handle pin change interrupt for A0 to A4 here
ISR(PCINT1_vect) {
	if(time_pressed != 0 && (millis() - time_pressed) < BUTTON_DEBOUNCE_MS) { return; }
	time_pressed = 0;
	//! TODO edge detection
	if(digitalRead(A0) == LOW) {
		next_action = NAVIGATE_LEFT;
		time_pressed = millis();
		need_redraw = true;
	}
	else if(digitalRead(A1) == LOW) {
		next_action = NAVIGATE_RIGHT;
		time_pressed = millis();
		need_redraw = true;
	}
	if(digitalRead(A2) == LOW) {
		next_action = NAVIGATE_DOWN;
		time_pressed = millis();
		need_redraw = true;
	}
	else if(digitalRead(A3) == LOW) {
		next_action = NAVIGATE_UP;
		time_pressed = millis();
		need_redraw = true;
	}
}


static void enter_button(void) {
	if(time_pressed != 0 && (millis() - time_pressed) < BUTTON_DEBOUNCE_MS) { return; }
	time_pressed = 0;
	next_action = NAVIGATE_ENTER;
	time_pressed = millis();
	need_redraw = true;
}


static void nrf_new_radio_rx_id(uint16_t rx_id) {
	storage.radio_rx_id = (uint8_t) rx_id;
	Serial.print("RX ID: "); Serial.println(rx_id);
	//! TODO: Assign to radio
	storage_sync(&storage);
}


static void nrf_new_radio_tx_id(uint16_t tx_id) {
	storage.radio_tx_id = (uint8_t) tx_id;
	Serial.print("TX ID: "); Serial.println(tx_id);
	//! TODO: Assign to radio
	storage_sync(&storage);
}


static void sync_remote_enable(bool enable) {
	storage.radio_enable = enable;
	if(enable == false) { menu_vled_set(VLED_NRF_TX, false); }
	Serial.print("Remote enable: "); Serial.println(enable);
	storage_sync(&storage);
}
