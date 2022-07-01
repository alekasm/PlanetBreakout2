#include "GameLoader.h"
#include <vector>
#include <fstream>
#include <conio.h>
#include <filesystem>
#include "Constants.h"
#include "ResourceLoader.h"
#include "Menu.h"

LevelMap GameLoader::level_map;
std::vector<Brick> GameLoader::assetBricks;
std::vector<std::wstring> GameLoader::assetBackgrounds;

std::vector<Brick>& GameLoader::GetAssetBricks()
{
  return assetBricks;
}
std::vector<std::wstring>& GameLoader::GetAssetBackgrounds()
{
  return assetBackgrounds;
}

//Used for parsing bricks from map files and for asset maps
//type,sprite,x,y
bool ParseBrick(std::wstring& line, bool hasPosition, std::vector<Brick>& out)
{
  const std::wstring original(line);
  BrickType subtype;
  uint32_t x = 0;
  uint32_t y = 0;
  std::wstring sprite;
  const unsigned fields = hasPosition ? 4 : 2;
  for (unsigned i = 0; i < fields; ++i)
  {
    std::wstring token = line;
    std::wstring::size_type it = line.find(',');
    if (i < fields - 1 && it == std::wstring::npos)
    {
      printf("Missing fields for brick: %ls\n", original.c_str());
      return false;
    }
    token = line.substr(0, it);
    line.erase(0, it + 1);

    if (token.empty())
    {
      printf("Field is empty for brick: %ls\n", original.c_str());
      return false;
    }

    if (i == 0)
    {
      subtype = (BrickType)std::stoul(token);
      if (!ISVALID_BRICKTYPE(subtype))
      {
        printf("Invalid brick type (%u): %ls\n", subtype, original.c_str());
        return false;
      }
    }
    else if (i == 1)
    {
      sprite = token;
      if (!ResourceLoader::ContainsSprite(sprite))
      {
        printf("Invalid brick sprite (%ls): %ls\n", sprite.c_str(), original.c_str());
        return false;
      }
    }
    else if (i == 2)
    {
      x = std::stoul(token);
      if (x > GRID_COLUMNS)
      {
        printf("Invalid brick x (%u): %ls\n", x, original.c_str());
        return false;
      }
    }
    else if (i == 3)
    {
      y = std::stoul(token);
      if (y > GRID_ROWS)
      {
        printf("Invalid brick y (%u): %ls\n", y, original.c_str());
        return false;
      }
    }
  }
  out.push_back(Brick(subtype, sprite, x, y));
  return true;
}

enum class FileType { MAP, ASSET };

static bool LoadFile(
  const std::wstring& filename,
  FileType fileType,
  std::vector<Brick>& bricks,
  std::vector<std::wstring>& backgrounds)
{
  std::wifstream file_stream(filename.c_str());
  if (!file_stream.good())
  {
    printf("Invalid File: %ls\n", filename.c_str());
    return false;
  }
  const bool parseBrick = fileType == FileType::MAP;
  std::wstring line;
  while (std::getline(file_stream, line))
  {
    if (line.empty()) continue;
    if (line.at(0) == L'#') continue;

    //Expecting a delimeter
    std::wstring::size_type it = line.find(':');
    if (it == std::wstring::npos) continue;

    std::wstring token = line.substr(0, it);
    line.erase(0, it + 1);

    if (token == L"brick")
    {
      if (!ParseBrick(line, parseBrick, bricks))
      {
        file_stream.close();
        return false;
      }
    }
    else if (token == L"background")
    {
      if (!ResourceLoader::ContainsSprite(line))
      {
        printf("Invalid background sprite: %ls\n", line.c_str());
        file_stream.close();
        return false;
      }
      backgrounds.push_back(line);
    }
  }
  file_stream.close();
  return true;
}

//Used only for level editing
bool GameLoader::LoadAssets(const std::wstring& filename)
{
  return LoadFile(filename, FileType::ASSET, assetBricks, assetBackgrounds);
}

bool GameLoader::LoadMap(const std::wstring& filename, GameLevel& out)
{
  GameLevel level;
  std::vector<std::wstring> backgrounds;
  std::vector<Brick> bricks;
  if (!LoadFile(filename, FileType::MAP, bricks, backgrounds))
    return false;

  if (backgrounds.empty())
  {
    //level.background = DEFAULT_BACKGROUND;
  }
  else
  {
    level.background = backgrounds.at(0);
  }
  try {
    std::wstring key = std::filesystem::path(filename).filename().wstring();
    level.map_name = key;
    level_map[key] = level;
    out = level;
    return true;
  }
  catch (const std::exception& e)
  {
    printf("Error: %s\n", e.what());
    return false;
  }
  for (Brick& brick : bricks)
  {
    uint32_t index = GameLevel::GetIndex(brick.col, brick.row);
    level.brickMap[index].push_back(brick);
  }
}

bool GameLoader::SaveMap(GameLevel& level)
{
  std::wstring savename(level.map_name);
  std::filesystem::path map_path = ResourceLoader::GetMapPath();
  if (savename.empty())
  {
    unsigned next = 0;
    do
    {
      if (next > 0x7FFFFFFF) return false;
      savename = std::wstring(L"custom_");
      savename.append(std::to_wstring(next++));
      std::filesystem::path test(map_path);
      test.append(savename);
      if (!std::filesystem::exists(test))
      {
        map_path = test;
        break;
      }
    } while (true);
  } 
  else
  {
    map_path.append(level.map_name);
  }
  std::wofstream output(map_path.wstring());
  if (!output.is_open())
  {
    printf("Failed to save file (%d): %ls\n", GetLastError(),
      map_path.wstring().c_str());
    return false;
  }
  level.map_name = savename;

  if (!level.background.empty())
  {
    output << L"background:" << level.background << L"\n";
  }
  BrickMap::iterator map_it = level.brickMap.begin();
  for (; map_it != level.brickMap.end(); ++map_it)
  {
    for (const Brick& brick : map_it->second)
    {
      output << L"brick:";
      output << std::to_wstring(brick.subtype) + L",";
      output << brick.sprite + L",";
      output << std::to_wstring(brick.col) + L",";
      output << std::to_wstring(brick.row) << L"\n";
    }
  }
  output.close();
  printf("Saved map: %ls\n", savename.c_str());
  return true;
}

bool GameLoader::GetLevel(const std::wstring& key, GameLevel& level)
{
  auto it = level_map.find(key);
  if (it != level_map.end())
  {
    level = it->second;
    return true;
  }
  return false;
}