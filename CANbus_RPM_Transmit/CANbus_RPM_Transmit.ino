#include <mcp2515.h>
#include <SPI.h>

MCP2515 mcp2515(10);

unsigned long risetime;
volatile byte interrupt = 0;
unsigned long lastrisetime;
float timedifference;
float conversion = 60000000/2;
float RPM = 0;
const byte outputPin = 8;
const byte interruptPin = 3;
struct can_frame canMsg;

void float2Bytes(float val,byte* bytes_array){
  // Create union of shared memory space
  union {
    float float_variable;
    byte rpm_array[4];
  } u;
  // Overite bytes of union with float variable
  u.float_variable = val;
  // Assign bytes to input array
  memcpy(bytes_array, u.rpm_array, 4);
}

void setup() {
  // put your setup code here, to run once:
  canMsg.can_id  = 0x0F5;
  canMsg.can_dlc = 8;
  canMsg.data[0] = 0x8E;
  canMsg.data[1] = 0x87;
  canMsg.data[2] = 0x32;
  canMsg.data[3] = 0xFA;
  canMsg.data[4] = 0x26;
  canMsg.data[5] = 0x8E;
  canMsg.data[6] = 0xBE;
  canMsg.data[7] = 0x86;
  
  pinMode(outputPin, OUTPUT);
  digitalWrite(outputPin, HIGH);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3), Interrupt, RISING);
  Serial.begin(115200);

  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();

}

void loop() {
  // put your main code here, to run repeatedly:
if (interrupt == 1){
  interrupt = 0;
  risetime= micros();
  timedifference = risetime - lastrisetime;
  RPM = conversion/timedifference;
  Serial.println(RPM);
  lastrisetime = risetime;
  
  byte rpmvalue[4];
  
  float2Bytes(RPM,&rpmvalue[0]);
  canMsg.data[0] = rpmvalue[0];
  canMsg.data[1] = rpmvalue[1];
  canMsg.data[2] = rpmvalue[2];
  canMsg.data[3] = rpmvalue[3];

  mcp2515.sendMessage(&canMsg);
  }
}

void Interrupt() {
  interrupt = 1;
}
