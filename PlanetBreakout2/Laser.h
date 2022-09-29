#pragma once
#include "DynamicCollider.h"

struct Laser : DynamicCollider
{
  Laser() :
    DynamicCollider(EntityType::POWERUP, POWERUP_SPRITE, 2, 1)
  {
    active = false;
    RegisterCollision(CollisionType::BRICK);
    RegisterCollision(CollisionType::WALL_HOR);
  }
  void Start() override;
  virtual void CollisionBrick(uint32_t index) override;
  virtual void CollisionHorizontalWall() override;
  virtual void PostFrameUpdate() override;
};