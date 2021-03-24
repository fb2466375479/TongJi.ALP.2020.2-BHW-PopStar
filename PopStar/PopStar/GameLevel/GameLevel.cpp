#include <Windows.h>
#include "GameLevel.h"
#include "../Tools/Tools.h"
#include "../UIThreadPainter/UIThreadPainter.h"
#include "../StarActor/StarActor.h"

GameLevel::GameLevel(int x, int y, int lenX, int lenY)
{
	Loc = new Pair(x, y);
	GameBoardSize = new Pair(lenX, lenY);
	Stars = NULL;
	bStarGroupSelected = false;
	iFocusedStarIdx = -1;
}

GameLevel::GameLevel(Pair Loc, int lenX, int lenY)
{
	GameLevel(Loc.iX, Loc.iY, lenX, lenY);
}

void GameLevel::BindUIPainter(UIThreadPainter* UIPainter)
{
	// 仅使用沈老师的 CCT 工具，则不需要这个操作。
	this->UIPainter = UIPainter;
}

void GameLevel::ShowBorder()
{
	// 注意：本函数用到了 UI Painter 里的绘图方法。实际用 cct_showch/str/int 也是一样的，甚至更好。
	for (int i = 0; i < GameBoardSize->iX; i++) {
		UIPainter->ShowInt(6 * (i + 1) + Loc->iX, Loc->iY + 1, i, COLOR_BLACK, COLOR_WHITE);
	}
	UIPainter->ShowStr(2 + Loc->iX, 1 + Loc->iY + 1, "╔", COLOR_HWHITE, COLOR_BLACK);
	UIPainter->ShowStr(2 + 2 + Loc->iX, 1 + Loc->iY + 1, "═", COLOR_HWHITE, COLOR_BLACK, GameBoardSize->iX * 3);
	UIPainter->ShowStr(2 + Loc->iX + 6 * GameBoardSize->iX + 2, 1 + Loc->iY + 1, "╗", COLOR_HWHITE, COLOR_BLACK);
	for (int i = 0; i < 3 * GameBoardSize->iY; i++) {
		UIPainter->ShowStr(2 + Loc->iX, i + 2 + Loc->iY + 1, "║", COLOR_HWHITE, COLOR_BLACK);
		UIPainter->ShowStr(2 + Loc->iX + 6 * GameBoardSize->iX + 2, i + 2 + Loc->iY + 1, "║", COLOR_HWHITE, COLOR_BLACK);
		if (i % 3 == 1) {
			UIPainter->ShowCh(Loc->iX, i + 2 + Loc->iY + 1, 'A' + i / 3);
		}
	}
	UIPainter->ShowStr(2 + Loc->iX, 2 + Loc->iY + 3 * GameBoardSize->iY + 1, "╚", COLOR_HWHITE, COLOR_BLACK);
	UIPainter->ShowStr(2 + 2 + Loc->iX, 2 + Loc->iY + 3 * GameBoardSize->iY + 1, "═", COLOR_HWHITE, COLOR_BLACK, GameBoardSize->iX * 3);
	UIPainter->ShowStr(2 + Loc->iX + 6 * GameBoardSize->iX + 2 + 3 * GameBoardSize->iY + 1, 2 + Loc->iY, "╝", COLOR_HWHITE, COLOR_BLACK);
}

void GameLevel::InitStars()
{
	if (Stars != NULL) {
		// 本项目中星星数组是动态声明的，需要手动回收。
		// 如果是静态声明（目前学过的），应该可以不管回收的问题。
		RecycleStars();
		delete[]Stars;
	}
	int Len = GameBoardSize->iX * GameBoardSize->iY;
	Stars = new StarActor *[Len];
	for (int i = 0; i < Len; i++) {
		Stars[i] = new StarActor();
		Stars[i]->BindUIPainter(UIPainter);
		Stars[i]->BindGameLevel(this);
		Stars[i]->SetBgColor(StarNum2Color(RandInt(0, 4)));
		Pair GameCoord = StarIndex2Coord(i);
		Stars[i]->SetLocVal(GameCoord);
		Stars[i]->Draw(GameCoord);
	}
}

