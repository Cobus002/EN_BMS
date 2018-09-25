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
#define IN2_S3		3
#define IN2_S4		4
#define IN2_S5		5

#define TEMP_S0		0
#define TEMP_S1		1
#define TEMP_S2		2
#define TEMP_S3		3
#define TEMP_S4		4
#define TEMP_S5		5

uint8_t OUT1_PIN = 3;
uint8_t OUT2_PIN = 4;

//Set up ticks for the different input modes
uint16_t tickIN1Count=0;
uint16_t tickIN2Count=0;
uint16_t tickTemp1Count=0;
uint16_t tickTemp2Count=0;

//Keep track if the timer is active
uint8_t tickIN1Active=0;
uint8_t tickIN2Active=0;
uint8_t tickTemp1Active=0;
uint8_t tickTemp2Active=0;

//Keep track if the timer is done
uint8_t tickIN1Done = 0;
uint8_t tickIN2Done = 0;
uint8_t tickTemp1Done = 0;
uint8_t tickTemp2Done = 0;

uint8_t in1LastState = IN1_S0;
uint8_t in2LastState = IN2_S0;

uint8_t in1CurrentState = IN1_S0;
uint8_t in2CurrentState = IN2_S0;
uint8_t temp1CurrentState = TEMP_S0;
uint8_t temp2CurrentState = TEMP_S0;

uint8_t tempCount = 0;
uint16_t tempVal = 0;



void initTimer(void);

//Start the timers for the respective inputs
void startIN1Timer(uint16_t timeSeconds);
void startIN2Timer(uint16_t timeSeconds);
void startTempTimer1(uint16_t timeSeconds);
void startTempTimer2(uint16_t timeSeconds);

//Handle the sub programs for individual pins
void handleSubProgA(uint8_t in1PinState);
void handleSubProgB(uint8_t in1PinState);
void handleSubProgC(uint16_t temperature);

