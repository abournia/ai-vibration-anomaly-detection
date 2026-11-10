#include <Arduino.h>
#include <Wire.h>

void setup() {
    Serial.begin(115200);

    // SDA = GPIO4 (D2)
    // SCL = GPIO5 (D1)
    Wire.begin(4, 5);

    // Put the FXOS8700 into active mode
    Wire.beginTransmission(0x1F);
    Wire.write(0x2A);  // CTRL_REG1
    Wire.write(0x01);  // ACTIVE = 1
    Wire.endTransmission();
}

void loop() {

    // Tell the FXOS8700 we want to start reading at X_MSB
    Wire.beginTransmission(0x1F);
    Wire.write(0x01);  // X_MSB
    Wire.endTransmission(false);

    // Request X, Y, and Z data
    // X = registers 0x01-0x02
    // Y = registers 0x03-0x04
    // Z = registers 0x05-0x06
    int bytesReceived = Wire.requestFrom(0x1F, 6);

    if (bytesReceived == 6) {

        // Read all 6 bytes
        int xMSB = Wire.read();
        int xLSB = Wire.read();

        int yMSB = Wire.read();
        int yLSB = Wire.read();

        int zMSB = Wire.read();
        int zLSB = Wire.read();

        // Combine the two bytes for each axis
        uint16_t rawX = ((xMSB << 8) | xLSB) >> 2;
        uint16_t rawY = ((yMSB << 8) | yLSB) >> 2;
        uint16_t rawZ = ((zMSB << 8) | zLSB) >> 2;

        // Convert 14-bit two's-complement values to signed values
        int16_t x = rawX;
        int16_t y = rawY;
        int16_t z = rawZ;

        if (rawX & 0x2000) {
            x -= 16384;
        }

        if (rawY & 0x2000) {
            y -= 16384;
        }

        if (rawZ & 0x2000) {
            z -= 16384;
        }

        // Convert raw counts to acceleration in g
        float xG = x / 4096.0;
        float yG = y / 4096.0;
        float zG = z / 4096.0;

        // Print the results
        Serial.print("X: ");
        Serial.print(xG);
        Serial.print(" g    ");

        Serial.print("Y: ");
        Serial.print(yG);
        Serial.print(" g    ");

        Serial.print("Z: ");
        Serial.print(zG);
        Serial.println(" g");
    }

    delay(500);
}