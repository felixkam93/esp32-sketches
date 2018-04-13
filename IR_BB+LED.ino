
/*
 * IRremote: IRsendDemo - demonstrates sending IR codes with IRsend
 * An IR LED must be connected to Arduino PWM pin 3.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */


#include <IRremote.h>

#define PIN_IR 27
#define PIN_LIGHT_ONEP 25
#define PIN_LIGHT_TWOP 33

IRsend irsend(PIN_IR);

void setup()
{
      irsend.enableIROut(38);
      irsend.mark(0);

  pinMode(PIN_LIGHT_ONEP, OUTPUT);
  pinMode(PIN_LIGHT_TWOP, OUTPUT);
  
  digitalWrite(PIN_LIGHT_ONEP, true);
  digitalWrite(PIN_LIGHT_TWOP, true);
}

void loop() {


}
