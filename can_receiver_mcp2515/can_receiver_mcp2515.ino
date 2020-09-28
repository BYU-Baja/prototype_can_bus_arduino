// Receiver code

#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsg;
MCP2515 mcp2515(10);


void setup() {
  Serial.begin(115200);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
  
  Serial.println("------- CAN Read ----------");
  Serial.println("ID  DLC   DATA");
}

void loop() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    Serial.print(canMsg.can_id, HEX); // print ID
    Serial.print(" "); 
    Serial.print(canMsg.can_dlc, HEX); // print DLC
    Serial.print(" ");

    if (canMsg.can_id == 0x0F6) {
      byte tempHum[4];

      tempHum[0] = canMsg.data[0];
      tempHum[1] = canMsg.data[1];
      tempHum[2] = canMsg.data[2];
      tempHum[3] = canMsg.data[3];

      Serial.print("Temperature: ");
      Serial.print(*(float *)tempHum);
      Serial.print(", ");

      tempHum[0] = canMsg.data[4];
      tempHum[1] = canMsg.data[5];
      tempHum[2] = canMsg.data[6];
      tempHum[3] = canMsg.data[7];

      Serial.print("Humidity: ");
      Serial.print(*(float *)tempHum);
    } else {
      for (int i = 0; i<canMsg.can_dlc; i++)  {  // print the data
        Serial.print(canMsg.data[i],HEX);
        Serial.print(" ");
      }
    }

    Serial.println();      
  }
}
