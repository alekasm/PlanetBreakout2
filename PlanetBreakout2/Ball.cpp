#include "GameTypes.h"
#include "GameController.h"
#include "LogicHelper.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <random>
#include <set>

void Ball::Collision(BallCollisionType type)
{
  if (type == BallCollisionType::VERTICAL)
  {
    direction = -(atan2(sin(direction), cos(direction)) + (M_PI));
  }
  else
  {
    direction = -(atan2(sin(direction), cos(direction)) + (2.f * M_PI));
  }
  speed = std::clamp(speed + 0.05f, 1.0f, 8.0f);
}

void Ball::CollisionVerticalWall()
{
  Collision(BallCollisionType::VERTICAL);
}
void Ball::CollisionHorizontalWall()
{
  Collision(BallCollisionType::HORIZONTAL);
}
void Ball::CollisionBrick(uint32_t index)
{
  if (GameController::GetInstance()->BreakBrick(this, index))
  {
    RECT brickRect = GetBrickRect(index);
    if (old_y > brickRect.bottom || (old_y + BALL_DIMENSION) < brickRect.top)
    {
      Collision(BallCollisionType::HORIZONTAL);
    }
    else
    {
      Collision(BallCollisionType::VERTICAL);
    }
    //(old_y + BALL_DIMENSION) > brickRect.left
  }
  //Seems fine for now, instead of manually setting ball position
  real_x = old_x; // real_x + cos(direction);
  real_y = old_y; // real_y + sin(direction);
}
void Ball::CollisionBat(float x1, float x2)
{
  //Prevent an immediate retrigger of collision
  real_y = BAT_Y - BALL_DIMENSION - 0.5f;
  float portion = std::clamp((x2 - real_x) / (BAT_WIDTH + BALL_DIMENSION), 0.0f, 1.0f);
  //float dir = M_PI / 6.f + ((4.f * portion) * M_PI / 6.f); //pi/7  - 5pi/6
  //float dir = M_PI / 8.f + ((6.f * portion) * M_PI / 8.f); //pi/8  - 7pi/8
  float dir = M_PI / 10.f + ((8.f * portion) * M_PI / 10.f); //pi/10 - 9pi/10
  direction = -dir;
}

void Ball::Start()
{
  speed = 3.f;
  real_x = x;
  real_y = y;
  float min = (M_PI / 4.f);
  float max = ((3.f * M_PI) / 4.f);
  RandomDirection(min, max);
  //direction = -(3*M_PI / 4);
} 