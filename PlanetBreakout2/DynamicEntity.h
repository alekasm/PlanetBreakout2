#pragma once
#include "GameTypes.h"
#include <unordered_map>

enum CollisionType { WALL_VERT = 1, WALL_HOR = 2, BRICK = 4, BAT = 8 };
//typedef std::unordered_map<CollisionType, CollisionFunc> CollisionMap;
struct DynamicEntity : Entity
{
  DynamicEntity(EntityType type, std::wstring sprite,
    unsigned width, unsigned height) :
    Entity(type, sprite, width, height)
  {
  }
  void UpdateFrame(int64_t elapsed);
  virtual void Start() = 0;
  const bool IsActive() const;
  float GetSpeed();
  float GetRealX();
  float GetRealY();
  void RegisterCollision(CollisionType);
  virtual void CollisionVerticalWall();
  virtual void CollisionHorizontalWall();
  virtual void CollisionBrick(uint32_t index);
  virtual void CollisionBat(float x1, float x2);
protected:
  bool active = true;
  float speed = 1.0f;
  float direction = 0.f;
  float real_x = 0.f;
  float real_y = 0.f;
  float old_x = 0.f;
  float old_y = 0.f;
private:
  unsigned collision_mask = 0;
};
