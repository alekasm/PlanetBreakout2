#pragma once
#include "DynamicEntity.h"
#include <unordered_map>

enum CollisionType { WALL_VERT = 1, WALL_HOR = 2, BRICK = 4, BAT = 8 };
//typedef std::unordered_map<CollisionType, CollisionFunc> CollisionMap;
struct DynamicCollider : DynamicEntity
{
  DynamicCollider(EntityType type,
    unsigned width, unsigned height) :
    DynamicEntity(type, width, height)
  {
  }
  void PreFrameUpdate() override;
  void RegisterCollision(CollisionType);
  virtual void CollisionVerticalWall();
  virtual void CollisionHorizontalWall();
  virtual bool CollisionBrick(uint32_t index);
  virtual void CollisionBat(float x1, float x2);
private:
  unsigned collision_mask = 0;
};
