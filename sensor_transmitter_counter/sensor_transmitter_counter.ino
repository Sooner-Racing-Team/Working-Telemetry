#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "max6675.h" 
#include <SoftwareSerial.h>
#include <math.h>

/*****************************************************************************************/
/**********************************- GLOBAL VARIABLES - **********************************/

// Accelerometer Global Variables

Adafruit_MPU6050 mpu; // Accelerometer library function needed for variables used in accel code
double a_avg_x = 0;
double a_avg_y = 0;
double a_movingAverage = 0;
double time_total = 0;
const int MPU = 0x68; // MPU6050 I2C address
float AccX, AccY, AccZ;
double a_treadmill_x[10] = {0};
double a_treadmill_y[10] = {0};

// Thermocouple Global Variables
/*
int SO = 2; // has to be digital pin (changed from the original thermocouple code where the pins were digital PWM, troubleshoot this if thermocouple doesn't work
int CS = 3; // digital pin
int sck = 4; // digital pin
MAX6675 module(sck, CS, SO);
*/

int thermoDO = 29;
int thermoCLK = 28;

int thermoCS_1 =  34;
int thermoCS_2 =  36;
int thermoCS_3 =  38;
int thermoCS_4 =  40;

MAX6675 thermocouple_1(thermoCLK, thermoCS_1, thermoDO);
MAX6675 thermocouple_2(thermoCLK, thermoCS_2, thermoDO);
MAX6675 thermocouple_3(thermoCLK, thermoCS_3, thermoDO);
MAX6675 thermocouple_4(thermoCLK, thermoCS_4, thermoDO);

// Linear Potentiometer Global Variables

const int potPin = A3; // pin A0 to read analog input
int value; // save analog value


// Transceiver Global Variables
SoftwareSerial HC12(3,2); // HC-12 TX Pin, HC-12 RX Pin


/*****************************************************************************************/
int counter = 0;
void setup() {
 Serial.begin(9600);
 
Serial.print("Void setup has been entered");
double a_avg_x, milli, temperature_1, temperature_2, temperature_3, temperature_4;
// int counter = 25;
  
 /* ***********************/
 // ACCELEROMETER SETUP CODE
 Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission
  /*
  // Configure Accelerometer Sensitivity - Full Scale Range (default +/- 2g)
  Wire.beginTransmission(MPU);
  Wire.write(0x1C);                  //Talk to the ACCEL_CONFIG register (1C hex)
  Wire.write(0x10);                  //Set the register bits as 00010000 (+/- 8g full scale range)
  Wire.endTransmission(true);
  // Configure Gyro Sensitivity - Full Scale Range (default +/- 250deg/s)
  Wire.beginTransmission(MPU);
  Wire.write(0x1B);                   // Talk to the GYRO_CONFIG register (1B hex)
  Wire.write(0x10);                   // Set the register bits as 00010000 (1000deg/s full scale)
  Wire.endTransmission(true);
  delay(20);
  */
  // Call this function if you need to get the IMU error values for your module
  delay(20);
  //}
  Serial.println("MPU6050 Found!");

  // set accelerometer range to +-8G
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G); // range of g's that can be read, +- 2 to +- 16 is available. The smaller the range, the more accurate the reported values. 

  // set gyro range to +- 500 deg/s
  mpu.setGyroRange(MPU6050_RANGE_500_DEG); // gyroscopic set range (no need to touch ever)

  // set filter bandwidth to 21 Hz
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ); //Default bandwidth used by accelerometer

  
  
 /* ***********************/
 // THERMOCOUPLE SETUP CODE


 /* ***********************/ 
 // LINEAR POTENTIOMETER SETUP CODE

  pinMode(potPin, INPUT); 

 /* ***********************/ 
 // TRANSCEIVER SETUP CODE
 
 HC12.begin(9600); // Serial port to HC12

while (true) {
  Serial.begin(9600);
 delay(4);
  // Serial.print("in da perma loop");
a_avg_x = accelerometerX(); // run code to collect acceleration data
a_avg_y = accelerometerY();



if (counter == 38) {
  
  temperature_1 = getTemperature_1(); // run code to collect temperature data
  temperature_2 = getTemperature_2(); 
  temperature_3 = getTemperature_3(); 
  temperature_4 = getTemperature_4(); 
 

 // delay(152);
    
  counter = 0;
}

  transmit(temperature_1, temperature_2, temperature_3, temperature_4, a_avg_x, a_avg_y); // run code to send the collected sensor data to the receiving transceiver


counter++;
// HC12.println(counter);
//milli = potentiometer(); // run code to collect potentiometer data

  }
}

