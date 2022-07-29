#include "GameTypes.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <random>

inline float RandomDirection(float min, float max, size_t current_ms)
{
  std::mt19937 generator(current_ms);
  //Allows me to use real math where y pos is up
  std::uniform_real_distribution<float> distribution(-max, -min);
  float rand = distribution(generator);
  //float normalize = atan2f(sinf(rand), cosf(rand));
  //printf("Changed direction: %f\n", normalize * (180.f / M_PI));
  return rand;
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
    float min = (M_PI / 6.f);
    float max = (M_PI / 3.f);
    direction = RandomDirection(min, max, current_ms);
  }
  else if (real_x + BALL_DIMENSION >= GAME_WIDTH)
  {
    real_x = GAME_WIDTH - BALL_DIMENSION;
    float min = (2.f * M_PI / 3.f);
    float max = (5.f * M_PI / 6.f);
    direction = RandomDirection(min, max, current_ms);
  }
  if (real_y < 0.f) 
  {
    real_y = 0.f;
    float min = (7.f * M_PI / 6.f);
    float max = (11.f * M_PI / 6.f);
    direction = RandomDirection(min, max, current_ms);
  }
  else if (real_y + BALL_DIMENSION >= GAME_HEIGHT)
  {
    real_y = GAME_HEIGHT - BALL_DIMENSION;
    float min = (M_PI / 6.f);
    float max = (5.f * M_PI / 6.f);
    direction = RandomDirection(min, max, current_ms);
  }
  Update(real_x, real_y);
}

void Ball::Start()
{
  speed = 5.f;
  real_x = GAME_WIDTH / 2;
  real_y = GAME_HEIGHT /2;
  start_x = x;
  start_y = y;
  float min = (M_PI / 4.f);
  float max = ((3.f * M_PI) / 4.f);
  //direction = -(3*M_PI) / 2;
  //direction = M_PI;
  //direction = -(M_PI / 2);
  direction = RandomDirection(min, max, rand() % (std::numeric_limits<uint64_t>::max)());
} 