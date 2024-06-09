#ifndef COMMANDS_H
#define COMMANDS_H

#include <SerialCommands.h>

#include "NamedActuators.h"
#include "RodActuator.h"

bool sendingCommand = false;
bool testing = false;
unsigned long startTimeTest;

void updateTest();
void nextCommand();

// G-Code Definitions

void cmdUnrecognized(SerialCommands *, const char *);
void cmdGetGCodes(SerialCommands *);
void cmdDelay(SerialCommands *);
void cmdMoveActuator(SerialCommands *);
void cmdSetGlobalMode(SerialCommands *);
void cmdSetRelativeMode(SerialCommands *);
void cmdSetHome(SerialCommands *);
void cmdSetSecondHome(SerialCommands *);
void cmdMoveToHome(SerialCommands *);
void cmdMoveToSecondHome(SerialCommands *);
void cmdSetUnitInches(SerialCommands *);
void cmdSetUnitMillimeters(SerialCommands *);

// M-Code Definitions

void cmdGetMCodes(SerialCommands *);
void cmdStop(SerialCommands *);
void cmdSetTimeout(SerialCommands *);
void cmdGetActuatorPositions(SerialCommands *);
void cmdTestActuators(SerialCommands *);
void cmdStartAutoTuning(SerialCommands *);
void cmdSetPIDConstants(SerialCommands *);

SerialCommand GHelp("G", &cmdGetGCodes);
SerialCommand G4("G4", &cmdDelay);
SerialCommand G1("G1", &cmdMoveActuator);
SerialCommand G20("G20", &cmdSetUnitInches);
SerialCommand G21("G21", &cmdSetUnitMillimeters);
SerialCommand G28("G28", &cmdMoveToHome);
SerialCommand G28_1("G28.1", &cmdMoveToSecondHome);
SerialCommand G90("G90", &cmdSetGlobalMode);
SerialCommand G91("G91", &cmdSetRelativeMode);
SerialCommand G92("G92", &cmdSetHome);
SerialCommand G92_1("G92.1", &cmdSetSecondHome);

SerialCommand MHelp("M", &cmdGetMCodes);
SerialCommand M00("M00", &cmdStop);
SerialCommand M85("M85", &cmdSetTimeout);
SerialCommand M114("M114", &cmdGetActuatorPositions);
SerialCommand M300("M300", &cmdTestActuators);
SerialCommand M303("M303", &cmdStartAutoTuning);
SerialCommand M301("M301", &cmdSetPIDConstants);

void endCmd(SerialCommands *sender) {
  sender->GetSerial()->println("!EOC");
}

void nextCommand() {
  if (!sendingCommand)
    return;
  for (NamedActuator actuator : namedActuators) {
    if (!actuator.object->Next()) {
      return;
    }
  }
  Serial.println("!EOC");
  sendingCommand = false;
}

float getvel(int t) {
  float v;
  float m = 255.0 / 500.0;
  if (t < 500 || t > 4000) {
    v = 0;
  } else if (t < 1000) {  // entre 500 y 1000 ms rampa
    v = m * (t - 500);
  } else if (t < 1500) {  // mantener velocidad máxima por 500 ms
    v = 255;
  } else if (t < 2000) {  // disminuir velocidad
    v = -m * (t - 1500) + 255;
  } else if (t < 2500) {  // mantener velocidad máxima en reversa por 500 ms
    v = 0;
  } else if (t < 3000) {
    v = -m * (t - 2500);
  } else if (t < 3500) {
    v = -255;
  } else if (t < 4000) {
    v = m * (t - 3500) - 255;
  }
  return v;
}

void updateTest() {
  unsigned long t = millis() - startTimeTest;
  Serial.print(t);
  float v = getvel(t);
  Serial.print(";");
  Serial.print(v);
  for (NamedActuator actuator : namedActuators) {
    Serial.print(";");
    actuator.object->TestVelocity(v);
    actuator.object->Update();
    Serial.print(actuator.object->GetPosition());
  }
  Serial.println("");
  if (t >= 4500) {
    testing = false;
    for (NamedActuator actuator : namedActuators) {
      actuator.object->Stop();
    }
    Serial.println("!EOC");
  }
}

