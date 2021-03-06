//---CONFIG_BEGIN---
#define EI_ARDUINO_INTERRUPTED_PIN
#pragma GCC push_options
#pragma GCC optimize ("Os")

#include "Adafruit_VL53L0X.h"
#include <EnableInterrupt.h>
Adafruit_VL53L0X lox = Adafruit_VL53L0X(); // for the TOF LIDAR
#include <SPI.h>
#include <Servo.h> // For the servos

// For TOF/LIDAR
VL53L0X_RangingMeasurementData_t measure;
// need an array to keep track of the music notes
volatile int notes[41] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
// Need to keep track of state
volatile int state = -1; // -1 - Soft reset (return to home), 0 - Reset and erase recording (return to home), 1 - Play Live, 2 - Record, 3 - Play Recording
volatile int toggleRecord = 1; // 0 means record now. 1 means pause the recording / STOP
volatile int togglePlayRecord = 1; // 0 means play recording now. 1 means pause the recording being played now.
volatile unsigned long time; // used for reset functionality
volatile bool needHome = false;
int speaker = 3; // PWM pin 3
int greenLED = 0;
int redLED = 1;
Servo colOne;
int colOneCurr = 0; // current position of 1
Servo colTwo;
int colTwoCurr = 0; // current position of 2
Servo colThree;
int colThreeCurr = 0; // current position of 3
Servo colFour;
int colFourCurr = 0; // current position of 4
bool play = false;

void reset() {
  if(arduinoPinState <= 0){
    // then we know that we pressed the button
    // start the timer
    time = millis();
  }else{
    // then we released the button.
    // check the time.
    if((millis() - time) >= 2800){
      // We need to reset state and erase the recording.
      state = 0;
    }else{
      // Just a soft reset of state.
      state = -1;
    }
  }
}

void play_live() {
  state = 1;
}

void record() {
  static unsigned long last_interrupt_time1 = 0;
  unsigned long interrupt_time1 = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  //Serial.println("Pressed record button");
  if (interrupt_time1 - last_interrupt_time1 > 200){
    if(state == 3 || state == 1){
      // we just came from play recording/play live and need to go home
      // send columns home and reset the pause.
      needHome = true;
      togglePlayRecord = 1;
    }
    state = 2;
    if(toggleRecord == 1){
      // if the recording was already paused then record again. 
      toggleRecord = 0;
    }else if(toggleRecord == 0){
      // if the recording was already occuring then pause it.
      toggleRecord = 1;
    }
  }
  last_interrupt_time1 = interrupt_time1;
}

