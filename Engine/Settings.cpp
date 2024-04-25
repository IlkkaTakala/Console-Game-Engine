#include "Settings.h"
#include <regex>


INISettings::INISettings(const std::string& File)
{
	file = std::ifstream(File);
	Path = File;
	if (file.good())
		parse(file);
	file.seekg(0);
}

// From stackoverflow https://codereview.stackexchange.com/questions/127819/ini-file-parser-in-c
void INISettings::parse(std::istream& in) {
	static const std::regex comment_regex{ R"x(\s*[;#])x" };
	static const std::regex section_regex{ R"x(\s*\[([^\]]+)\])x" };
	static const std::regex value_regex{ R"x(\s*(\S[^ \t=]*)\s*=\s*((\s?\S+)+)\s*$)x" };
	std::string current_section;
	std::smatch pieces;
	for (std::string line; std::getline(in, line);)
	{
		if (line.empty() || std::regex_match(line, pieces, comment_regex)) {
			// skip comment lines and blank lines                    
		}
		else if (std::regex_match(line, pieces, section_regex)) {
			if (pieces.size() == 2) { // exactly one match
				current_section = pieces[1].str();
			}
		}
		else if (std::regex_match(line, pieces, value_regex)) {
			if (pieces.size() == 4) { // exactly enough matches
				map[current_section][pieces[1].str()] = pieces[2].str();
			}
		}
	}
}

void INISettings::Save()
{
	std::ofstream out(Path, std::ios_base::trunc);
	for (auto section : map) {
		out << '[' << section.first << ']' << '\n';
		for (auto data : section.second) {
			out << data.first << '=' << data.second << '\n';
		}
	}
}
