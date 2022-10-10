#include "DynamicCollider.h"
#include "GameController.h"
#include "LogicHelper.h"
#include <cmath>


void DynamicCollider::CollisionVerticalWall() {}
void DynamicCollider::CollisionHorizontalWall() {}
void DynamicCollider::CollisionBrick(uint32_t index) {}
void DynamicCollider::CollisionBat(float x1, float x2) {}


void DynamicCollider::RegisterCollision(CollisionType type)
{
  collision_mask |= type;
}


void DynamicCollider::PreFrameUpdate()
{
  if (type == EntityType::VISUAL)
    return;

  float y_new_bottom = real_y + height;
  float y_old_bottom = old_y + height;
  bool old_above = y_old_bottom <= BAT_Y;
  bool new_below = y_new_bottom >= BAT_Y;

  bool collision = false;
  
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
    //collision = true;
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
}