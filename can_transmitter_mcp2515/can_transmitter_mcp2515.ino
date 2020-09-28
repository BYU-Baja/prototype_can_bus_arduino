#include <DHT.h>
#include <DHT_U.h>
#include <SPI.h>
#include <mcp2515.h>

#define DHTTYPE DHT11   // DHT 11
#define DHT_PIN 7
#define DHT_PWR_PIN 6

struct can_frame canMsg;
MCP2515 mcp2515(10);
DHT dht(DHT_PIN, DHTTYPE);

void float2Bytes(float val,byte* bytes_array){
  // Create union of shared memory space
  union {
    float float_variable;
    byte temp_array[4];
  } u;
  // Overite bytes of union with float variable
  u.float_variable = val;
  // Assign bytes to input array
  memcpy(bytes_array, u.temp_array, 4);
}

void setup() {
  canMsg.can_id  = 0x0F6;
  canMsg.can_dlc = 8;
  canMsg.data[0] = 0x8E;
  canMsg.data[1] = 0x87;
  canMsg.data[2] = 0x32;
  canMsg.data[3] = 0xFA;
  canMsg.data[4] = 0x26;
  canMsg.data[5] = 0x8E;
  canMsg.data[6] = 0xBE;
  canMsg.data[7] = 0x86;
  
  while (!Serial);
  Serial.begin(115200);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();

  pinMode(DHT_PWR_PIN, OUTPUT);
  digitalWrite(DHT_PWR_PIN, HIGH);
  Serial.println("Read temp and humidity sensor and transmit data on CAN Bus.");

  dht.begin();
  
  Serial.println("Initializing sensor...");
  delay(2000); // Wait for the sensor to be initialized before running
}

void loop() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  Serial.print("Temp: ");
  Serial.print(f);
  Serial.print(", hum: ");
  Serial.println(h);

  byte tempHum[4];
  float2Bytes(f,&tempHum[0]);

  canMsg.data[0] = tempHum[0];
  canMsg.data[1] = tempHum[1];
  canMsg.data[2] = tempHum[2];
  canMsg.data[3] = tempHum[3];

  float2Bytes(h,&tempHum[0]);

  canMsg.data[4] = tempHum[0];
  canMsg.data[5] = tempHum[1];
  canMsg.data[6] = tempHum[2];
  canMsg.data[7] = tempHum[3];
  
  mcp2515.sendMessage(&canMsg);
  
  delay(1000);
}
