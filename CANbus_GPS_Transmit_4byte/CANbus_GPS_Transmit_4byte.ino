
#include <SPI.h>
#include <mcp2515.h>
#include <TinyGPS++.h>
#include <SparkFun_I2C_GPS_Arduino_Library.h>
I2CGPS myI2CGPS;
TinyGPSPlus gps;

struct can_frame canMsg;
MCP2515 mcp2515(10);

String configString;
int n;
float spd;
float la;
float ln;

void float2Bytes(float val,byte* bytes_array){
  // Create union of shared memory space
  union {
    float float_variable;
    byte lat_array[4];
  } u;
  // Overite bytes of union with float variable
  u.float_variable = val;
  // Assign bytes to input array
  memcpy(bytes_array, u.lat_array, 4);
}


void setup() {
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
  
  while (!Serial);
  Serial.begin(115200);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();

  Serial.println("Read GPS location; transmit data on CAN Bus.");
  
  Serial.println("Initializing sensor...");

  if (myI2CGPS.begin() == false)
  {
    Serial.println("Module failed to respond. Please check wiring and reset.");
    while (1); //Freeze!
  }
  Serial.println("GPS module found!");

  configString = myI2CGPS.createMTKpacket(220, ",100"); //Configures a packet to set the GPS update rate to 10Hz. (Set 100 for 10Hz, 500 for 2Hz, 1000 for 1Hz, etc.)
        myI2CGPS.sendMTKpacket(configString); //Sends the packet

  configString = myI2CGPS.createMTKpacket(251, ",115200"); //Configures a packet to set the GPS baude rate to 115200
            myI2CGPS.sendMTKpacket(configString); //Sends the packet
            
}

void loop() {

    while (myI2CGPS.available()){ //available() returns the number of new bytes available from the GPS module
    gps.encode(myI2CGPS.read()); //Feed the GPS parser
  }

  if (gps.time.isUpdated()){ //Check to see if new GPS info is available
    displayInfo(); //Runs the function to output GPS data
  }
}

  //Display new GPS info
void displayInfo()
{
  if (gps.time.isValid()) //Checks if the GPS has valid data for time
  {
    
  }
  else
  {
    Serial.println(F("Time not yet valid"));
  }


  if (gps.location.isValid()) //Checks if the GPS has valid location data
  {
    if (gps.location.age() > 2000 && gps.location.isValid()) //Runs if the location data has not been updated in 2 seconds, indicating a loss of signal
    {
      Serial.println();
      Serial.print("Location Lost... Reacquiring Signal");
      delay(1000);
    }    
    n = n+1;
    spd = spd+gps.speed.mph(); //Assigns the variable to the speed of the GPS sensor in MPH
  }
  else
  {
   Serial.println(F("Acquiring Location")); //Runs if the GPS has not acquired an initial signal
   delay(1000);
  }
  
  
  if (n==10) //Change value of n to update Hz
  {
    la = gps.location.lat();
    ln = gps.location.lng();
    
      byte gpslocation[4];
  float2Bytes(la,&gpslocation[0]);

  canMsg.data[0] = gpslocation[0];
  canMsg.data[1] = gpslocation[1];
  canMsg.data[2] = gpslocation[2];
  canMsg.data[3] = gpslocation[3];

  float2Bytes(ln,&gpslocation[0]);

  canMsg.data[4] = gpslocation[0];
  canMsg.data[5] = gpslocation[1];
  canMsg.data[6] = gpslocation[2];
  canMsg.data[7] = gpslocation[3];

   mcp2515.sendMessage(&canMsg);

  n=0;
  }
}
