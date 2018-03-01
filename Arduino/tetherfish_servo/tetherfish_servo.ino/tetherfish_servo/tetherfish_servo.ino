#include <Servo.h>

Servo servo;

int servoPin = 10;

float offset = 90; //ensure that all numbers fed to servo are positive
float omega = 2; //tailbeat frequency in Hz
float phi = 0;
float ytail; //servo input
float amp = 50; //amplitude of tailbeat
unsigned long oldt;
unsigned long timestamp;
float dt;

long lastprinttime = 0;
long lastservotime = 0;
int printdelay = 20;
//int servodelay = 50;

//one revolution moves gantry 2 and 5/16 inches

//linear rail motor
int A = 2;                    // the pin connected to the wire A of the coil A (or to the H-bridge pin controlling the same wire)
int A_bar = 3;                // the pin connected to the wire A- of the coil A (or to the H-bridge pin controlling the same wire)
int B = 4;                    // the pin connected to the wire B of the coil A (or to the H-bridge pin controlling the same wire)
int B_bar = 5;                // the pin connected to the wire B- of the coil A (or to the H-bridge pin controlling the same wire)
//int x = 100;                 // smaller values may make the motor produce more speed and less torque
//int stepsPerRevolution = 400; // you can the number of steps required to make a complete revolution in the data sheet of your motor

//rotational motor
int A_2 = 6;
int A_bar2 = 7;
int B2 = 8;
int B_bar2 = 9;
//int x2 = 5000;

int linDesired = 400 * 12; //number of steps you want the gantry to go
int rotDesired = 400 / 2; //number of steps you want the rotation servo to take

boolean left = true;
boolean right = false;
boolean rotate = false;
boolean rotateL = false;
boolean reset = false;
boolean set_left;
boolean set_right;
boolean set_rotate;
boolean set_rotateL;
boolean set_reset;

long stepsLeft = 0;
long stepsRight = 0;
long stepsRot = 0;
long stepsRotL = 0;
long stepsGlob=0;
long stepsGlob2 = 0;


float revLeft = 0;
float revRight = 0;
float pos = 0;
int posRot;

int cnt = 1;
int cnt2 = 1;

unsigned long laststeptime = 0;
int stepdelay = 3000; //in microseconds

unsigned long laststeptime2 = 0;
int stepdelay2 = 600000;

unsigned long timestampMicros;

