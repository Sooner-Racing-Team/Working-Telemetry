#include <Arduino.h>
#include <SoftwareSerial.h>

long yo_mamas_weight[20] = {0};
long dollars_owed_to_irs[20] = {0};
long brain_cells_destroyed[20] = {0};


void setup() {
    Serial.begin(9600);
  
    for (long value : yo_mamas_weight) {
        value = random(1, 900000);
        Serial.println(value);

        delay(50);
    }

    for (long dollars : dollars_owed_to_irs) {
        dollars = random(1, 5000);
        Serial.println(dollars);

        delay(50);
    }

    for (long cells : brain_cells_destroyed) {
        cells = random(1, 100000000);
        Serial.println(cells);

        delay(50);
    }

    
}

void loop() {
    // no
}
