/*
Robot Auto - Remote car receives joystick values from MX-FS-03V transmitter to the XY-MK-5V receiver
*/

// download library van http://www.pjrc.com/teensy/td_libs_VirtualWire.html
#include <VirtualWire.h>

const int receive_pin = 11;

bool test = true;

// save values
int Hval, Vval, Kval;

int calib_max_speed, calib_no_speed;

//internal variabeles
int right_speed;
int max_speed;
int min_speed;
int left_speed;

unsigned int gebruiker = 2000 * 0;

void setup() {
  // Begin Seriele monitor
  if (test) Serial.begin(9600);
  
  vw_set_rx_pin(receive_pin);
  vw_setup(2000);     
  
  // Start the receiver PLL running
  vw_rx_start();       
  
  calib_max_speed = 255; //maximum speed forward motors
  calib_no_speed  = 100; //min speed to motors that makes them stand still

  max_speed = calib_max_speed;
  min_speed = calib_no_speed;
  
  //startwaarden joystick
  Hval = 0;
  Vval = 0;
  Kval = 1;
}

void loop(){  
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  uint16_t tmp_Hval, tmp_Vval, tmp_Kval;
  
  // non blocking, read values when processed
  if (vw_get_message(buf, &buflen)) { // Non-blocking
    // Message with a good checksum received, print it.
    if (test) {
      Serial.print("Got: ");
      for (int i = 0; i < buflen; i++) {
        Serial.print(buf[i], HEX);
        Serial.print(' ');
      }
      Serial.println();
    }
    //uint8 received, convert to normal uint16:
    if (buflen == 6)  {
      tmp_Hval =  buf[0]; tmp_Hval = (tmp_Hval<<8) +  buf[1];
      tmp_Vval =  buf[2]; tmp_Vval = (tmp_Vval<<8) +  buf[3];
      tmp_Kval =  buf[4]; tmp_Kval = (tmp_Kval<<8) +  buf[5];

      // discrimate between cars using the gebruiker varible. Sensor values are 0 to 1024
      if (tmp_Hval >= gebruiker && tmp_Hval < gebruiker+1025 &&
          tmp_Vval >= gebruiker && tmp_Vval < gebruiker+1025 &&
          tmp_Kval >= gebruiker && tmp_Kval < gebruiker+1025 ) {
        // waarden van de juiste gebruiker !
        Hval = tmp_Hval-gebruiker;
        Vval = tmp_Vval-gebruiker;
        Kval = tmp_Kval-gebruiker;  // zal 0 of 1 zijn
        if (test) {
          Serial.print("Computed received values: ");
          toonWaarden();
        }
        //now convert to a speed between -255 en 255
        Hval = map(Hval, 0, 1023, -calib_max_speed, calib_max_speed);
        Vval = map(Vval, 0, 1023, -calib_max_speed, calib_max_speed);
        if (test) {
          Serial.print("Speed values: ");
          toonWaarden();
        }
      } else {
        if (test) {
          Serial.print("Problem, Converted to: ");
          Serial.print ("  "); 
          Serial.print (tmp_Hval);
          Serial.print ("  --  "); 
          Serial.print (tmp_Vval);
          Serial.print ("  --  "); 
          Serial.print (tmp_Kval);
          Serial.print ("  --  "); 
          Serial.print ((unsigned int) buf[0]);
          Serial.print ("  --  "); 
          Serial.print ((unsigned int) buf[1]);
          Serial.print ("  --  "); 
          Serial.print ((unsigned int) buf[4]);
          Serial.print ("  --  "); 
          Serial.print ((unsigned int) buf[5]);
          Serial.println ("  --");
        }
      }
    } else {
      if (test) {Serial.println("Error, buffer of wrong length received!");}
    }
  }
  
  motor_drive(Vval+Hval, Vval-Hval);
  
}

void toonWaarden() {
  Serial.print ("  "); 
  Serial.print (Hval);
  Serial.print ("  --  "); 
  Serial.print (Vval);
  Serial.print ("  --  "); 
  Serial.print (Kval);
  Serial.println ("  --");
}

void motor_drive(int right_speed, int left_speed){
  // Motors bewegen met de gegeven snelheid. 
  // snelheid is een waarde tussen -255 en 255!
  int sgvr = 1;
  if (right_speed != 0) {sgvr = abs(right_speed)/right_speed;}
  int sgvl = 1;
  if (left_speed != 0) {sgvl = abs(left_speed)/left_speed;}
  int vr = abs(right_speed) + calib_speed_corrR;
  int vl = abs(left_speed)  + calib_speed_corrL;
  if (vr > calib_max_speed) {vr = calib_max_speed;}
  else if (vr<0) {vr = 0;}
  vr = vr * sgvr;
  if (vl > calib_max_speed) {vl = calib_max_speed;}
  else if (vl<0) {vl = 0;}
  vl = vl * sgvl;
  
  if (right_speed>=0){
    digitalWrite(motorrechtsDir, LOW); //forward
    analogWrite(motorrechtsPWM, vr);
  } else {
    digitalWrite(motorrechtsDir, HIGH); //backward
    analogWrite(motorrechtsPWM, 255+vr);
  }
  if (left_speed>=0){
    digitalWrite(motorlinksDir, LOW); //forward
    analogWrite(motorlinksPWM, vl);
  } else {
    digitalWrite(motorlinksDir, HIGH); //backward
    analogWrite(motorlinksPWM, 255+vl);
  }
}