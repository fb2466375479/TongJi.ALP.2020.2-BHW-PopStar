#include <Windows.h>
#include "Tools.h"
#include "../StarActor/StarActor.h"

Pair::Pair(int x, int y)
{
	iX = x;
	iY = y;
}

Pair::Pair()
{
	iX = iY = 0;
}

int RandInt(int a, int b) 
{
	return rand() % (b - a + 1) + a;
}
