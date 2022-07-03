#include "GameLevel.h"
#include "LogicHelper.h"


void GameLevel::clear()
{
  //bricks.clear();
  brickMap.clear();
  background.clear();
  map_name.clear();
}

//TODO validate invincible bricks don't have any bricks underneath
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
  if (IsReservedBrick(brick)) return;
  uint32_t index = GetBrickIndex(brick.col, brick.row);
  if (brick.subtype == BrickType::INVINCIBLE_BRICK)
    brickMap[index].clear();
  brickMap[index].push_back(brick);
}