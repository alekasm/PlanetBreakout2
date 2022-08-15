#pragma once
#include <unordered_map>
#include "Constants.h"
#include "Button.h"
#include "ResourceLoader.h"

enum class MainMenuState {MAIN, INFO, HIGHSCORE, CAMPAIGN_SELECT};
struct MainMenu
{
  void initialize();
  //std::vector<Button*>& GetButtons(MainMenuState);
  std::vector<Button*>& GetButtons();
private:
  std::unordered_map<MainMenuState, std::vector<Button*>> buttons;
  MainMenuState state = state = MainMenuState::MAIN;
};