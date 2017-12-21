/*
 Name:		AdcRaduino.ino
 Created:	12/10/2017 6:26:55 PM
 Author:	pawel
*/

// the setup function runs once when you press reset or power the board
#include <SPI.h>
#include "Arduino-shared\ADS1232.h"
#include "Arduino-shared\EEPROMAnything.h"
#include "Arduino-shared\external\ADS1231\ADS1231.h"
#include <stdarg.h>
#include <string.h>


#define SPI_SCK     13
#define SPI_MOSI    11
#define SPI_MISO    12

struct config_t
{
	long offsetRawADC;
} config;

void p(char *fmt, ...);

//#define DISP_RESET   8
//#define DISP_CS     10
//#define DISP_DC      9
//
//#define SCALE_CS     7
//#define SCALE_PDWN   2
//#define SCALE_GAIN0  0
//#define SCALE_GAIN1  1
//#define SCALE_SPEED  3
//#define SCALE_A0     4
//#define SCALE_TEMP   5
//
//ADS1232 scale_adc(SCALE_CS, SPI_MISO, SCALE_PDWN, SCALE_GAIN0, SCALE_GAIN1, SCALE_SPEED, SCALE_A0, SCALE_TEMP);
//
//void setup() {
//	Serial.begin(115200);
//
//	/* Initialize SPI bus */
//	//SPI.cl (SPI_SCK);
//	//SPI.setMOSI(SPI_MOSI);
//	//SPI.setMISO(SPI_MISO);
//	SPI.begin();
//
//	scale_adc.init(ADS1232::GAIN128);
//	return;
//}
//
//// the loop function runs over and over again until power down or reset
//void loop() {
//	int32_t scaleval;
//
//	scaleval = scale_adc.read();
//
//	Serial.println(scaleval);
//
//	//delay(1000);
//
//	return;
//}

uint8_t CLK_PIN = 13;
uint8_t DATA_PIN = 12;
uint32_t counter = 0;

ADS1231 adc(CLK_PIN, DATA_PIN);

void setup() {
	Serial.begin(115200);
	int i = eepromReadStruct(0, config);
	p("Config i = %d; offset = %d\n", i, config.offsetRawADC);

	if (!adc.begin()) {
		Serial.println("Invalid pins bro!");
	}
	else
		delay(1000); // czas startu convertera
	
}

void loop() {
	long val;
	if (adc.getValue(val)) {        //this call blocks until a sample is ready!
		counter++;
		//Serial.print(counter);
		//Serial.print(": ");
		//Serial.println(val);
		// IN+MAX = 0x7FFFFF = 8388607
		// IN-MIN = 0x800000 = -8388608
		float d = 0.0;// val / 16777216.0 *39.0; //to uproszczenie wprowadza blad
		if (val > 0)
			d = val / 8388607.0 *19.5;
		else
			d = val / 8388608.0 * 19.5;
		//Serial.println(d, 10);  //23 bits of accuracy (24th is sign) with 4.2 volt (measured) AVDD. 2^23 = 8388607 !
		String s =  String(d, 7);
		p("%5d: ", counter);
		Serial.println(s + " uV");
	}
	else {
		Serial.println("Failed to get data");
	}
}


void p(char *fmt, ...) {
	char buf[128]; // resulting string limited to 128 chars
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, 128, fmt, args);
	va_end(args);
	Serial.print(buf);
}