#pragma once
#include <istream>
#include <unordered_map>
#include <fstream>


class INISettings
{
public:
	INISettings(const std::string& File);
	~INISettings() { file.close(); }
	bool IsValid() { return map.size() > 0; }
	const std::unordered_map<std::string, std::string>& GetSection(const std::string& section) { return map[section]; }
	const std::string GetValue(const std::string& section, const std::string& name) { return map[section][name]; }
	void SetValue(const std::string& section, const std::string& name, const std::string& value) { map[section][name] = value; Save(); }
private:
	std::ifstream file;
	std::string Path;
	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> map;
	void parse(std::istream& in);
	void Save();
};