void loop() {
Serial.print("stuck in void loop");
}

 double accelerometerX() {
  // === Read acceleromter data === //
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value
  Serial.print("Acceleration in x direction: ");
  Serial.println(AccX);
//  Serial.print("Acceleration in y direction: ");
 // Serial.println(AccY);
 
  for (int i = 0; i < 9; i++) {
    a_treadmill_x[i] == a_treadmill_x[i+1];
  }
  a_treadmill_x[9] = AccX;


  for (int i = 0; i <= 9; i++) {
    a_avg_x += a_treadmill_x[i];
  }

 /*  a_treadmill_y[0] = a_treadmill_y[1];
  a_treadmill_y[1] = a_treadmill_y[2];
  a_treadmill_y[2] = a_treadmill_y[3];
  a_treadmill_y[3] = a_treadmill_y[4];
  a_treadmill_y[4] = a_treadmill_y[5];
  a_treadmill_y[5] = a_treadmill_y[6];
  a_treadmill_y[6] = a_treadmill_y[7];
  a_treadmill_y[7] = a_treadmill_y[8];
  a_treadmill_y[8] = a_treadmill_y[9];
  a_treadmill_y[9] = AccY;

  a_avg_y = (a_treadmill_y[0] + a_treadmill_y[1] + a_treadmill_y[2] + a_treadmill_y[3] + a_treadmill_y[4] + a_treadmill_y[5] + a_treadmill_y[6] + a_treadmill_y[7] + a_treadmill_y[8] + a_treadmill_y[9]) / 10;
  
  */
  
  //Serial.print("Moving average: ");
  Serial.print(a_avg_x);
//  Serial.print('\t'); //needed for plotting multiple variables; must be placed after every variable



  return (a_avg_x);
  
}

double accelerometerY() {
  // === Read acceleromter data === //
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value
  Serial.print("Acceleration in x direction: ");
  Serial.println(AccX);
//  Serial.print("Acceleration in y direction: ");
 // Serial.println(AccY);

 a_treadmill_y[0] = a_treadmill_y[1];
  a_treadmill_y[1] = a_treadmill_y[2];
  a_treadmill_y[2] = a_treadmill_y[3];
  a_treadmill_y[3] = a_treadmill_y[4];
  a_treadmill_y[4] = a_treadmill_y[5];
  a_treadmill_y[5] = a_treadmill_y[6];
  a_treadmill_y[6] = a_treadmill_y[7];
  a_treadmill_y[7] = a_treadmill_y[8];
  a_treadmill_y[8] = a_treadmill_y[9];
  a_treadmill_y[9] = AccY;

  a_avg_y = (a_treadmill_y[0] + a_treadmill_y[1] + a_treadmill_y[2] + a_treadmill_y[3] + a_treadmill_y[4] + a_treadmill_y[5] + a_treadmill_y[6] + a_treadmill_y[7] + a_treadmill_y[8] + a_treadmill_y[9]) / 10;

  return (a_avg_y);
}

double getTemperature_1() {
  float temperature = thermocouple_1.readFahrenheit(); // library command for reading the fahrenheit module
 // Serial.print("Temperature: ");
 // Serial.print(temperature);
//  Serial.println(F("°C "));   
//  delay(1000);
  return temperature;
}

double getTemperature_2() {
  float temperature = thermocouple_2.readFahrenheit(); // library command for reading the fahrenheit module
 // Serial.print("Temperature: ");
 // Serial.print(temperature);
//  Serial.println(F("°C "));   
//  delay(1000);
  return temperature;
}

double getTemperature_3() {
  float temperature = thermocouple_3.readFahrenheit(); // library command for reading the fahrenheit module
 // Serial.print("Temperature: ");
 // Serial.print(temperature);
//  Serial.println(F("°C "));   
//  delay(1000);
  return temperature;
}

double getTemperature_4() {
  float temperature = thermocouple_4.readFahrenheit(); // library command for reading the fahrenheit module
 // Serial.print("Temperature: ");
 // Serial.print(temperature);
//  Serial.println(F("°C "));   
//  delay(1000);
  return temperature;
}

double potentiometer() {

   value = analogRead(potPin);          //Read and save analog value from potentiometer
  float calibration_metric = 52.0/1023;
  float calibration_imperial = 2.047/1023;

  float inches = calibration_imperial * value;
  float milli = calibration_metric * value;
//  Serial.print(inches);
//  Serial.println(" inches");
  //Serial.print("Raw value");
 // Serial.println(value);
  Serial.print(milli);
  Serial.println(" millimeters");
  // Serial.println(value);
  delay(100);                          //Small delay
  
  return (milli);
}

//Separate everything by commas!!!!

double transmit(double temp1, double temp2, double temp3, double temp4, double a_avg_x, double a_avg_y) {

//HC12.println(accel);
HC12.print(temp1);
HC12.print(",");
HC12.print(temp2);
HC12.print(",");
HC12.print(temp3);
HC12.print(",");
HC12.print(temp4);
HC12.print(", X g's: ");
HC12.print(a_avg_x);
HC12.print(", Y g's: ");
HC12.println(a_avg_y);



//HC12.println(millimeters);

  while (HC12.available()) {        // If HC-12 has data
    Serial.write(HC12.read());      // Send the data to Serial monitor
  }
  while (Serial.available()) {      // If Serial monitor has data
    HC12.write(Serial.read());      // Send that data to HC-12
  }
}
