#pragma once
#include "DynamicEntity.h"
struct Ball : DynamicEntity
{
  Ball(std::wstring sprite) :
    DynamicEntity(EntityType::BALL, sprite, BALL_DIMENSION, BALL_DIMENSION)
  {
    RegisterCollision(CollisionType::WALL_VERT);
    RegisterCollision(CollisionType::WALL_HOR);
    RegisterCollision(CollisionType::BAT);
    RegisterCollision(CollisionType::BRICK);
  }
  void Start() override;
  virtual void CollisionVerticalWall() override;
  virtual void CollisionHorizontalWall() override;
  virtual void CollisionBrick(uint32_t index) override;
  virtual void CollisionBat(float x1, float x2) override;
  void SetPosition(float x, float y);
private:
  enum class BallCollisionType { HORIZONTAL, VERTICAL };
  void Collision(BallCollisionType);
  void RandomDirection(float min, float max);
};