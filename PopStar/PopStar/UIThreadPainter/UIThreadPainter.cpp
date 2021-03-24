#include <thread>
#include <Windows.h>
#include <iostream>
#include "../LarryCCT/cmd_console_tools.h"
#include "UIThreadPainter.h"

UIThreadPainter::UIThreadPainter()
{
	std::ios::sync_with_stdio(false);
	bSwitch = false;
	bQueOperating = false;
	iDataQueue = 0;
}

void UIThreadPainter::Start()
{
	if (!bSwitch) {
		bSwitch = true;
		std::thread T(&UIThreadPainter::PainterLoop, this);
		T.detach();
	}
}

void UIThreadPainter::End()
{
	bSwitch = false;
	Sleep(1);
}

void UIThreadPainter::ShowInt(int x, int y, int i, int bgColor, int fgColor, int rpt)
{
	while (bQueOperating) {
		//Sleep(1);
	}
	bQueOperating = true;
	DataPack data;
	data.Type = UI_THREAD_QUEUE_DATA_TYPE_INT;
	data.i = i;
	data.x = x;
	data.y = y;
	data.bgColor = bgColor;
	data.fgColor = fgColor;
	data.rpt = rpt;
	queDataQueue.push(data);
	bQueOperating = false;
}

void UIThreadPainter::ShowCh(int x, int y, char c, int bgColor, int fgColor, int rpt)
{
	while (bQueOperating) {
		//Sleep(1);
	}
	bQueOperating = true;
	DataPack data;
	data.Type = UI_THREAD_QUEUE_DATA_TYPE_CHR;
	data.ch = c;
	data.x = x;
	data.y = y;
	data.bgColor = bgColor;
	data.fgColor = fgColor;
	data.rpt = rpt;
	queDataQueue.push(data);
	bQueOperating = false;
}

void UIThreadPainter::ShowStr(int x, int y, std::string str, int bgColor, int fgColor, int rpt)
{
	while (bQueOperating) {
		//Sleep(1);
	}
	bQueOperating = true;
	DataPack data;
	data.Type = UI_THREAD_QUEUE_DATA_TYPE_STR;
	data.str = str;
	data.x = x;
	data.y = y;
	data.bgColor = bgColor;
	data.fgColor = fgColor;
	data.rpt = rpt;
	queDataQueue.push(data);
	bQueOperating = false;
}

void UIThreadPainter::PainterLoop()
{
	int size;
	int i;
	while (bSwitch) {
		size = queDataQueue.size();
		if (size > 0) {
			bQueOperating = true;
			DataPack &tarData = queDataQueue.front();
			cct_gotoxy(tarData.x, tarData.y);
			cct_setcolor(tarData.bgColor, tarData.fgColor);
			if (tarData.Type == UI_THREAD_QUEUE_DATA_TYPE_INT) {
				for (i = 0; i < tarData.rpt; i++)
					printf("%d", tarData.i);
			}
			else if (tarData.Type == UI_THREAD_QUEUE_DATA_TYPE_CHR) {
				for (i = 0; i < tarData.rpt; i++)
					putchar(tarData.ch);
			}
			else if (tarData.Type == UI_THREAD_QUEUE_DATA_TYPE_STR) {
				for (i = 0; i < tarData.rpt; i++)
				std::cout << tarData.str;
			}
			queDataQueue.pop();
			bQueOperating = false;
		}
	}
}