void cmdUnrecognized(SerialCommands *sender, const char *cmd) {
  sendingCommand = true;

  sender->GetSerial()->print("Unrecognized command [");
  sender->GetSerial()->print(cmd);
  sender->GetSerial()->println("]");
}

void cmdGetGCodes(SerialCommands *sender) {
  sendingCommand = true;

  sender->GetSerial()->println("G: Show this help");
  sender->GetSerial()->println("G1: Move actuator");
  sender->GetSerial()->println("G4: Pause robot by a determined P<time>");
  sender->GetSerial()->println("G20: Set inches as distance units");
  sender->GetSerial()->println("G21: Set millimeters as distance units");
  sender->GetSerial()->println("G28: Move actuators to home position");
  sender->GetSerial()->println("G28.1: Move actuators to second home position");
  sender->GetSerial()->println("G90: Set global positioning mode");
  sender->GetSerial()->println("G91: Set relative positioning mode");
  sender->GetSerial()->println("G92: Move to home position");
  sender->GetSerial()->println("G92.1: Move to second home position");
}

void cmdGetMCodes(SerialCommands *sender) {
  sendingCommand = true;

  sender->GetSerial()->println("M: Show this help");
  sender->GetSerial()->println("M00: Stop actuators");
  sender->GetSerial()->println("M85: Set command timeout with P<milliseconds> or S<seconds>");
  sender->GetSerial()->println("M114: Report actual position of actuators");
  sender->GetSerial()->println("M301: Set PID constants with P<value> I<value> D<value>");
  sender->GetSerial()->println("M303: Starts autotuning");
}

void cmdDelay(SerialCommands *sender) {
  sendingCommand = true;

  // sender->GetSerial()->println("Starting delay");
  char *arg = sender->Next();
  if (arg != NULL) {
    char parameter = arg[0];
    float value = atof(arg + 1);
    if (parameter == 'P') {
      delay(value);
    }
  }
}

void cmdMoveActuator(SerialCommands *sender) {
  sendingCommand = true;
  uint8_t speed = 255;
  char *arg;
  while ((arg = sender->Next()) != NULL) {
    char id = arg[0];
    float value = atof(arg + 1);
    if (id == 'F') {
      speed = value;
      continue;
    }
    RodActuator *actuator = getActuatorByName(id);
    actuator->Move(value);
    actuator->SetMaxSpeed(speed);
  }
}

void cmdStop(SerialCommands *sender) {
  sendingCommand = true;
  for (NamedActuator actuator : namedActuators) {
    actuator.object->Stop();
  }
}

void cmdSetTimeout(SerialCommands *sender) {
  sendingCommand = true;
  float maximumTimeout = 60e3;
  float timeout = 5000;
  char *arg = sender->Next();
  if (arg != NULL) {
    char parameter = arg[0];
    float value = atof(arg + 1);
    if (parameter == 'P') {
      timeout = value;
    } else if (parameter == 'S') {
      timeout = value * 1000;
    }
    if (timeout > maximumTimeout)
      timeout = maximumTimeout;
    for (NamedActuator actuator : namedActuators) {
      actuator.object->SetTimeout(timeout);
    }
  } else {
    timeout = namedActuators[0].object->GetTimeout();
  }
  sender->GetSerial()->print("Timeout:");
  sender->GetSerial()->println(timeout);
}

void cmdGetActuatorPositions(SerialCommands *sender) {
  sendingCommand = true;

  int decimalPlaces = 5;

  String positions = "";
  for (int i = 0; i < N_ACTUATORS; ++i) {
    String name = (String)namedActuators[i].name;
    float position = namedActuators[i].object->GetPosition();
    positions += name + ":" + String(position);
    if (i < N_ACTUATORS - 1) {
      {
        positions += " ";
      }
    }
  }
  sender->GetSerial()->println(positions);
}

void cmdStartAutoTuning(SerialCommands *sender) {
  sendingCommand = true;

  char *arg;
  while ((arg = sender->Next()) != NULL) {
    char id = arg[0];
    Serial.print("Starting auto tuning in actuator ");
    Serial.println(id);
    RodActuator *actuator = getActuatorByName(id);
    actuator->AutoTune();
  }
}

