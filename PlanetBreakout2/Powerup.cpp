#include "GameTypes.h"
#include "GameController.h"
#include "LogicHelper.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <random>

void Powerup::CollisionBat(float x1, float x2)
{
  GameController::GetInstance()->AddPowerup();
  active = false;
}

void Powerup::Start()
{
  speed = 1.f;
  real_x = x;
  real_y = y;
  direction = M_PI / 2.f;
}