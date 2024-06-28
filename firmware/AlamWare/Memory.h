#ifndef MEMORY_H
#define MEMORY_H

#include <EEPROM.h>

class Memory {
 private:
  float kp;
  float kd;
  float ki;

  uint8_t kpAddress = 10;
  uint8_t kiAddress = 12;
  uint8_t kdAddress = 14;

  int lastPositions[6];
  uint8_t pAddresses[6];

 public:
  Memory();
  ~Memory();

  int GetLastPosition(uint8_t actuatorId);
  void SavePosition(uint8_t actuatorId, int position);
  void GetTunings(float* kp, float* ki, float* kd);
  void SaveTunings(float kp, float ki, float kd);
};

#endif
