#include "Laser.h"
#include "GameController.h"
#define _USE_MATH_DEFINES
#include <math.h>

void Laser::Start()
{
  active = true;
  height = 1;
  speed = 9.f;
  real_x = x;
  real_y = y;
  direction = -(M_PI / 2.f);
}
void Laser::CollisionBrick(uint32_t index)
{
  if(GameController::GetInstance()->BreakBrick(this, index))
    active = false;
}

void Laser::PostFrameUpdate(float elapsed)
{
  height = std::clamp((BAT_Y + BAT_HEIGHT) - y - 1, 1U, 24U);
  RecalculateBounds();
}

void Laser::CollisionHorizontalWall()
{
  active = false;
}