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

void DynamicEntity::SetPosition(float x, float y)
{
  real_x = x;
  real_y = y;
  Update(real_x, real_y);
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

  bool collision = false;

  if (type == EntityType::VISUAL)
    goto end_frame_update;
  
  if (real_x < 0.f)
  {
    collision = true;
    real_x = 0.f;
    if (collision_mask & CollisionType::WALL_VERT)
      CollisionVerticalWall();
  }
  else if (real_x + width >= GAME_WIDTH)
  {
    collision = true;
    real_x = GAME_WIDTH - width;
    if (collision_mask & CollisionType::WALL_VERT)
      CollisionVerticalWall();
  }

  if (real_y < 0.f)
  {
    collision = true;
    real_y = 0.f;
    if (collision_mask & CollisionType::WALL_HOR)
      CollisionHorizontalWall();
  }
  else if (real_y > GAME_HEIGHT)
  {
    collision = true;
    active = false;
  }
  else if (old_above && new_below)
  {
    collision = true;
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
  }
  
  if(!collision && (collision_mask & CollisionType::BRICK))
  {
    //Check brick collisions 
    float right = real_x + width;
    float bottom = real_y + height;
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
      uint32_t index;
      if (!GetBrickIndex(p.x, p.y, index))
        continue;
      BrickMap::iterator it = map.find(index);
      if (it != map.end() && !it->second.empty())
      {
        CollisionBrick(it->first);
        break;
      }
    }
  }
  end_frame_update:
  Update(real_x, real_y);
  PostFrameUpdate();
}