/**
* 控制游戏运行的核心部分。结束时会返回一个值，表示游戏失败或成功。
*/
int GameLevel::Run()
{
	int MX, MY, MAction, Keycode1, Keycode2;
	int LastIdx = -1;
	while (true) {
		int InputEvent = cct_read_keyboard_and_mouse(MX, MY, MAction, Keycode1, Keycode2);
		if (InputEvent == CCT_MOUSE_EVENT) {

			if (!IsScreenCoordOnStars(Pair(MX, MY))) {
				// 如果目前鼠标的位置在游戏区域外，则不应继续处理。
				continue;
			}

			// TarCoord stands for Target Coord.
			Pair TarCoord = ScreenCoord2GameCoord(Pair(MX, MY));
			int TarIdx = StarCoord2Index(TarCoord);

			if (MAction == MOUSE_ONLY_MOVED) {

				// TarIdx stands for Target Index, the one your mouse is on.
				Stars[TarIdx]->OnFocusChange(true);
				if (LastIdx != TarIdx && LastIdx != -1) {
					Stars[LastIdx]->OnFocusChange(false);
				}

				// 存下当前聚焦的块块，这样当鼠标离开它时可以快速找到它并取消其聚焦状态。
				LastIdx = TarIdx;
			}
			else if (MAction == MOUSE_LEFT_BUTTON_CLICK) {
				// 进入这个 if，说明接收到了鼠标左键指令。

				if (!bStarGroupSelected) {
					// 如果未曾选中星星，则看一下当前点击的这个星星能不能选。
					// DFS 会返回一个 bool，表示当前的星星是否为“成群结队”。
					// 如果是，则我可以选中它。
					bStarGroupSelected = DFS(TarCoord, GAMELEVEL_DFS_SELECT);
					// 注意，如果可以选，则 DFS 执行过程会将这一团星星全标为选中。详见 DFS 函数具体代码。
				}
				else {
					bStarGroupSelected = false; // 清除“选中”状态。
					if (Stars[TarIdx]->IsSelected()) {
						// 当我点下的星星是我选中的星星时，消灭它。

						DFS(TarCoord, GAMELEVEL_DFS_POP); // 第二个参数设为 POP，告诉 DFS 我要消灭这一团星星。

						TidyBlocks(); // 消灭完，会在棋盘中间出现一些空洞。要清理一下。
						
						int iGameStatus = CheckGameStatus(); // 看看游戏结束了嘛？
						if (iGameStatus != GAMELEVEL_RUN_RES_CONTINUE) {
							return iGameStatus;
						}

					}
					else {
						// 如果按下的位置不是之前选中的......

						// 就取消所有星星的“被选中”状态。
						for (int i = 0; i < GameBoardSize->iX * GameBoardSize->iY; i++) {
							if (Stars[i]->IsSelected()) {
								Stars[i]->OnSelectChange(false);
							}
						}
					}
				}
			}
		}
	}
}
// （上面这一堆"}"好丑 qwq...）

Pair GameLevel::GameCoord2ScreenCoord(Pair GameCoord)
{
	return Pair(4 + Loc->iX + GameCoord.iX * 6, 3 + Loc->iY + GameCoord.iY * 3);
}

Pair GameLevel::ScreenCoord2GameCoord(Pair ScreenCoord)
{
	return Pair((ScreenCoord.iX - 4 - Loc->iX) / 6, (ScreenCoord.iY - 3 - Loc->iY) / 3);
}

bool GameLevel::IsScreenCoordOnStars(Pair ScreenCoord)
{
	int X = ScreenCoord.iX;
	int Y = ScreenCoord.iY;
	if (X < Loc->iX + 4 || Y < Loc->iY + 3) {
		return false;
	}

	if (X > 3 + 6 * GameBoardSize->iX + Loc->iX || Y > 2 + 3 * GameBoardSize->iY + Loc->iY) {
		return false;
	}

	return true;
}

