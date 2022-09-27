#pragma once
#include <unordered_map>
#include "Constants.h"
#include "Button.h"
#include "ResourceLoader.h"
#include "Menu.h"
#include "Star.h"

enum class MainMenuState {MAIN, INFO, HIGHSCORE, CAMPAIGN_SELECT};
struct MainMenu
{
  void initialize(ClientMenu*);
  //std::vector<Button*>& GetButtons(MainMenuState);
  std::vector<Button*>& GetButtons();
  MainMenuState GetState();
  uint32_t GetCampaignPage();
  const std::vector<Star> GetStars() const;
private:
  std::unordered_map<MainMenuState, std::vector<Button*>> buttons;
  MainMenuState state = MainMenuState::MAIN;
  uint32_t campaign_page = 0;
  std::vector<Star> stars;
};