#pragma once
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <string>
#include "GameTypes.h"
#include "GameLevel.h"

/*
struct cmp_campaign {
  bool operator() (const Campaign& a, const Campaign& b) const {
    return a.GetId() < b.GetId();
  }
};
*/

//typedef std::set<Campaign, cmp_campaign> CampaignSet;
/*
bool cmp(std::pair<std::wstring, Campaign>& a,
  std::pair<std::wstring, Campaign>& b)
{
  return a.second.GetId() < b.second.GetId();
}
*/
typedef std::map<std::wstring, Campaign> CampaignMap;

struct GameLoader
{
  static bool LoadMap(const std::wstring& filename, GameLevel& level);
  static bool LoadCampaigns();
  static bool SaveMap(GameLevel& level, std::wstring& save_out);
  static bool SaveCampaign(const Campaign&);
  static std::vector<Campaign>& GetCampaigns();
private:
  static std::vector<Campaign> campaignVec;
  //static CampaignMap campaignMap;
  //static CampaignSet campaignSet;
  static HCURSOR cursor;
};