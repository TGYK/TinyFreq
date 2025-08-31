#include <Arduino.h>

#define T1_RST TCNT1 &= 0x00
#define T0_RST TCNT0 &= 0x00
#define T0_STP TCCR0B &= ((0<<CS00) | (0<<CS01) | (0<<CS02));
#define T0_STA TCCR0B |= (1<<CS02)

#define DIV_FACTOR 2

volatile bool OVF_OCCURRED = true;

ISR(INT0_vect){ //Rising input edge detected
  cli(); //Service watchdog
  asm("WDR");
  sei();
  if(OVF_OCCURRED){ //If starting up or T1 overflow happened, discard current count/sync
    T1_RST;
    OVF_OCCURRED = false;
    return;
  }
  T0_STA; //Start output timer if not running
  volatile uint8_t newQPW = TCNT1 / (DIV_FACTOR * 2);
  if(TCNT0 > (newQPW - 5)){ //If currently-running putput timer is close to new qpw val or above it, toggle pulse early/sync
    T0_RST; //Reset output timer
    TCCR0B |= (1<<FOC0A); //Force toggle. Bit self-resets? Datasheet ambiguous - FOC0A is a "Strobe bit"
  }
  OCR0B = newQPW; //Set new half-pulsewidth.
  T1_RST; //Reset input timer
  return;
}

ISR(TIM1_OVF_vect){ //Input pulse length overflow
  cli(); //Disable interrupts
  asm("WDR"); //Service watchdog
  OVF_OCCURRED = true;
  T0_STP; //Stop output timer
  T0_RST; //Reset output timer
  OCR0B = 0xFF; //Reset output half-pulsewidth
  PORTB &= ~(1<<PORTB1); //Set pin low
  sei(); //Enable interrupts
  return;
}

void setup() {
  //Setup clock
  CLKPR = (CLKPR | (1<<CLKPCE)) & ~((1<<CLKPS3) | (1<<CLKPS2) | (1<<CLKPS1) | (1<<CLKPS0)); //Enable prescaler change
  CLKPR = (CLKPR | (1<<CLKPS3)) & ~(1<<CLKPCE); // clock prescaler = /256
  
  //Setup watchdog 
  WDTCR |= ((1<<WDCE) | (1<<WDE) | (1<<WDP3) | (1<<WDP0)); //Enable watchdog, 8s timeout

  //Setup timers
  T0_STA; //Timer 0 prescaler = /256
  TCCR0A |= ((1<<COM0B0) | (1<<WGM01)); //Toggle OC0B0 on match, CTC/mode2.
  TCCR1 |= ((1<<CS10) | (1<<CS13)); //Timer 1 prescaler = /256
  TIMSK |= (1<<TOIE1); //Overflow int enable on Timer 1

  //Setup DDRB/PORTB
  DDRB = (DDRB | (1<<DDB1)) & ~((1<<DDB0) | (1<<DDB2) | (1<<DDB3) | (1<<DDB4) | (1<<DDB5)); //Unused pins PB0,PB3-5 input for pull-up/high state per datasheet, PB2 input for int0, PB1 output for OC0B0
  PORTB = (PORTB | (1<<PORTB0) | (1<<PORTB2) | (1<<PORTB3) | (1<<PORTB4) | (1<<PORTB5)) & ~(1<<PORTB1); //Pull-up enable, PB1 low

  //Setup interrupts
  MCUCR |= (1<<ISC00) | (1<<ISC01); //INT0 on rising edge
  GIMSK |= (1<<INT0); //Enable INT0 

  //Setup sleep mode
  MCUCR &= ~((1<<SM0) | (1<<SM1)); //Idle mode
  MCUCR |= (1<<SE); //Enable sleep

  //Setup power-reduction
  PRR |= (1<<PRUSI) | (1<<PRADC); //Disable USI and ADC
}

void loop() {
  asm("SLEEP"); //Enter idle sleep/wait for interrupt
}
