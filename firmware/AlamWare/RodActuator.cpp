#include "RodActuator.h"

#include <Arduino.h>

#define DEBUG false

RodActuator::RodActuator(uint8_t ENA, uint8_t ENB, uint8_t IN1, uint8_t IN2, volatile long &angPosition, void (*interruptHandler)()) : ENA(ENA), ENB(ENB), IN1(IN1), IN2(IN2), angPosition(angPosition) {
  pinMode(ENA, INPUT);
  pinMode(ENB, INPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(ENA), interruptHandler, CHANGE);

  // Calculate position factor
  // 1 Revolución en el encoder son 7 cambios
  // La relación de engranages de 235:1
  // El valor de _position está en revoluciones
  // 1/(7*235) = 6.0790273556231e-4
  positionFactor = 2.0 * R * PI / (ticksRev * gearRatio);

  // Inicializar control PID
  PID = QuickPID(&position, &angVelocity, &desiredPosition);
  PID.SetOutputLimits(outputMin, outputMax);
  PID.SetSampleTimeUs(2030);
  PID.SetDerivativeMode(QuickPID::dMode::dOnError);
  PID.SetProportionalMode(QuickPID::pMode::pOnError);
  PID.SetAntiWindupMode(QuickPID::iAwMode::iAwCondition);
  PID.SetMode(QuickPID::Control::automatic);
  PID.SetTunings(22.1, 67, 3.8632);
}

void RodActuator::Update() {
  updatePosition();

  if (tune) {
    switch (tuner.Run()) {
      case tuner.sample:
        updateVelocity();
        break;

      case tuner.tunings:
        angVelocity = 0;
        updateVelocity();
        tuner.GetAutoTunings(&Kp, &Ki, &Kd);
        SetPIDConstants(Kp, Ki, Kd);
        tune = false;
        next = true;
        break;

      default:
        break;
    }
  }

  else if (move) {
    if (startup && position > desiredPosition - 10) {
      startup = false;
      angVelocity = 0.01 * angVelocity;
      updateVelocity();
      PID.SetMode(QuickPID::Control::manual);
      PID.SetMode(QuickPID::Control::automatic);
    }
    PID.Compute();
    updateVelocity();
  }

  else if (test) {
    updateVelocity();
  }

  lastPosition = position;
  lastAngvelocity = angVelocity;

  if (isStable(1.5)) {
    next = true;
  }

  if (!next && move) {
    unsigned long currentTime = millis();
    if (currentTime - timer > timeout) {
      next = true;
    }
  }
}

void RodActuator::Stop() {
  angVelocity = 0;
  next = true;
  move = false;
  test = false;
  updateVelocity();
  updatePosition();
}

void RodActuator::Move(float desiredPosition) {
  next = false;
  timer = millis();
  move = true;
  startup = true;
  if (moveMode == MoveMode::Global) {
    RodActuator::desiredPosition = desiredPosition;
  } else if (moveMode == MoveMode::Relative) {
    RodActuator::desiredPosition += desiredPosition;
  }
}

float RodActuator::GetHomePosition() {
  return homePosition;
}

float RodActuator::GetTargetPosition() {
  return targetPosition;
}

void RodActuator::SetHomePosition(float newHome) {
  RodActuator::homePosition = newHome;
}

void RodActuator::SetTargetPosition(float newSecondHome) {
  RodActuator::targetPosition = newSecondHome;
}

void RodActuator::MoveToHome() {
  float desiredPosition = homePosition;
  if (moveMode == MoveMode::Relative) {
    desiredPosition = homePosition - desiredPosition;
  }
  Move(desiredPosition);
}

void RodActuator::MoveToTarget() {
  float desiredPosition = targetPosition;
  if (moveMode == MoveMode::Relative) {
    desiredPosition = targetPosition - desiredPosition;
  }
  Move(desiredPosition);
}

float RodActuator::GetPosition() {
  return position;
}

void RodActuator::SetPIDConstants(float Kp, float Ki, float Kd) {
  Kp = Kp;
  Ki = Ki;
  Kd = Kd;
  PID.SetTunings(Kp, Ki, Kd);
}

bool RodActuator::Next() {
  return next;
}

void RodActuator::SetMaxSpeed(uint8_t maxSpeed) {
  if (maxSpeed < 50) {
    maxSpeed = 50;
  }
  RodActuator::speed = maxSpeed;
}

void RodActuator::TestVelocity(float velocity) {
  RodActuator::angVelocity = velocity;
  test = true;
}

void RodActuator::SetTimeout(float timeout) {
  RodActuator::timeout = timeout;
}

int RodActuator::GetTimeout() {
  return timeout;
}

void RodActuator::AutoTune() {
  tune = true;
  next = false;
  tuner.Configure(1000, 200, 100, 0, 3, 1, 500);
  tuner.SetEmergencyStop(1000);
}

void RodActuator::SetMoveMode(MoveMode moveMode) {
  RodActuator::moveMode = moveMode;
}

// =========================================
// Private methods
// =========================================
bool RodActuator::isStable(float threshold = 1) {
  return abs(position - desiredPosition) <= threshold;
}

void RodActuator::updateVelocity() {
  uint8_t vel1 = 0;
  uint8_t vel2 = 0;
  float minInput = 0;
  uint8_t minPWM = 0;
  if (angVelocity > minInput) {
    // vel1 = abs((float)_angVelocity / (float)_outputMax) * (float)_speed;
    vel1 = map(abs(angVelocity), minInput, outputMax, minPWM, RodActuator::speed);
    if (vel1 < 40)
      vel1 = 0;
  } else if (angVelocity < -minInput) {
    // vel2 = abs((float)_angVelocity / (float)_outputMax) * (float)_speed;
    vel2 = map(abs(angVelocity), minInput, outputMax, minPWM, RodActuator::speed);
    if (vel2 < 40)
      vel2 = 0;
  }
  // Serial.println(vel1);
  if (DEBUG && (move && !next)) {
    Serial.print("Velocity: ");
    Serial.print(angVelocity);
    Serial.print(" ");
    Serial.print(vel1);
    Serial.print(" ");
    Serial.print(vel2);
    Serial.print("  Position: ");
    Serial.print(position);
    Serial.println();
  }
  analogWrite(IN1, vel1);
  analogWrite(IN2, vel2);
}

void RodActuator::updatePosition() {
  position = angPosition * positionFactor;
}
