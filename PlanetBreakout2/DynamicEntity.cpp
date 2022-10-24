#include "DynamicEntity.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <random>

void DynamicEntity::PreFrameUpdate() {}
void DynamicEntity::PostFrameUpdate(float elapsed) {}


float DynamicEntity::GetSpeed()
{
  return speed;
}
float DynamicEntity::GetRealX()
{
  return real_x;
}
float DynamicEntity::GetRealY()
{
  return real_y;
}

void DynamicEntity::Reset()
{
  speed = 0.0f;
  direction = 0.f;
  real_x = 0.f;
  real_y = 0.f;
  old_x = 0.f;
  old_y = 0.f;
}

void DynamicEntity::SetPosition(float x, float y)
{
  real_x = x;
  real_y = y;
  Update(real_x, real_y);
}

void DynamicEntity::RandomDirection(double min, double max)
{
  std::mt19937 generator(rand() % (std::numeric_limits<uint64_t>::max)());
  //Allows me to use real math where y pos is up
  std::uniform_real_distribution<double> distribution(-max, -min);
  direction = distribution(generator);
}

void DynamicEntity::UpdateFrame(int64_t elapsed)
{
  if (!active) return;

  float frame_modifier = 1.0f;
  if (elapsed < TARGET_FRAMES) //slow down
    frame_modifier = elapsed / TARGET_FRAMES;
  float distance = frame_modifier * speed;
  old_x = real_x;
  old_y = real_y;

  //cap the distance or x/y?
  //TODO calculate worst case scenario for bricks
  real_x = real_x + cos(direction) * distance;
  real_y = real_y + sin(direction) * distance;

  PreFrameUpdate();
  Update(real_x, real_y);
  PostFrameUpdate(frame_modifier);
}