GameLevel::~GameLevel()
{
	// 如果使用现有知识完成开数组等任务，可以不用做下面这几行的操作。
	delete Loc;
	delete GameBoardSize;
	if (Stars != NULL) {
		delete []Stars;
	}
}

int GameLevel::StarNum2Color(int Num)
{
	if (Num == 0) {
		return COLOR_HRED;
	}
	else if (Num == 1) {
		return COLOR_GREEN;
	}
	else if (Num == 2) {
		return COLOR_PINK;
	}
	else if (Num == 3) {
		return COLOR_HCYAN;
	}
	else if (Num == 4) {
		return COLOR_HBLUE;
	}
	return -1; // Error. 正常情况下应该不会走到这里。
}

Pair GameLevel::StarIndex2Coord(int Idx)
{
	int X = Idx % GameBoardSize->iX;
	int Y = Idx / GameBoardSize->iX;
	return Pair(X, Y);
}

int GameLevel::StarCoord2Index(Pair Coord)
{
	return Coord.iY * GameBoardSize->iX + Coord.iX;
}

/**
* 注：DFS 是“深度（Depth）优先（First）搜索（Search）”，在上学期的扫雷大作业中有涉及相关思想。
*/
bool GameLevel::DFS(Pair Coord, int iReason, bool bIsRoot, int iTarColor)
{
	int iCurrStarIdx = StarCoord2Index(Coord); // 目前位置的星星的数组下标。
	if (!bIsRoot) {
		if (iReason == GAMELEVEL_DFS_SELECT) {
			Stars[iCurrStarIdx]->OnSelectChange(true);
		}
	}
	int iTargetColor = iTarColor;
	if (bIsRoot && iReason == GAMELEVEL_DFS_POP) {
		iTargetColor = Stars[iCurrStarIdx]->GetBgColor();
	}
	if (iReason == GAMELEVEL_DFS_POP) {
		Stars[iCurrStarIdx]->Kill();
	}
	
	bool bRet = false; // Ret means Return Value.

	for (int i = Coord.iX - 1; i <= Coord.iX + 1; i++) {
		for (int j = Coord.iY - 1; j <= Coord.iY + 1; j++) {

			// 从这里开始......
			if (i == Coord.iX && j == Coord.iY) {
				continue;
			}
			if (i == Coord.iX - 1 && j != Coord.iY) {
				continue;
			}
			if (i == Coord.iX + 1 && j != Coord.iY) {
				continue;
			}
			if (i >= GameBoardSize->iX || i < 0 || j >= GameBoardSize->iY || j < 0) {
				continue;
			}
			// 到这里结束...... 是在判断某个坐标是否合法。判断项目包括：是否出界，是否在对角线上，是否为自己。
			// 上面这几行代码挺丑的，应该可以美化一下。

			int iTarIdx = StarCoord2Index(Pair(i, j));

			// 下面再进行一些检查，包括是否为同类方块，是否为空。
			if (Stars[iTarIdx]->IsDead()) {
				continue;
			}
			if ((iReason == GAMELEVEL_DFS_SELECT || iReason == GAMELEVEL_DFS_CHECKFAIL) && Stars[iTarIdx]->GetBgColor() != Stars[iCurrStarIdx]->GetBgColor()) {
				continue;
			}
			if (iReason == GAMELEVEL_DFS_POP && Stars[iTarIdx]->GetBgColor() != iTargetColor) {
				continue;
			}
			if (iReason == GAMELEVEL_DFS_CHECKFAIL) {
				// 如果我只想知道游戏有没有输，那么不需要往下走，直接回去即可。
				// 这样看，其实这样用 DFS 并没有用到其递归搜索能力。
				return true;
			}

			bRet = true;
			if (iReason == GAMELEVEL_DFS_SELECT && !Stars[iTarIdx]->IsSelected()) {
				DFS(Pair(i, j), iReason, false, iTarColor);
			}
			if (iReason == GAMELEVEL_DFS_POP) {
				DFS(Pair(i, j), iReason, false, iTargetColor);
			}

		}
	}
	if (bRet && bIsRoot && iReason == GAMELEVEL_DFS_SELECT) {
		Stars[iCurrStarIdx]->OnSelectChange(true);
	}
	return bRet;
}

