#include "Memory.h"
#include "Commands.h"

// SerialCommands Setup
char serialCommandBuffer[128];
SerialCommands serialCommands(&Serial, serialCommandBuffer, sizeof(serialCommandBuffer));

void addSerialCommands() {
  serialCommands.SetDefaultHandler(cmdUnrecognized);

  serialCommands.AddCommand(&GHelp);
  serialCommands.AddCommand(&G1);
  serialCommands.AddCommand(&G4);
  serialCommands.AddCommand(&G92);
  serialCommands.AddCommand(&G92_1);
  serialCommands.AddCommand(&G28);
  serialCommands.AddCommand(&G28_1);
  serialCommands.AddCommand(&G90);
  serialCommands.AddCommand(&G91);

  serialCommands.AddCommand(&MHelp);
  serialCommands.AddCommand(&M00);
  serialCommands.AddCommand(&M85);
  serialCommands.AddCommand(&M114);
  serialCommands.AddCommand(&M303);
  serialCommands.AddCommand(&M301);
  serialCommands.AddCommand(&M300);
}

void setup() {
  Serial.begin(250000);
  Serial.print("this is alam");
  addSerialCommands();
}

void loop() {
  if (testing) {
    updateTest();
  } else {
    serialCommands.ReadSerial();

    updateNamedActuators();
    nextCommand();
  }
}
