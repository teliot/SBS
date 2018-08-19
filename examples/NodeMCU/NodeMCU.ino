#define CONSOLE_BAUD 115200

// Acer battery AL12X32 has this address, use an i2cdetect sketch to get your battery address
#define BATTERY_ADDRESS 0x0B
#define BATTERY_SCL D1 //nodeMCU pin marked D1
#define BATTERY_SDA D2 //nodeMCU pin marked D2

#include <SBS.h> // this library should be in Arduino\libraries\SBS
SBS battery = SBS(BATTERY_ADDRESS, BATTERY_SDA, BATTERY_SCL);

void setup() {
  Serial.begin(CONSOLE_BAUD);
  delay(100); //for sanity
  printBatteryRegisters();
}

void loop() {
  delay(1000 * 60 *12);
}

void printBatteryRegisters() {
  // print over serial the data for each register we know about, handeling each register based on type.
  for(int i = 1; i < 38; i++) {
    Serial.print(battery.commands[i].slaveFunction + " "); // print the register name
    if(battery.commands[i].type == "Boolean") { //only used for atRateOk register
      Serial.println(battery.sbsReadByte(battery.commands[i].code) == 0x00 ? "FALSE" : "TRUE");
    } // Long block of simple 2 digit numbers.
    else if(battery.commands[i].type == "mV") { // divide by 1000 for Volts
      Serial.print((int)battery.sbsReadInt(battery.commands[i].code));
      Serial.println(" " + battery.commands[i].type);
    }
    else if(battery.commands[i].type == "mAh") { // divide by 1000 for Amps
      Serial.print((int)battery.sbsReadInt(battery.commands[i].code));
      Serial.println(" " + battery.commands[i].type);
    }
    else if(battery.commands[i].type == "minutes") { // divide by 60 for hours
      Serial.print((int)battery.sbsReadInt(battery.commands[i].code));
      Serial.println(" " + battery.commands[i].type);
    }
    else if(battery.commands[i].type == "percent") { // divide by 100 for decimal
      Serial.print((int)battery.sbsReadInt(battery.commands[i].code));
      Serial.println(" " + battery.commands[i].type);
    }
    else if(battery.commands[i].type == "count") { // only used for CycleCount register
      Serial.println((int)battery.sbsReadInt(battery.commands[i].code));
    }
    else if(battery.commands[i].type == "number") { // only used for SerialNumber register
      Serial.println((int)battery.sbsReadInt(battery.commands[i].code));
    }
    else if(battery.commands[i].type == "0.1K") { // only used for temperature register
      Serial.print(((int)battery.sbsReadInt(battery.commands[i].code) / 10 - 273.15) * 1.8 + 32); //register is in kelvin, this math changes to C, and converts to F
      Serial.println("F");
    }
    else if(battery.commands[i].type == "mA") { // divide by 1000 for Amps
      Serial.print((int)battery.sbsReadInt(battery.commands[i].code));
      Serial.println(" " + battery.commands[i].type);
    }
    else if(battery.commands[i].slaveFunction == "ManufactureDate") { // only one date register
	  int raw = (int)battery.sbsReadInt(battery.commands[i].code);
	  Serial.print((raw >> 5 ) & 0xF); Serial.print("/");
	  Serial.print(raw & 0xF); Serial.print("/");
      Serial.println((raw >> 9 ) + 1980);
    }
    else
    Serial.println("?");
  }
}

