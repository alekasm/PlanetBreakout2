#pragma once
#include "DynamicCollider.h"
struct Ball : DynamicCollider
{
  Ball(std::wstring sprite) :
    DynamicCollider(EntityType::SPRITE, BALL_DIMENSION, BALL_DIMENSION)
  {
    SetSprite(sprite);
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
private:
  enum class BallCollisionType { HORIZONTAL, VERTICAL };
  void Collision(BallCollisionType);
};