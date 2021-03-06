#pragma once
#include "TranslationFactory.h"

class ZH_CN final : public TranslationFactory
{
public:
	const juce::String getName()override { return "zh-cn"; };
	const std::map<juce::String, juce::String> getList()override
	{
		std::map<juce::String, juce::String> result;
		result["LB_Config"] = L"设置";
		result["LB_Url"] = L"词库地址";
		result["LB_Port"] = L"词库端口";
		result["LB_Query"] = L"查询参数";
		result["LB_List"] = L"列表字段";
		result["LB_LAF"] = L"颜色模式";
		result["LB_Lan"] = L"界面语言";

		result["BT_Tim"] = L"跟随时间";
		result["BT_Sys"] = L"跟随系统";
		result["BT_Dar"] = L"默认黑暗";
		result["BT_Lig"] = L"默认明亮";

		result["TP_Tim"] = L"在6点至18点之间为明亮主题，其余时间为黑暗主题";
		result["TP_Sys"] = L"跟随系统设置中的明亮与黑暗改变主题";
		result["TP_Dar"] = L"在启动时默认使用黑暗主题";
		result["TP_Lig"] = L"在启动时默认使用明亮主题";

		result["TE_Loa"] = L"正在同步单词列表...";

		result["WT_Err"] = L"出错";
		result["TE_Con"] = L"无法连接至服务器";

		result["BT_OK"] = L"确定";

		result["TE_Empty"] = L"搜索单词列表";

		result["TE_Null"] = L"单词不在词库中，请刷新词库。";
		result["TE_NNF"] = L"无法完成通信，状态码：";

		result["TE_Ser"] = L"正在搜索...";
		result["TE_Sec"] = L"搜索完成";
		return result;
	}
};