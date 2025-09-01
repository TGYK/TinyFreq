#include <Arduino.h>

#define T1_RST TCNT1 &= 0x00
#define T1_STP TCCR1 &= ~((1<<CS10) | (1<<CS11) | (1<<CS12) | (1<<CS13))
#define T1_STA TCCR1 |= ((1<<CS10) | (1<<CS13))
#define T0_RST TCNT0 &= 0x00
#define T0_STP TCCR0B &= ~((1<<CS00) | (1<<CS01) | (1<<CS02))
#define T0_STA TCCR0B |= (1<<CS02)
#define INT0_DS GIMSK &= ~(1<<INT0)
#define INT0_EN GIMSK |= (1<<INT0)

#define FREQ_MULT 2

volatile bool OVF_OCCURRED = true;

ISR(INT0_vect){ //Rising input edge detected
  //Service timer 1 and watchdog, get timer 1 running again ASAP
  T1_STP; //Stop T1 counter
  asm("WDR"); //Reset watchdog timer
  volatile uint8_t togglePW = TCNT1 / (FREQ_MULT * 2); //Read T1 counter, calculate new half PW.. Calculation step may need to be seperate if it takes too long 
  T1_RST; //Reset T1 counter
  T1_STA; //Restart T1 counter

  //OVF flag check - Make sure to measure a full rise-to-rise pulse and not the remainder following a T1 overflow, or a half-pulse during startup.
  if(OVF_OCCURRED){ //If starting up or T1 overflow happened, discard current count/sync
    OVF_OCCURRED = false; //Reset flag
    return; //End interrupt
  }

  //Edge-case - If timer is close to compare match, or is past the new compare match value, trigger an early compare match to toggle output and start a new pulse
  if(TCNT0 > (togglePW - 5)){ //If currently-running putput timer is close to new qpw val or above it, toggle pulse early/sync
    T0_RST; //Reset output timer
    TCCR0B |= (1<<FOC0A); //Force toggle. Bit self-resets? Datasheet ambiguous - FOC0A is a "Strobe bit"
  }

  //Set new compare match for output timer
  OCR0B = togglePW; //Set new half-pulsewidth
  T0_STA; //Start output timer if not already running
  return; //End interrupt
}

ISR(TIM1_OVF_vect){ //Input pulse length overflow
  INT0_DS; //Disable input interrupt
  asm("WDR"); //Reset watchdog timer
  T0_STP; //Stop output timer
  OVF_OCCURRED = true; //Set OVF flag
  T0_RST; //Reset output timer
  PORTB &= ~(1<<PORTB1); //Set output low
  INT0_EN; //Enable input interrupt
  return; //End interrupt
}

void setup() {
  cli(); //Disable global interrupts during setup
  
  //Setup clock
  CLKPR = (CLKPR | (1<<CLKPCE)) & ~((1<<CLKPS3) | (1<<CLKPS2) | (1<<CLKPS1) | (1<<CLKPS0)); //Enable prescaler change
  CLKPR = (CLKPR | (1<<CLKPS3)) & ~(1<<CLKPCE); // clock prescaler = /256
  
  //Setup watchdog 
  WDTCR |= ((1<<WDCE) | (1<<WDE) | (1<<WDP3) | (1<<WDP0)); //Enable watchdog, 8s timeout

  //Setup timers
  TCCR0A |= ((1<<COM0B0) | (1<<WGM01)); //Toggle OC0B0 on match, CTC/mode2
  TIMSK |= (1<<TOIE1); //Overflow int enable on Timer 1

  //Setup DDRB/PORTB
  DDRB = (DDRB | (1<<DDB1)) & ~((1<<DDB0) | (1<<DDB2) | (1<<DDB3) | (1<<DDB4) | (1<<DDB5)); //Unused pins PB0,PB3-5 input for pull-up/high state per datasheet, PB2 input for int0, PB1 output for OC0B0
  PORTB = (PORTB | (1<<PORTB0) | (1<<PORTB2) | (1<<PORTB3) | (1<<PORTB4) | (1<<PORTB5)) & ~(1<<PORTB1); //Pull-up enable, PB1 low

  //Setup interrupts
  MCUCR |= (1<<ISC00) | (1<<ISC01); //INT0 on rising edge
  INT0_EN; //Enable INT0 

  //Setup sleep mode
  MCUCR &= ~((1<<SM0) | (1<<SM1)); //Idle mode
  MCUCR |= (1<<SE); //Enable sleep

  //Setup power-reduction
  ACSR |= (1<<ACD); //Disable analog comparator
  ADCSRA &= ~(1<<ADEN); //Disable ADC
  PRR |= ((1<<PRUSI) | (1<<PRADC)); //Disable USI and ADC clocks

  sei(); //Enable global interrupts
}

void loop() {
  asm("SLEEP"); //Enter idle sleep/wait for interrupt - May be detrimental due to wakeup time (4 cycles + SUT fuse setting)
}
