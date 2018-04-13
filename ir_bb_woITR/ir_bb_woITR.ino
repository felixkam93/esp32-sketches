#include <IRremote.h>

#define PIN_IR 27

#define PIN_LIGHT_ONEP 25 //5
#define PIN_LIGHT_TWOP 26 //6
#define PIN_DETECT 35 //2
#define PIN_DETECT_OUTER 34 //3
#define PIN_STATUS 5 //13
#define PIN_STATUS_OUTER 33 //8
//reduces the rate to read the sensors to every 200 ms
#define DELAY 100

enum IR_BEAMS {NONE, INNER, OUTER };
int state_inner;
int state_outer;
int state_inner_old;
int state_outer_old;
volatile boolean itr_on_inner;
volatile boolean itr_on_outer;

unsigned long last_read_inner;
unsigned long last_read_outer;

int room_occupation = 1;



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

  last_read_inner = millis();
  last_read_outer = millis();
  
  digitalWrite(PIN_LIGHT_ONEP, false);
  digitalWrite(PIN_LIGHT_TWOP, false);
 
  irsend.enableIROut(38);
  irsend.mark(0);
  delay(500);
   
  //not broken beams -> schmitt trigger gives us HIGH
  state_inner = HIGH;
  state_outer = HIGH;
  state_inner_old = HIGH;
  state_outer_old = HIGH;
  itr_on_inner = false;
  itr_on_outer = false;
  
  Serial.begin(9600);
  Serial.println("READY");
  delay(500);
}

void loop() {

  //digitalWrite(PIN_STATUS, digitalRead(PIN_DETECT));
  //digitalWrite(PIN_STATUS_OUTER, digitalRead(PIN_DETECT_OUTER));

  if(millis() - DELAY > last_read_inner ){
      digitalWrite(PIN_STATUS, !digitalRead(PIN_DETECT));
      state_inner = digitalRead(PIN_DETECT);
      last_read_inner = millis();
  }


  if(millis() - DELAY > last_read_outer ){
    digitalWrite(PIN_STATUS_OUTER, digitalRead(PIN_DETECT_OUTER));
    state_outer = digitalRead(PIN_DETECT_OUTER);
    last_read_outer = millis();
  }

  if(state_inner_old == HIGH && state_inner == LOW){
    Serial.println("Inner switched from HIGH -> LOW, Beam was interrupted");
    last_broken = INNER;
    state_inner_old = state_inner;
  }else if(state_inner_old == LOW && state_inner == HIGH){
    Serial.println("inner switched from LOW -> HIGH, Beam was restored");
    if(first_restored == NONE){
      Serial.println("changed first restored to inner");
      Serial.println(INNER);
      first_restored = INNER;
    }
    state_inner_old = state_inner;
  }

  if(state_outer_old == HIGH && state_outer == LOW){
    Serial.println("outer switched from HIGH -> LOW, Beam was interrupted");
    last_broken = OUTER;
    state_outer_old = state_outer;
  }else if(state_outer_old == LOW && state_outer == HIGH){
    Serial.println("outer switched from LOW -> HIGH, Beam was restored");
    if(first_restored == NONE){
      Serial.println("changed first restored to outer");
      Serial.println(OUTER);
      first_restored = OUTER;
    }
    state_outer_old = state_outer;
  }

  //checkStateChange(INNER, digitalRead(PIN_DETECT), state_inner);
  //checkStateChange(OUTER, digitalRead(PIN_DETECT_OUTER), state_outer);

 if(state_inner == HIGH && state_outer == HIGH){
      
          if(last_broken == INNER && first_restored == OUTER ){
            room_occupation++;
            first_restored = NONE;
            last_broken = NONE;
           Serial.println("Room occ");
            Serial.println(room_occupation);  
          }else if(last_broken == OUTER && first_restored == INNER){
            room_occupation--;
            first_restored = NONE;
            last_broken = NONE;
            Serial.println("Room occ");
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



void checkStateChange(IR_BEAMS location, int new_state, int &old_state){

    if(old_state != new_state){
      if(new_state == HIGH ){ //restoration occured, only set when value is default
        Serial.println("State changed to HIGH at:");
        Serial.println(location);
        if(first_restored == NONE){
          Serial.println("changed first restored to:");
          Serial.println(location);
          first_restored = location;
        }

        
        
      }else { //new_state is low -> break occured
        Serial.println("changed last broken to:");
        Serial.println(location);
       last_broken = location;
      }
       Serial.println("leaving state change method");
    //return new_state so old_state can be updated with this value
    old_state = new_state;
  }
}