int GameLevel::CheckGameStatus()
{
	bool bIsSuccess = true;
	for (int i = 0; i < StarCount(); i++) {
		if (Stars[i]->IsAlive()) {
			bIsSuccess = false;
			break;
		}
	}
	if (bIsSuccess) {
		return GAMELEVEL_RUN_RES_SUCCESS;
	}

	bool bIsFail = true;
	for (int i = 0; i < StarCount(); i++) {
		if (DFS(StarIndex2Coord(i), GAMELEVEL_DFS_CHECKFAIL)) {
			bIsFail = false;
			break;
		}
	}
	if (bIsFail) {
		return GAMELEVEL_RUN_RES_FAIL;
	}

	return GAMELEVEL_RUN_RES_CONTINUE;
}

int GameLevel::StarCount()
{
	return GameBoardSize->iX * GameBoardSize->iY;
}

void GameLevel::RecycleStars()
{
	// 本项目中星星数组是动态声明的，需要手动回收。
	// 如果是静态声明（目前学过的），应该可以不管回收的问题。
	for (int i = 0; i < StarCount(); i++) {
		delete Stars[i];
	}
}

void GameLevel::TidyBlocks()
{
	// 安慰牛顿。
	for (int i = 0; i < GameBoardSize->iX; i++) {
		for (int ii = 0; ii < GameBoardSize->iY - 1; ii++) {
			for (int j = GameBoardSize->iY - 1 - 1; j >= 0; j--) {
				int iLowerIdx = StarCoord2Index(Pair(i, j + 1));
				int iUpperIdx = StarCoord2Index(Pair(i, j));

				if (Stars[iLowerIdx]->IsDead() && Stars[iUpperIdx]->IsAlive()) {
					Stars[iUpperIdx]->Drop(Stars[iUpperIdx]->GetLocVal(), Stars[iLowerIdx]->GetLocVal());
					Pair TmpVal = Stars[iLowerIdx]->GetLocVal();
					Stars[iLowerIdx]->SetLocVal(Stars[iUpperIdx]->GetLocVal());
					Stars[iUpperIdx]->SetLocVal(TmpVal);

					StarActor* Tmp = Stars[iLowerIdx];
					Stars[iLowerIdx] = Stars[iUpperIdx];
					Stars[iUpperIdx] = Tmp;
				}
			}
		}
	}

	// 向左推。
	for (int i = 0; i < GameBoardSize->iX - 1; i++) {
		for (int j = 1; j < GameBoardSize->iX; j++) {
			bool bShouldPushLeft = true;
			for (int k = 0; k < GameBoardSize->iY; k++) {
				if (Stars[StarCoord2Index(Pair(j - 1, k))]->IsAlive()) {
					bShouldPushLeft = false;
					break;
				}
			}
			if (bShouldPushLeft) {
				for (int k = 0; k < GameBoardSize->iY; k++) {
					int iLeftIdx = StarCoord2Index(Pair(j - 1, k));
					int iRightIdx = StarCoord2Index(Pair(j, k));
					if (Stars[iRightIdx]->IsDead()) {
						continue;
					}
					Stars[iRightIdx]->MoveLeft(Stars[iRightIdx]->GetLocVal(), Stars[iLeftIdx]->GetLocVal());
					Pair TmpVal = Stars[iLeftIdx]->GetLocVal();
					Stars[iLeftIdx]->SetLocVal(Stars[iRightIdx]->GetLocVal());
					Stars[iRightIdx]->SetLocVal(TmpVal);

					StarActor* Tmp = Stars[iRightIdx];
					Stars[iRightIdx] = Stars[iLeftIdx];
					Stars[iLeftIdx] = Tmp;
				}
			}
		}
	}
}
