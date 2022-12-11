#pragma once
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <string>
#include "GameTypes.h"
#include "GameLevel.h"

struct GameLoader
{
  static bool LoadMap(const std::wstring& filename, GameLevel& level);
  static bool LoadCampaigns();
  static bool SaveMap(GameLevel& level, std::wstring& save_out);
  static bool SaveCampaign(const Campaign&);
  static std::vector<Campaign>& GetCampaigns();
private:
  static std::vector<Campaign> campaignVec;
  static HCURSOR cursor;
};