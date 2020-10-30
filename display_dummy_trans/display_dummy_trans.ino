#include <Wire.h>

#define SLAVE_ADDR 0x16

#define SPEED_ID 0x01
#define RPM_ID 0x02
#define FUEL_LEVEL_ID 0x03

#define SPEED_COUNTER_MAX 100

void sendRPMData();
void sendFuelLevelData();
void sendSpeedData();

uint8_t speed = 30;
uint16_t rpm = 0;
float fuel_level = 100.0;

uint32_t speed_counter = 0;

union float_byte_array {
  float f;
  uint8_t array[4];
};

void setup()
{
  Wire.begin();
}

void loop()
{
  sendRPMData();
  sendFuelLevelData();

  if (speed_counter >= SPEED_COUNTER_MAX) {
    sendSpeedData();
    speed_counter = 0;
  } else 
    speed_counter++;

  delay(10);
}

void sendRPMData() {
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(RPM_ID);
  Wire.write(rpm & 0xFF);
  Wire.write(rpm >> 8);
  Wire.endTransmission();

  rpm++; // We can just roll over rpm
}

void sendFuelLevelData() {
  float_byte_array converter;
  converter.f = fuel_level;
  
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(FUEL_LEVEL_ID);
  Wire.write(converter.array[0] & 0xFF);
  Wire.write(converter.array[1] >> 8);
  Wire.write(converter.array[2] >> 16);
  Wire.write(converter.array[3] >> 24);
  Wire.endTransmission();

  fuel_level -= 0.1; // We can just roll over rpm
}

void sendSpeedData() {
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(SPEED_ID);
  Wire.write(speed);
  Wire.endTransmission();
  
  if (random(0, 10) > 4)
    speed++;
  else
    speed++;
}
