/*
   attiny84
   0->PB1
   1->PB1
   2->PB2
   3->PA7
   4->PA6
   5->PA5
   6->PA4
   7->PA3
   8->PA2
   9->PA1
   10->PA0
   11?


 */

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
/*
#define LEDLEVEL0 255
#define LEDLEVEL1 180
#define LEDLEVEL2 90
#define LEDLEVEL3 0

 */
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
#define LEDLEVEL3 255
#define LEDLEVEL2 180
#define LEDLEVEL1 90
#define LEDLEVEL0 0
#define VBATTHRESH 9.2
#define VBATTHRESH_CRIT 6.9
#define SLEEPIDLESECS 4
#define BATCHECKSECS 10
#define BATCHECKCRITSECS 60

#define LASER_MAX_SECS 60
#define LIGHT_MAX_SECS 3600

#define LASER_MAX_ON LASER_MAX_SECS*8/10
#define LIGHT_MAX_ON LIGHT_MAX_SECS*8/10

#define LONGBUTTONPRESS 1000
#define DEBOUNCEMS 10
#include "Arduino.h"
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

void int0f();
void toggle_led(byte pinno,volatile byte * state);
void set_led(byte pinno,volatile byte * state,byte val);
void int0r();
void btnpress();
void setup();
void loop();
const byte led = 6;
const byte led2 = 0;
const byte btn  = 2;
const byte light1 = 3;
const byte light2 = 4;
const byte laser = 7;
const byte batadc= 9;
const float arefVolts = 5.0;

unsigned long timepressed;
unsigned long timediff;



float volts;
unsigned int lightcounter=0;
unsigned int lasercounter=0;

byte clocktick=0;
byte wdtcounter=0;
byte gotosleep=1;
byte sleepcounter=0;
byte checkbat=0;
byte dosystemtest=1;
//int btn2  = 3;

volatile byte laserstate=0;
volatile byte lightstate=0;
volatile byte command_pending=0;
volatile byte ledstate=0;
volatile byte ledstate2=0;

void initvars()
{
  clocktick=0;
  lightcounter=0;
  lasercounter=0;
  sleepcounter=SLEEPIDLESECS;
  dosystemtest=0;
  laserstate=0;
  lightstate=0;
  command_pending=0;
  ledstate=0;
  ledstate2=0;
}

#define BODS 7
#define BODSE 2
#if defined(BODS) && defined(BODSE)
 
 #define sleep_bod_disable() \
 do { \
   uint8_t tempreg; \
   __asm__ __volatile__("in %[tempreg], %[mcucr]" "\n\t" \
                        "ori %[tempreg], %[bods_bodse]" "\n\t" \
                        "out %[mcucr], %[tempreg]" "\n\t" \
                        "andi %[tempreg], %[not_bodse]" "\n\t" \
                        "out %[mcucr], %[tempreg]" \
                        : [tempreg] "=&d" (tempreg) \
                        : [mcucr] "I" _SFR_IO_ADDR(MCUCR), \
                          [bods_bodse] "i" (_BV(BODS) | _BV(BODSE)), \
                          [not_bodse] "i" (~_BV(BODSE))); \
 } while (0)
 
 #endif

/*
   void int0()
   {
//     int val;
//   state = !state;
detachInterrupt(0);
state=!digitalRead(btn);
if(state)
{
timepressed=millis();
digitalWrite(led,HIGH);
attachInterrupt(0, int0, RISING);                     
}
else
{
timediff=millis()-timepressed;
digitalWrite(led,LOW);
command_pending=1;
}

}
 */
  void switch_off()
{
  digitalWrite(laser,LOW);
  digitalWrite(led,LOW);
  digitalWrite(led2,LOW);
  laserstate=ledstate=ledstate2=0;
analogWrite(light2,LEDLEVEL0);
analogWrite(light1,LEDLEVEL0);   

  
}

void check_bat()
{
  unsigned int analog1;
 
//  int i;
  analog1=analogRead(1);
  volts=(analog1/1023.0)*arefVolts*((220+330)/330.0);
      

//  for(i=0;i<2*volts;i++)
//  {
//         delay(500);
 
//  }
//       	set_led(led2,&ledstate,0);
  
}

