#include "Memory.h"

Memory::Memory() {
  for (int i = 0; i < 6; i++) {
    pAddresses[i] = 20 + sizeof(float) * i;
  }
}

Memory::~Memory() {
}

void Memory::SavePosition(uint8_t actuatorId, int position) {
  EEPROM.put(pAddresses[actuatorId], position);
}

int Memory::GetLastPosition(uint8_t actuatorId) {
  return lastPositions[actuatorId];
}

void Memory::SaveTunings(float kp, float ki, float kd) {
  EEPROM.put(kpAddress, kp);
  EEPROM.put(kiAddress, ki);
  EEPROM.put(kdAddress, kd);
}

void Memory::GetTunings(float* kp, float* ki, float* kd) {
  EEPROM.get(kpAddress, kp);
  EEPROM.get(kiAddress, ki);
  EEPROM.get(kdAddress, kd);
}