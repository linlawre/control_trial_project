#include <Wire.h>
#include <Servo.h>

Servo servo1, servo2;
int servo1Pin = 8;
int servo2Pin = 9;


int rotation_x, rotation_y, rotation_z;
float AccX, AccY, AccZ;

float cali_x = 0;
float cali_y = 0;
float cali_z = 0;
int y_angle;
int init_z_rotation = 90;
int real_time_z_rotation = 0;


void signals(void);


void setup() {
  Serial.begin(9600);

  Serial.print("init.........  ");

  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  Wire.begin();
  delay(250);
  Wire.beginTransmission(0x68); 
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();


  for (int i = 0; i < 100; i++) {
    signals();
    cali_x += rotation_x;
    cali_y += rotation_y;
    cali_z += rotation_z;
  }

  cali_x = cali_x / 100;
  cali_y = cali_y / 100;
  cali_z = cali_z / 100;

  Serial.print("cali_x:  ");
  Serial.println(cali_x);
  Serial.print("cali_y:  ");
  Serial.println(cali_y);
  Serial.print("cali_z:  ");
  Serial.println(cali_z);
  Serial.println("====================================");
}

void loop() {
  signals();

  init_z_rotation = (init_z_rotation + (rotation_z/8));

  if (init_z_rotation >= 180)
    real_time_z_rotation = 180;
  else if (init_z_rotation <= 0)
    real_time_z_rotation = 0;
  else
    real_time_z_rotation = init_z_rotation;


  Serial.print("init_z_rotation:  ");
  Serial.println(init_z_rotation);
  Serial.print("y_angle:  ");
  Serial.println(y_angle);
  Serial.println("====================================");

  servo1.write(init_z_rotation);
  servo2.write(y_angle);

  // delay(100);
}


void signals(void) {

  Wire.beginTransmission(0x68);
  Wire.write(0x1A);
  Wire.write(0x05);
  Wire.endTransmission();

  Wire.beginTransmission(0x68);
  Wire.write(0x1C);
  Wire.write(0x10);
  Wire.endTransmission();
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission(); 
  Wire.requestFrom(0x68,6);

  int16_t AccX_raw = Wire.read() << 8 | Wire.read();
  int16_t AccY_raw = Wire.read() << 8 | Wire.read();
  int16_t AccZ_raw = Wire.read() << 8 | Wire.read();


  Wire.beginTransmission(0x68);
  Wire.write(0x1B); 
  Wire.write(0x8);
  Wire.endTransmission();                                                   
  Wire.beginTransmission(0x68);
  Wire.write(0x43);
  Wire.endTransmission();
  Wire.requestFrom(0x68,6);

  int16_t GyroX=Wire.read()<<8 | Wire.read();
  int16_t GyroY=Wire.read()<<8 | Wire.read();
  int16_t GyroZ=Wire.read()<<8 | Wire.read();

  rotation_x=(float)GyroX/65.5 - cali_x;
  rotation_y=(float)GyroY/65.5 - cali_y;
  rotation_z=(float)GyroZ/65.5 - cali_z;

  AccX=(float)AccX_raw/4096;
  AccY=(float)AccY_raw/4096;
  AccZ=(float)AccZ_raw/4096;

  if (AccZ >= 0)
    y_angle=atan(AccY/sqrt(AccX*AccX+AccZ*AccZ))*1/(3.142/180);
  else
    y_angle=180 - atan(AccY/sqrt(AccX*AccX+AccZ*AccZ))*1/(3.142/180);
}


