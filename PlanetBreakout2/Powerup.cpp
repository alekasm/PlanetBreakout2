#include "GameTypes.h"
#include "GameController.h"
#include "LogicHelper.h"
#include "ResourceLoader.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <random>

void Powerup::CollisionBat(float x1, float x2)
{
  for(uint32_t i = 0; i < count; ++i)
    GameController::GetInstance()->AddPowerup();
  active = false;
}

void Powerup::AddCount()
{
  ++count;
   countString = L"x" + std::to_wstring(count);
}

const D2D1_RECT_F Powerup::GetCountRect() const
{
  D2D1_RECT_F rect1(d2d1Rect);
  rect1.top = rect1.bottom;
  rect1.bottom += 20.f;
  return rect1;
}

const int Powerup::GetCountColor() const
{
  if (count == 1)
    return ColorBrush::WHITE;
  if (count == 2)
    return ColorBrush::GREEN;
  if (count == 3)
    return ColorBrush::ORANGE;

  return ColorBrush::PURPLE;
}

const std::wstring Powerup::GetCountString() const
{
  return countString;
}

void Powerup::Start()
{
  speed = 1.f;
  real_x = (float)x;
  real_y = (float)y;
  direction = M_PI / 2.f;
}