int main(void)
{

	initTimer();
	initRelayOutputs();
	//checkRelayOutputs();
	initStatusLed();
	//start the temperature reading
	initTempSensor();
	
	//checkStatusLED();
	writeLEDOutput(0,0,0);
	
	in1CurrentState = IN1_S0;
	in2CurrentState = IN2_S0;
	sei();
	
	//Variables to store the input results
	uint8_t in1PinState = 0;
	uint8_t in2PinState = 0;
	
	//Make sure the timers are in the inactive state
	tickIN1Active = 0;
	tickIN1Count = 0;
	tickIN1Done = 1;
	
	tickIN2Active = 0;
	tickIN2Count = 0;
	tickIN2Done = 1;		
	// loop forever
	while(1)
	{
		//BEGIN WHILE LOOP
		//Read the inputs
		in1PinState = readInput(EN_GPIO_INPUT_4);
		in2PinState = readInput(EN_GPIO_INPUT_3);
		tempVal = readTempSens();
		//handleSubProgA(in1PinState);
		//handleSubProgB(in2PinState);
		
		//Read the temperature
		
		
		if(tempVal>184){
			writeLEDOutput(0,0,1);
		}else if(tempVal>150){
			writeLEDOutput(1,0,0);
		}else if(tempVal>100){
			writeLEDOutput(0,1,0);
		}else if(tempVal == 0){
			writeLEDOutput(0,0,0);
		}
		_delay_ms(1000);
		
		
		
		
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
	cli();//Clear interrupt so that we do not get interrupted
	
	//tickIN1 stuff
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
	
	//tickIN2 stuff
	if(tickIN2Active){
		if(tickIN2Count > 0){
			tickIN2Count --;
		}else{
			tickIN2Done = 1;
			tickIN2Active = 0;
		}
	}
	
	sei();
	
}


void startIN1Timer(uint16_t timeSeconds){
	tickIN1Done=0;
	tickIN1Count = timeSeconds; //Set the time count
	tickIN1Active = 1; //Activate the timer
	
}
void startIN2Timer(uint16_t timeSeconds){
	tickIN2Done=0;
	tickIN2Count = timeSeconds;
	tickIN2Active = 1;
	
}

void startTempTimer1(uint16_t timeSeconds){
	tickTemp1Done=0;
	tickTemp1Count = timeSeconds;
	tickTemp1Active=1;
}

void startTempTimer2(uint16_t timeSeconds){
	tickTemp2Done=0;
	tickTemp2Count=timeSeconds;
	tickTemp2Active=1;
}

void handleSubProgA(uint8_t in1PinState){
	switch (in1CurrentState)
	{
		case IN1_S0:
		//Needs to wait for timer
		if (!tickIN1Done)
		{
			break;
		}
		//Make sure that relay 3 is off when returning from IN1_S5
		writeRelayOutput(EN_GPIO_OUTPUT_3, 0);
		if(in1PinState == EN_INPUT_LOW){
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
		
		if(in1PinState == EN_INPUT_LOW){
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
		
		if(in1PinState == EN_INPUT_LOW){
			writeLEDOutput(0,0,1);
			//Turn on TOR and Outputs 1-3
			writeRelayOutput(EN_GPIO_TOR_1, 1);
			writeRelayOutput(EN_GPIO_TOR_2, 1);
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
		//turn off TOR & the Outputs 1&2
		writeRelayOutput(EN_GPIO_TOR_1, 0);
		writeRelayOutput(EN_GPIO_TOR_2, 0);
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
		if(in1PinState==EN_INPUT_LOW){
			in1CurrentState = IN1_S2;
			}else{
			//Go to next state
			in1CurrentState = IN1_S0;
			startIN1Timer(6*60);
		}
		break;
	}
}


void handleSubProgB(uint8_t in2PinState){
	switch (in2CurrentState)
	{
	case IN2_S0:
		//Wait for timer to stop
		if(!tickIN2Done){
			break;
		}
		
		if(in2PinState==EN_INPUT_LOW){
			in2CurrentState = IN2_S1;
			startIN2Timer(5); //Start 15sec timer
		}
		break;
		
	case IN2_S1:
		if(!tickIN2Done){
			break;
		}
		if(in2PinState==EN_INPUT_LOW){
			in2CurrentState = IN2_S2;
			startIN2Timer(5);//Wait 15 sec
		}else{
			in2CurrentState = IN2_S0;
			startIN2Timer(2);//Wait to second before going back
		}
		break;
	
	case IN2_S2:
		if(!tickIN2Done){
			break;
		}
		//Turn on TOR1&2
		writeRelayOutput(EN_GPIO_TOR_1, 1);
		writeRelayOutput(EN_GPIO_TOR_2, 1);
		//Turn on Outputs 1-2
		writeRelayOutput(EN_GPIO_OUTPUT_1, 1);
		writeRelayOutput(EN_GPIO_OUTPUT_2, 1);
		
		in2CurrentState = IN2_S3;
		startIN2Timer(5);//Wait 10min
		
		break;
	
	case IN2_S3:
		if(!tickIN2Done){
			break;
		}
		if(in2PinState == EN_INPUT_LOW){
			in2CurrentState = IN2_S2;
			startIN2Timer(2); //small delay to go back
		}else{
			//Turn TOR & Outputs 1&2 off
			writeRelayOutput(EN_GPIO_TOR_1, 0);
			writeRelayOutput(EN_GPIO_TOR_2, 0);
			writeRelayOutput(EN_GPIO_OUTPUT_1, 0);
			writeRelayOutput(EN_GPIO_OUTPUT_2, 0);
			in2CurrentState = IN2_S0; //Go back to the 
			startIN2Timer(2);
		}
		break;
	
	}
	
}

/*
 *	Related to the temperature stuff
 */
void handleSubProgC(uint16_t temperature){
	switch(temp1CurrentState){
		case TEMP_S0:
			if (temperature>EN_TEMP_LOW)
			{
				temp1CurrentState = TEMP_S1;
				startTempTimer1(15);//Wait 15sec
			}
			break;
		case TEMP_S1:
			if(temperature>EN_TEMP_LOW){
				//still above low temp
				//turn on output 4
				writeRelayOutput(EN_GPIO_OUTPUT_4, 1);
				temp1CurrentState = TEMP_S2;
				startTempTimer1(60); //Wait 1min
			}
			break;
			
		case TEMP_S2:
			if(temperature>EN_TEMP_LOW){
				//Keep the fan on
				temp1CurrentState = TEMP_S1;
				startTempTimer1(2);
			}else{
				//Temp is low enough to turn fan off
				writeRelayOutput(EN_GPIO_OUTPUT_4, 0);
				temp1CurrentState = TEMP_S0;
				startTempTimer1(2);
			}
			break;
			
	}
}