void setup() {
  pinMode(A, OUTPUT);
  pinMode(A_bar, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(B_bar, OUTPUT);

  pinMode(A2, OUTPUT);
  pinMode(A_bar2, OUTPUT);
  pinMode(B2, OUTPUT);
  pinMode(B_bar2, OUTPUT);

  servo.attach(servoPin);

  Serial.begin(115200);
}

void loop() {

  timestamp = millis();
  timestampMicros = micros();
  dt = (timestamp - oldt) * .001;
  oldt = timestamp;

  phi += omega * dt * 2 * 3.14; //calculate phi and convert from cycles to radians
  ytail = amp * sin(phi) + offset;
  servo.write(int(ytail));

  //  if ((timestamp - lastservotime) > servodelay) {
  //    ytail = amp * sin(phi) + offset;
  //    servo.write(int(ytail));
  //    lastservotime = timestamp;
  //  }

  set_left = left && stepsLeft <= linDesired|| rotate && stepsRot > rotDesired;
  set_rotate = rotate && stepsRot <= rotDesired  || right && stepsRight > linDesired;
  set_right = right && stepsRight  <= linDesired || rotateL && stepsRotL > rotDesired;
  set_rotateL = rotateL && stepsRotL <= rotDesired || left && stepsLeft > linDesired;
  set_reset = rotate && stepsRot > rotDesired || rotateL && stepsRotL > rotDesired;

  left = set_left;
  rotate = set_rotate;
  rotateL = set_rotateL;
  right = set_right;
  reset = set_reset;

  revLeft = stepsLeft / 400.0;
  revRight = stepsRight / 400.0;
  float revs = stepsGlob / 400.0;
  pos = (revs) * 2.3125; //position of fish along rail (converted from revolutions to inches)
  posRot = stepsGlob2 * 1.8 / 2; //rotational position of fish in degrees
  
  
  if(left){
    if((timestampMicros-laststeptime)>stepdelay){
      stepsLeft++;
      stepsGlob++;
      halfStep1(true);
      laststeptime=timestampMicros;
    }
  }
  else{
    stepsLeft=0;
     
        }
  
  if(right){
     if((timestampMicros-laststeptime)>stepdelay){
      stepsRight++;
      stepsGlob--;
      halfStep1(false);
      laststeptime=timestampMicros;
    }
  }
  else{
    stepsRight=0;

  }

  if(rotateL){
     if((timestampMicros-laststeptime2)>stepdelay2){
      stepsRotL++;
      stepsGlob2++;
      halfStep2(false);
      laststeptime2=timestampMicros;
    }
  }
  else{
    stepsRotL=0;
  }
  
 if(rotate){
     if((timestampMicros-laststeptime2)>stepdelay2){
      stepsRot++;
      stepsGlob2--;
      halfStep2(true);
      laststeptime2=timestampMicros;
    }
  }
  else{
    stepsRot=0;
  }

  if ((timestamp - lastprinttime) > printdelay) {
    //  Serial.print(stepsLeft);
    //  Serial.print('\t');
    //
    //  Serial.print(revLeft);
    //  Serial.print('\t');
    //
    //  Serial.print(stepsRight);
    //  Serial.print('\t');
    //
    //  Serial.print(revRight);
    //  Serial.print('\t');
    //
    //  Serial.print(stepsRot);
    //  Serial.print('\t');


    //    Serial.print(dt, 5);
    //    Serial.print('\t');

    Serial.print(pos);
    Serial.print("\t");
    //0Serial.println();

    Serial.print(posRot);
    Serial.print('\t');

    Serial.print(timestamp * .001);
    Serial.print('\t');

    Serial.print(pos);
    
    Serial.println();
    
    

    //  Serial.print(ytail);
    //  Serial.print('\t');
    //
    ////  Serial.print(dt, 4);
    ////  Serial.print('\t');
    //
    //  Serial.print(phi);
    //  Serial.println();

    lastprinttime = timestamp;
  }

}



void halfStep1(boolean dir) {
      if (cnt == 1) {
        digitalWrite(A, HIGH);
        digitalWrite(A_bar, LOW);
        digitalWrite(B, LOW);
        digitalWrite(B_bar, LOW);
      }

      if (cnt == 2) {
        digitalWrite(A, HIGH);
        digitalWrite(A_bar, LOW);
        digitalWrite(B, HIGH);
        digitalWrite(B_bar, LOW);
      }

      if (cnt == 3) {
        digitalWrite(A, LOW);
        digitalWrite(A_bar, LOW);
        digitalWrite(B, HIGH);
        digitalWrite(B_bar, LOW);
      }

      if (cnt == 4) {
        digitalWrite(A, LOW);
        digitalWrite(A_bar, HIGH);
        digitalWrite(B, HIGH);
        digitalWrite(B_bar, LOW);
      }

      if (cnt == 5) {
        digitalWrite(A, LOW);
        digitalWrite(A_bar, HIGH);
        digitalWrite(B, LOW);
        digitalWrite(B_bar, LOW);
      }

      if (cnt == 6) {
        digitalWrite(A, LOW);
        digitalWrite(A_bar, HIGH);
        digitalWrite(B, LOW);
        digitalWrite(B_bar, HIGH);
      }

      if (cnt == 7) {
        digitalWrite(A, LOW);
        digitalWrite(A_bar, LOW);
        digitalWrite(B, LOW);
        digitalWrite(B_bar, HIGH);
      }

      if (cnt == 8) {
        digitalWrite(A, HIGH);
        digitalWrite(A_bar, LOW);
        digitalWrite(B, LOW);
        digitalWrite(B_bar, HIGH);
      }
      
      if(dir){
        cnt ++;
      if (cnt > 8) {
        cnt = 1;
      }
      }else{
        cnt--;
        if(cnt<1){
          cnt=8;
        }
    }
}


   void halfStep2(boolean dir) {

      if (cnt2 == 1) {
        digitalWrite(A_2, HIGH);
        digitalWrite(A_bar2, LOW);
        digitalWrite(B2, LOW);
        digitalWrite(B_bar2, HIGH);
      }

      if (cnt2 == 2) {
        digitalWrite(A_2, LOW);
        digitalWrite(A_bar2, LOW);
        digitalWrite(B2, LOW);
        digitalWrite(B_bar2, HIGH);
      }

      if (cnt2 == 3) {
        digitalWrite(A_2, LOW);
        digitalWrite(A_bar2, HIGH);
        digitalWrite(B2, LOW);
        digitalWrite(B_bar2, HIGH);
      }

      if (cnt2 == 4) {
        digitalWrite(A_2, LOW);
        digitalWrite(A_bar2, HIGH);
        digitalWrite(B2, LOW);
        digitalWrite(B_bar2, LOW);
      }

      if (cnt2 == 5) {
        digitalWrite(A_2, LOW);
        digitalWrite(A_bar2, HIGH);
        digitalWrite(B2, HIGH);
        digitalWrite(B_bar2, LOW);
      }

      if (cnt2 == 6) {
        digitalWrite(A_2, LOW);
        digitalWrite(A_bar2, LOW);
        digitalWrite(B2, HIGH);
        digitalWrite(B_bar2, LOW);
      }

      if (cnt2 == 7) {
        digitalWrite(A_2, HIGH);
        digitalWrite(A_bar2, LOW);
        digitalWrite(B2, HIGH);
        digitalWrite(B_bar2, LOW);
      }

      if (cnt2 == 8) {
        digitalWrite(A_2, HIGH);
        digitalWrite(A_bar2, LOW);
        digitalWrite(B2, LOW);
        digitalWrite(B_bar2, LOW);
      }
    if(dir){
      cnt2 ++;
      if (cnt2 > 8) {
        cnt2 = 1;
      }
    }else{
      cnt2--;
      if(cnt2<1){
        cnt2=8;
      }}


    }    




