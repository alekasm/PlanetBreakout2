#pragma once
#include <exception>
#include <d2d1helper.h>
#include <string>
#include "Constants.h"
#include "Drawable.h"

typedef int EntityId;
enum EntityType { BAT, BALL, BRICK, POWERUP, TILE };
enum BallType { NORMAL_BALL, FAST_BALL, SLOW_BALL };
enum PowerUpType { FAST_BALL_PU, SLOW_BALL_PU, LONG_BAT_PU, SHORT_BAT_PU, LASER_BAT_PU };
enum BatType { SHORT_BAT, NORMAL_BAT, LONG_BAT, LASER_BAT };
enum BrickType { NORMAL_BRICK, INVINCIBLE_BRICK };
enum TileType { NONE_TILE };

#define ISVALID_ENTITYTYPE(v) (v >= BAT && v <= TILE)
#define ISVALID_BALLTYPE(v) (v >= NORMAL_BALL && v <= SLOW_BALL)
#define ISVALID_POWERUPTYPE(v) (v >= FAST_BALL_PU && v <= LASER_BAT_PU)
#define ISVALID_BATTYPE(v) (v >= SHORT_BAT && v <= LASER_BAT)
#define ISVALID_BRICKTYPE(v) (v >= NORMAL_BRICK && v <= INVINCIBLE_BRICK)
#define ISVALID_TILETYPE(v) (v == NONE_TILE)

struct Entity :  Drawable
{
  EntityType type;
  std::wstring sprite;
  Entity(EntityType type, std::wstring sprite, unsigned width, unsigned height) :
    Drawable(width, height),
    type(type), sprite(sprite)
  {
  }
  void MoveCenterX(unsigned cx)
  {
    Update(cx - (width / 2), y);
  }

  void MoveCenterY(unsigned cy)
  {
    Update(x, cy - (height / 2));
  }

  void MoveCenter(unsigned cx, unsigned cy)
  {
    Update(cx - (width / 2), cy - (height / 2));
  }
};

struct Bat : Entity
{
  BatType subtype;
  Bat(std::wstring sprite) :
    Entity(EntityType::BAT, sprite, BAT_WIDTH, BAT_HEIGHT),
    subtype(BatType::NORMAL_BAT)
  {
  }
  void UpdateType(BatType subtype)
  {
  }
};

struct Ball : Entity
{
  BallType subtype;
  Ball(std::wstring sprite) :
    Entity(EntityType::BALL, sprite, BALL_DIMENSION, BALL_DIMENSION),
    subtype(BallType::NORMAL_BALL)
  {
  }
  void UpdateFrame(float elapsed, size_t current_ms);
  void Start();
  void UpdateType(BallType subtype);
  bool IsActive();
private:
  enum CollisionType {HORIZONTAL, VERTICAL};
  void Collision(CollisionType);
  void RandomDirection(float min, float max);
  bool active = true;
  float speed = 1.0f;
  float direction = 0.f;
  float real_x = 0.f;
  float real_y = 0.f;
};


struct Brick : Entity
{
  unsigned col, row;
  BrickType subtype;
  Brick() = default;
  Brick(BrickType subtype, std::wstring sprite, uint32_t x, uint32_t y) :
    Entity(EntityType::BRICK, sprite, BRICK_WIDTH, BRICK_HEIGHT),
    subtype(subtype), col(x), row(y)
  {
    Update(x * BRICK_WIDTH, y * BRICK_HEIGHT);
  }

  Brick(const Brick& other, uint32_t x, uint32_t y) :
    Entity(EntityType::BRICK, other.sprite, BRICK_WIDTH, BRICK_HEIGHT),
    subtype(other.subtype), col(x), row(y)
  {
    Update(x * BRICK_WIDTH, y * BRICK_HEIGHT);
  }
};