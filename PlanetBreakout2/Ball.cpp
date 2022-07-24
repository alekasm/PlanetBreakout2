#include "GameTypes.h"
#define _USE_MATH_DEFINES
#include <math.h>

void Ball::UpdateFrame(float elapsed_frames)
{
  float distance = elapsed_frames * this->speed;
  float new_x = real_x + cos(direction) * distance;
  float new_y = real_y + sin(direction) * distance;
  real_x = new_x;
  real_y = new_y;
  Update(real_x, real_y);
}

void Ball::Start()
{
  speed = 5.f;
  real_x = x;
  real_y = y;
  float random = ((float)rand()) / (float)RAND_MAX;
  float normal = -2.0f * M_PI;
  float min = (M_PI / 4.f);
  float max = ((3.f * M_PI) / 4.f);
  float range = max - min;
  direction = (min + (random * range)) - M_PI;
}