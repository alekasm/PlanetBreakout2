#include "GameTypes.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <random>

inline float RandomDirection(float min, float max, size_t current_ms)
{
  std::mt19937 generator(current_ms);
  //Allows me to use real math where y pos is up
  std::uniform_real_distribution<float> distribution(-max, -min);
  return distribution(generator);
}

void Ball::Collision()
{

  float x1 = start_x;
  float y1 = start_y;
  float x2 = real_x;
  float y2 = real_y;

  float x3 = start_x;
  float y3 = real_y;

  float x4 = (x1 + x2) / 2.f;
  float y4 = (y1 + y2) / 2.f;

  float dist = sqrt(pow(x2 - x4, 2) + pow(y2 - y4, 2));

  float x5 = x4;
  //float dir = atan(y3 - y1 / x3 - x1);
  //prevents divide by zero
  float dir = -atan2(y1 - y3, x1 - x3);
  float y5 = y2 + sin(dir) * dist;

  //float dir2 = atan2(y5 - y2, x5 - x2);
  float dir2 = atan2(y5 - y2, x5 - x2);
  direction = dir2;
  start_x = real_x;
  start_y = real_y;
}

void Ball::UpdateFrame(float elapsed, size_t current_ms)
{
  float capped_frames = elapsed;
  if (capped_frames > TARGET_FPS / 10.f)
    capped_frames = TARGET_FPS / 10.f;
  float distance = capped_frames * speed;
  //cap the distance or x/y?
  real_x = real_x + cos(direction) * distance;
  real_y = real_y + sin(direction) * distance;

  if (real_x < 0.f)
  {
    real_x = 0.f;
    Collision();
  }
  else if (real_x + BALL_DIMENSION > GAME_WIDTH)
  {
    real_x = GAME_WIDTH - BALL_DIMENSION;
    Collision();
  }
  if (real_y < 0.f) 
  {
    real_y = 0.f;
    float min = (7.f * M_PI / 6.f);
    float max = (11.f * M_PI / 6.f);
    direction = RandomDirection(min, max, current_ms);
    start_x = real_x;
    start_y = real_y;
    //Collision();
  }
  else if (real_y + BALL_DIMENSION > GAME_HEIGHT)
  {
    real_y = GAME_HEIGHT - BALL_DIMENSION;
    float min = (M_PI / 6.f);
    float max = (5.f * M_PI / 6.f);
    direction = RandomDirection(min, max, current_ms);
    start_x = real_x;
    start_y = real_y;
    //Collision();
  }



 
  Update(real_x, real_y);
}

void Ball::Start()
{
  speed = 5.f;
  real_x = GAME_WIDTH / 2;
  real_y = GAME_HEIGHT /2;
  start_x = real_x;
  start_y = real_y;
  float min = (M_PI / 4.f);
  float max = ((3.f * M_PI) / 4.f);
  //direction = -(3*M_PI) / 2;
  //direction = M_PI;
  direction = -(M_PI / 6);
  //direction = RandomDirection(min, max, rand() % (std::numeric_limits<uint64_t>::max)());
} 