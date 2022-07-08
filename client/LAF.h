#pragma once

enum class LAFColorType {
	Dark = 0,
	Midnight = 1,
	Grey = 2,
	Light = 3
};

#define LAFDay LAFColorType::Light
#define LAFNight LAFColorType::Midnight

enum class LAFInitType
{
	Time,
	System,
	AlwaysDark,
	AlwaysLight
};

class LAFManager final
{
private:
	LAFManager() = delete;
	~LAFManager() = delete;

public:
	static LAFColorType set(LAFColorType type)
	{
		LAFManager::_current = type;
		return type;
	};

	static LAFColorType get()
	{
		return LAFManager::_current;
	};

private:
	static LAFColorType _current;
};

namespace AppControl
{
	const int DayTime = 6;
	const int NightTime = 18;
}
