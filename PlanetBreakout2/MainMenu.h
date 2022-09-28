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
  const std::wstring& GetDescription(PowerupType) const;
  const std::wstring& GetInfoDescription() const;
private:
  std::unordered_map<MainMenuState, std::vector<Button*>> buttons;
  MainMenuState state = MainMenuState::MAIN;
  uint32_t campaign_page = 0;
  std::unordered_map<PowerupType, std::wstring> powerupDescriptions;
  const std::wstring infoDescription =
    L"Written by Aleksander Krimsky (2022)\n"
    L"Created with C++17 and Direct2D\n";
};