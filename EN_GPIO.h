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

//Available output states
#define EN_OUTPUT_HIGH		(uint8_t)1
#define EN_OUTPUT_LOW		(uint8_t)0

void initRelayOutputs();
void initStatusLed();
void initInputs();

void checkRelayOutputs();
void checkStatusLED();

uint8_t readInput(uint8_t en_input);
void writeRelayOutput(uint8_t en_output, uint8_t val);
void writeLEDOutput(uint8_t red, uint8_t blue, uint8_t green);



#endif