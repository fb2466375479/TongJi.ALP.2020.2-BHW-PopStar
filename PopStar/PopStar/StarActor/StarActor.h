#pragma once

/**
* 将每颗星星视为一个对象。本文件存的是一个“星星类（Class）”。
* 至于为什么叫 Actor... 没为啥，直接叫 Star 其实也可的。
*/

#define STARACTOR_MOVEMENT_DELAY 2
class StarActor {
public:
	StarActor();
	~StarActor();
	void BindUIPainter(class UIThreadPainter* UIPainter);
	void BindGameLevel(class GameLevel* Level);
	void SetBgColor(int iBgColor);

	/**
	* 设置坐标。
	*/
	void SetLocVal(int X, int Y);
	void SetLocVal(class Pair Coord);
	class Pair GetLocVal();

	void Drop(int iScreenX, int iSourceScreenY, int iTargetScreenY, bool bUpdateLocVal = false);
	void Drop(class Pair Source, class Pair Target, bool bUpdateLocVal = false);
	void MoveLeft(int iScreenY, int iSourceScreenX, int iTargetScreenX, bool bUpdateLocVal = false);
	void MoveLeft(class Pair Source, class Pair Target, bool bUpdateLocVal = false);
	void Draw(int iScreenX, int iScreenY, bool bIsVisible = true);
	void Draw(class Pair Target, bool bIsVisible = true);

	int GetBgColor();

	void OnFocusChange(bool bIsFocused);
	void OnSelectChange(bool bIsSelected);
	bool IsSelected();

	void Kill();
	void Pop(); // Same as "Kill".

	bool IsAlive();
	bool IsDead();

private:
	class UIThreadPainter* UIPainter;
	class GameLevel* Level;
	class Pair* Loc;
	int iBgColor;
	bool bIsAlive;

	bool bIsFocused;
	bool bIsSelected;

	int GenerateFgColor(); // 动态判断前景色应该是什么。
};
