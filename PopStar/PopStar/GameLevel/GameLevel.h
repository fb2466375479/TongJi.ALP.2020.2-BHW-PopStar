#pragma once

#define GAMELEVEL_DFS_SELECT    0
#define GAMELEVEL_DFS_POP       1
#define GAMELEVEL_DFS_CHECKFAIL 2

#define GAMELEVEL_RUN_RES_SUCCESS 0
#define GAMELEVEL_RUN_RES_FAIL 1
#define GAMELEVEL_RUN_RES_CONTINUE 2

/**
* 游戏主体。
*/
class GameLevel {
public:

	/**
	* 构造函数。设置游戏界面的显示位置和横纵长度。
	*/
	GameLevel(int x, int y, int lenX, int lenY);
	GameLevel(class Pair Loc, int lenX, int lenY);

	/**
	* 绑定多线程绘制工具。实际不需要这个操作，直接用沈老师的 CCT 也能实现同样效果（甚至更好）。
	*/
	void BindUIPainter(class UIThreadPainter* UIPainter);

	/**
	* 显示边框及标尺数字。
	*/
	void ShowBorder();

	/**
	* 初始化星星并显示。
	*/
	void InitStars();

	/**
	* 运行游戏。详见 .cpp 中的定义。
	*/
	int Run();

	/**
	* 将游戏坐标转换为屏幕坐标。
	* 
	* 注：“游戏坐标”范围从 (0, 0) 到 (9, 9)。
	*/
	class Pair GameCoord2ScreenCoord(class Pair GameCoord);

	/**
	* 将屏幕坐标转换为游戏坐标。
	*/
	class Pair ScreenCoord2GameCoord(class Pair ScreenCoord);

	/**
	* 检测某个屏幕坐标是否在游戏区域内。
	*/
	bool IsScreenCoordOnStars(class Pair ScreenCoord);

	/**
	* 析构函数。或许可以不用。
	*/
	~GameLevel();

private:
	class Pair* Loc; // Left Top Coord.
	class Pair* GameBoardSize;
	class UIThreadPainter* UIPainter;
	class StarActor** Stars;

	int StarNum2Color(int Num);

	/**
	* 将一维数组下标转换为游戏坐标。
	*/
	Pair StarIndex2Coord(int Idx);

	/**
	* 将游戏坐标转换为一维数组下标。
	*/
	int StarCoord2Index(class Pair Coord);

	/**
	* 目前是否有星星被选中。
	*/
	bool bStarGroupSelected;

	/**
	* 当前聚焦的星星的数组下标。
	*/
	int iFocusedStarIdx;

	/**
	* 多功能深度优先搜索。
	* iReason 用于说明单次搜索的目的。
	*/
	bool DFS(class Pair Coord, int iReason, bool bIsRoot = true, int iTarColor = 0);

	/**
	* 检查游戏状态。
	* 返回内容包括：游戏成功、游戏失败、游戏继续。
	*/
	int CheckGameStatus();

	/**
	* 完整棋盘中的星星总数。
	*/
	int StarCount();

	/**
	* 动态释放内存。暂时没学到，可以用别的方法规避。
	*/
	void RecycleStars();

	/**
	* 整理方块。即，让会气到牛顿的方块掉下来，让可以往左推的列往左推。
	*/
	void TidyBlocks();
};
