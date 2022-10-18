#include "DroneLaser.h"
#include "LogicHelper.h"
#include "GameController.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <random>
void DroneLaser::Start()
{
  speed = 2.f;
  real_x = (float)x;
  real_y = (float)y;
  RandomDirection(0, 2.f * M_PI);
  active = true;
}
void DroneLaser::CollisionVerticalWall()
{
  active = false;
}

void DroneLaser::CollisionHorizontalWall()
{
  active = false;
}

void DroneLaser::CollisionBrick(uint32_t index)
{
  if (GameController::GetInstance()->BreakBrick(this, index))
    active = false;
}

void DroneLaser::CollisionBat(float x1, float x2)
{
  active = false;
  GameController::GetInstance()->DestroyBat();
}
