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
  const Difficulty& difficulty = GameController::GetInstance()->GetDifficulty();
  speed = std::clamp(speed + difficulty.incSpeed,
    difficulty.startSpeed,difficulty.maxSpeed);
}

void Ball::CollisionVerticalWall()
{
  Collision(BallCollisionType::VERTICAL);
}
void Ball::CollisionHorizontalWall()
{
  Collision(BallCollisionType::HORIZONTAL);

}

//https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
bool get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y,
  float p2_x, float p2_y, float p3_x, float p3_y, float* i_x, float* i_y)
{
  float s1_x, s1_y, s2_x, s2_y;
  s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
  s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

  float s, t;
  s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
  t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

  if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
  {
    // Collision detected
    if (i_x != NULL)
      *i_x = p0_x + (t * s1_x);
    if (i_y != NULL)
      *i_y = p0_y + (t * s1_y);
    return true;
  }

  return false; // No collision
}

struct Point
{
  float x;
  float y;
};

struct Line
{
  Point a;
  Point b;
};

enum HitCheck { FROM_LEFT, FROM_RIGHT, FROM_ABOVE, FROM_BOTTOM, NONE };

struct HitLine
{
  Line line;
  HitCheck check;
};

bool Ball::CollisionBrick(uint32_t index)
{
  RECT brickRect = GetBrickRect(index);

  bool going_right = old_x < real_x;
  bool going_left = old_x > real_x;
  bool going_down = old_y < real_y;
  bool going_up = old_y > real_y;

  Point old_topleft = { old_x, old_y };
  Point old_topright = { old_x + width, old_y };
  Point old_bottomleft = { old_x, old_y + height };
  Point old_bottomright = { old_x + width, old_y + height };

  Point new_topleft = { real_x, real_y };
  Point new_topright = { real_x + width, real_y };
  Point new_bottomleft = { real_x, real_y + height };
  Point new_bottomright = { real_x + width, real_y + height };

  std::vector<Line> lines = {
    { old_topleft, new_topleft },
    { old_topleft, new_topright },
    { old_topleft, new_bottomleft },
    { old_topleft, new_bottomright },

    { old_topright, new_topleft },
    { old_topright, new_topright },
    { old_topright, new_bottomleft },
    { old_topright, new_bottomright },

    { old_bottomleft, new_topleft },
    { old_bottomleft, new_topright },
    { old_bottomleft, new_bottomleft },
    { old_bottomleft, new_bottomright },

    { old_bottomright, new_topleft },
    { old_bottomright, new_topright },
    { old_bottomright, new_bottomleft },
    { old_bottomright, new_bottomright },
  };

  Line brickLeft = {
    {brickRect.left, brickRect.top},
    {brickRect.left, brickRect.bottom}
  };

  Line brickRight = {
   {brickRect.right, brickRect.top},
   {brickRect.right, brickRect.bottom}
  };

  Line brickBottom = {
   {brickRect.left, brickRect.bottom},
   {brickRect.right, brickRect.bottom}
  };

  Line brickTop = {
  {brickRect.left, brickRect.top},
  {brickRect.right, brickRect.top}
  };


  std::vector<HitLine> rectLines = {
    {brickLeft, HitCheck::FROM_LEFT},
    {brickRight, HitCheck::FROM_RIGHT},
  };

  //The only optimization which seems reliable
  if (going_up)
    rectLines.push_back({ brickBottom, HitCheck::FROM_BOTTOM });
  else
    rectLines.push_back({ brickTop, HitCheck::FROM_ABOVE });

  HitCheck check = HitCheck::NONE;
  float distance = FLT_MAX;
  for (const HitLine& rectLine : rectLines)
  {
    const Line& r = rectLine.line;
    for (const Line& l : lines)
    {
      float i_x, i_y;
      if (get_line_intersection(
        l.a.x, l.a.y, l.b.x, l.b.y,
        r.a.x, r.a.y, r.b.x, r.b.y,
        &i_x, &i_y))
      {
        float xdist = i_x - l.a.x;
        float ydist = i_y - l.a.y;
        float next_dist = sqrtf((xdist * xdist) + (ydist * ydist));
        if (next_dist < distance)
        {
          distance = next_dist;
          check = rectLine.check;
        }
      }
    }
  }

  if (check == HitCheck::NONE)
    return false;

  ResourceLoader::PlayAudio(L"brick.wav");
  if (GameController::GetInstance()->BreakBrick(this, index))
  {
    switch (check)
    {
    case HitCheck::FROM_ABOVE:
      Collision(BallCollisionType::HORIZONTAL);
      real_y = brickRect.top - BALL_DIMENSION - 0.1f;
      break;
    case HitCheck::FROM_BOTTOM:
      Collision(BallCollisionType::HORIZONTAL);
      real_y = brickRect.bottom + 0.1f;
      break;
    case HitCheck::FROM_RIGHT:
      Collision(BallCollisionType::VERTICAL);
      real_x = brickRect.right + 0.1f;
      break;
    case HitCheck::FROM_LEFT:
      Collision(BallCollisionType::VERTICAL);
      real_x = brickRect.left - BALL_DIMENSION - 0.1f;
      break;
    }
  }

  return true;
}
void Ball::CollisionBat(float x1, float x2)
{
  if (GameController::GetInstance()->IsPowerUpActive(PowerupType::GHOST))
  {
    double min = (M_PI / 6.f);
    double max = ((5.f * M_PI) / 6.f);
    RandomDirection(min, max);
    return;
  }
  //Prevent an immediate retrigger of collision
  real_y = BAT_Y - BALL_DIMENSION - 0.5f;
  double portion = std::clamp((x2 - real_x) / (BAT_WIDTH + BALL_DIMENSION), 0.0f, 1.0f);
  //float dir = M_PI / 6.f + ((4.f * portion) * M_PI / 6.f); //pi/7  - 5pi/6
  //float dir = M_PI / 8.f + ((6.f * portion) * M_PI / 8.f); //pi/8  - 7pi/8
  double dir = M_PI / 10.f + ((8.f * portion) * M_PI / 10.f); //pi/10 - 9pi/10
  direction = -dir;
  ResourceLoader::PlayAudio(L"brick.wav");
}

void Ball::Start()
{
  speed = GameController::GetInstance()->GetDifficulty().startSpeed;
  real_x = (float)x;
  real_y = (float)y;
  double min = (M_PI / 4.f);
  double max = ((3.f * M_PI) / 4.f);
  RandomDirection(min, max);
  //direction = -(3*M_PI / 4);
}