void cmdTestActuators(SerialCommands *sender) {
  testing = true;
  startTimeTest = millis();
  sender->GetSerial()->print("t");
  sender->GetSerial()->print(";PWM");
  for (NamedActuator actuator : namedActuators) {
    sender->GetSerial()->print(";");
    sender->GetSerial()->print(actuator.name);
  }
  sender->GetSerial()->println("");
}

void cmdSetPIDConstants(SerialCommands *sender) {
  sendingCommand = true;

  float kp = 0;
  float ki = 0;
  float kd = 0;

  char *arg;
  while ((arg = sender->Next()) != NULL) {
    char id = arg[0];
    float value = atof(arg + 1);
    if (id == 'P')
      kp = value;
    else if (id == 'I')
      ki = value;
    else if (id == 'D')
      kd = value;
  }

  for (NamedActuator actuator : namedActuators) {
    actuator.object->SetPIDConstants(kp, ki, kd);
  }

  sender->GetSerial()->print("Kp:");
  sender->GetSerial()->print(kp);
  sender->GetSerial()->print(" Kd:");
  sender->GetSerial()->print(kd);
  sender->GetSerial()->print(" Ki:");
  sender->GetSerial()->println(ki);
}

void cmdSetGlobalMode(SerialCommands *sender) {
  for (NamedActuator actuator : namedActuators) {
    actuator.object->SetMoveMode(actuator.object->Global);
  }
}

void cmdSetRelativeMode(SerialCommands *sender) {
  for (NamedActuator actuator : namedActuators) {
    actuator.object->SetMoveMode(actuator.object->Relative);
  }
}

void cmdSetHome(SerialCommands *sender) {
  sendingCommand = true;
  char *arg;
  while ((arg = sender->Next()) != NULL) {
    char id = arg[0];
    float value = atof(arg + 1);
    RodActuator *actuator = getActuatorByName(id);
    actuator->SetHomePosition(value);
  }
}

void cmdSetSecondHome(SerialCommands *sender) {
  sendingCommand = true;
  char *arg;
  while ((arg = sender->Next()) != NULL) {
    char id = arg[0];
    float value = atof(arg + 1);
    RodActuator *actuator = getActuatorByName(id);
    actuator->SetSecondHomePosition(value);
  }
}

void cmdMoveToHome(SerialCommands *sender) {
  sendingCommand = true;
  char *arg;
  arg = sender->Next();
  if (arg == NULL) {
    for (NamedActuator actuator : namedActuators) {
      actuator.object->MoveToHome();
    }
  } else {
    char id = arg[0];
    float value = atof(arg + 1);
    RodActuator *actuator = getActuatorByName(id);
    actuator->MoveToHome();

    while ((arg = sender->Next()) != NULL) {
      id = arg[0];
      float value = atof(arg + 1);
      actuator = getActuatorByName(id);
      actuator->MoveToHome();
    }
  }
}

void cmdMoveToSecondHome(SerialCommands *sender) {
  sendingCommand = true;
  char *arg;
  arg = sender->Next();
  if (arg == NULL) {
    for (NamedActuator actuator : namedActuators) {
      actuator.object->MoveToSecondHome();
    }
  } else {
    char id = arg[0];
    float value = atof(arg + 1);
    RodActuator *actuator = getActuatorByName(id);
    actuator->MoveToSecondHome();

    while ((arg = sender->Next()) != NULL) {
      id = arg[0];
      float value = atof(arg + 1);
      actuator = getActuatorByName(id);
      actuator->MoveToSecondHome();
    }
  }
}

void cmdSetUnitInches(SerialCommands *sender) {
  sendingCommand = true;
  for (NamedActuator actuator : namedActuators) {
    actuator.object->SetDistanceUnit(actuator.object->Inches);
  }
}

void cmdSetUnitMillimeters(SerialCommands *sender) {
  sendingCommand = true;
  for (NamedActuator actuator : namedActuators) {
    actuator.object->SetDistanceUnit(actuator.object->Millimeters);
  }
}

#endif