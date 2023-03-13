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
// Collects position information for stability data
struct PotentiometerData {
    double millimeters = 0.0;
    double inches = 0.0;
};

PotentiometerData position;

// Temperature Data Struct
// Collects temperature information from thermocouples
struct TemperatureData {
    double temperatures_in_fahrenheit[4] = {0.0};
};

TemperatureData temperature;

// Track time
unsigned long time;

/**
 * Entry point for Arduino.
 *
 * Begins the process of collecting and running code.
 */
void setup() {
    // Final bits of setup
    // Accelerometer
    hardware.mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    hardware.mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    hardware.mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    // Start connection to getter
    hardware.HC12.begin(9600);

    // Main telemetry loop
    while (true) {
        // Get acceleration average
        fill_acceleration(x);
        fill_acceleration(y);
        fill_acceleration(z);

        if (!check_acceleration_average_valid()) {
            continue;
        }

        // Get temperatures
        fill_temperature();

        if (!check_temperatures_valid()) {
            continue;
        }

        // Get position average
        // UNIMPLEMENTED

        // attempt transmission
        transmit();
    }
}

/**
 * Returns newest acceleration number from given direction in m/s^2.
 *
 * Treat -1234.0 as an error state!
 */
double get_acceleration(Direction direction) {
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
        Serial.println("Invalid parameter passed to getAcceleration...");
        return -1234.0;
    }
}

/**
 * Gets 10 acceleration values, then sets acceleration.average_x/y/z
 * accordingly.
 */
void fill_acceleration(Direction direction) {
    double *values;

    // from direction parameter, pick the array to edit
    switch (direction) {
    case x:
        values = acceleration.values_x;
        break;
    case y:
        values = acceleration.values_y;
        break;
    case z:
        values = acceleration.values_z;
        break;
    default:
        Serial.println("Invalid parameter passed to fillAcceleration...");
        return;
    }

    for (int i = 0; i < 10; i++) {
        values[i] = get_acceleration(direction);
    }
}

/**
 * Checks the validitity of a given acceleration direction's current average.
 *
 * In other words, this functions looks for "-1234.0" and gets upset if it finds
 * anything... :)
 */
bool check_acceleration_average_valid() {
    double accel_averages[3] = {acceleration.average_x, acceleration.average_y,
                                acceleration.average_z};
    for (double average : accel_averages) {
        if (average + 0.01 >= -1234 && average - 0.01 <= -1234) {
            Serial.println("An accel average was not properly initialized.");
            return false;
        }
    }
    return true;
}

/**
 * Returns temperature data from the selected thermocouple in Fahrenheit.
 *
 * Each sensor updates only once every 250 milliseconds.
 * Calling this function any more than that will result in duplicate values.
 *
 * Treat -1234.0 as an error state!
 */
double get_temperature(int thermocouple_index) {
    if (thermocouple_index >= 0 && thermocouple_index < 4) {
        return hardware.thermocouples[thermocouple_index].readFahrenheit();
    }

    return -1234.0;
}

/**
 * Sets each temperature value to the sensor's reading.
 */
void fill_temperature() {
    for (int i = 0; i < 4; i++) {
        temperature.temperatures_in_fahrenheit[i] = get_temperature(i);
    }
}

/**
 * Checks to see if any temperature value is invalid.
 */
bool check_temperatures_valid() {
    for (double temp : temperature.temperatures_in_fahrenheit) {
        if (temp + 0.01 >= -1234 && temp - 0.01 <= -1234) {
            Serial.println("A temp average was not properly initialized.");
            return false;
        }
    }
    return true;
}

/**
 * UNIMPLEMENTED: Returns potentiometer's average postional value.
 *
 * TODO: make this work when we get potentiometer info
 *
 * Treat -1234.0 as an error state!
 */
double get_position() { return -1234.0; }

/**
 * Uses a generic to print out a given value with a comma.
 *
 * For use in transmit().
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
bool transmit() {
    if (hardware.HC12.isListening()) {
        // when necessary, flush remaining data
        if (hardware.HC12.available() != 0) {
            hardware.HC12.flush();
        }

        // version info
        HC12_comma_print(sender_version);

        // temperature data
        for (double temp : temperature.temperatures_in_fahrenheit) {
            HC12_comma_print(temp);
        }

        // acceleration data
        HC12_comma_print(acceleration.average_x);
        HC12_comma_print(acceleration.average_y);
        // HC12_comma_print(acceleration.average_z);

        // potentiometer data
        // (nothing here yet)

        // time data
        hardware.HC12.print(millis());
        hardware.HC12.println();

        return true;
    }
    Serial.println("HC12 is not able to receieve data. transmit() failure!");
    return false;
}

/**
 * Required loop() function in Arduino.
 *
 * Generally shouldn't activate.
 */
void loop() {
    Serial.print("Loop function accessed. Please restart the sender.");
    delay(2000);
}
