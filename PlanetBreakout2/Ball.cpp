#include "GameTypes.h"
#include "GameController.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <random>


inline float normalize(float angle)
{
  return atan2(sin(angle), cos(angle)) + (2.f * M_PI);
}

void Ball::RandomDirection(float min, float max)
{
  start_x = real_x;
  start_y = real_y;
  std::mt19937 generator(rand() % (std::numeric_limits<uint64_t>::max)());
  //Allows me to use real math where y pos is up
  std::uniform_real_distribution<float> distribution(-max, -min);
  direction = distribution(generator);
}

inline float Random(float a, float b)
{
  float min = min(a, b);
  float max = max(a, b);
  std::mt19937 generator(rand() % (std::numeric_limits<uint64_t>::max)());
  std::uniform_real_distribution<float> distribution(min, max);
  return distribution(generator);
}

void Ball::Collision(CollisionType type)
{
  if (type == CollisionType::VERTICAL)
  {
    direction = -(atan2(sin(direction), cos(direction)) + (M_PI));
  }
  else
  {
    direction = -(atan2(sin(direction), cos(direction)) + (2.f * M_PI));
  }

  start_x = real_x;
  start_y = real_y;
}



bool Ball::IsActive()
{
  return active;
}

void Ball::UpdateFrame(float elapsed, size_t current_ms)
{
  if (!active) return;
  float capped_frames = elapsed;
  if (capped_frames > TARGET_FPS / 10.f)
    capped_frames = TARGET_FPS / 10.f;
  float distance = capped_frames * speed;
  float old_x = real_x;
  float old_y = real_y;

  //cap the distance or x/y?
  real_x = real_x + cos(direction) * distance;
  real_y = real_y + sin(direction) * distance;

  if (real_x < 0.f)
  {
    real_x = 0.f;
    Collision(CollisionType::VERTICAL);
  }
  else if (real_x + BALL_DIMENSION > GAME_WIDTH)
  {
    real_x = GAME_WIDTH - BALL_DIMENSION;
    Collision(CollisionType::VERTICAL);
  }
  
  if (real_y < 0.f) 
  {
    real_y = 0.f;
    Collision(CollisionType::HORIZONTAL);
  }
  else if (real_y + BALL_DIMENSION > GAME_HEIGHT)
  {
    active = false;
  }
  //Todo old_Y < BAT_Y - BAT_HEIGHT?
  else if ((real_y + BALL_DIMENSION >= BAT_Y) && old_y < BAT_Y)
  {
    float x = GameController::GetInstance()->bat->x;
    float x1 = x - BALL_DIMENSION;
    float x2 = x + BAT_WIDTH;
    if (old_y < BAT_Y)
    {
      if (real_x >= x1 && real_x <= x2)
      {
        real_y = BAT_Y - BALL_DIMENSION - 1;
        float portion = std::clamp((x2 - real_x) / (BAT_WIDTH + BALL_DIMENSION), 0.0f, 1.0f);

        
        float cdist = std::clamp(0.5 - fabs(portion - 0.5), 0.15, 0.5);
        float dir = -(log10f(cdist * 20.f) * M_PI / 2.f);
        if (portion > 0.5f)
          dir = -dir - M_PI;

       // printf("portion: %f, cdist = %f, dir=%f\n", portion, cdist, dir * (180 / M_PI));

        direction = dir;
        start_x = real_x;
        start_y = real_y;
        
        
        /*
        float min, max;
        if (portion > 0.75f)
        {
           min = ((3.f * M_PI) / 4.f);
           max = ((5.f * M_PI) / 6.f);
        }
        else if (portion < 0.25f)
        {
          min = (M_PI / 6.f);
          max = (M_PI / 4.f);
        }
        else
        {
          min = (M_PI / 4.f);
          max = ((3.f * M_PI) / 4.f);
        }
       RandomDirection(min, max);
       */
       
      }
    }
  }

 
  Update(real_x, real_y);
}

void Ball::Start()
{
  speed = 5.f;
  //real_x = GAME_WIDTH / 2;
  //real_y = GAME_HEIGHT /2;
  real_x = x;
  real_y = y;
  //start_x = real_x;
  //start_y = real_y;
  float min = (M_PI / 4.f);
  float max = ((3.f * M_PI) / 4.f);
  //direction = -(M_PI / 6);
  RandomDirection(min, max);
} 