#include <Windows.h>
#include <conio.h>
#include <time.h>
/**
* 2021年3月15日
*/

/**
* 注意
* 本项目方案仅供参考，其中有非常非常多内容未严格遵守沈老师的要求，且涉及一些暂未学到的内容。
* 本项目遵循面向对象的思想设计，若不理解，主要研究 GameLevel.cpp 中的思路即可。
* 本项目部分内容涉及多线程，实际开发中发现其实不需要用到该技术。相关内容可忽略。
* 
* 本项目应该能为本次大作业的一些技术难点提供思路指引，但个人版本的实现还应严格按照沈老师的规定来...
* 
* 另外，因为做这个项目的人太懒了，所以......很多地方有偷懒，还请见谅...
*/

/**
* 使用标准库中的 string 可以在某些地方简化开发。但沈老师不推荐。
*/
#include <string>
#include "./LarryCCT/cmd_console_tools.h"
#include "./UIThreadPainter/UIThreadPainter.h"
#include "./GameLevel/GameLevel.h"
#include "./Tools/Tools.h"

int main() {
	srand((unsigned int)time(0)); // 初始化随机数生成种子。
	cct_setconsoleborder(70, 38);
	cct_setfontsize("新宋体", 14, 8);
	cct_setcolor();
	cct_cls();
	cct_enable_mouse();
	cct_setcursor(CURSOR_INVISIBLE);

	/**
	* 下方 GameLevel 类和 UIThreadPainter 类详见相应 .h 头文件。细节暂时不需要研究清楚。
	*/
	GameLevel Level(0, 1, 10, 10); // 修改后面两个参数可以改变游戏“棋盘”大小。这里懒得写菜单，就直接这样搞了hhh
	UIThreadPainter Painter;
	Painter.Start();
	Level.BindUIPainter(&Painter);
	Level.ShowBorder();

	while (true) {
		Level.InitStars(); // 初始化星星。
		int iRunRes = Level.Run();
		std::string strMsg = "";
		if (iRunRes == GAMELEVEL_RUN_RES_FAIL) {
			strMsg = "失败...";
		}
		else if (iRunRes == GAMELEVEL_RUN_RES_SUCCESS) {
			strMsg = "成功！";
		}

		strMsg += "按Q退出。按其它键开始新局。";
		Painter.ShowStr(0, 0, strMsg);

		int iInput = _getch();
		if (iInput == 'q' || iInput == 'Q') {
			break;
		}
	}

	Painter.End();

	return 0;
}
