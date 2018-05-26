#include "SBS.h"

SBS::commandSet commands[38];
uint8_t smbusAddress;


SBS::SBS(uint8_t address) {
  //              Slave Function        Address,Writable?, byte count
  commands[0]  = {"ManufacturerAccess",     0x00, true,  1, "word"};
  commands[1]  = {"RemainingCapacityAlarm", 0x01, true,  2, "mAh"};
  commands[2]  = {"RemainingTimeAlarm",     0x02, true,  2, "minutes"};
  commands[3]  = {"BatteryMode",            0x03, true,  2, "bit flags"};
  commands[4]  = {"AtRate",                 0x04, true,  2, "mA"};
  commands[5]  = {"AtRateTimeToFull",       0x05, false, 2, "minutes"};
  commands[6]  = {"AtRateTimeToEmpty",      0x06, false, 2, "minutes"};
  commands[7]  = {"AtRateOK",               0x07, false, 1, "Boolean"};
  commands[8]  = {"Temperature",            0x08, false, 2, "0.1K"};
  commands[9]  = {"Voltage",                0x09, false, 2, "mV"};
  commands[10] = {"Current",                0x0a, false, 2, "mA"};
  commands[11] = {"AverageCurrent",         0x0b, false, 2, "mA"};
  commands[12] = {"MaxError",               0x0c, false, 2, "percent"};
  commands[13] = {"RelativeStateOfCharge",  0x0d, false, 2, "percent"};
  commands[14] = {"AbsoluteStateOfCharge",  0x0e, false, 2, "percent"};
  commands[15] = {"RemainingCapacity",      0x0f, false, 2, "mAh"};
  commands[16] = {"FullChargeCapacity",     0x10, false, 2, "mAh"};
  commands[17] = {"RunTimeToEmpty",         0x11, false, 2, "minutes"};
  commands[18] = {"AverageTimeToEmpty",     0x12, false, 2, "minutes"};
  commands[19] = {"AverageTimeToFull",      0x13, false, 2, "minutes"};
  commands[20] = {"ChargingCurrent",        0x14, false, 2, "mA"};
  commands[21] = {"ChargingVoltage",        0x15, false, 2, "mV"};
  commands[22] = {"BatteryStatus",          0x16, false, 2, "bit flags"};
  commands[23] = {"CycleCount",             0x17, false, 2, "count"};
  commands[24] = {"DesignCapacity",         0x18, false, 2, "mAh"};
  commands[25] = {"DesignVoltage",          0x19, false, 2, "mV"};
  commands[26] = {"SpecificationInfo",      0x1a, false, 2, "unsigned int"};
  commands[27] = {"ManufactureDate",        0x1b, false, 2, "unsigned int"};
  commands[28] = {"SerialNumber",           0x1c, false, 2, "number"};
  //reserved 0x1d - 0x1f
  commands[29] = {"ManufacturerName",       0x20, false, 0, "string"};
  commands[30] = {"DeviceName",             0x21, false, 1, "string"};
  commands[31] = {"DeviceChemistry",        0x22, false, 1, "string"};
  commands[32] = {"ManufacturerData",       0x23, false, 1, "data"};
  commands[33] = {"OptionalMfgFunction5",   0x2f, false, 1, "data"};
  commands[34] = {"VoltageCellFour",        0x3c, false, 1, "mV"};
  commands[35] = {"VoltageCellThree",       0x3d, false, 1, "mV"};
  commands[36] = {"VoltageCellTwo",         0x3e, false, 1, "mV"};
  commands[37] = {"VoltageCellOne",         0x3f, false, 1, "mV"};

  smbusAddress = address;
  
  Wire.begin();
  Wire.setClockStretchLimit(35000);
  Wire.setClock(40000);
}

byte SBS::sbsReadByte(uint8_t command) {
  byte b;
  Wire.beginTransmission(smbusAddress);
  Wire.write(command);
  if(Wire.endTransmission(false) == 0) {
    Wire.requestFrom(smbusAddress, 1, true);
    b = Wire.read();
    return b;
  }
  return 0;
}

short SBS::sbsReadInt(uint8_t command) {
  uint8_t b1, b2;
  Wire.beginTransmission(smbusAddress);
  Wire.write(command);
  if(Wire.endTransmission(false) == 0) {
    Wire.requestFrom(smbusAddress, 2, true);
    b1 = Wire.read();
    b2 = Wire.read();
    return b1 | (b2 << 8);
  }
  return 0;
}


//TODO: strings can be a max of 32 chars so just assume that +1 and toss in a /n
uint8_t SBS::sbsReadStringSize(uint8_t command) {
  Wire.beginTransmission(command);
  Wire.write(command);
  if(Wire.endTransmission(false) == 0) {
    Wire.requestFrom(smbusAddress, 1, false);
    int n = Wire.read();
    if(n>0)
      return Wire.read();
    else
      Wire.endTransmission();
      return 0;
  }
  return 0;
}

void SBS::sbsReadString(char *buf, uint8_t stringSize) {
  for(int i = 0; i < stringSize; i++) {
    buf[i] = Wire.read();
  }
}



float SBS::getFahrenheit() {
  return ((float)sbsReadInt(0x08)/10 - 273.15) * 1.8 + 32;
}

