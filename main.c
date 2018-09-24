/*
 * TimerTest.c
 *
 * Created: 9/24/2018 2:00:03 PM
 * Author : Cobus Hoffmann
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "EN_GPIO.h"

#define IN1_S0		0
#define IN1_S1		1
#define IN1_S2		2
#define IN1_S3		3
#define IN1_S4		4
#define IN1_S5		5
#define IN1_S6		6

#define IN2_S0		0
#define IN2_S1		1
#define IN2_S2		2

uint8_t OUT1_PIN = 3;
uint8_t OUT2_PIN = 4;

//Set up ticks for the different input modes
uint16_t tickIN1Count=0;
uint16_t tickIN2Count=0;

//Keep track if the timer is active
uint8_t tickIN1Active = 0;
uint8_t tickIN2Active = 0;


//Keep track if the timer is done
uint8_t tickIN1Done = 0;
uint8_t tickIN2Done = 0;

uint8_t in1LastState = IN1_S0;
uint8_t in2LastState = IN2_S0;

uint8_t in1CurrentState = IN1_S0;
uint8_t in2CurrentState = IN2_S0;

uint8_t temp = 0;



void initTimer(void);

//Start the timers for the respective inputs
void startIN1Timer(uint16_t timeSeconds);
void startIN2Timer(uint16_t timeSeconds);

//Handle the sub programs for individual pins
void handleSubProgA(uint8_t in1PinState);
void handleSubProgB(uint8_t in1PinState);

int main(void)
{

	initTimer();
	initRelayOutputs();
	//checkRelayOutputs();
	initStatusLed();
	//checkStatusLED();
	writeLEDOutput(0,0,0);
	
	in1CurrentState = IN1_S0;
	in2CurrentState = IN2_S0;
	sei();
	
	//Variables to store the input results
	uint8_t in1PinState = 0;
	uint8_t in2PinState = 0;		
	// loop forever
	while(1)
	{
		//BEGIN WHILE LOOP
		//Read the inputs
		
		in1PinState = readInput(EN_GPIO_INPUT_4);
		
		switch (in1CurrentState)
		{
			case IN1_S0:
				if(in1PinState == 0){
					in1LastState = in1CurrentState;
					in1CurrentState = IN1_S1;
					//Start 15sec timer
					startIN1Timer(5);//Wait for 30sec
					//writeLEDOutput(1, 0, 0); 
				}
				break;
				
			case IN1_S1:
				//gotta wait for 30sec
				//Check if timer is done
				if (tickIN1Done!=1)
				{
					break;
				}
				
				if(in1PinState == 0){
					//Pin still low move to next state
					writeLEDOutput(0,1,0); //Green
					in1LastState = in1CurrentState;
					in1CurrentState = IN1_S2;
					startIN1Timer(5);	
				}else{
					//Return to start
					in1LastState = in1CurrentState;
					in1CurrentState = IN1_S0;
				}
				break;
				
			case IN1_S2: 
				//Now in S2
				//Check timer done
				if(!tickIN1Done){
					break;
				}
				
				if(in1PinState == 0){
					writeLEDOutput(0,0,1);
					writeRelayOutput(EN_GPIO_OUTPUT_1, 1);
					writeRelayOutput(EN_GPIO_OUTPUT_2, 1);
					writeRelayOutput(EN_GPIO_OUTPUT_3, 1);
					in1CurrentState = IN1_S3;
					startIN1Timer(5);//Wait 15sec
				}else{
					in1CurrentState = IN1_S0;//Return to start
				}
			break;
			
			case IN1_S3:
				if(!tickIN1Done){
					break;
				}
				//Turn on latching relay
				_delay_ms(1000);
				//Turn off latching relay
				in1CurrentState = IN1_S4;
				startIN1Timer(5); //Wait 5sec
			break;
			
			case IN1_S4:
				if(!tickIN1Done){
					break;
				}
				//turn off the outputs 1&2
				writeRelayOutput(EN_GPIO_OUTPUT_1,0);
				writeRelayOutput(EN_GPIO_OUTPUT_2, 0);
				in1CurrentState = IN1_S5;
				startIN1Timer(10);//Wait 12min
			break;
			
			case IN1_S5:
				if(!tickIN1Done){
					break;
				}
				//Check input 1
				if(in1PinState==0){
					in1CurrentState = IN1_S2;
				}else{
					//Go to next state
					startIN1Timer(6*60);
					in1CurrentState = IN1_S0;
				}
				
			break;
			

		}
	
	_delay_ms(1);
	//END WHILE LOOP
	}
}


void initTimer(void){
	// set up timer with prescaler = 64 and CTC mode
	TCCR1B |= (1 << WGM12)|(1 << CS12)|(1 << CS10);
	
	// initialize counter
	TCNT1 = 0;
	
	// initialize compare value
	OCR1A = 1000;
	
	//Timer 2
	
	// enable compare interrupt
	TIMSK1 |= (1 << OCIE1A);
	
	// enable global interrupts
	sei();
}


//Timer ISR handler used for the second timer
ISR(TIMER1_COMPA_vect){
	
	if(tickIN1Active){
		//decrement the count
		if (tickIN1Count > 0){
			//Decrement the counter
			tickIN1Count --;
			
		}else{
			//The timer is done set done and deactivate the timer
			tickIN1Done = 1;
			tickIN1Active = 0;
		}
	}
	
}
void startIN1Timer(uint16_t timeSeconds){
	tickIN1Done=0;
	tickIN1Count = timeSeconds; //Set the time count
	tickIN1Active = 1; //Activate the timer
	
}
void startIN2Timer(uint16_t timeSeconds){
	
}