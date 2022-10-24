#pragma once
#include <exception>
#include <d2d1helper.h>
#include <string>
#include "Constants.h"
#include "Drawable.h"

enum class EntityType { SPRITE, VISUAL, VISUAL_EFFECT };
enum BrickType { NORMAL_BRICK, INVINCIBLE_BRICK, NO_POINT };
//Used for loading types, user cannot specify a no-point brick
#define ISVALID_BRICKTYPE(v) (v >= NORMAL_BRICK && v <= INVINCIBLE_BRICK)

#define POWERUP_SIZE 12
enum PowerupType {
  LASER_BAT, BONUS_POINTS, CREATOR_BALL,
  HYPER_BALL, BARRIER, EXTRA_LIFE, BRICK_SHIELD, GHOST,
  PORTAL, DRONE, STRIKE, EMP
};

struct Entity :  Drawable
{
  EntityType type;
  Entity(EntityType type, unsigned width, unsigned height);
  void SetSprite(std::wstring sprite);
  bool HasSprite();
  const std::wstring& GetSprite() const;
  void MoveCenterX(unsigned cx);
  void MoveCenterY(unsigned cy);
  void MoveCenter(unsigned cx, unsigned cy);
  void SetActive(bool value);
  const bool IsActive() const;
protected:
  std::wstring sprite;
  bool active = true;
};

struct Bat : Entity
{
  Bat(std::wstring sprite) :
    Entity(EntityType::SPRITE, BAT_WIDTH, BAT_HEIGHT)
  {
    SetSprite(sprite);
  }
};

struct Brick : Entity
{
  unsigned col, row;
  BrickType subtype;
  Brick() = default;
  Brick(BrickType subtype, std::wstring sprite, uint32_t x, uint32_t y) :
    Entity(EntityType::SPRITE, BRICK_WIDTH, BRICK_HEIGHT),
    subtype(subtype), col(x), row(y)
  {
    Update(x * BRICK_WIDTH, y * BRICK_HEIGHT);
    SetSprite(sprite);
  }

  Brick(const Brick& other, uint32_t x, uint32_t y) :
    Entity(EntityType::SPRITE, BRICK_WIDTH, BRICK_HEIGHT),
    subtype(other.subtype), col(x), row(y)
  {
    Update(x * BRICK_WIDTH, y * BRICK_HEIGHT);
    SetSprite(other.GetSprite());
  }
};