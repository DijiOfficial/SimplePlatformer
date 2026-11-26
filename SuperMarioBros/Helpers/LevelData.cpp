#include "LevelData.h"

#include "Engine/Singleton/Helpers.h"
#include "Engine/Singleton/JSONLoader.h"

// todo: replace the manual level data parsing with C++26 static reflection once MSVC supports it
namespace
{
    sf::Vector2i ParseCell(const JSONLoader::json& cellData)
    {
        return sf::Vector2i{ cellData.at(0).get<int>(), cellData.at(1).get<int>() };
    }

    diji::Directions ParseDirection(const std::string& direction)
    {
        if (direction == "Left")
            return diji::Directions::Left;
        if (direction == "Right")
            return diji::Directions::Right;
        if (direction == "Up")
            return diji::Directions::Up;
        if (direction == "Down")
            return diji::Directions::Down;

        throw std::runtime_error("Invalid warp direction in level data: " + direction);
    }

    superMarioBros::WarpExitType ParseExitType(const std::string& exitType)
    {
        if (exitType == "Drop")
            return superMarioBros::WarpExitType::Drop;
        if (exitType == "RiseFromPipe")
            return superMarioBros::WarpExitType::RiseFromPipe;

        throw std::runtime_error("Invalid warp exit type in level data: " + exitType);
    }

    superMarioBros::WarpEntrance ParseEntrance(const JSONLoader::json& entranceData)
    {
        return superMarioBros::WarpEntrance{ .cell = ParseCell(entranceData.at("cell")), .direction = ParseDirection(entranceData.at("enter").get<std::string>()) };
    }

    superMarioBros::WarpArrival ParseArrival(const JSONLoader::json& arrivalData)
    {
        return superMarioBros::WarpArrival{ .cell = ParseCell(arrivalData.at("cell")), .exitType = ParseExitType(arrivalData.at("exit").get<std::string>()) };
    }

    superMarioBros::MapInfo ParseMapInfo(const JSONLoader::json& mapData)
    {
        superMarioBros::MapInfo mapInfo;
        mapInfo.file = mapData.at("file").get<std::string>();
        mapInfo.isBonusArea = mapData.value("type", std::string{}) == "Bonus";

        if (mapData.contains("start"))
            mapInfo.start = ParseCell(mapData.at("start"));

        if (mapData.contains("arrival"))
            mapInfo.arrival = ParseArrival(mapData.at("arrival"));

        if (mapData.contains("exit"))
            mapInfo.exit = ParseEntrance(mapData.at("exit"));

        if (mapData.contains("warps"))
        {
            for (const auto& warpData : mapData.at("warps"))
            {
                mapInfo.warps.push_back(superMarioBros::WarpInfo
                {
                    .entrance = ParseEntrance(warpData.at("entrance")),
                    .targetMap = warpData.at("map").get<std::string>(),
                    .returnPoint = ParseArrival(warpData.at("return"))
                });
            }
        }

        return mapInfo;
    }

    JSONLoader::json WriteCell(const sf::Vector2i& cell)
    {
        return JSONLoader::json::array({ cell.x, cell.y });
    }

    std::string WriteDirection(const diji::Directions direction)
    {
        switch (direction)
        {
        case diji::Directions::Left:
            return "Left";
        case diji::Directions::Right:
            return "Right";
        case diji::Directions::Up:
            return "Up";
        case diji::Directions::Down:
            return "Down";
        case diji::Directions::None:
            break;
        }

        throw std::runtime_error("Cannot save a warp without a direction.");
    }

    std::string WriteExitType(const superMarioBros::WarpExitType exitType)
    {
        switch (exitType)
        {
        case superMarioBros::WarpExitType::Drop:
            return "Drop";
        case superMarioBros::WarpExitType::RiseFromPipe:
            return "RiseFromPipe";
        }

        throw std::runtime_error("Cannot save a warp with an unknown exit type.");
    }

    JSONLoader::json WriteEntrance(const superMarioBros::WarpEntrance& entrance)
    {
        return JSONLoader::json{ { "cell", WriteCell(entrance.cell) }, { "enter", WriteDirection(entrance.direction) } };
    }

    JSONLoader::json WriteArrival(const superMarioBros::WarpArrival& arrival)
    {
        return JSONLoader::json{ { "cell", WriteCell(arrival.cell) }, { "exit", WriteExitType(arrival.exitType) } };
    }

    void WriteMapInfo(JSONLoader::json& mapData, const superMarioBros::MapInfo& mapInfo)
    {
        mapData["file"] = mapInfo.file;

        if (mapInfo.isBonusArea)
            mapData["type"] = "Bonus";
        else
            mapData.erase("type");

        if (mapInfo.start.has_value())
            mapData["start"] = WriteCell(mapInfo.start.value());
        else
            mapData.erase("start");

        if (mapInfo.arrival.has_value())
            mapData["arrival"] = WriteArrival(mapInfo.arrival.value());
        else
            mapData.erase("arrival");

        if (mapInfo.exit.has_value())
            mapData["exit"] = WriteEntrance(mapInfo.exit.value());
        else
            mapData.erase("exit");

        if (mapInfo.warps.empty())
        {
            mapData.erase("warps");
            return;
        }

        JSONLoader::json warpsData = JSONLoader::json::array();
        for (const superMarioBros::WarpInfo& warp : mapInfo.warps)
            warpsData.push_back(JSONLoader::json{ { "entrance", WriteEntrance(warp.entrance) }, { "map", warp.targetMap }, { "return", WriteArrival(warp.returnPoint) } });

        mapData["warps"] = warpsData;
    }
}

superMarioBros::LevelDataFile superMarioBros::LevelData::Load()
{
    const JSONLoader::json data = JSONLoader::Load(LEVEL_DATA_PATH);

    LevelDataFile levelData;
    levelData.transitionMap = data.at("transitionMap").get<std::string>();

    for (const auto& levelEntryData : data.at("levels"))
        levelData.levels.push_back(LevelEntry{ .name = levelEntryData.at("name").get<std::string>(), .map = levelEntryData.at("map").get<std::string>() });

    for (const auto& mapItem : data.at("maps").items())
        levelData.maps[mapItem.key()] = ParseMapInfo(mapItem.value());

    return levelData;
}

void superMarioBros::LevelData::SaveMapInfo(const std::string& mapName, const MapInfo& mapInfo)
{
    JSONLoader::json data = JSONLoader::Load(LEVEL_DATA_PATH);

    WriteMapInfo(data["maps"][mapName], mapInfo);

    JSONLoader::Save(LEVEL_DATA_PATH, data);
}
