#include "GameController.h"
GameController* GameController::instance = nullptr;

GameController* GameController::GetInstance()
{
  if (instance == nullptr)
    instance = new GameController();
  return instance;
}