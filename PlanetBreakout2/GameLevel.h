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
  void validate()
  {
    std::vector<Brick>::iterator it;
    for (it = bricks.begin(); it != bricks.end(); ++it)
    {
      std::vector<Brick>::iterator prev_it = std::prev(it);
      if (it->col == 0)
      {
        bricks.erase(it);
        it = prev_it;
      }
    }
  }
};