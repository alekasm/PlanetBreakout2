#pragma once
#include <vector>
#include <string>
#include <set>

#include <unordered_map>
#include "GameTypes.h"

struct BrickMap : std::unordered_map<uint32_t, std::vector<Brick>>
{
  bool Erase(uint32_t index, bool force = false);
  void Add(uint32_t, Brick&);
  void Clear();
  bool Empty();
private:
  std::set<uint32_t> brick_check;
};

struct GameLevel
{
  std::wstring author;
  std::wstring map_name;
  void clear();
  void AddBrick(Brick);
  BrickMap& GetBrickMap();
private:
  BrickMap brickMap;
};