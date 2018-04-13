#include <IRremote.h>

#define PIN_IR 27
#define PIN_DETECT 25
#define LED_BUILTIN 1

IRsend irsend(PIN_IR);

void setup() {


  pinMode(PIN_DETECT, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  irsend.enableIROut(38);
  irsend.mark(0);
   delay(500);
}
void loop(){

  digitalWrite(LED_BUILTIN,digitalRead(PIN_DETECT));


  
}