void play_recording() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
   // If interrupts come faster than 200ms, assume it's a bounce and ignore
   if (interrupt_time - last_interrupt_time > 200){
    if(state == 2){
      // if we came from play recording then need to make sure it won't be paused next
      needHome = true;
      toggleRecord = 1;
    }
    state = 3;
    //Serial.println("Triggered interrupt for play recording");
    if(togglePlayRecord == 1){
      // if the playing recording was already paused then play it. 
      togglePlayRecord = 0;
    }else if(togglePlayRecord == 0){
      // if the playing recording was already playing then pause it.
      togglePlayRecord = 1;
    }
   }
   last_interrupt_time = interrupt_time;
}
// moves column 1 up and the rest of them down.
void tone1(){
  // play note 1
  if(colOneCurr == 1){
    // column already in position
    // just play the sound
    int two = 90;
    int three = 90;
    int four = 90;
    // also move other columns down.
    if(colTwoCurr != 0){
      // drive col 2 back home
      two = 180;
    }
    if(colThreeCurr != 0){
      // drive three home
      three = 180;
    }
    if(colFourCurr != 0){
      four = 180;
    }
    colTwo.write(two);
    colThree.write(three);
    colFour.write(four);
    delay(1400);
    colTwo.write(90);
    colThree.write(90);
    colFour.write(90);
  }else{
    int two = 90;
    int three = 90;
    int four = 90;
    // also move other columns down.
    if(colTwoCurr != 0){
      // drive col 2 back home
      two = 180;
    }
    if(colThreeCurr != 0){
      // drive three home
      three = 180;
    }
    if(colFourCurr != 0){
      four = 180;
    }
    colOne.write(0);
    colTwo.write(two);
    colThree.write(three);
    colFour.write(four);
    delay(1400); // let them all go home
    colOne.write(90);
    colTwo.write(90);
    colThree.write(90);
    colFour.write(90);
  }    
  colOneCurr = 1;   
  colTwoCurr = 0;
  colThreeCurr = 0;
  colFourCurr = 0;
}
//moves column 2 up and the rest of them down.
void tone2(){
  // play note 2
  // move motor 2
  if(colTwoCurr == 1){
    // column already in position
    // just play the sound
    int one = 90;
    int three = 90;
    int four = 90;
    // also move other columns down.
    if(colOneCurr != 0){
      // drive col 1 back home
      one = 180;
    }
    if(colThreeCurr != 0){
      // drive three home
      three = 180;
    }
    if(colFourCurr != 0){
      four = 180;
    }
    colOne.write(one);
    colThree.write(three);
    colFour.write(four);
    delay(1400);
    colOne.write(90);
    colThree.write(90);
    colFour.write(90);
  }else{
    //tone(speaker, 110, 500);
    int one = 90;
    int three = 90;
    int four = 90;
    // also move other columns down.
    if(colOneCurr != 0){
      // drive col 2 back home
      one = 180;
    }
    if(colThreeCurr != 0){
      // drive three home
      three = 180;
    }
    if(colFourCurr != 0){
      four = 180;
    }
    colOne.write(one);
    colTwo.write(0);
    colThree.write(three);
    colFour.write(four);
    delay(1400); // let them all go home
    colOne.write(90);
    colTwo.write(90);
    colThree.write(90);
    colFour.write(90);
  }    
  colOneCurr = 0;   
  colTwoCurr = 1;
  colThreeCurr = 0;
  colFourCurr = 0;
}
//moves column 3 up and the rest of them down.
void tone3(){
  // play note 3
  if(colThreeCurr == 1){
    // column already in position
    // just play the sound
    int one = 90;
    int two = 90;
    int four = 90;
    // also move other columns down.
    if(colOneCurr != 0){
      // drive col 1 back home
      one = 180;
    }
    if(colTwoCurr != 0){
      // drive three home
      two = 180;
    }
    if(colFourCurr != 0){
      four = 180;
    }
    colOne.write(one);
    colTwo.write(two);
    colFour.write(four);
    delay(1400);
    colOne.write(90);
    colTwo.write(90);
    colFour.write(90);
  }else{
    //tone(speaker, 110, 500);
    int one = 90;
    int two = 90;
    int four = 90;
    // also move other columns down.
    if(colOneCurr != 0){
      // drive col 1 back home
      one = 180;
    }
    if(colTwoCurr != 0){
      // drive three home
      two = 180;
    }
    if(colFourCurr != 0){
      four = 180;
    }
    colOne.write(one);
    colTwo.write(two);
    colThree.write(0);
    colFour.write(four);
    delay(1400); // let them all go home
    colOne.write(90);
    colTwo.write(90);
    colThree.write(90);
    colFour.write(90);
  }    
  colOneCurr = 0;   
  colTwoCurr = 0;
  colThreeCurr = 1;
  colFourCurr = 0;
}
//moves column 4 up and the rest of them down.
void tone4(){
  // play note 4
  if(colFourCurr == 1){
    // column already in position
    // just play the sound
    int one = 90;
    int two = 90;
    int three = 90;
    // also move other columns down.
    if(colOneCurr != 0){
      // drive col 1 back home
      one = 180;
    }
    if(colTwoCurr != 0){
      // drive three home
      two = 180;
    }
    if(colThreeCurr != 0){
      three = 180;
    }
    colOne.write(one);
    colTwo.write(two);
    colThree.write(three);
    delay(1400);
    colOne.write(90);
    colTwo.write(90);
    colThree.write(90);
  }else{
    //tone(speaker, 110, 500);
    int one = 90;
    int two = 90;
    int three = 90;
    // also move other columns down.
    if(colOneCurr != 0){
      // drive col 1 back home
      one = 180;
    }
    if(colTwoCurr != 0){
      // drive three home
      two = 180;
    }
    if(colThreeCurr != 0){
      three = 180;
    }
    colOne.write(one);
    colTwo.write(two);
    colThree.write(three);
    colFour.write(0);
    delay(1400); // let them all go home
    colOne.write(90);
    colTwo.write(90);
    colThree.write(90);
    colFour.write(90);
  }    
  colOneCurr = 0;   
  colTwoCurr = 0;
  colThreeCurr = 0;
  colFourCurr = 1;
}
// moves column 1 and 2 up and the rest of them down.
void tone5() {
  // play note 5
  if(colOneCurr == 1 && colTwoCurr == 1){
    // column already in position
    // just play the sound
    int four = 90;
    int three = 90;
    // also move other columns down.
    if(colFourCurr != 0){
      // drive three home
      four = 180;
    }
    if(colThreeCurr != 0){
      three = 180;
    }
    colFour.write(four);
    colThree.write(three);
    delay(1400);
    colFour.write(90);
    colThree.write(90);
  }else{
    //tone(speaker, 110, 500);
    int three = 90;
    int four = 90;
    // also move other columns down.
    if(colFourCurr != 0){
      // drive col 1 back home
      four = 180;
    }
    if(colThreeCurr != 0){
      three = 180;
    }
    if(colOneCurr != 1){
      colOne.write(0);
    }
    if(colTwoCurr != 1){
      colTwo.write(0);
    }
    colThree.write(three);
    colFour.write(four);
    delay(1400); // let them all go home
    colOne.write(90);
    colTwo.write(90);
    colThree.write(90);
    colFour.write(90);
  }    
  colOneCurr = 1;   
  colTwoCurr = 1;
  colThreeCurr = 0;
  colFourCurr = 0;
}
// moves column 3 and 4 up and the rest of them down.
void tone6(){
  // play note 6
  if(colThreeCurr == 1 && colFourCurr == 1){
    // column already in position
    // just play the sound
    int one = 90;
    int two = 90;
    // also move other columns down.
    if(colOneCurr != 0){
      // drive three home
      one = 180;
    }
    if(colTwoCurr != 0){
      two = 180;
    }
    colOne.write(one);
    colTwo.write(two);
    delay(1400);
    colOne.write(90);
    colTwo.write(90);
  }else{
    //tone(speaker, 110, 500);
    int one = 90;
    int two = 90;
    // also move other columns down.
    if(colOneCurr != 0){
      // drive col 1 back home
      one = 180;
    }
    if(colTwoCurr != 0){
      two = 180;
    }
    if(colThreeCurr != 1){
      colThree.write(0);
    }
    if(colFourCurr != 1){
      colFour.write(0);
    }
    colOne.write(one);
    colTwo.write(two);
    delay(1400); // let them all go home
    colOne.write(90);
    colTwo.write(90);
    colThree.write(90);
    colFour.write(90);
  }    
  colOneCurr = 0;   
  colTwoCurr = 0;
  colThreeCurr = 1;
  colFourCurr = 1;
}
// moves column 1 and 3 up and the rest of them down.
void tone7(){
  // play note 7
  if(colOneCurr == 1 && colThreeCurr == 1){
    // column already in position
    // just play the sound
    int two = 90;
    int four = 90;
    // also move other columns down.
    if(colTwoCurr != 0){
      // drive three home
      two = 180;
    }
    if(colFourCurr != 0){
      four = 180;
    }
    colTwo.write(two);
    colFour.write(four);
    delay(1400);
    colTwo.write(90);
    colFour.write(90);
  }else{
    //tone(speaker, 110, 500);
    int two = 90;
    int four = 90;
    // also move other columns down.
    if(colTwoCurr != 0){
      // drive col 1 back home
      two = 180;
    }
    if(colFourCurr != 0){
      four = 180;
    }
    if(colOneCurr != 1){
      colOne.write(0);
    }
    if(colThreeCurr != 1){
      colThree.write(0);
    }
    colTwo.write(two);
    colFour.write(four);
    delay(1400); // let them all go home
    colOne.write(90);
    colTwo.write(90);
    colThree.write(90);
    colFour.write(90);
  }    
  colOneCurr = 1;   
  colTwoCurr = 0;
  colThreeCurr = 1;
  colFourCurr = 0;
}
// moves columns 2 and 4 up and the rest of them down.
void tone8(){
  // play note 8
  if(colTwoCurr == 1 && colFourCurr == 1){
    // column already in position
    // just play the sound
    int one = 90;
    int three = 90;
    // also move other columns down.
    if(colOneCurr != 0){
      // drive three home
      one = 180;
    }
    if(colThreeCurr != 0){
      three = 180;
    }
    colOne.write(one);
    colThree.write(three);
    delay(1400);
    colOne.write(90);
    colThree.write(90);
  }else{
    //tone(speaker, 110, 500);
    int one = 90;
    int three = 90;
    // also move other columns down.
    if(colOneCurr != 0){
      // drive col 1 back home
      one = 180;
    }
    if(colThreeCurr != 0){
      three = 180;
    }
    if(colTwoCurr != 1){
      colTwo.write(0);
    }
    if(colFourCurr != 1){
      colFour.write(0);
    }
    colOne.write(one);
    colThree.write(three);
    delay(1400); // let them all go home
    colOne.write(90);
    colTwo.write(90);
    colThree.write(90);
    colFour.write(90);
  }    
  colOneCurr = 0;   
  colTwoCurr = 1;
  colThreeCurr = 0;
  colFourCurr = 1;
}

