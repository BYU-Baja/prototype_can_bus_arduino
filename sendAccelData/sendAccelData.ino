
// Basic demo for accelerometer readings from Adafruit LIS3DH

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include <sx_pro.h>

// Used for software SPI
#define LIS3DH_CLK 13
#define LIS3DH_MISO 12
#define LIS3DH_MOSI 11
// Used for hardware & software SPI
#define LIS3DH_CS 10

// software SPI
//Adafruit_LIS3DH lis = Adafruit_LIS3DH(LIS3DH_CS, LIS3DH_MOSI, LIS3DH_MISO, LIS3DH_CLK);
// hardware SPI
//Adafruit_LIS3DH lis = Adafruit_LIS3DH(LIS3DH_CS);
// I2C
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

SXPro module = SXPro(9, false);

void readParam(char *param) {
    Serial.print(param);
    Serial.print(": ");
    Serial.println(module.readParam(param));
}

void setup(void) {
  Serial.begin(9600);
  while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("LIS3DH test!");

  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1) yield();
  }
  Serial.println("LIS3DH found!");

  // lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!

  Serial.print("Range = "); Serial.print(2 << lis.getRange());
  Serial.println("G");

  // lis.setDataRate(LIS3DH_DATARATE_50_HZ);
  Serial.print("Data rate set to: ");
  switch (lis.getDataRate()) {
    case LIS3DH_DATARATE_1_HZ: Serial.println("1 Hz"); break;
    case LIS3DH_DATARATE_10_HZ: Serial.println("10 Hz"); break;
    case LIS3DH_DATARATE_25_HZ: Serial.println("25 Hz"); break;
    case LIS3DH_DATARATE_50_HZ: Serial.println("50 Hz"); break;
    case LIS3DH_DATARATE_100_HZ: Serial.println("100 Hz"); break;
    case LIS3DH_DATARATE_200_HZ: Serial.println("200 Hz"); break;
    case LIS3DH_DATARATE_400_HZ: Serial.println("400 Hz"); break;

    case LIS3DH_DATARATE_POWERDOWN: Serial.println("Powered Down"); break;
    case LIS3DH_DATARATE_LOWPOWER_5KHZ: Serial.println("5 Khz Low Power"); break;
    case LIS3DH_DATARATE_LOWPOWER_1K6HZ: Serial.println("16 Khz Low Power"); break;
  }

      
    Serial5.begin(9600);
    Wire.begin();
    module.begin(Serial5);

    if (module.enterCommandMode()) {
        Serial.println("Entered command mode.");
        module.reset();
        delay(2000);
        readParam("AF");
        readParam("ID");
        readParam("BR");
        readParam("PL");
        readParam("NI");
        module.setParam("NI", "Baja Vehicle");
        module.setParam("ID", "2015");
        readParam("NI");
        readParam("ID");
        if (module.exitCommandMode())
            Serial.println("Exited command mode.");
    }

    Serial.println("Done.");
}

void loop() {
  lis.read();      // get X Y and Z data at once
  // Then print out the raw data
  Serial.print("X:  "); Serial.print(lis.x);
  Serial.print("  \tY:  "); Serial.print(lis.y);
  Serial.print("  \tZ:  "); Serial.print(lis.z);

  /* Or....get a new sensor event, normalized */
  sensors_event_t event;
  lis.getEvent(&event);

  /* Display the results (acceleration is measured in m/s^2) */
  Serial5.print("\t\tX: "); Serial5.print(event.acceleration.x);
  Serial5.print(" \tY: "); Serial5.print(event.acceleration.y);
  Serial5.print(" \tZ: "); Serial5.print(event.acceleration.z);
  Serial5.println(" m/s^2 ");

  Serial.println();
/*
    if (Serial.available()) { // If anything comes in Serial (USB),

        Serial5.write(Serial.read()); // read it and send it out Serial1 (pins 0 & 1)
    }

    if (Serial5.available()) { // If anything comes in Serial1 (pins 0 & 1)

        Serial.write(Serial5.read()); // read it and send it out Serial (USB)
    }
*/
  delay(200);
}
