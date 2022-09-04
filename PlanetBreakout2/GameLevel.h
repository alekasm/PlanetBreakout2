#pragma once
#include <vector>
#include <string>
#include <set>
#include <unordered_map>
#include "GameTypes.h"

#define PB2_BRICKMAP_ERASE_TOP 0x0001
#define PB2_BRICKMAP_ERASE_ALL 0x0002
#define PB2_BRICKMAP_ERASE_ANY 0x0004



struct BrickMap : std::unordered_map<uint32_t, std::vector<Brick>>
{
  //bool Erase(uint32_t index, bool force = false);
  size_t Erase(uint32_t brick_index, UINT uFlags);
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

struct Highscore
{
  std::wstring name;
  uint16_t score = 0;
  time_t date;
  bool pseudo = false;
  bool operator<(const Highscore& other) const
  {
    return score > other.score;
  }
};

struct Campaign
{
  Campaign();
  std::wstring bat_sprite = L"bat";
  std::wstring ball_sprite = L"ball";
  std::wstring name = L"None";
  std::vector<GameLevel> levels;
  std::wstring path;
  void AddHighscore(Highscore&);
  bool NewHighscore(uint16_t score);
  std::vector<Highscore>& GetHighscores();
  void SaveHighscores();
private:
  std::vector<Highscore> highscores;
};