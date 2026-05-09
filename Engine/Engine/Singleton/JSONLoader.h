#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

namespace JSONLoader
{
    using json = nlohmann::json;

    static json Load(const std::string& filepath)
    {
        std::ifstream file(filepath);

        if (!file.is_open())
            throw std::runtime_error("Failed to open JSON file: " + filepath);

        json data;
        file >> data;

        return data;
    }

    static void Save(const std::string& filepath, const json& data, const int indent = 4)
    {
        std::ofstream file(filepath);

        if (!file.is_open())
            throw std::runtime_error("Failed to save JSON file: " + filepath);

        file << data.dump(indent);
    }

    static bool IsValid(const std::string& filepath)
    {
        try
        {
            std::ifstream file(filepath);

            if (!file.is_open())
                return false;

            json test;
            file >> test;

            return true;
        }
        catch (...)
        {
            return false;
        }
    }
}
