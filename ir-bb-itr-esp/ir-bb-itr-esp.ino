#include <IRremote.h>

#define PIN_IR 27

#define PIN_LIGHT_ONEP 5
#define PIN_LIGHT_TWOP 6
#define PIN_DETECT 2
#define PIN_DETECT_OUTER 3
#define PIN_STATUS 13
#define PIN_STATUS_OUTER 8

enum IR_BEAMS {NONE, INNER, OUTER };
int state_inner;
int state_outer;


volatile boolean itr_on_inner;
volatile boolean itr_on_outer;

int room_occupation = 0;



IR_BEAMS last_broken = NONE;
IR_BEAMS first_restored = NONE;



IRsend irsend(PIN_IR);

void setup() {
  pinMode(PIN_DETECT, INPUT_PULLUP);
  pinMode(PIN_DETECT_OUTER, INPUT_PULLUP);
  pinMode(PIN_STATUS, OUTPUT);
  pinMode(PIN_STATUS_OUTER, OUTPUT);
  pinMode(PIN_LIGHT_ONEP, OUTPUT);
  pinMode(PIN_LIGHT_TWOP, OUTPUT);


  
  digitalWrite(PIN_LIGHT_ONEP, false);
  digitalWrite(PIN_LIGHT_TWOP, false);
 
  irsend.enableIROut(38);
  irsend.mark(0);
   delay(500);
  attachInterrupt(digitalPinToInterrupt(PIN_DETECT), itr_inner, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_DETECT_OUTER), itr_outer, CHANGE);

    //not broken beams -> schmitt trigger gives us HIGH
  state_inner = HIGH;
  state_outer = HIGH;
  itr_on_inner = false;
  itr_on_outer = false;
  
  Serial.begin(115200);
  Serial.println("READY");
  delay(500);
}

void loop() {

  digitalWrite(PIN_STATUS, digitalRead(PIN_DETECT));
  digitalWrite(PIN_STATUS_OUTER, digitalRead(PIN_DETECT_OUTER));

  checkBreak(itr_on_inner, state_inner, INNER);
  checkRestoration(itr_on_inner, state_inner, INNER);
  checkBreak(itr_on_outer, state_outer, OUTER);
  checkRestoration(itr_on_outer, state_outer, OUTER);
  
  if(digitalRead(PIN_DETECT) == HIGH && digitalRead(PIN_DETECT_OUTER) == HIGH){

          if(last_broken == INNER && first_restored == OUTER ){
            room_occupation++;
            first_restored = NONE;
            last_broken = NONE;
            Serial.println(room_occupation);  
          }else if(last_broken == OUTER && first_restored == INNER){
            room_occupation--;
            first_restored = NONE;
            last_broken = NONE;
            Serial.println(room_occupation);
          }else if((first_restored == OUTER && last_broken == OUTER) || (first_restored == INNER && last_broken == INNER)){
            Serial.println("Resetting first_restored to NONE after false movement and restored beams ");
            first_restored = NONE;
            last_broken = NONE;
           // state_inner = LOW;
           // state_outer= LOW;
        }
      
   }
  if(room_occupation >= 2){
    digitalWrite(PIN_LIGHT_ONEP, HIGH);
    digitalWrite(PIN_LIGHT_TWOP, HIGH);
  }else if(room_occupation == 1){
    digitalWrite(PIN_LIGHT_ONEP, HIGH);
    digitalWrite(PIN_LIGHT_TWOP, LOW);
  }else if(room_occupation < 1){
    digitalWrite(PIN_LIGHT_ONEP, LOW);
  }

  
}

void itr_inner(){
  itr_on_inner = true;
  state_inner = digitalRead(PIN_DETECT);
 // Serial.println("Interrupt occured on INNER");
 // Serial.println(state_inner);
  
}

void itr_outer(){
  itr_on_outer = true;
  state_outer = digitalRead(PIN_DETECT_OUTER);
 // Serial.println("Interrupt occured on OUTER");
 // Serial.println(state_outer);
}

void checkBreak(boolean wasInterrupted, int &state, IR_BEAMS location ){
  if(wasInterrupted && state == LOW){
      Serial.println("Setting last_broken to");
      Serial.println(location);
      last_broken = location;
      if(location == INNER){
        itr_on_inner = false;
      }else {
        itr_on_outer = false;
      }
  }
}

void checkRestoration(boolean wasInterrupted, int &state, IR_BEAMS location ){
  if(wasInterrupted && state == HIGH){
      Serial.println("Restored Beam detected on:");
      Serial.println(location);
      if(first_restored == NONE){
        Serial.println("Setting first_restored to INNER");
        first_restored = location;
      }
      if(location == INNER){
        itr_on_inner = false;
      }else {
        itr_on_outer = false;
      }
  }
}