#define SYSTEMTESTLOOPS 10
 void systemtest()
{
  byte i;
  unsigned char lightlevel=255/SYSTEMTESTLOOPS;
  for(i=0;i<SYSTEMTESTLOOPS;i++)
  {
  		toggle_led(laser,&laserstate);
  		toggle_led(led,&ledstate);
  		toggle_led(led2,&ledstate2);
		analogWrite(light2,lightlevel);
		analogWrite(light1,lightlevel);
                 lightlevel+=255/SYSTEMTESTLOOPS;
                delay(1000);
  }
  
}

void int0f()
{
	//     int val;
	//   state = !state;
	//sleep_disable();
	detachInterrupt(0);
	//  state=!digitalRead(btn2);
	timepressed=millis();
	//   digitalWrite(led,HIGH);
	command_pending=1;
	attachInterrupt(0, int0r, RISING);                     

}

void toggle_led(byte pinno,volatile byte * state)
{
	//       laserstate=!laserstate;
	if(!*state)
		digitalWrite(pinno,HIGH);
	else
		digitalWrite(pinno,LOW);
	*state=!*state;
}
void set_led(byte pinno,volatile byte * state,byte val)
{
	//       laserstate=!laserstate;
	if(val)
		digitalWrite(pinno,HIGH);
	else
		digitalWrite(pinno,LOW);
	*state=val;
}


void go_sleep()
{
  cbi(ADCSRA,ADEN);  // switch Analog to Digitalconverter OFF
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        cli();
	sleep_enable();	
	sleep_bod_disable();
	sei();
	sleep_cpu();
	//sleep_mode();
	/* wake up here */
	sleep_disable();
        sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON
        initvars();
        setup();
}


//Prepare LED pin
//initialize watchdog
void WDT_Init(void)
{
	//disable interrupts
	cli();
	//reset watchdog
	wdt_reset();
	//set up WDT interrupt
	WDTCSR = (1<<WDCE)|(1<<WDE);
	//Start watchdog timer with 1s prescaller
	WDTCSR = (1<<WDIE)|(1<<WDE)|(1<<WDP2)|(1<<WDP1);

	wdtcounter=0;
        clocktick=0;
	//Enable global interrupts
	sei();
}
void WDT_Stop(void)
{
	//disable interrupts
	cli();
	//reset watchdog
	WDTCSR = (1<<WDCE)|(1<<WDE);
	//Stop watchdog
	WDTCSR = (1<<WDCE);
	sei();
}


//Watchdog timeout ISR
ISR(WDT_vect)
{
        WDTCSR = (1<<WDIE)|(1<<WDE)|(1<<WDP2)|(1<<WDP1);
        set_led(led,&ledstate,1);

        clocktick++;
}

void int0r()
{
	//     int val;
	//   state = !state;
	detachInterrupt(0);
	//  state=!digitalRead(btn2);
	//    if(state)
	//    {
	timediff=millis()-timepressed;
	//    digitalWrite(led,LOW);
	if(timediff>DEBOUNCEMS)
		command_pending=2;
 	attachInterrupt(0, int0f, FALLING);

}
void set_laser(byte lstate)
{
  set_led(laser,&laserstate,lstate);
  if(lstate)
    lasercounter=0;
}


void set_lights(byte lstate)
{
  	switch(lstate)
		{
			case 0:
				analogWrite(light2,LEDLEVEL0);
				analogWrite(light1,LEDLEVEL0);

				break;
			case 1:
				analogWrite(light2, LEDLEVEL1);
				analogWrite(light1, LEDLEVEL1);

				break;
			case 2:
				analogWrite(light2, LEDLEVEL2);
				analogWrite(light1, LEDLEVEL2);
				break;
			case 3:
				//        digitalWrite(light2, HIGH);
				//        digitalWrite(light1, HIGH);
				analogWrite(light2, LEDLEVEL3);
				analogWrite(light1, LEDLEVEL3);
				break;
				/*          case 4:
					    analogWrite(light2, LEDLEVEL3);
					    analogWrite(light1, LEDLEVEL1);
					    digitalWrite(light1,HIGH);
					    break;
				 */      }
}

