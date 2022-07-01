#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "GameTypes.h"
typedef std::unordered_map<uint32_t, std::vector<Brick>> BrickMap;
struct GameLevel
{
  BrickMap brickMap;
  //std::vector<Brick> bricks;
  std::wstring background;
  std::wstring map_name;
  void clear();
  void AddBrick(Brick);
  void validate();
  static inline uint32_t GetIndex(uint32_t col, uint32_t row);
};