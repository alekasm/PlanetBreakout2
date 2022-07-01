#include "GameLevel.h"

void GameLevel::clear()
{
  //bricks.clear();
  brickMap.clear();
  background.clear();
  map_name.clear();
}

inline uint32_t GameLevel::GetIndex(uint32_t col, uint32_t row)
{
  return col + GRID_COLUMNS * row;
}

bool IsReservedBrick(const Brick& brick)
{
  return (brick.row == 0 || brick.row >= GRID_ROWS - 4 ||
    brick.col == 0 || brick.col == GRID_COLUMNS - 1);
}

void GameLevel::validate()
{
  BrickMap::iterator map_it;
  for (map_it = brickMap.begin(); map_it != brickMap.end(); ++map_it)
  {
    std::vector<Brick>::iterator it;
    for (it = map_it->second.begin(); it != map_it->second.end();)
    {
      if (IsReservedBrick(*it))
      {
        it = map_it->second.erase(it);
        continue;
      }
      ++it;
    }
  }
}

void GameLevel::AddBrick(Brick brick)
{
  if (!IsReservedBrick(brick))
    brickMap[GetIndex(brick.col, brick.row)].push_back(brick);
}