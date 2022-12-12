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
  const std::set<uint32_t>& GetBrickCheckSet() const;
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
  time_t date = 0;
  DifficultyType difficulty;
  bool pseudo = false;
  bool operator<(const Highscore& other) const
  {
    return score > other.score;
  }
  const uint64_t CalculateChecksum(const std::wstring& mission) const
  {
    uint64_t count = 0;
    for (wchar_t w : name)
      count += w;
    for (wchar_t w : mission)
      count += w;
    count += score;
    count += date;
    count += (int)difficulty;
    return UINT64_MAX - count;
  }

  const bool IsValid(const std::wstring& mission, uint64_t checksum) const
  {
    return checksum == CalculateChecksum(mission);
  }
};

struct CampaignLevel
{
  std::wstring name;
  uint32_t priority = 0xFFFFFFFF;
};

typedef std::unordered_map<DifficultyType, std::vector<Highscore>> HighscoreMap;
struct Campaign
{
  Campaign();
  std::wstring bat_sprite = L"bat";
  std::wstring ball_sprite = L"ball";
  std::wstring name = L"None";
  std::vector<GameLevel> levels;
  std::vector<CampaignLevel> clevels;
  std::wstring path;
  void AddHighscore(Highscore&);
  bool NewHighscore(DifficultyType, uint16_t score);
  const std::vector<Highscore>& GetHighscores(DifficultyType) const;
  void SetTestMode();
  bool IsTestMode();
  const HighscoreMap& GetHighscores() const;
  void SetPriority(uint32_t);
  const uint32_t GetPriority() const;
  bool operator<(const Campaign& other) const {
    return priority < other.GetPriority();
  }
  bool operator==(const Campaign& other) const {
    return name.compare(other.name.c_str()) == 0;
  }
private:
  HighscoreMap highscores{
    {DifficultyType::EASY, {}},
    {DifficultyType::MEDIUM, {}},
    {DifficultyType::HARD, {}}
  };
  bool test = false;
  uint32_t priority = 0xFFFFFFFF;
 
};