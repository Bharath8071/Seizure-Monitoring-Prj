#include <Wire.h>

#define LSM6DSO_ADDR  0x6A // Change to 0x6B if SDO is high

void writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(LSM6DSO_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

uint8_t readRegister(uint8_t reg) {
    Wire.beginTransmission(LSM6DSO_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(LSM6DSO_ADDR, 1);
    return Wire.read();
}

void readSensorData(uint8_t reg, int16_t &x, int16_t &y, int16_t &z) {
    Wire.beginTransmission(LSM6DSO_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(LSM6DSO_ADDR, 6);
    
    x = Wire.read() | (Wire.read() << 8);
    y = Wire.read() | (Wire.read() << 8);
    z = Wire.read() | (Wire.read() << 8);
}

void setup() {
    Serial.begin(115200);
    Wire.begin(21, 22); // ESP32 Default SDA = 21, SCL = 22

    // Check WHO_AM_I Register
    uint8_t whoAmI = readRegister(0x0F);
    Serial.print("WHO_AM_I: 0x");
    Serial.println(whoAmI, HEX);
    
    if (whoAmI != 0x6C) {
        Serial.println("LSM6DSO not detected! Check connections.");
        while (1);
    } else {
        Serial.println("LSM6DSO found!");
    }

    // Configure Accelerometer (CTRL1_XL)
    writeRegister(0x10, 0b10000000); // ODR=104Hz, ±2g, BW=400Hz

    // Configure Gyroscope (CTRL2_G)
    writeRegister(0x11, 0b10000000); // ODR=104Hz, ±250 dps
}

void loop() {
    int16_t ax, ay, az, gx, gy, gz;

    // Read accelerometer data
    readSensorData(0x28, ax, ay, az);

    // Read gyroscope data
    readSensorData(0x22, gx, gy, gz);

    // Convert raw values (LSM6DSO outputs 16-bit signed values)
    float accelX = ax * 0.061 * (2.0 / 32768.0);
    float accelY = ay * 0.061 * (2.0 / 32768.0);
    float accelZ = az * 0.061 * (2.0 / 32768.0);

    float gyroX = gx * 8.75 * (250.0 / 32768.0);
    float gyroY = gy * 8.75 * (250.0 / 32768.0);
    float gyroZ = gz * 8.75 * (250.0 / 32768.0);

    // Print data
    Serial.print("Accel (m/s^2): X=");
    Serial.print(accelX);
    Serial.print(" Y=");
    Serial.print(accelY);
    Serial.print(" Z=");
    Serial.println(accelZ);

    Serial.print("Gyro (dps): X=");
    Serial.print(gyroX);
    Serial.print(" Y=");
    Serial.print(gyroY);
    Serial.print(" Z=");
    Serial.println(gyroZ);

    delay(500);
}
