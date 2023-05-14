#pragma once

#include <string>
#include <iostream>
#include <shlwapi.h>
#include <filesystem>
#include <fstream>
#include <json.hpp>

#pragma comment(lib, "shlwapi.lib")

using namespace nlohmann;

class Config
{
public:
	inline static json config;
public:
	inline static std::string directory;
	inline static std::string path;
public:
	static void init()
	{
		char* appdata = getenv("APPDATA");
		directory = std::string(appdata) + "\\Luconia Injector\\";

		BOOL existsPath = PathFileExistsA(directory.c_str());
		if (!existsPath) CreateDirectoryA(directory.c_str(), NULL);

		path = directory + "config.json";

		std::ifstream file(path);

		if (file.is_open())
		{
			file >> config;
		}
		else
		{
			if (config["settings"].is_null())
			{
				config["settings"]["use_custom_path"] = false;
				config["settings"]["show_discord_rpc"] = true;
				config["settings"]["close_after_inject"] = false;
				config["settings"]["custom_path"] = "";
			}

			save();
		}

		file.close();
	}

	static void save()
	{
		std::ofstream file(path);
		file << std::setw(4) << config << '\n';
		file.close();
	}
};