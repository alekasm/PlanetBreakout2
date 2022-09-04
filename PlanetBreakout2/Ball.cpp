#include "GameTypes.h"
#include "GameController.h"
#include "LogicHelper.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <random>
#include <set>


inline float normalize(float angle)
{
  if (angle < 0)
    return atan2(sin(angle), cos(angle)) + (2.f * M_PI);
 return atan2(sin(angle), cos(angle)) + M_PI;
}

void Ball::RandomDirection(float min, float max)
{
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
  speed = std::clamp(speed + 0.05f, 1.0f, 8.0f);
}

const bool Ball::IsActive() const
{
  return active;
}

float Ball::GetSpeed()
{
  return speed;
}

void Ball::UpdateFrame(int64_t elapsed)
{
  if (!active) return;

  float frame_modifier = 1.0f;
  if (elapsed < TARGET_FRAMES) //slow down
    frame_modifier = elapsed / TARGET_FRAMES;
  float distance = frame_modifier * speed;
  float old_x = real_x;
  float old_y = real_y;

  //cap the distance or x/y?
  //TODO calculate worst case scenario for bricks
  real_x = real_x + cos(direction) * distance;
  real_y = real_y + sin(direction) * distance;

  float y_new_bottom = real_y + BALL_DIMENSION;
  float y_old_bottom = old_y + BALL_DIMENSION;
  bool old_above = y_old_bottom <= BAT_Y;
  bool new_below = y_new_bottom >= BAT_Y;

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
  else if (old_above && new_below)
  {
    float bat_x = GameController::GetInstance()->bat->x;
    //Add a little forgiveness to the bat dimension
    float x1 = bat_x - BALL_DIMENSION - 2.5f;
    float x2 = bat_x + BAT_WIDTH + 2.5f;
    if (real_x >= x1 && real_x <= x2)
    {
      //Prevent an immediate retrigger of collision
      real_y = BAT_Y - BALL_DIMENSION - 0.5f;
      float portion = std::clamp((x2 - real_x) / (BAT_WIDTH + BALL_DIMENSION), 0.0f, 1.0f);
      //float dir = M_PI / 6.f + ((4.f * portion) * M_PI / 6.f); //pi/7  - 5pi/6
      //float dir = M_PI / 8.f + ((6.f * portion) * M_PI / 8.f); //pi/8  - 7pi/8
      float dir = M_PI / 10.f + ((8.f * portion) * M_PI / 10.f); //pi/10 - 9pi/10
      direction = -dir;
    }
  }
  else
  { 
    //Check brick collisions 
    float right = real_x + BALL_DIMENSION;
    float bottom = real_y + BALL_DIMENSION;
    long px1 = real_x / BRICK_WIDTH;
    long py1 = real_y / BRICK_HEIGHT;
    long px2 = right / BRICK_WIDTH;
    long py2 = bottom / BRICK_HEIGHT;

    std::vector<POINT> index_check = {
      POINT { px1, py1 },
      POINT { px1, py2 },
      POINT { px2, py1 },
      POINT { px2, py2 }
    };

    BrickMap& map = GameController::GetInstance()->GetBrickMap();
    for (const POINT&  p : index_check)
    {
      BrickMap::iterator it = map.find(GetBrickIndex(p.x, p.y));
      if (it != map.end() && !it->second.empty())
      {
        GameController::GetInstance()->BreakBrick(this, it->first);

        RECT brickRect = GetBrickRect(p.x, p.y);
        if (old_y > brickRect.bottom || (old_y + BALL_DIMENSION) < brickRect.top)
          Collision(CollisionType::HORIZONTAL);
        else
          Collision(CollisionType::VERTICAL);
       
        

        //Seems fine for now, instead of manually setting ball position
        real_x = real_x + cos(direction);
        real_y = real_y + sin(direction);
        break;
      }
    }
  } 
  Update(real_x, real_y);
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