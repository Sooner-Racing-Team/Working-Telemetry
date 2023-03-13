#include "max6675.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ArduinoSTL.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <math.h>

//////////////////////////////////
// Hardcoded hardware setup values
// (change these if necessary)

// Version information
const char sender_version[] = "0.0.0"; // replaces old "transmit key"
const int protocol_version = 0;        // increase when making breaking changes

// Thermocouple
int thermo_DO = 29;                  // data output pin
int thermo_CLK = 28;                 // clock pin
int thermo_CS[4] = {34, 36, 38, 40}; // chip select pins

// Potentiometer
const int pot_pin = A3; // pin A0 to read analog input
int pot_value;          // save analog value

// Transceiver
int transceiver_tx_pin = 3;
int transceiver_rx_pin = 2;

//////////////////////
// Initialize hardware

struct Hardware {
    Adafruit_MPU6050 mpu;
    MAX6675 thermocouples[4] = {
        MAX6675(thermo_CLK, thermo_CS[0], thermo_DO),
        MAX6675(thermo_CLK, thermo_CS[1], thermo_DO),
        MAX6675(thermo_CLK, thermo_CS[2], thermo_DO),
        MAX6675(thermo_CLK, thermo_CS[3], thermo_DO),
    };
    SoftwareSerial HC12 =
        SoftwareSerial(transceiver_rx_pin, transceiver_tx_pin);
};

Hardware hardware;

// Direction for acceleration
enum Direction { x, y, z };

/////////////////////
// Data structures :)

// Acceleration Data Struct
// Contains reimplementation of "treadmill"
struct AccelerationData {
    double values_x[10] = {0.0};
    double values_y[10] = {0.0};
    double values_z[10] = {0.0};
    double average_x = 0.0;
    double average_y = 0.0;
    double average_z = 0.0;
};

AccelerationData acceleration;

// Potentiometer Data Struct
// Collections position information for stability data
struct PotentiometerData {
    double millimeters;
    double inches;
};

PotentiometerData position;

// Track time
unsigned long time;

void setup() {
    // Final bits of setup
    // Accelerometer
    hardware.mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    hardware.mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    hardware.mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    // Start connection to getter
    hardware.HC12.begin(9600);
    time = millis(); // todo: add to loop since it's just the board's uptime
}

/**
 * Returns newest acceleration number from given direction in m/s^2.
 *
 *
 */
double getAcceleration(Direction direction) {
    sensors_event_t *accel, *_gyro, *_temp;
    hardware.mpu.getEvent(accel, _gyro, _temp);

    switch (direction) {
    case x:
        return accel->acceleration.x;
        break;
    case y:
        return accel->acceleration.y;
        break;
    case z:
        return accel->acceleration.z;
        break;
    default:
        return 0.0;
    }
}

/**
 * Returns temperature data from the selected thermocouple in Fahrenheit.
 *
 * Each sensor updates only once every 250 milliseconds.
 * Calling this function any more than that will result in duplicate values.
 *
 * Treat 0.0 as an error state!
 */
double getTemperature(int thermocouple_index) {
    if (thermocouple_index >= 0 && thermocouple_index < 4) {
        return hardware.thermocouples[thermocouple_index].readFahrenheit();
    }

    return 0.0;
}

/**
 * UNIMPLEMENTED: Returns potentiometer's postional information.
 *
 * TODO: make this work when we get potentiometer info
 */
double getPosition() {
    return 0.0; // TODO
}

/**
 * Uses a generic to print out a given value with a comma.
 */
template <typename T> void HC12_comma_print(T info) {
    hardware.HC12.print(info);
    hardware.HC12.print(",");
}

/**
 * Sends the collected data to the receiver.
 *
 * Returns bool representing whether it actually sent data or not.
 */
boolean transmit() {
    if (hardware.HC12.isListening()) {
        // flush remaining data
        hardware.HC12.flush();

        // version info
        HC12_comma_print(sender_version);

        // temperature data
        HC12_comma_print("REPLACE_ME: temp_sensor_1_data");
        HC12_comma_print("REPLACE_ME: temp_sensor_2_data");
        HC12_comma_print("REPLACE_ME: temp_sensor_3_data");
        HC12_comma_print("REPLACE_ME: temp_sensor_4_data");

        // acceleration data
        HC12_comma_print(acceleration.average_x);
        HC12_comma_print(acceleration.average_y);
        // HC12_comma_print(acceleration.average_z);

        // potentiometer data
        // (nothing here yet)

        // time data
        hardware.HC12.print(time);
        hardware.HC12.println();

        return true;
    }
    return false; // TODO
}

// Generally shouldn't activate...
void loop() {
    Serial.print("Loop function accessed. Please restart the sender.");
    delay(2000);
}
