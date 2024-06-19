#ifndef NAMEDACTUATORS_H
#define NAMEDACTUATORS_H

#include "RodActuator.h"


// Order by ribbon connection
// (1.1) 21, 24, 12, 13
// (1.2) 18, 22, 44, 45
// (1.3) 20, 23, 6, 7
// (2.1) 19, 65, 8, 9
// (2.2) 3, 66, 10, 11
// (2.3) 2, 64, 4, 5


// 1.1
#define A_ENA 21
#define A_ENB 24
#define A_IN1 12
#define A_IN2 13
// 1.3
#define B_ENA 20
#define B_ENB 23
#define B_IN1 6
#define B_IN2 7
// 2.2
#define C_ENA 3
#define C_ENB 66
#define C_IN1 10
#define C_IN2 11
// 2.1
#define I_ENA 19
#define I_ENB 65
#define I_IN1 8
#define I_IN2 9
// 2.3
#define J_ENA 2
#define J_ENB 64
#define J_IN1 4
#define J_IN2 5
// 1.2
#define K_ENA 18
#define K_ENB 22
#define K_IN1 44
#define K_IN2 45

enum Actuators
{
  A,
  B,
  C,
  I,
  J,
  K,
  N_ACTUATORS
};

struct NamedActuator
{
  char name;
  RodActuator *object;
};

// Posiciones
volatile long angPosition[N_ACTUATORS] = {0, 0, 0, 0, 0, 0};

//
// Interrupt Handlers
//
static void A_interruptHandler()
{
  if (digitalRead(A_ENA) != digitalRead(A_ENB))
    angPosition[A]++;
  else
    angPosition[A]--;
}
static void B_interruptHandler()
{
  if (digitalRead(B_ENA) != digitalRead(B_ENB))
    angPosition[B]++;
  else
    angPosition[B]--;
}

static void C_interruptHandler()
{
  if (digitalRead(C_ENA) != digitalRead(C_ENB))
    angPosition[C]++;
  else
    angPosition[C]--;
}

static void I_interruptHandler()
{
  if (digitalRead(I_ENA) != digitalRead(I_ENB))
    angPosition[I]++;
  else
    angPosition[I]--;
}

static void J_interruptHandler()
{
  if (digitalRead(J_ENA) != digitalRead(J_ENB))
    angPosition[J]++;
  else
    angPosition[J]--;
}

static void K_interruptHandler()
{
  if (digitalRead(K_ENA) != digitalRead(K_ENB))
    angPosition[K]++;
  else
    angPosition[K]--;
}

NamedActuator namedActuators[N_ACTUATORS] = {
    {'A', new RodActuator(A_ENA, A_ENB, A_IN1, A_IN2, angPosition[A], A_interruptHandler)},
    {'B', new RodActuator(B_ENA, B_ENB, B_IN1, B_IN2, angPosition[B], B_interruptHandler)},
    {'C', new RodActuator(C_ENA, C_ENB, C_IN1, C_IN2, angPosition[C], C_interruptHandler)},
    {'I', new RodActuator(I_ENA, I_ENB, I_IN1, I_IN2, angPosition[I], I_interruptHandler)},
    {'J', new RodActuator(J_ENA, J_ENB, J_IN1, J_IN2, angPosition[J], J_interruptHandler)},
    {'K', new RodActuator(K_ENA, K_ENB, K_IN1, K_IN2, angPosition[K], K_interruptHandler)},
};

RodActuator *getActuatorByName(char name)
{
  for (int i = 0; i < N_ACTUATORS; ++i)
  {
    if (namedActuators[i].name == name)
    {
      return namedActuators[i].object;
    }
  }
  return nullptr;
}

void updateNamedActuators()
{
  for (NamedActuator actuator : namedActuators)
  {
    actuator.object->Update();
  }
}

#endif