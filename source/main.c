/*	Author: Albert Dang adang018
 *      Partner(s) Name: Min-Hua Wu
 *	Lab Section: 022
 *	Assignment: Lab #7  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
    TCCR1B = 0x0B;
    OCR1A = 125;
    TIMSK1 = 0x02;
    TCNT1 = 0;
    _avr_timer_cntcurr = _avr_timer_M;
    SREG |= 0x80;
}

void TimerOff() {
    TCCR1B = 0x00;
}

void TimerISR() {
    TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
    _avr_timer_cntcurr--;
    if(_avr_timer_cntcurr == 0) {
        TimerISR();
        _avr_timer_cntcurr = _avr_timer_M;
    }
}

void TimerSet(unsigned long M) {
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

enum States {Init, Wait, Inc, Dec, Zero} state;
unsigned char incButton;
unsigned char decButton;
unsigned char counter;

void Tick();

int main(void) {
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
  
    
    state = Init;
    counter = 0;
   
    LCD_init();
    //LCD_Cursor(1);
    //LCD_WriteCommand(counter + '0');
    const unsigned char* hw = "Hello World";
    LCD_DisplayString(1, hw);
    TimerSet(1000);
    TimerOn();    
 
    while(1) {
        //incButton = (~PINA & 0x01);
        //decButton = (~PINA & 0x02);
        //Tick();
        while(!TimerFlag);
        TimerFlag = 0;
    }
    
    return 1;
}

void Tick() {
    switch(state) {
        case Init:
            state = Wait;
            break;
        case Wait:
            if(!incButton && !decButton) {
                state = Wait;
            } else if(incButton && !decButton) {
                state = Inc;
            } else if(!incButton && decButton) {
                state = Dec;
            } else if(incButton && decButton) {
                state = Zero;
            }
            break;
        case Inc:
            if(!incButton) {
                state = Wait;
            } else if(incButton && !decButton) {
                state = Inc;
            } else if(incButton && decButton) {
                state = Zero;
            }
            break;
        case Dec:
            if(!decButton) {
                state = Wait;
            } else if(!incButton && decButton) {
                state = Dec;
            } else if(incButton && decButton) {
                state = Zero;
            }
            break;
        case Zero:
            state = Wait;
            break;
        default:
            state = Init;
            break;
    }
    
    switch(state) {
        case Init:
            counter = 0;
            break;
        case Wait:
            break;
        case Inc:
            if(counter < 9) {
                counter++;
            }
            break;
        case Dec:
            if(counter > 0) {
                counter--;
            }
            break;
        case Zero:
            counter = 0;
            break;
        default:
            counter = 0;
            break;
    }
    LCD_Cursor(1);
    LCD_WriteCommand(counter + '0');   
}
