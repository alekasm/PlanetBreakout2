#include "GameLevel.h"

void GameLevel::clear()
{
  bricks.clear();
  background.clear();
  map_name.clear();
}

bool IsReservedBrick(const Brick& brick)
{
  return (brick.row == 0 || brick.row >= GRID_ROWS - 4 ||
    brick.col == 0 || brick.col == GRID_COLUMNS - 1);
}

void GameLevel::validate()
{
  std::vector<Brick>::iterator it;
  for (it = bricks.begin(); it != bricks.end();)
  {
    if(IsReservedBrick(*it))
    {
      it = bricks.erase(it);
      continue;
    }
    ++it;
  }
}

void GameLevel::AddBrick(Brick brick)
{
  if (!IsReservedBrick(brick))
    bricks.push_back(brick);
}