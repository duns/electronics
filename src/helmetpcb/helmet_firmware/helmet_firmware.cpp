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

#define LEDLEVEL3 255
#define LEDLEVEL2 180
#define LEDLEVEL1 90
#define LEDLEVEL0 0


#define LEDCOUNTER 10
#define LONGBUTTONPRESS 1000
#define DEBOUNCEMS 10
#include "Arduino.h"
void int0f();
void toggle_led(int pinno,volatile int * state);
void int0r();
void btnpress();
void setup();
void loop();
int led = 6;
int btn  = 2;
int light1 = 3;
int light2 = 4;
int laser = 7;


int led2 = 0;

//int btn2  = 3;

unsigned char pwm  = 0;
volatile int state=0;
volatile int laserstate=0;
volatile int lightstate=0;
volatile int command_pending=0;
unsigned long timepressed;
unsigned long timediff;
volatile int ledstate=0;
volatile int ledstate2=0;
int ledcnt=LEDCOUNTER;

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
void int0f()
{
//     int val;
 //   state = !state;
 detachInterrupt(0);
//  state=!digitalRead(btn2);
  timepressed=millis();
//   digitalWrite(led,HIGH);
   attachInterrupt(0, int0r, RISING);                     
   command_pending=1;
  
}

void toggle_led(int pinno,volatile int * state)
{
//       laserstate=!laserstate;
      if(!*state)
        digitalWrite(pinno,HIGH);
        else
          digitalWrite(pinno,LOW);
       *state=!*state;
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

void btnpress()
{
//     val=digitalRead(btn);
      if(timediff<LONGBUTTONPRESS)
      {
        lightstate=++lightstate%4;
        switch(lightstate)
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
      else
      {
      toggle_led(led,&ledstate);      
      toggle_led(laser,&laserstate);
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
  digitalWrite(led, LOW);   // turn the LED on (HIGH is the voltage level)
 digitalWrite(light1, LOW);   // turn the LED on (HIGH is the voltage level)
 digitalWrite(laser, LOW);   // turn the LED on (HIGH is the voltage level)

  pinMode(led, OUTPUT);     
  pinMode(led2, OUTPUT);     
  pinMode(laser, OUTPUT);     
  pinMode(btn, INPUT);      
  digitalWrite(led, ledstate);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(led2, ledstate2);   // turn the LED on (HIGH is the voltage level)
 digitalWrite(laser, LOW);   // turn the LED on (HIGH is the voltage level)

//   pinMode(light1, OUTPUT); 
  analogWrite(light1, LEDLEVEL0);
  analogWrite(light2, LEDLEVEL0);
  
// digitalWrite(light1, LOW);   // turn the LED on (HIGH is the voltage level)
 state=!digitalRead(btn);
//state=0;
 //int0();
 attachInterrupt(0, int0f, FALLING);
}

// the loop routine runs over and over again forever:
void loop() {
//    analogWrite(9, 60);
//  TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
//  TCCR2B = _BV(CS22);
//  OCR2A = 180;
//  OCR2B = 50;
  
//  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);               // wait for a second
      if(command_pending==2)
      {
         btnpress();
          command_pending=0;
         
      }
      if(command_pending==1&&millis()-timepressed>LONGBUTTONPRESS)
        int0r();
      toggle_led(led2,&ledstate2);
      
//  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
//  delay(500);               // wait for a second
}


