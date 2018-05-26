#ifndef _KUNDARSA_SBS_H
#define _KUNDARSA_SBS_H

#ifndef Arduino_h
#include "Arduino.h"
#endif

#include "Wire.h"

class SBS {
public:
  struct commandSet{
    String slaveFunction;
    uint8_t code;
    bool writeable;
    int bytes; //make enum
    String type;
  } commands[38];
  
  /* enum type : uint8_t {
    a = 1,
    b = 2
  };
 */
  SBS(uint8_t address);
  byte sbsReadByte(uint8_t command);
  short sbsReadInt(uint8_t command);
  uint8_t sbsReadStringSize(uint8_t command);
  void sbsReadString(char *buf, uint8_t stringSize);

  float getFahrenheit();
private:
};


#endif
