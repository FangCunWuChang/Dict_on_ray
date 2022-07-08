#pragma once
#include "TranslationFactory.h"

class EN_US final : public TranslationFactory
{
public:
	const juce::String getName()override { return "en-us"; };
	const std::map<juce::String, juce::String> getList()override
	{
		std::map<juce::String, juce::String> result;
		result["LB_Config"] = "Config";
		result["LB_Url"] = "Server Url";
		result["LB_Port"] = "Server Port";
		result["LB_Query"] = "Word Query";
		result["LB_List"] = "List Entry";
		result["LB_LAF"] = "Color Mode";
		result["LB_Lan"] = "UI Language";

		result["BT_Tim"] = "Time";
		result["BT_Sys"] = "System";
		result["BT_Dar"] = "Dark";
		result["BT_Lig"] = "Light";

		result["TP_Tim"] = "Between 6 o'clock and 18 o'clock, use the light color, and other times use the dark color";
		result["TP_Sys"] = "Follow the light and dark in the system settings to change the color";
		result["TP_Dar"] = "Use dark color by default at startup";
		result["TP_Lig"] = "Use light color by default at startup";

		result["TE_Loa"] = "Loading word list...";

		result["WT_Err"] = "Error";
		result["TE_Con"] = "Can't connect to the server";

		result["BT_OK"] = "OK";

		result["TE_Empty"] = "Search for word list";

		result["TE_Null"] = "Word isn't exists.Please refresh the word list.";
		result["TE_NNF"] = "Can't connect with the server.Status code:";

		result["TE_Ser"] = "Searching...";
		result["TE_Sec"] = "Searching complete";
		return result;
	}
};