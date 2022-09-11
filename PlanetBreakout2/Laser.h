#pragma once
#include "DynamicEntity.h"

struct Laser : DynamicEntity
{
  Laser() :
    DynamicEntity(EntityType::POWERUP, POWERUP_SPRITE, 3, 1)
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