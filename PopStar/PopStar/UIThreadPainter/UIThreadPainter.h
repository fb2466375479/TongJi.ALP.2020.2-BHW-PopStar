#pragma once

/**
* UI Thread Painter 是基于沈老师的 CCT 工具设计的简单伪图形界面绘制工具。
* 传统的 CCT 工具在涉及多线程同时绘图时会出现“花屏”现象，使用 UI Thread Painter 可以解决该问题。
*/

#include <string>
#include <queue>
#include "../LarryCCT/cmd_console_tools.h"

#define UI_THREAD_QUEUE_DATA_TYPE_CHR 0
#define UI_THREAD_QUEUE_DATA_TYPE_STR 1
#define UI_THREAD_QUEUE_DATA_TYPE_INT 2
class UIThreadPainter {
public:
	UIThreadPainter();

	/**
	* 使用前调用。
	*/
	void Start();

	/**
	* 如果不再需要使用，则应调用本方法。
	*/
	void End();

	/**
	* 下方三个方法类似 CCT 中的方法。
	*/
	void ShowInt(int x, int y, int i, int bgColor = COLOR_BLACK, int fgColor = COLOR_WHITE, int rpt = 1);
	void ShowCh(int x, int y, char c, int bgColor = COLOR_BLACK, int fgColor = COLOR_WHITE, int rpt = 1);
	void ShowStr(int x, int y, std::string str, int bgColor = COLOR_BLACK, int fgColor = COLOR_WHITE, int rpt = 1);

private:
	bool bSwitch;
	bool bQueOperating;
	int iDataQueue;
	struct DataPack {
		int Type;
		int x;
		int y;
		char ch;
		std::string str;
		int i;
		int bgColor;
		int fgColor;
		int rpt;
	};
	std::queue<DataPack> queDataQueue;

	void PainterLoop();
};
