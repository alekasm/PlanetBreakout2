#include "GameController.h"
GameController* GameController::instance = nullptr;

GameController* GameController::GetInstance()
{
    if (instance == nullptr)
        instance = new GameController();
    return instance;
}

void GameController::RemoveIllegalBricks(GameLevel& level)
{
    std::vector<Brick>::iterator it;
    for(it = level.bricks.begin(); it != level.bricks.end();)
    {
        if (it->row == 0 || it->row >= GRID_ROWS - 2 ||
            it->col == 0 || it->col == GRID_COLUMNS - 1)
        {
            it = level.bricks.erase(it);
            continue;
        }
        ++it;
    }
}
void GameController::AddGameBricks(GameLevel& level)
{

}
