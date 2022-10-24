#pragma once
#include "DynamicCollider.h"

struct DroneLaser : DynamicCollider
{
  DroneLaser() :
    DynamicCollider(EntityType::VISUAL, 8, 8)
  {
    active = false;
    RegisterCollision(CollisionType::BRICK);
    RegisterCollision(CollisionType::WALL_HOR);
    RegisterCollision(CollisionType::BAT);
    RegisterCollision(CollisionType::WALL_VERT);
  }
  void Start() override;
  virtual void CollisionVerticalWall() override;
  virtual void CollisionHorizontalWall() override;
  virtual void CollisionBrick(uint32_t index) override;
  virtual void CollisionBat(float x1, float x2) override;
};