bool allHome = false;
void setup() {
  // put your setup code here, to run once:
  // Used to setup interrup on any Arduino Pin.
  //Serial.begin(115200);
  enableInterrupt(A0, reset, CHANGE); // reset/stop
  enableInterrupt(A1, play_live, FALLING); // Play Live
  enableInterrupt(A2, record, FALLING); // Record
  enableInterrupt(A3, play_recording, FALLING); // Play Recording
  lox.begin();
  colOne.attach(10);
  colTwo.attach(9);
  colThree.attach(6);
  colFour.attach(5);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
}

void loop() {
  
  // put your main code here, to run repeatedly:
  /*
   * First step will be to check the current mode switches.
  */
  //Serial.print("State is: ");
  //Serial.println(state);
  //Serial.print("reset is: ");
  //Serial.println(analogRead(A1));
  //Serial.print("mode 1 is: ");
  //Serial.println(analogRead(A2));
  if(state == -1){
    togglePlayRecord = 1;
    toggleRecord = 1;
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);
    // soft reset state
    // stop the music.
    noTone(speaker);
    // does not do anything (is an idle state)
    allHome = colOneCurr==0 && colTwoCurr==0 && colThreeCurr==0 && colFourCurr==0;
    // send columns home
    if(colOneCurr != 0){
      colOne.write(180);
    }
    if(colTwoCurr != 0){
      colTwo.write(180);
    }
    if(colThreeCurr != 0){
      colThree.write(180);
    }
    if(colFourCurr != 0){
      colFour.write(180);
    }
    delay(1600);
    colOne.write(90);
    colTwo.write(90);
    colThree.write(90);
    colFour.write(90);
    // at this point all columns are back home
    colOneCurr = 0;
    colTwoCurr = 0;
    colThreeCurr = 0;
    colFourCurr = 0;
    // moves columns back home
  }else if(state == 0){
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);
    delay(500);
    // stop the music.
    noTone(speaker);
    // erase the recording and send the columns back home
    allHome = colOneCurr==0 && colTwoCurr==0 && colThreeCurr==0 && colFourCurr==0;
    for(int i = 0; i < 41; i++){
      notes[i] = 0;
    }
    // send columns home
    if(colOneCurr != 0){
      colOne.write(180);
    }
    if(colTwoCurr != 0){
      colTwo.write(180);
    }
    if(colThreeCurr != 0){
      colThree.write(180);
    }
    if(colFourCurr != 0){
      colFour.write(180);
    }
    delay(1600);
    colOne.write(90);
    colTwo.write(90);
    colThree.write(90);
    colFour.write(90);
    state = -1; // no need to keep erasing.
    digitalWrite(redLED, LOW);
    delay(500);
    // at this point all columns are back home
    colOneCurr = 0;
    colTwoCurr = 0;
    colThreeCurr = 0;
    colFourCurr = 0;
    digitalWrite(redLED, HIGH);
  }else if(state == 1){
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, HIGH);
    // play live state
    // need to read the TOF
    VL53L0X_RangingMeasurementData_t measure;
    int distance;
    //Serial.print("Reading a measurement... ");
    lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
    if (measure.RangeStatus != 4) {  // phase failures have incorrect data
      //Serial.print("Distance (mm): "); 
      distance = measure.RangeMilliMeter;
      if(distance >= 100 && distance < 125){
        tone(speaker, 440);
        tone1(); // move columns to position 1 - column 1 is up.
      }else if(distance >= 125 && distance < 150){
        tone(speaker, 493.88);
        tone2(); // move columns to position 2 - column 2 is up.
      }else if(distance >= 150 && distance < 175){
        tone(speaker, 523.25);
        tone3(); // move columns to position 3 - column 3 is up.
      }else if(distance >= 175 && distance < 200){
        tone(speaker, 587.33);
        tone4(); // move columns to position 4 - column 4 is up.
      }else if(distance >= 200 && distance < 225){
        tone(speaker, 659.25);
        tone5(); // move columns to position 5 - columns 1 and 2 are up.
      }else if(distance >= 225 && distance < 250) {
        tone(speaker, 698.46);
        tone6(); // move columns to position 6 - columns 3 and 4 are up.
      }else if(distance >= 250 && distance < 275) {
        tone(speaker, 783.99);
        tone7(); // move columns to position 7 - columns 1 and 3 are up.
      }else if(distance >= 275 && distance < 300) {
        tone(speaker, 830.61);
        tone8(); // move columns to position 8 - columns 2 and 4 are up.
      }else{
        // if current reading is not mapped do not play a note.
        noTone(speaker);
        //return columns home.
        if(colOneCurr != 0){
          colOne.write(180);
        }
        if(colTwoCurr != 0){
          colTwo.write(180);
        }
        if(colThreeCurr != 0){
          colThree.write(180);
        }
        if(colFourCurr != 0){
          colFour.write(180);
        }
        delay(1400);
        colOne.write(90);
        colTwo.write(90);
        colThree.write(90);
        colFour.write(90);
        colOneCurr = 0;
        colTwoCurr = 0;
        colThreeCurr = 0;
        colFourCurr = 0;
      }
    } else {
      //Serial.println(" out of range ");
      noTone(speaker);
      //return columns home.
     if(colOneCurr != 0){
      colOne.write(180);
    }
    if(colTwoCurr != 0){
      colTwo.write(180);
    }
    if(colThreeCurr != 0){
      colThree.write(180);
    }
    if(colFourCurr != 0){
      colFour.write(180);
    }
    delay(1400);
    colOne.write(90);
    colTwo.write(90);
    colThree.write(90);
    colFour.write(90);
    colOneCurr = 0;
    colTwoCurr = 0;
    colThreeCurr = 0;
    colFourCurr = 0;
    }
    delay(100);
  }else if(state == 2){
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, HIGH);
    if(needHome){
      needHome = false;
      noTone(speaker);
      // send columns home
      if(colOneCurr != 0){
        colOne.write(180);
      }
      if(colTwoCurr != 0){
        colTwo.write(180);
      }
      if(colThreeCurr != 0){
        colThree.write(180);
      }
      if(colFourCurr != 0){
        colFour.write(180);
      }
      delay(1500);
      colOne.write(90);
      colTwo.write(90);
      colThree.write(90);
      colFour.write(90);
      // at this point all columns are back home
      colOneCurr = 0;
      colTwoCurr = 0;
      colThreeCurr = 0;
      colFourCurr = 0;
      // moves columns back home
    }
    // Record state
    // check if the array is full
    if(notes[0] == 1){
      //memory is full, no more recording!
      state = -1; // go to stop/soft reset state.
    }else{
      // we can record!
      // find first index we can record in
      int i = 1;
      while(i<41 && state == 2){
        if(notes[i] == 0){
          // this spot is free
          break;
        }
        i++;
      }
      // use the index found as beginning of for loop
      // now actually perform the location tracking
      while(i < 41 && state == 2){
        //Serial.print("Toggle record is: ");
        //Serial.println(toggleRecord);
        if(toggleRecord == 1){
          // we need to pause the recording.
          noTone(speaker);
          continue;
        }
        // recording goes here
        // need to read the TOF
        VL53L0X_RangingMeasurementData_t measure;
        int distance;
        //Serial.print("Reading a measurement... ");
        lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
        if (measure.RangeStatus != 4) {  // phase failures have incorrect data
          //Serial.print("Distance (mm): "); 
          distance = measure.RangeMilliMeter;
          if(distance >= 100 && distance < 125){
            // record tone 1
            notes[i] = 1;
            // play tone 1
            tone(speaker, 440);
            delay(1000); // 1/2 second note recording.            
          }else if(distance >= 125 && distance < 150){
            // record tone 2
            notes[i] = 2;
            // play tone 2
            tone(speaker, 493.88);
            delay(1000); // 1/2 second note recording.
          }else if(distance >= 150 && distance < 175){
            // record tone 3
            notes[i] = 3;
            // play tone 3
            tone(speaker, 523.25);
            delay(1000); // 1/2 second note recording.
          }else if(distance >= 175 && distance < 200){
            // record tone 4
            notes[i] = 4;
            // play tone 4
            tone(speaker, 587.33);
            delay(1000); // 1/2 second note recording.
          }else if(distance >= 200 && distance < 225){
            // record tone 5
            notes[i] = 5;
            // play tone 5
            tone(speaker, 659.25);
            delay(1000); // 1/2 second note recording.
          }else if(distance >= 225 && distance < 250){
            // record tone 6
            notes[i] = 6;
            // play tone 6
            tone(speaker, 698.46);
            delay(1000); // 1/2 second note recording.
          }else if(distance >= 250 && distance < 275){
            // record tone 7
            notes[i] = 7;
            // play tone 7
            tone(speaker, 783.99);
            delay(1000); // 1/2 second note recording.
          }else if(distance >= 275 && distance < 300){
            // record tone 8
            notes[i] = 8;
            // play tone 8
            tone(speaker, 830.61);
            delay(1000); // 1/2 second note recording.
          }else{
            // the detected distance is not within the pre-programmed range.
            noTone(speaker);
            continue;
          }
        } else {
          //Serial.println(" out of range ");
          noTone(speaker);
          continue;
        }
        delay(100);
        i++;
      }
      if(i >= 41){
        notes[0] = 1; // the memory is full!
      }
      //print the arry for debug
      for(int j = 0; j < 41; j++){
        //Serial.print(notes[j]);
        //Serial.print(",");
      }
      //Serial.println();
    }
  }else if(state == 3){
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, HIGH);
    if(needHome){
      needHome = false;
      noTone(speaker);
      // send columns home
      if(colOneCurr != 0){
        colOne.write(180);
      }
      if(colTwoCurr != 0){
        colTwo.write(180);
      }
      if(colThreeCurr != 0){
        colThree.write(180);
      }
      if(colFourCurr != 0){
        colFour.write(180);
      }
      delay(1500);
      colOne.write(90);
      colTwo.write(90);
      colThree.write(90);
      colFour.write(90);
      // at this point all columns are back home
      colOneCurr = 0;
      colTwoCurr = 0;
      colThreeCurr = 0;
      colFourCurr = 0;
      // moves columns back home
    }
    // Play Recording state
    // read the memory array
    //Serial.println(state);
    int i = 1;
    while(i < 41 && state == 3){
      // go through each element of the array
      //Serial.print("togglePlayRecord is: ");
      //Serial.println(togglePlayRecord);
      if(togglePlayRecord == 1){
        // need to pause playing the recording
        continue;
      }
      if(notes[i] == 0){
        // this is invalid note, skip it.
        //Serial.println("skipping first");
        i++;
        continue;    
      }
      // otherwise read the note and play the tone.
      switch (notes[i]) {
        case 1:
          // play note 1
          tone(speaker, 440,1000);
          tone1(); // move columns to position 1 - column 1 is up.
          break;
        case 2:
          // play note 2
          // move motor 2
          tone(speaker, 493.88,1000);
          tone2(); // move columns to position 2 - column 2 is up.
          break;
        case 3:
          // play note 3
          tone(speaker, 523.25,1000);
          tone3(); // move columns to position 3 - column 3 is up.
          break;
        case 4:
          // play note 4
          tone(speaker, 587.33,1000);
          tone4(); // move columns to position 4 - column 4 is up.
          break;
        case 5:
          // play note 5
          tone(speaker, 659.25,1000); // move columns to position 5 - columns 1 and 2 are up.
          tone5();
          break;
        case 6:
          // play note 6
          tone(speaker, 698.46,1000); // move columns to position 6 - columns 3 and 4 are up.
          tone6();
          break;
        case 7:
          // play note 7
          tone(speaker, 783.99,1000); // move columns to position 7 - columns 1 and 3 are up.
          tone7();
          break;
        case 8:
          // play note 8
          tone(speaker, 830.61,1000);
          tone8(); // move columns to position 8 - columns 2 and 4 are up.
          break;
      }
      i++;
      if(state != 3){
        break;
      }
    }
  }
}
