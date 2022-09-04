#include "GameLevel.h"
#include "LogicHelper.h"
#include <algorithm>
#include <time.h>


//Returns true if a brick was removed
size_t BrickMap::Erase(uint32_t brick_index, UINT uFlags)
{
  BrickMap::iterator map_it = find(brick_index);
  if (map_it == end() || map_it->second.empty())
    return 0;

  std::vector<Brick>& bricks = map_it->second;

  std::vector<Brick>::reverse_iterator brick_it;
  size_t erased = 0;
  for (brick_it = bricks.rbegin(); brick_it != bricks.rend();)
  {
    if ((uFlags & PB2_BRICKMAP_ERASE_ANY) ||
      brick_it->subtype == BrickType::NORMAL_BRICK)
    {
      brick_it = decltype(brick_it){
        bricks.erase(std::next(brick_it).base())
      };
      ++erased;
      
      bool end_of_index = false;
      if (brick_it == bricks.rend())
      {
        erase(brick_index);
        end_of_index = true;
      }
      else if (brick_it->subtype == BrickType::INVINCIBLE_BRICK)
      {
        end_of_index = true;
      }

      if (end_of_index)
      {
        auto it = brick_check.find(brick_index);
        if (it != brick_check.end())
          brick_check.erase(it);
      }

      if (end_of_index || uFlags & PB2_BRICKMAP_ERASE_TOP)
      { 
        return erased;
      }
    }
    else
    {
      ++brick_it;
    }
  }
  return erased;
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

namespace
{
  const std::wstring HighscoreNames[HIGHSCORE_SIZE] =
  {
    L"Zoomie", L"Alek", L"Tundra", L"David", L"Juneau",
    L"Kimber", L"Max", L"Minnie", L"Kobi", L"Nanuq"
  };
}


void Campaign::AddHighscore(Highscore& highscore)
{
  highscores.push_back(highscore);
  std::sort(highscores.begin(), highscores.end());
  highscores.erase(highscores.begin() + HIGHSCORE_SIZE, highscores.end());
}

bool Campaign::NewHighscore(uint16_t score)
{
  for (Highscore& h : highscores)
  {
    if (score > h.score)
      return true;
  }
  return false;
}

Campaign::Campaign()
{
  for (unsigned i = 0; i < HIGHSCORE_SIZE; ++i)
  {
    Highscore h;
    h.name = HighscoreNames[rand() % HIGHSCORE_SIZE];
    h.score = (i + 1) * 100;
    time_t now = time(NULL);
    h.date = now - (rand() % 365 + 1) * 24 * 60 * 60;
    h.pseudo = true;
    highscores.push_back(h);
  }
  std::sort(highscores.begin(), highscores.end());
}

std::vector<Highscore>& Campaign::GetHighscores()
{
  return highscores;
}

void Campaign::SaveHighscores()
{

}