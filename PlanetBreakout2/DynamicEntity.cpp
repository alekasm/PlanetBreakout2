#include "DynamicEntity.h"
#include "GameController.h"
#include "LogicHelper.h"
#include <cmath>


void DynamicEntity::CollisionVerticalWall() {}
void DynamicEntity::CollisionHorizontalWall() {}
void DynamicEntity::CollisionBrick(uint32_t index) {}
void DynamicEntity::CollisionBat(float x1, float x2) {}
void DynamicEntity::PostFrameUpdate() {}

const bool DynamicEntity::IsActive() const
{
  return active;
}
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

void DynamicEntity::RegisterCollision(CollisionType type)
{
  collision_mask |= type;
}

void DynamicEntity::SetActive(bool value)
{
  active = value;
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

  float y_new_bottom = real_y + height;
  float y_old_bottom = old_y + height;
  bool old_above = y_old_bottom <= BAT_Y;
  bool new_below = y_new_bottom >= BAT_Y;


  if (real_x < 0.f)
  {
    real_x = 0.f;
    if (collision_mask & CollisionType::WALL_VERT)
      CollisionVerticalWall();
  }
  else if (real_x + BALL_DIMENSION > GAME_WIDTH)
  {
    real_x = GAME_WIDTH - BALL_DIMENSION;
    if (collision_mask & CollisionType::WALL_VERT)
      CollisionVerticalWall();
  }

  if (real_y < 0.f)
  {
    real_y = 0.f;
    if (collision_mask & CollisionType::WALL_HOR)
      CollisionHorizontalWall();
  }
  else if (real_y /* + height */ > GAME_HEIGHT)
  {
    active = false;
  }
  else if (old_above && new_below)
  {
    if(collision_mask & CollisionType::BAT)
    {
      float bat_x = GameController::GetInstance()->bat->x;
      //Add a little forgiveness to the bat dimension
      float x1 = bat_x - width - 2.5f;
      float x2 = bat_x + BAT_WIDTH + 2.5f;
      if (real_x >= x1 && real_x <= x2)
      {
        CollisionBat(x1, x2);
      }
    }
  } //...
  else if(collision_mask & CollisionType::BRICK)
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
    for (const POINT& p : index_check)
    {
      BrickMap::iterator it = map.find(GetBrickIndex(p.x, p.y));
      if (it != map.end() && !it->second.empty())
      {
        CollisionBrick(it->first);
        break;
      }
    }
  }
  Update(real_x, real_y);
  PostFrameUpdate();
}