void btnpress()
{
	//     val=digitalRead(btn);
	if(timediff<LONGBUTTONPRESS)
	{
		lightstate=++lightstate%4;
	         set_lights(lightstate);
                 if(!lightstate)
                         lightcounter=0;

	}
	else
	{
//		toggle_led(led,&ledstate);    
                if(laserstate==0)
                  laserstate=1;
                  else
                  laserstate=0;
//                laserstate=!laserstate;
                set_laser(laserstate);

	}
	//  digitalWrite(led,HIGH);

}

// 9 ->Light2
//10 LASER
//11 LED
//12 PWM light 1
// the setup routine runs once when you press reset:
void setup() {                
	// initialize the digital pin as an output.
		WDT_Stop();
	digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
	digitalWrite(led2, HIGH);   // turn the LED on (HIGH is the voltage level)
	digitalWrite(light1, LOW);   // turn the LED on (HIGH is the voltage level)
	digitalWrite(light2, LOW);   // turn the LED on (HIGH is the voltage level)
	digitalWrite(laser, LOW);   // turn the LED on (HIGH is the voltage level)

	pinMode(led, OUTPUT);     
	pinMode(led2, OUTPUT);     
	pinMode(laser, OUTPUT);     
	pinMode(btn, INPUT);      
	pinMode(batadc, INPUT);      

        //ADC prescaler
        sbi(ADCSRA,ADPS2);
        sbi(ADCSRA,ADPS1);
        sbi(ADCSRA,ADPS0);
	// digitalWrite(light1, LOW);   // turn the LED on (HIGH is the voltage level)
	if(dosystemtest&&!digitalRead(btn))
            systemtest();
            dosystemtest=0;
        switch_off();  
        sleepcounter=0;
        check_bat();
//        attachInterrupt(0, int0f, FALLING);
	//state=0;
	//int0();
//WDT_Init();
//	        toggle_led(led2,&ledstate2);

}

void int0()
{
  detachInterrupt(0);
}

// the loop routine runs over and over again forever:
void loop() {
  byte lclocktick=0;
 //       int j;
	//    analogWrite(9, 60);
	//  TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
	//  TCCR2B = _BV(CS22);
	//  OCR2A = 180;
	//  OCR2B = 50;

	//  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
	if(!(laserstate||lightstate||command_pending))
	{
              
            if(sleepcounter==0)
            {
                attachInterrupt(0, int0, LOW);

		WDT_Stop();
                set_led(led,&ledstate,0);
               set_led(led2,&ledstate2,0);
//                delay(100);
		go_sleep();
                int0f();
		WDT_Init();
            }
	}
        else
            sleepcounter=SLEEPIDLESECS;
        if(clocktick)
        {
          cli();
            lclocktick=clocktick;
            clocktick=0;
          sei();
       	set_led(led,&ledstate,0);

            wdtcounter+=lclocktick;
            sleepcounter--;
           if(!(wdtcounter%BATCHECKSECS))
               checkbat=1;
            if(laserstate)
            		lasercounter++;
            
             if(lightstate)
			lightcounter++;


        }

	if(command_pending==2)
	{
		btnpress();

		command_pending=0;
            

	}
	if(command_pending==1&&millis()-timepressed>LONGBUTTONPRESS)
		int0r();
//	toggle_led(led2,&ledstate2);

	if(laserstate&&(lasercounter>LASER_MAX_ON))
	{
		set_laser(0);
                laserstate=0;

	}
	if(lightstate&&(lightcounter>LIGHT_MAX_ON))
	{
              set_lights(0);
              lightstate=0;
	}

        if(checkbat)
        {
          check_bat();
          checkbat=0;
        }
        if(volts<VBATTHRESH)
        {
             if(lclocktick)
         	    toggle_led(led2,&ledstate2);   
        }
        else   
            set_led(led2,&ledstate2,0);      
        if(volts<VBATTHRESH_CRIT)
        {
          if(!(wdtcounter%BATCHECKCRITSECS))
          {
   //         for(j=0;j<3;j++)
   //         {
              set_lights(0);
              delay(100);
              set_lights(lightstate);
               delay(100);
   //         }
            
          }
          
        }            


	//delay(100);
	//  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
	//  delay(500);               // wait for a second
}

