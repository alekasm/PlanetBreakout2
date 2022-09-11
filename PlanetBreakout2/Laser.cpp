#include "Laser.h"
#include "GameController.h"
#define _USE_MATH_DEFINES
#include <math.h>

void Laser::Start()
{
  active = true;
  speed = 7.f;
  real_x = x;
  real_y = y;
  direction = -(M_PI / 2.f);
}
void Laser::CollisionBrick(uint32_t index)
{
  GameController::GetInstance()->BreakBrick(this, index);
  active = false;
}

void Laser::PostFrameUpdate()
{
  height = std::clamp(BAT_Y - y, 1U, 24U);
  RecalculateBounds();
}

void Laser::CollisionHorizontalWall()
{
  active = false;
}