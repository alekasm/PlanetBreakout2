#pragma once
#include <vector>
#include <string>
#include "GameTypes.h"
struct GameLevel
{
  std::vector<Brick> bricks;
  std::wstring background;
  std::wstring map_name;
  void clear();
  void AddBrick(Brick);
  void validate();
};