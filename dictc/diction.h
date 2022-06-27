#pragma once

#include <string>
#include <memory>
#include <vector>

class Diction final
{
public:
	Diction(std::string wTag = "word", std::string tTag = "translation");
	~Diction() = default;
	
	bool load(std::string file);
	bool parse();
	bool save(std::string file);
	
private:
	const std::string wTag = "word", tTag = "translation";
	const std::string secIdw = ".idw", secIdt = ".idt", secWords = ".lst", secTrans = ".tsl";

private:
	std::unique_ptr<char[]> _source;
	uint32_t _source_size = 0;
	
	void quickSort(int left, int right);
	
	inline static int strcmp(char* str1, char* str2);
	
public:
	std::vector<uint32_t> wIndex, tIndex;
	std::vector<char> words, translations;
	
	enum class PlaceType
	{
		Other,
		Word,
		Translate
	};
};
