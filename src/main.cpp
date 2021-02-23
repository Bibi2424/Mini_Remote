#include <Arduino.h>

#include <SPI.h>
#include <NRFLite.h>

#define TRUE 1
#define FALSE 0

int left_vertical_pin = A0;
int left_vertical_value = 0;
bool left_vertical_inversed = TRUE;
int left_horizontal_pin = A1;
int left_horizontal_value = 0;
bool left_horizontal_inversed = FALSE;

int right_vertical_pin = A2;
int right_vertical_value = 0;
bool right_vertical_inversed = TRUE;
int right_horizontal_pin = A3;
int right_horizontal_value = 0;
bool right_horizontal_inversed = TRUE;

const static uint8_t RADIO_ID = 1;			   // Our radio's id.
const static uint8_t DESTINATION_RADIO_ID = 2; // Id of the radio we will transmit to.
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

void setup() {
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
}

void loop() {
    _radioDebug.OnTimeMillis = millis();

    left_vertical_value = analogRead(left_vertical_pin);
    if(left_vertical_inversed) { left_vertical_value = 1024 - left_vertical_value; }
    left_horizontal_value = analogRead(left_horizontal_pin);
    if(left_horizontal_inversed) { left_horizontal_value = 1024 - left_horizontal_value; }

    right_vertical_value = analogRead(right_vertical_pin);
    if(right_vertical_inversed) { right_vertical_value = 1024 - right_vertical_value; }
    right_horizontal_value = analogRead(right_horizontal_pin);
    if(right_horizontal_inversed) { right_horizontal_value = 1024 - right_horizontal_value; }

	// Serial.print("Left V: ");
	// Serial.print(left_vertical_value);
	// Serial.print(",H:");
	// Serial.println(left_horizontal_value);

	// Serial.print("Right V: ");
	// Serial.print(right_vertical_value);
	// Serial.print(",H:");
	// Serial.println(right_horizontal_value);

    left_vertical_value = map(left_vertical_value, 0, 1024, -100, 100);
    if(left_vertical_value > 0 && left_vertical_value < 10) {
      left_vertical_value = 0;
    }
    else if(left_vertical_value < 0 && left_vertical_value > -10) {
      left_vertical_value = 0;
    }

    left_horizontal_value = map(left_horizontal_value, 0, 1024, -100, 100);
    if(left_horizontal_value > 0 && left_horizontal_value < 10) {
      left_horizontal_value = 0;
    }
    else if(left_horizontal_value < 0 && left_horizontal_value > -10) {
      left_horizontal_value = 0;
    }

    right_vertical_value = map(right_vertical_value, 0, 1024, -100, 100);
    if(right_vertical_value > 0 && right_vertical_value < 10) {
      right_vertical_value = 0;
    }
    else if(right_vertical_value < 0 && right_vertical_value > -10) {
      right_vertical_value = 0;
    }

    right_horizontal_value = map(right_horizontal_value, 0, 1024, -100, 100);
    if(right_horizontal_value > 0 && right_horizontal_value < 10) {
      right_horizontal_value = 0;
    }
    else if(right_horizontal_value < 0 && right_horizontal_value > -10) {
      right_horizontal_value = 0;
    }
    
    // verticalValue = verticalValue - 512;
    // float angle = ((float)(horizontalValue-512)/512.0*3.1416);
    // int sign = (horizontalValue-512)/abs((horizontalValue-512));

    // _radioJoystick.leftWheel = verticalValue + (int)((float)(verticalValue)*sin(angle));
    // _radioJoystick.rightWheel = verticalValue - (int)((float)(verticalValue)*sin(angle));

	_radioJoystick.vertical = left_vertical_value;
	_radioJoystick.horizontal = left_horizontal_value;

	Serial.print("V: ");
	Serial.print(_radioJoystick.vertical);
	Serial.print(",H:");
	Serial.println(_radioJoystick.horizontal);
	//Serial.print();

	Serial.print("Sending ");
	Serial.print(_radioDebug.OnTimeMillis);
	Serial.print(" ms");

	if (_radio.send(DESTINATION_RADIO_ID, &_radioJoystick, sizeof(_radioJoystick))) {
		Serial.println("...Success");
	}
	else {
		Serial.println("...Failed");
		_radioDebug.FailedTxCount++;
	}

	// delay(1000);
	delay(50);

	/*
    By default, 'send' transmits data and waits for an acknowledgement.
    You can also send without requesting an acknowledgement.
    _radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData), NRFLite::NO_ACK)
    _radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData), NRFLite::REQUIRE_ACK) // THE DEFAULT
    */
}
