#ifndef RODACTUATOR_H
#define RODACTUATOR_H

#include <Arduino.h>
#include "QuickPID.h"
#include "sTune.h"

class RodActuator {
 public:  
  RodActuator(uint8_t ENA, uint8_t ENB, uint8_t IN1, uint8_t IN2, volatile long &angPosition, void (*interruptHandler)());

  enum MoveMode {
    Global,
    Relative
  };

  enum DistanceUnit {
    Millimeters,
    Inches
  };

  void Stop();
  void Update();
  void SetPIDConstants(float Kp, float Ki, float Kd);
  void Move(float desiredPosition);
  float GetPosition();
  int GetTimeout();
  void TestVelocity(float velocity);
  void SetLeftLimit(float leftLimit);
  void SetRightLimit(float rightLimit);
  void SetMaxSpeed(uint8_t maxSpeed);
  void SetDistanceUnit(DistanceUnit unitSystem);
  void SetMoveMode(MoveMode);
  void SetTimeout(float timeout);
  void MoveToHome();
  void MoveToSecondHome();
  void SetHomePosition(float newHomePosition);
  void SetSecondHomePosition(float newSecondHomePosition);
  float GetHomePosition();
  float GetSecondHomePosition();
  void AutoTune();
  bool Next();

 private:
  float homePosition = 0;
  float secondHomePosition = 0;
  DistanceUnit distanceUnit = Millimeters;

  bool isStable(float threshold);
  volatile long &angPosition;
  uint8_t IN1;  // Pin velocidad adelante
  uint8_t IN2;  // Pin velocidad atrás
  uint8_t ENA;  // Pin encoder A
  uint8_t ENB;  // Pin encoder B

  float ticksRev = 7.0;     // ticks in angPosition that makes a rev
  float gearRatio = 235.0;  // reducion gear of gearRatio:1
  float R = 5.3;
  float positionFactor;

  uint8_t speed = 255;
  unsigned long timeout = 5000;
  unsigned long timer = millis();
  float position = 0;  // Posición lineal actual
  float lastPosition = 0;
  float desiredPosition = 0;  // Posición lineal deseada
  float lastAngvelocity = 0;
  float angVelocity = 0;  // Velocidad angular de los motores (interfaz para el PWM)

  MoveMode moveMode = Global;

  bool next = true;
  bool move = false;
  float Kp, Kd, Ki;
  float outputMin = -255;
  float outputMax = 255;
  float outputSpan;
  bool startup = false;
  QuickPID PID;  // Controlador PID

  bool test = false;
  bool tune = false;
  sTune tuner = sTune(&position, &angVelocity, tuner.NoOvershoot_PI, tuner.directIP, tuner.printALL);

  float _leftLimit = -2000;
  float _rightLimit = 2000;

  void updateVelocity();
  void updatePosition();
};

#endif