#include <Windows.h>
#include "StarActor.h"
#include "../UIThreadPainter/UIThreadPainter.h"
#include "../Tools/Tools.h"
#include "../GameLevel/GameLevel.h"

StarActor::StarActor()
{
	bIsFocused = false;
	bIsSelected = false;
	bIsAlive = true;
	Loc = new Pair(0, 0);
}

StarActor::~StarActor()
{
	delete Loc;
}

void StarActor::BindUIPainter(UIThreadPainter* UIPainter)
{
	this->UIPainter = UIPainter;
}

void StarActor::BindGameLevel(GameLevel* Level)
{
	this->Level = Level;
}

void StarActor::SetBgColor(int iBgColor)
{
	this->iBgColor = iBgColor;
}

void StarActor::SetLocVal(int X, int Y)
{
	Loc->iX = X;
	Loc->iY = Y;
}

void StarActor::SetLocVal(Pair Coord)
{
	SetLocVal(Coord.iX, Coord.iY);
}

Pair StarActor::GetLocVal()
{
	return *Loc;
}

void StarActor::Drop(int iScreenX, int iSourceScreenY, int iTargetScreenY, bool bUpdateLocVal)
{
	for (int i = iSourceScreenY; i < iTargetScreenY; i++) {
		Sleep(STARACTOR_MOVEMENT_DELAY);
		Draw(iScreenX, i, false);
		Draw(iScreenX, i + 1);
	}
	Pair GameCoord = Level->ScreenCoord2GameCoord(Pair(iScreenX, iTargetScreenY));
	if (bUpdateLocVal) {
		SetLocVal(GameCoord);
	}
}

void StarActor::Drop(Pair Source, Pair Target, bool bUpdateLocVal)
{
	Pair ScrSource = Level->GameCoord2ScreenCoord(Source);
	Pair ScrTarget = Level->GameCoord2ScreenCoord(Target);
	Drop(ScrSource.iX, ScrSource.iY, ScrTarget.iY, bUpdateLocVal);
}

void StarActor::MoveLeft(int iScreenY, int iSourceScreenX, int iTargetScreenX, bool bUpdateLocVal)
{
	for (int i = iSourceScreenX; i > iTargetScreenX; i--) {
		Sleep(STARACTOR_MOVEMENT_DELAY);
		Draw(i, iScreenY, false);
		Draw(i - 1, iScreenY);
	}
	Pair GameCoord = Level->ScreenCoord2GameCoord(Pair(iTargetScreenX, iScreenY));
	if (bUpdateLocVal) {
		SetLocVal(GameCoord);
	}
}

void StarActor::MoveLeft(Pair Source, Pair Target, bool bUpdateLocVal)
{
	Pair ScrSource = Level->GameCoord2ScreenCoord(Source);
	Pair ScrTarget = Level->GameCoord2ScreenCoord(Target);
	MoveLeft(ScrSource.iY, ScrSource.iX, ScrTarget.iX, bUpdateLocVal);
}

void StarActor::Draw(int iScreenX, int iScreenY, bool bIsVisible)
{
	if (bIsVisible) {
		UIPainter->ShowStr(iScreenX, iScreenY, "¨X¨T¨[", iBgColor, GenerateFgColor());
		UIPainter->ShowStr(iScreenX, iScreenY + 1, "¨U¡ï¨U", iBgColor, GenerateFgColor());
		UIPainter->ShowStr(iScreenX, iScreenY + 2, "¨^¨T¨a", iBgColor, GenerateFgColor());
	}
	else {
		UIPainter->ShowCh(iScreenX, iScreenY, ' ', COLOR_HWHITE, COLOR_HWHITE, 6);
		UIPainter->ShowCh(iScreenX, iScreenY + 1, ' ', COLOR_HWHITE, COLOR_HWHITE, 6);
		UIPainter->ShowCh(iScreenX, iScreenY + 2, ' ', COLOR_HWHITE, COLOR_HWHITE, 6);
	}
}

void StarActor::Draw(Pair Target, bool bIsVisible)
{
	Pair ScrCoord = Level->GameCoord2ScreenCoord(Target);
	Draw(ScrCoord.iX, ScrCoord.iY, bIsVisible);
}

int StarActor::GetBgColor()
{
	return iBgColor;
}

void StarActor::OnFocusChange(bool bIsFocused)
{
	this->bIsFocused = bIsFocused;
	Draw(*Loc);
}

void StarActor::OnSelectChange(bool bIsSelected)
{
	this->bIsSelected = bIsSelected;
	Draw(*Loc);
}

bool StarActor::IsSelected()
{
	return bIsSelected;
}

void StarActor::Kill()
{
	iBgColor = COLOR_HWHITE;
	bIsAlive = false;
	Draw(*Loc, false);
}

void StarActor::Pop()
{
	Kill();
}

bool StarActor::IsAlive()
{
	return bIsAlive;
}

bool StarActor::IsDead()
{
	return !bIsAlive;
}

int StarActor::GenerateFgColor()
{
	if (!bIsAlive) {
		return COLOR_HWHITE;
	}
	if (bIsFocused) {
		return COLOR_HWHITE;
	}
	else if (bIsSelected) {
		return COLOR_WHITE;
	}
	else {
		return COLOR_BLACK;
	}
}
