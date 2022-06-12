#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "GameTypes.h"
struct GameLevel
{
  std::vector<Brick> bricks;
  std::wstring background;
  std::wstring map_name;
  void clear()
  {
    bricks.clear();
    background.clear();
    map_name.clear();
  }
};