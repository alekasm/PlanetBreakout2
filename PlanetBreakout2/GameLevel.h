#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "GameTypes.h"

typedef std::unordered_map<uint32_t, std::vector<Brick>> BrickMap;
struct GameLevel
{
  BrickMap brickMap;
  std::wstring author;
  std::wstring map_name;
  void clear();
  void AddBrick(Brick);
  void validate();
};