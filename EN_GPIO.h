#ifndef _EN_GPIO_
#define _EN_GPIO_
#include <avr/io.h>
#include <util/delay.h>


//Available Inputs
#define EN_GPIO_INPUT_1		(uint8_t)0
#define EN_GPIO_INPUT_2		(uint8_t)1
#define EN_GPIO_INPUT_3		(uint8_t)2
#define EN_GPIO_INPUT_4		(uint8_t)3

//Available Outputs
#define EN_GPIO_OUTPUT_1	(uint8_t)4
#define EN_GPIO_OUTPUT_2	(uint8_t)5
#define EN_GPIO_OUTPUT_3	(uint8_t)6
#define EN_GPIO_OUTPUT_4	(uint8_t)7
#define EN_GPIO_TOR_1		(uint8_t)8
#define EN_GPIO_TOR_2		(uint8_t)9

//Available output states
#define EN_OUTPUT_HIGH		(uint8_t)1
#define EN_OUTPUT_LOW		(uint8_t)0

//Available input states
#define EN_INPUT_HIGH		(uint8_t)0
#define EN_INPUT_LOW		(uint8_t)1

//Temperature Constants
#define EN_TEMP_HIGH		(uint16_t)195 //40C
#define EN_TEMP_MED			(uint16_t)184 //36C 
#define EN_TEMP_LOW			(uint16_t)180

void initRelayOutputs();
void initStatusLed();
void initInputs();
void initTempSensor();

void checkRelayOutputs();
void checkStatusLED();

uint8_t readInput(uint8_t en_input);

uint16_t readTempSens();

void writeRelayOutput(uint8_t en_output, uint8_t val);
void writeLEDOutput(uint8_t red, uint8_t blue, uint8_t green);



#endif