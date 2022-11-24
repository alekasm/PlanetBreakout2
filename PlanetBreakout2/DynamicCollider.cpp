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
  if (type == EntityType::VISUAL_EFFECT)
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
      float bat_x = GameController::GetInstance()->GetBat()->x;
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

    //TODO: check to see if already in a brick


    //The brick check collision is slightly more advanced than checking
    //if the destination will hit a brick. Instead it checks all the bricks
    //along a path. This is to prevent brick-skipping, in which the speed
    //of the ball skips over a brick.  

    float next_right = ceilf(real_x + width);
    float next_bottom = ceilf(real_y + height);
    uint32_t next_px1 = floorf(real_x) / BRICK_WIDTH;
    uint32_t next_py1 = floorf(real_y) / BRICK_HEIGHT;
    uint32_t next_px2 = next_right / BRICK_WIDTH;
    uint32_t next_py2 = next_bottom / BRICK_HEIGHT;

    float old_right = ceilf(old_x + width);
    float old_bottom = ceilf(old_y + height);
    uint32_t old_px1 = floorf(old_x) / BRICK_WIDTH;
    uint32_t old_py1 = floorf(old_y) / BRICK_HEIGHT;
    uint32_t old_px2 = old_right / BRICK_WIDTH;
    uint32_t old_py2 = old_bottom / BRICK_HEIGHT;

    int32_t x_dir = old_px1 < next_px1 ? 1 : -1;
    int32_t y_dir = old_py1 < next_py1 ? 1 : -1;

    uint32_t x_start = x_dir < 0 ? old_px2 : old_px1;
    uint32_t x_end = x_dir > 0 ? next_px2 : next_px1;

    uint32_t y_start = y_dir < 0 ? old_py2 : old_py1;
    uint32_t y_end = y_dir > 0 ? next_py2 : next_py1;

    BrickMap& map = GameController::GetInstance()->GetBrickMap();
    for (uint32_t y = y_start; y != (y_end + y_dir); y += y_dir)
    {
      for (uint32_t x = x_start; x != (x_end + x_dir); x += x_dir)
      {
        uint32_t index;
        if (!GetBrickIndex(x, y, index))
          continue;
        BrickMap::iterator it = map.find(index);
        if (it != map.end() && !it->second.empty())
        {
          CollisionBrick(it->first);
          return;
        }
      }
    }


  }
}