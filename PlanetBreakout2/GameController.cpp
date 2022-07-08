#include "GameController.h"
GameController* GameController::instance = nullptr;

GameController* GameController::GetInstance()
{
  if (instance == nullptr)
    instance = new GameController();
  return instance;
}

void GameController::CreateGame(Campaign& campaign)
{
  this->campaign = campaign;
  bat = new Bat(campaign.bat_sprite);
  ball = new Ball(campaign.ball_sprite);
  ball->Update(0, (GRID_ROWS - 4) * BRICK_HEIGHT);
  ball->MoveCenterX(GAME_WIDTH / 2);
  bat->Update(0, (GRID_ROWS - 2) * BRICK_HEIGHT);
  bat->MoveCenterX(GAME_WIDTH / 2);
  type = GAME_NORMAL;
}

void GameController::EndGame()
{
  if (bat != nullptr)
  {
    delete bat;
    bat = nullptr;
  }
  if (ball != nullptr)
  {
    delete ball;
    ball = nullptr;
  }  
  current_level = 0;
  type = GAME_EDITOR;
}