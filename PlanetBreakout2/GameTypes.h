#pragma once
#include <exception>
#include <d2d1helper.h>
#include <string>
#include "Constants.h"
#include "Drawable.h"

typedef int EntityId;
enum class EntityType { BAT, BALL, BRICK, POWERUP, VISUAL, TILE };
enum class BatType { SHORT_BAT, NORMAL_BAT, LONG_BAT, LASER_BAT };
enum BrickType { NORMAL_BRICK, INVINCIBLE_BRICK, NO_POINT };
enum TileType { NONE_TILE };

//Used for loading types, user cannot specify a no-point brick
#define ISVALID_BRICKTYPE(v) (v >= NORMAL_BRICK && v <= INVINCIBLE_BRICK)
#define ISVALID_TILETYPE(v) (v == NONE_TILE)


#define POWERUP_SIZE 12
enum PowerupType { LASER_BAT, BONUS_POINTS, CREATOR_BALL,
  HYPER_BALL, BARRIER, EXTRA_LIFE, BRICK_SHIELD, GHOST,
  PORTAL, DRONE, STRIKE, EMP };

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