#include "GameLevel.h"
#include "LogicHelper.h"
#include <algorithm>
#include <time.h>

//Returns true if a brick was removed
bool BrickMap::Erase(uint32_t brick_index, bool force)
{
  std::vector<Brick>& bricks = at(brick_index);
  if (bricks.empty()) return false;
  size_t vector_index = bricks.size() - 1;
  if (force || bricks.at(vector_index).subtype == BrickType::NORMAL_BRICK)
  {
    bricks.erase(bricks.begin() + vector_index);
    if (bricks.empty())
    {
      erase(brick_index);
      auto it = brick_check.find(brick_index);
      if (it != brick_check.end())
        brick_check.erase(it);
    }
    return true;
  }
  return false;
}

void BrickMap::Add(uint32_t index, Brick& brick)
{
  auto it = brick_check.find(index);
  if (brick.subtype == BrickType::INVINCIBLE_BRICK)
  {
    if (it != brick_check.end())
      brick_check.erase(it);
    this->operator[](index).clear();
  }
  else
  {
    if (it == brick_check.end())
      brick_check.insert(index);
  }
  this->operator[](index).push_back(brick);
}

void BrickMap::Clear()
{
  clear();
  brick_check.clear();
}

bool BrickMap::Empty()
{
  return brick_check.empty();
}

void GameLevel::clear()
{
  brickMap.Clear();
  author.clear();
  map_name.clear();
}

void GameLevel::AddBrick(Brick brick)
{
  if (IsReservedBrick(brick)) return;
  uint32_t index = GetBrickIndex(brick.col, brick.row);
  brickMap.Add(index, brick);
}

BrickMap& GameLevel::GetBrickMap()
{
  return brickMap;
}

void Campaign::AddHighscore(Highscore& highscore)
{
  if (highscores.size() < 5)
    highscores.push_back(highscore);
}

namespace
{
  const std::wstring HighscoreNames[HIGHSCORE_SIZE] =
  {
    L"Zoomie", L"Alek", L"Tundra", L"David", L"Juneau"
  };
}

std::vector<Highscore>& Campaign::GetHighscores()
{
  if (highscores.size() < HIGHSCORE_SIZE)
  {
    //srand(time(NULL));
    for (unsigned i = highscores.size(); i < HIGHSCORE_SIZE; ++i)
    {
      Highscore h;
      h.name = HighscoreNames[i];
      h.score = rand() % 5000 + 100;
      time_t now = time(NULL);
      h.date = now - (rand() % 365 + 1) * 24 * 60 * 60;
      highscores.push_back(h);
    }
    std::sort(highscores.begin(), highscores.end());
  }
  return highscores;
}

void Campaign::SaveHighscores()
{

}