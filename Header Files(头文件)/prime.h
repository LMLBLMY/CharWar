/*--------------------------------/
	HP_Prime Library for C++
	Only part of the function is provided.
	There may be many mistakes and things that can be improved.
	Auther:LMLB

	惠普Prime库给C++
	只提供了部分函数
	可能还有很多错误和能改进的地方
/--------------------------------*/

#pragma once

#include <stdio.h>
#include <windows.h>
#include <graphics.h>
#include <easyx.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <vector>
#include <string>
#include <iostream>
#include <direct.h>
#include <Shlwapi.h>


//#include <algorithm>
//由于win11控制台的问题删除了<conio.h>

static double pi = 3.14159265;
//int valreturn[10];

static int QRArry[37][37] = {
{1,1,1,1,1,1,1,0,1,0,1,1,0,1,0,1,0,0,0,1,1,0,1,1,0,1,0,1,1,0,1,1,1,1,1,1,1},
{1,0,0,0,0,0,1,0,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,0,0,1,0,1,0,0,0,0,0,1},
{1,0,1,1,1,0,1,0,1,1,1,0,1,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,1,0,1,0,1,1,1,0,1},
{1,0,1,1,1,0,1,0,0,0,0,0,1,1,0,0,0,1,0,0,1,0,0,1,1,1,1,0,0,0,1,0,1,1,1,0,1},
{1,0,1,1,1,0,1,0,1,0,0,1,0,1,0,0,1,0,0,0,0,1,1,1,0,1,1,0,1,0,1,0,1,1,1,0,1},
{1,0,0,0,0,0,1,0,0,0,1,1,0,1,0,1,0,0,0,0,0,0,1,1,0,1,1,0,0,0,1,0,0,0,0,0,1},
{1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1},
{0,0,0,0,0,0,0,0,0,1,0,0,1,1,1,0,0,1,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
{1,0,0,1,1,1,1,1,1,0,0,0,1,1,0,1,0,0,0,1,0,1,1,0,0,1,1,1,1,1,0,0,1,0,1,1,1},
{1,1,1,1,1,1,0,1,1,1,1,1,1,0,0,0,1,0,1,0,0,1,0,0,1,1,1,0,0,0,1,1,0,1,0,1,0},
{0,1,1,0,0,0,1,0,0,1,0,0,1,0,0,1,1,0,0,1,1,0,0,1,0,1,0,0,0,0,1,0,0,0,1,0,1},
{1,1,0,1,1,0,0,1,1,0,0,1,1,0,1,0,1,1,1,1,1,0,1,1,1,0,0,1,0,0,1,1,1,1,1,1,1},
{0,1,1,0,1,1,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,1,0,0,0,1,1,1,0,0,0,0,0,1},
{1,1,0,1,0,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,1,0,1,0},
{1,1,0,0,0,0,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,0,1,1,1,0,1},
{1,1,0,0,1,1,0,1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,0,1,1,0,1},
{1,1,0,0,0,1,1,1,0,1,0,0,0,0,1,1,0,0,0,1,1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,1,1},
{0,1,1,1,1,1,0,1,1,0,0,0,0,0,1,0,0,0,0,0,1,1,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0},
{1,1,1,1,0,0,1,1,0,0,1,0,0,0,1,1,0,0,0,0,0,1,1,0,1,0,1,1,0,0,0,0,1,0,0,0,1},
{0,1,1,0,1,0,0,0,1,1,1,1,0,0,1,1,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,1,0,0,0,1},
{0,0,0,1,1,1,1,0,1,1,0,0,0,0,1,1,1,1,1,0,0,1,1,0,1,0,1,1,1,1,1,1,1,0,0,0,0},
{0,0,0,1,1,1,0,0,0,1,0,0,1,0,1,1,1,1,1,1,1,1,1,0,0,1,1,1,0,1,0,0,1,0,1,1,0},
{0,0,0,1,1,1,1,0,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,0,0,1,0,1,1,0,0,1,0,1,0,1,1},
{1,1,0,1,0,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1,0,1,1,1,1,0,1},
{1,1,1,0,1,0,1,1,0,0,1,0,0,1,0,0,0,1,0,0,1,1,1,0,0,1,0,1,1,0,0,1,1,0,0,1,1},
{1,1,0,0,0,1,0,1,1,0,0,1,0,0,1,0,0,1,1,1,0,1,1,0,1,0,1,0,1,0,1,0,0,1,1,0,0},
{1,1,1,1,0,1,1,1,0,0,1,0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,1,0,1,1,1,1,1,0,1,1,1},
{1,0,1,1,0,0,0,1,0,1,1,0,0,1,1,0,1,0,1,1,0,0,1,0,1,0,0,1,0,1,0,1,0,0,1,0,1},
{1,0,0,1,1,0,1,1,0,0,0,1,0,0,1,1,0,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,0,1,1,1},
{0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,1,0,0,0,1,0,1,0,0},
{1,1,1,1,1,1,1,0,1,1,1,1,0,0,1,1,0,1,1,1,0,0,0,1,0,1,1,0,1,0,1,0,1,0,0,0,1},
{1,0,0,0,0,0,1,0,1,0,0,1,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1,1,0,0,0,1,0,0,0,1},
{1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,0,0,1,1},
{1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,0,1,0,0,1,1,1,0,0,1,0,1,1,1,1,0,0,0,1,1,0},
{1,0,1,1,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,0,1,0,0,1,1,1,1,1},
{1,0,0,0,0,0,1,0,0,1,1,1,1,0,0,1,0,1,1,0,0,0,0,0,0,0,1,1,1,1,1,0,1,0,1,1,1},
{1,1,1,1,1,1,1,0,1,1,1,0,0,1,1,0,1,0,0,1,0,0,1,0,1,0,1,1,1,0,1,1,0,1,1,0,1} };

static int QQR[30][30] = {
{0,0,1,1,1,0,0,0,1,1,0,0,1,1,0,1,0,0,1,0,0,0,0,0,1,1,1,0,0},
{0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,1,0,1,0,0,0,1,0,0,0,1,0},
{1,0,1,1,1,0,1,0,0,1,1,1,0,1,0,0,1,1,1,1,1,0,1,0,1,1,1,0,1},
{1,0,1,1,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,1,1,0,1},
{1,0,1,1,1,0,1,0,1,0,1,0,0,0,1,1,0,0,0,0,0,0,1,0,1,1,1,0,1},
{0,1,0,0,0,1,0,0,1,0,0,0,0,1,0,0,1,1,0,0,1,0,0,1,0,0,0,1,0},
{0,0,1,1,1,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,1,1,0,0},
{0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0},
{0,1,1,1,1,1,1,1,0,1,1,0,0,1,1,0,0,0,0,0,1,0,0,1,1,0,0,0,1},
{1,1,0,1,1,0,0,1,0,0,0,1,0,0,1,0,1,0,0,0,0,1,1,0,0,1,0,1,1},
{0,0,1,0,1,0,1,1,0,1,1,1,0,1,1,0,0,1,1,1,0,0,0,1,1,0,1,0,1},
{0,1,0,0,1,1,0,1,0,1,0,0,0,0,0,0,0,0,1,0,1,0,1,1,1,1,1,0,0},
{0,1,1,1,0,1,1,0,1,1,1,0,1,0,1,1,0,0,0,0,1,1,0,0,1,1,0,1,0},
{1,0,1,0,0,1,0,1,0,1,0,0,1,1,1,0,1,0,1,1,1,0,1,0,1,1,0,0,1},
{0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,1,0,0,0,1,0,0,0,1,0,0,1,0,1},
{0,0,1,1,0,1,0,1,1,1,1,0,1,0,1,0,1,0,1,1,0,0,1,1,1,0,1,1,1},
{0,0,0,1,1,0,1,0,1,0,0,0,1,1,1,1,0,0,1,0,1,0,0,1,0,0,0,0,0},
{1,1,0,1,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,0},
{1,0,0,1,1,1,1,1,1,0,1,1,0,0,1,1,0,1,0,0,1,1,0,0,0,0,1,0,0},
{1,0,0,1,1,1,0,1,1,0,1,1,0,1,1,1,0,0,0,1,1,0,1,0,1,1,1,1,1},
{1,0,1,0,1,0,1,0,1,1,0,0,0,0,1,1,0,0,1,0,1,1,1,1,1,1,1,1,1},
{0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,0,1,0,0,1,1,0,0,0,1,1,0,0,1},
{0,0,1,1,1,0,0,0,1,1,1,0,1,0,0,0,0,1,0,1,1,0,1,0,1,1,1,1,0},
{0,1,0,0,0,1,0,0,1,0,1,0,0,0,0,0,0,1,1,1,1,0,0,0,1,1,0,1,1},
{1,0,1,1,1,0,1,0,1,0,1,0,0,1,1,1,0,1,1,1,1,1,1,1,1,0,0,0,0},
{1,0,1,1,1,0,1,0,1,1,1,1,0,0,1,0,0,0,0,0,1,1,1,0,1,1,0,0,0},
{1,0,1,1,1,0,1,0,1,1,0,0,0,1,0,1,1,1,0,0,0,1,1,1,1,1,1,1,0},
{0,1,0,0,0,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,1},
{0,0,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,0,1,0,0,0,1,1,1,1,0,0,0}
};

static int Pay[45][45] = {
{1,1,1,1,1,1,1,0,1,0,1,1,1,0,1,0,0,1,0,1,1,0,1,0,0,0,1,1,0,0,1,1,0,0,0,0,1,0,1,1,1,1,1,1,1},
{1,0,0,0,0,0,1,0,1,0,0,0,1,1,1,0,0,0,1,0,1,1,1,1,1,1,0,0,0,1,1,1,0,1,0,1,0,0,1,0,0,0,0,0,1},
{1,0,1,1,1,0,1,0,1,0,1,1,1,0,1,0,0,0,0,1,1,1,1,0,0,1,0,1,0,1,1,0,1,0,0,1,0,0,1,0,1,1,1,0,1},
{1,0,1,1,1,0,1,0,1,0,0,0,0,1,0,1,1,1,0,0,0,1,0,0,0,1,1,1,1,1,0,0,1,0,0,1,1,0,1,0,1,1,1,0,1},
{1,0,1,1,1,0,1,0,1,1,1,0,1,1,0,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,0,0,0,1,1,1,1,0,1,0,1,1,1,0,1},
{1,0,0,0,0,0,1,0,0,0,1,1,1,1,0,0,1,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0,1,1,0,0,0,0,1,0,0,0,0,0,1},
{1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1},
{0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,0,1,1,0,1,0,0,0,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,1,1,0,1,0,1,1,0,1,1,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1,0,1,1,0,0,1,1,0,1,0,1,1,1,1,1},
{1,0,0,0,0,1,0,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,0,0,0,1,1,0,1,1,0,0,0},
{0,1,0,1,1,0,1,1,0,0,0,1,0,0,0,0,1,0,1,1,0,1,0,1,0,0,1,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,0,0,1},
{0,0,0,0,1,1,0,1,0,1,1,0,1,0,1,0,0,0,0,1,0,0,1,0,1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,1,0,0,0,0},
{1,1,0,1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,0,0,1,1,0,0,0,1,0,1,1,1,0,0,0,1,1,0,1,0,0,1,0,1,1,1,1},
{1,0,0,1,1,0,0,1,0,0,1,1,1,1,1,0,0,1,0,1,1,1,1,1,1,1,0,0,1,0,0,0,0,1,0,0,0,0,1,1,0,0,0,1,0},
{1,1,1,0,0,1,1,1,0,0,0,1,1,1,1,0,0,0,0,0,1,1,0,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1},
{0,1,0,1,0,0,0,1,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,1,0,1,1,1,0},
{0,0,0,1,1,0,1,0,0,0,1,1,1,1,1,0,1,0,0,0,1,0,1,0,0,1,1,1,0,0,0,1,1,0,0,0,0,0,1,1,0,1,1,1,1},
{0,0,0,0,0,1,0,1,1,1,0,1,0,0,1,0,1,0,0,1,0,1,0,1,0,1,0,0,1,0,0,1,1,0,1,1,1,0,1,1,1,1,0,1,0},
{1,0,0,1,1,1,1,1,1,1,0,1,1,1,0,0,1,0,0,1,0,1,0,1,0,1,1,1,0,0,0,1,1,1,0,1,0,1,1,1,1,1,0,1,1},
{0,1,0,0,0,1,0,1,1,0,0,1,0,0,1,0,1,0,0,1,0,1,0,1,0,1,0,0,1,0,0,1,1,0,0,1,0,0,0,1,0,0,0,1,1},
{1,1,1,0,1,1,1,1,1,0,1,0,0,1,1,0,1,1,1,1,0,1,0,1,1,1,1,1,0,0,1,0,1,1,0,0,1,1,1,1,1,0,1,0,0},
{0,1,0,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,0,0,1,0,0,0,0},
{0,1,1,0,1,0,1,0,1,0,0,1,1,0,0,0,1,1,1,0,0,0,1,1,0,0,1,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,1},
{1,0,0,0,1,0,0,0,1,1,1,0,1,0,0,0,1,0,0,1,0,1,0,0,1,0,1,0,1,0,0,1,1,0,0,0,1,0,0,0,1,1,1,1,1},
{0,0,1,0,1,1,1,1,1,0,1,0,1,0,1,0,1,0,0,1,0,1,0,0,1,0,0,1,0,0,0,1,1,1,0,0,1,1,1,1,1,0,0,1,1},
{0,1,1,1,0,1,0,1,1,1,1,1,1,0,1,0,1,1,1,0,0,1,1,1,1,0,0,1,0,0,0,1,1,1,0,0,1,1,0,1,1,0,0,1,0},
{1,0,0,1,0,0,1,0,0,0,1,0,1,0,1,0,1,0,0,1,0,1,0,0,1,0,0,1,0,0,0,1,1,0,0,1,0,0,1,0,1,0,1,0,1},
{0,0,1,0,0,1,0,1,0,1,0,0,1,0,0,0,1,0,1,0,1,1,0,0,1,0,0,1,0,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1},
{1,0,0,1,1,0,1,1,0,1,1,1,0,0,0,0,1,0,0,1,0,1,0,0,1,0,0,1,1,0,1,0,1,1,0,1,0,1,1,1,1,0,0,1,1},
{0,0,0,1,1,0,0,0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,1},
{1,0,0,1,0,0,1,0,0,1,0,0,0,1,1,1,1,0,1,0,1,1,1,1,0,0,0,0,0,0,0,0,1,0,1,0,1,1,1,0,1,1,1,0,1},
{1,0,1,1,0,1,0,1,0,0,1,0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1},
{0,0,1,1,1,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,1,1,1,1,0,1,1,0,1,0,1,0,0,0,1,0,1,0,1,1,1,0,0,0,1},
{1,0,1,0,1,0,0,0,1,0,1,0,0,1,1,0,1,0,1,1,1,0,0,0,1,0,1,0,1,1,1,1,0,1,0,1,1,1,0,0,1,1,0,1,0},
{0,0,0,0,1,0,1,0,0,1,1,1,1,0,1,1,0,1,1,0,0,1,1,1,0,0,0,1,1,0,1,1,1,1,1,0,1,0,1,0,1,1,0,0,0},
{0,1,1,1,1,0,0,0,0,1,0,0,1,0,0,0,1,1,0,0,0,1,1,1,0,1,0,0,1,1,0,0,0,0,1,0,1,1,0,0,0,0,0,0,1},
{1,0,0,1,1,0,1,0,1,1,1,0,0,1,0,1,1,0,0,1,1,1,1,1,1,0,1,1,1,1,0,1,1,0,0,0,1,1,1,1,1,0,1,0,1},
{0,0,0,0,0,0,0,0,1,0,0,0,1,1,0,1,1,0,0,1,1,0,0,0,1,0,0,1,1,1,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0},
{1,1,1,1,1,1,1,0,1,0,0,1,1,0,0,0,1,0,0,1,1,0,1,0,1,1,0,1,1,1,0,1,1,0,1,1,1,0,1,0,1,1,0,0,0},
{1,0,0,0,0,0,1,0,0,1,0,1,0,0,0,1,0,0,1,0,1,0,0,0,1,0,0,1,0,0,0,1,1,1,0,0,1,0,0,0,1,0,0,0,1},
{1,0,1,1,1,0,1,0,1,1,0,0,0,1,0,0,0,0,0,0,1,1,1,1,1,0,1,0,1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,1,1},
{1,0,1,1,1,0,1,0,0,0,0,0,1,0,1,0,0,0,1,1,1,1,0,0,1,0,0,0,1,1,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0},
{1,0,1,1,1,0,1,0,1,0,0,1,1,0,1,0,0,1,0,0,0,0,0,1,0,1,1,0,1,1,0,1,1,1,0,0,0,1,0,1,1,0,0,0,1},
{1,0,0,0,0,0,1,0,1,1,0,1,0,0,1,0,1,1,1,0,1,1,1,0,0,0,0,1,0,0,1,1,0,0,0,1,1,1,0,0,1,0,1,0,1},
{1,1,1,1,1,1,1,0,0,0,1,1,1,0,0,1,1,1,0,0,1,0,0,1,0,1,0,1,0,0,0,0,0,1,1,1,1,0,1,1,1,0,1,1,1}
};

//精准毫秒延迟，比较占CPU
void delay(int milliseconds, int pause = 0) {     //毫秒级暂停,本物の暂停函数
	clock_t start_time = clock();
	while (clock() < start_time + milliseconds);
	if (pause == 1) system("pause");               //防止闪退 prevent flashover
}

//Alpha颜色
inline COLORREF AlphaPixel(UINT alpha, COLORREF clr1, COLORREF clr2) //cl2为背景色
{

	double k = (double)alpha / 256.0;

	double reverse_k = 1.0 - k;

	BYTE r = BYTE(GetRValue(clr2) * k + GetRValue(clr1) * reverse_k);
	BYTE g = BYTE(GetGValue(clr2) * k + GetGValue(clr1) * reverse_k);
	BYTE b = BYTE(GetBValue(clr2) * k + GetBValue(clr1) * reverse_k);

	return RGB(r, g, b);

}

//颜色结构体
typedef struct {
	int red;
	int green;
	int blue;
} Color;

//生成随机不太亮的颜色
Color generateRandomColor() {
	Color color;
	color.red = rand() % 256;
	color.green = rand() % 180;//防止太亮
	color.blue = rand() % 256;
	return color;
}

//判断颜色是否相近
int isColorNear(Color color, Color colors[], int count) {
	int i;
	for (i = 0; i < count; ++i) {
		if (abs(color.red - colors[i].red) < 30 &&
			abs(color.green - colors[i].green) < 30 &&
			abs(color.blue - colors[i].blue) < 30) {
			return 1; // 颜色相近
		}
	}
	return 0; // 颜色不相近
}

//返回小数部分
double FP(double input)
{
	double fractpart, intpart;

	input = 8.123456;
	fractpart = modf(input, &intpart);

	//printf("整数部分 = %lf\n", intpart);
	//printf("小数部分 = %lf \n", fractpart);

	return(fractpart);
}

//浮点数截断
double  TRUNCATE(double val, int place)    //截断val小数点后place位数
{
	double factor = pow(10, place);
	double truncated_val = floor(val * factor) / factor; // 先放大倍数，再向下取整，最后缩小倍数

	return truncated_val;
}   //TRUNCATE 浮点数截断

//产生随机浮点数，包括RAND_min和RAND_max
double  RANDOM(double RAND_min, double RAND_max)
{
	double max, min, value;
	max = RAND_max - RAND_min;
	min = -RAND_min;
	value = rand() * (max / RAND_MAX) - min;
	return value;
}

double DISTANCE(double x1, double y1, double x2, double y2)
{
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

/*绘制矩形
* 原型：RECT([x1, y1,] [x2, y2,] [边缘颜色,] [填充颜色])，分别是左上角和右下角坐标
* 不输入默认窗口填白(只输入rgb则全部填该颜色)
* 输入坐标和边缘颜色时为指定坐标绘制填充矩形
*/
void  Prime_RECT(int x1 = 0, int y1 = 0, int x2 = getwidth(), int y2 = getheight(), int R1 = 255, int G1 = 255, int B1 = 255, int R2 = 255, int G2 = 255, int B2 = 255)
{      //RGB1 for intside color  RGB2 for outside color
	   //最好不要错误输入格式，出什么bug不保证
	int wide = getwidth();
	int high = getheight();

	if (R1 + G1 + B1 > 765) { R1 = 255; G1 = 255; B1 = 255; }
	if (R2 + G2 + B2 > 765) { R2 = 255; G2 = 255; B2 = 255; }
	if (R1 + G1 + B1 < 0) { R1 = 0; G1 = 0; B1 = 0; }
	if (R2 + G2 + B2 < 0) { R1 = 0; G1 = 0; B1 = 0; }
	setfillcolor(RGB(R1, G1, B1));
	setlinecolor(RGB(R1, G1, B1));

	if (x1 == 0 && y1 == 0 && x2 == wide && y2 == high && R1 == 255 && G1 == 255 && B1 == 255 && R2 == 255 && G2 == 255 && B2 == 255)
	{   //input nothing then fill white        不输入值，默认填白
		  //setfillcolor(RGB(255, 255, 255));
		  //fillrectangle(-1, -1, wide+1, high+1
		setbkcolor(WHITE);
		cleardevice();
	}
	if (R1 + G1 + B1 <= 765 && R2 + G2 + B2 == 765)
	{   //just input x1y1x2y2 RGB1             x1y1x2y2全填充颜色
		setfillcolor(RGB(R1, G1, B1));
		fillrectangle(x1, y1, x2, y2);
	}
	if (x2 != wide && y2 == high && R1 == 255 && G1 == 255 && B1 == 255 && R2 == 255 && G2 == 255 && B2 == 255)
	{   //just input RGB1  then fill RGB1      窗口全屏填充颜色
		  //setfillcolor(RGB(x1, y1, x2));
		  //fillrectangle(-1, -1, wide+1, high+1);
		setbkcolor(RGB(x1, y1, x2));
		cleardevice();
	}
	if (R1 + G1 + B1 < 765 && R2 + G2 + B2 < 765)
	{   //inside and outside color at wiil     外框及内部颜色填充
		setlinecolor(RGB(R1, G1, B1));
		rectangle(x1, y1, x2, y2);
		setfillcolor(RGB(R2, G2, B2));
		solidrectangle(x1 + 1, y1 + 1, x2 - 1, y2 - 1);
	}
	if (R1 + G1 + B1 == 765 && R2 + G2 + B2 < 765)
	{  //inside transparent                     内部透明
		setlinecolor(RGB(R2, G2, B2));
		rectangle(x1, y1, x2, y2);
	}
}   //RECT 画矩形

//绘制直线
void LINE(double x1, double y1, double x2, double y2, Color LineColor)
{
	setlinecolor(RGB(LineColor.red, LineColor.green, LineColor.blue));
	line(x1, y1, x2, y2);
}

/*绘制文字
* 原型：TEXTOUT(文本, x, y, 大小, 字体, [文本颜色,] [背景颜色])
* 文本与坐标值是必填的，大小默认为16像素(10+2*size)，幼圆黑色透明背景
* 默认透明背景
*/
void TEXTOUT(LPCTSTR str, float x, float y, float size = 3.0, LPCTSTR font = _T("幼圆"), int R1 = 0, int G1 = 0, int B1 = 0, int R2 = 255, int G2 = 255, int B2 = 256)
{
	int wide = 10 + 2 * size;
	settextstyle(wide, 0, font);
	setbkmode(TRANSPARENT);
	settextcolor(RGB(R1, G1, B1));
	if (R2 + G2 + B2 != 766)  //背景色非透明  text background color change
	{
		setbkmode(OPAQUE);
		setbkcolor(RGB(R2, G2, B2));
	}
	outtextxy(x, y, str);
}

//获取剪切板数据
char* GetClipboardText() {
	if (!OpenClipboard(NULL)) {
		printf("无法打开剪贴板\n");
		outtextxy(100, 400, _T("剪贴板打开不了~"));
		return NULL;
	}

	HANDLE hData = GetClipboardData(CF_TEXT);
	if (hData == NULL || hData == INVALID_HANDLE_VALUE) {
		printf("无法获取剪贴板中的数据\n");
		outtextxy(100, 400, _T("剪贴板没有东西~"));
		CloseClipboard();
		return NULL;
	}

	char* pszText = NULL;
	pszText = (char*)GlobalLock(hData);
	if (pszText == NULL) {
		printf("无法锁定剪贴板数据\n");
		outtextxy(100, 400, _T("锁定不了数据~"));
		CloseClipboard();
		return NULL;
	}

	char* clipboardText = _strdup(pszText);

	GlobalUnlock(hData);
	CloseClipboard();

	return clipboardText;
}

//渐变按钮
void button(LPCTSTR text, int x, int y, int w = 50, int h = 24, int size = 4, LPCTSTR font = _T("方正姚体"), int R1 = 255, int G1 = 0, int B1 = 200, int R2 = 20, int G2 = 200, int B2 = 255)
{
	int rgbclr[] = { 0,0,0,1,-1,-1 };
	if (x == -1 && y == -1)
	{
		x = getwidth() - 51;
		y = getheight() - 25;
	}

	int wide = 10 + 2 * size;
	settextstyle(wide, 0, font);
	int tx = x + (w - textwidth(text)) / 2;
	int ty = y + (h - textheight(text)) / 2;

	setorigin(0, 0);
	setbkmode(TRANSPARENT);
	settextcolor(RGB(R1, G1, B1));

	Prime_RECT(x, y, x + w, y + h, 0, 0, 0, 0, 0, 0);
	for (int i = 0; i <= h - 4; i++)
	{
		if (rgbclr[3] == 1) rgbclr[0] = R2 + i * 5;		else  rgbclr[0] = R2 + i * 5;	if (rgbclr[0] > 255) rgbclr[3] = -rgbclr[3];
		if (rgbclr[4] == -1) rgbclr[1] = G2 - i * 5;	else  rgbclr[1] = G2 + i * 5;	if (rgbclr[1] > 255) rgbclr[4] = -rgbclr[4];
		if (rgbclr[5] == -1) rgbclr[2] = B2 - i * 5;	else  rgbclr[2] = R2 + i * 5;	if (rgbclr[2] > 255) rgbclr[5] = -rgbclr[5];
		setlinecolor(RGB(rgbclr[0], rgbclr[1], rgbclr[2]));
		line(x + 2, y + 2 + i, x + w - 2, y + 2 + i);
	}

	outtextxy(tx, ty, text);
}

/*
int* Wait(int FuntRetVal)   //WAIT function return value
{
	int x1, y1, x2, y2, clicktime;
	char key = 0;
	int time = 0;
	ExMessage m;

	int value = 0;
	while (value == 0)
	{
		m = getmessage(EX_MOUSE | EX_KEY);

		if (m.message == WM_LBUTTONDOWN)
		{
			x1 = m.x;	y1 = m.y;
			while (value == 0)
			{
				if (peekmessage(&m, EX_MOUSE)) m = getmessage(EX_MOUSE);
				Sleep(1);    //dog shit sleep make me sleep
				time++;

				if (time >= 100 && m.message != WM_MOUSEMOVE) value = 1;
				else
				{
					time = 0;
					while (value == 0)
					{
						m = getmessage(EX_MOUSE);
						if (m.message == WM_MOUSEMOVE) { Sleep(1); time++; }
						if (m.message == WM_LBUTTONUP) { x2 = m.x;	y2 = m.y; value = 6; clicktime = time; }
					}
				}
			}
		}//left down

	}

	valreturn[1] = FuntRetVal;
	valreturn[2] = x1;
	valreturn[3] = y1;

	return valreturn;
}
*/

/*选择窗口
* 原型：CHOOSE(变量, "标题", "字体", "项1", "项1",[..."项n"])
*选择框自适应最长的选择项
* 返回：选择的项，取消返回-1
*/
template <typename... Options>
int CHOOSE(int& value, LPCTSTR title = "Choose", LPCTSTR font = "幼圆", Options... options) {
	ExMessage m;
	int maxwidth = 0, rectleft, select;
	int nums = sizeof...(options); // 获取选项的数量
	using std::vector;
	std::vector<LPCTSTR> optionList = { options... }; // 将选项包装成一个容器

	// 计算最大宽度
	maxwidth = textwidth(title);
	using std::vector;
	std::vector<int> widths(nums + 1, 0);  // 存储每个选项的宽度

	widths[0] = textwidth(title);  // 标题的宽度
	for (int i = 0; i < nums; ++i) {
		widths[i + 1] = textwidth(optionList[i]);
		maxwidth = max(maxwidth, widths[i + 1]);
	}

	select = value;
	if (select > nums) select = nums - 1;
	if (select < 1) select = 1;
	rectleft = (getwidth() - maxwidth) / 2 - 24;

	// 设置窗口和按钮
	setorigin(0, 0);
	Prime_RECT();
	button(_T("确认"), -1, -1);
	button(_T("取消"), getwidth() - 51 * 2 - 2, getheight() - 25);

	settextstyle(20, 0, font);
	Prime_RECT(rectleft, 50, getwidth() - rectleft, 50 + (nums + 1) * 24 + 3, 255, 255, 255, 0, 0, 0);
	Prime_RECT(rectleft + 1, 50 + 1, getwidth() - rectleft - 1, 50 + 2 + 20, 38, 80, 160);
	TEXTOUT(title, rectleft + 5, 50 + 2, 3, font, 255, 255, 255);
	line(rectleft, 50 + 2 + 20, getwidth() - rectleft, 50 + 2 + 20);

	Prime_RECT(rectleft + 2, 50 + 24 * select, getwidth() - rectleft - 2, 50 + 24 + 24 * select, 173, 204, 255);

	// 绘制选项
	for (int i = 0; i < nums; ++i) {
		TEXTOUT(optionList[i], rectleft + 16, 50 + 28 + 24 * i);
	}

	// 获取用户输入
	int key = -1;
	flushmessage();

	while (1) {
		m = getmessage(EX_MOUSE | EX_KEY);
		key = -1;

		switch (m.message) {
		case WM_LBUTTONDOWN:
			if (rectleft < m.x && m.x < getwidth() - rectleft) {
				for (int i = 1; i <= nums; ++i) {
					if (50 + 24 * i < m.y && m.y < 50 + 24 + 24 * i) {
						select = i;
						key = 13;
					}
				}
			}
			if (getheight() - 25 <= m.y && m.y <= getheight()) {
				if (getwidth() - 51 <= m.x && m.x <= getwidth() && getheight() - 25 <= m.y && m.y <= getheight()) {
					key = 13; // confirm
				}
				if (getwidth() - 51 * 2 - 2 <= m.x && m.x <= getwidth() - 51 - 2 && getheight() - 25) {
					key = 27; // cancel
				}
			}
			flushmessage(EX_MOUSE);
			break;

		case WM_KEYDOWN:
			if (m.vkcode == VK_RETURN || m.vkcode == VK_SEPARATOR) key = 13;
			if (m.vkcode == VK_ESCAPE) key = 27;
			if (m.vkcode == VK_UP || m.vkcode == VK_DOWN) {
				Prime_RECT(rectleft + 2 - 1, 50 + 24 * select - 1, getwidth() - rectleft - 2 + 1, 50 + 24 + 24 * select + 1, 255, 255, 255, 255, 255, 255);
				if (m.vkcode == VK_UP) {  // up arrow
					select--;
					if (select < 1) select = nums;
				}
				if (m.vkcode == VK_DOWN) {  // down arrow
					select++;
					if (select > nums) select = 1;
				}
				Prime_RECT(rectleft + 2, 50 + 24 * select, getwidth() - rectleft - 2, 50 + 24 + 24 * select, 173, 204, 255);
				for (int i = 0; i < nums; ++i) {
					TEXTOUT(optionList[i], rectleft + 16, 50 + 28 + 24 * i);
				}
			}
			break;
		}

		if (key == 13 || key == 27) break;
	}

	if (key == 27)
		select = -1;
	value = select;
	return value;
}

//创建文件夹
int FileCrt(char* filepath)
{
	int res;
	//filepath为绝对路径
	//如果文件夹不存在
	if (_mkdir(filepath) == 0)
		res = 1;
	//remove(filename) 删除文件
	else
		res = 0;
	return res;
}

//MBCS转Uni(PATH)
TCHAR* PATHcTow(char* path)
{
	int bufferSize = MultiByteToWideChar(CP_ACP, 0, path, -1, nullptr, 0);
	TCHAR* wPATH = (TCHAR*)malloc(bufferSize * sizeof(TCHAR));  // 动态分配内存
	if (wPATH != nullptr)
	{
		MultiByteToWideChar(CP_ACP, 0, path, -1, wPATH, bufferSize);
	}
	return wPATH;
}

char* TCHAR_to_char(TCHAR* tchar_str) {
	size_t len = wcslen(tchar_str) + 1;  // 计算wchar_t字符串的长度（包括结尾的'\0'）

	char* char_str = (char*)malloc(len * sizeof(char));
	if (char_str == NULL) {
		return NULL;
	}

	wcstombs(char_str, tchar_str, len);
	return char_str;
}

//文件窗窗窗 0为打开 1为另存为
static int FileDialog(TCHAR* path, bool saveDialog, const wchar_t* dialogTitle = L"ChalloWar", const wchar_t* Filter = L"bmp/jpg/png\0*.bmp;*.jpg;*.png;\0只能选上面这些类型\0*.bmp\0要不然会爆炸的唷\0*.jpg\0\0")
{
	// 文件选择窗
	OPENFILENAMEW ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);  // 结构大小
	ofn.lpstrFile = path;           // 路径
	ofn.nMaxFile = MAX_PATH;        // 路径大小
	ofn.lpstrFilter = Filter;  // 文件类型

	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;   // 需要选择已经存在的文件（针对“打开”对话框）
	ofn.lpstrTitle = dialogTitle;

	if (saveDialog) {
		// 设置为 "另存为" 对话框
		ofn.Flags &= ~OFN_FILEMUSTEXIST;
		return GetSaveFileNameW(&ofn);    // 另存为
	}
	else {
		// 设置为 "打开" 对话框
		return GetOpenFileNameW(&ofn);    // 打开
	}
}

char* getFileName(TCHAR* filepath) {
	// 假设TCHAR是wchar_t类型
	// 1. 获取文件名部分（路径最后的部分）
	TCHAR* filename = wcsrchr(filepath, '\\'); // 找到最后一个'\\'字符
	if (filename == NULL) {
		filename = filepath;  // 如果没有找到'\\'，说明没有路径部分，直接返回文件名
	}
	else {
		filename++;  // 跳过'\\'字符，指向文件名部分
	}

	// 2. 提取文件名的扩展名部分
	TCHAR* dot_position = wcsrchr(filename, '.'); // 查找最后一个'.'字符
	size_t filename_len = dot_position ? (size_t)(dot_position - filename) : wcslen(filename); // 计算文件名长度，不包含扩展名

	// 3. 将文件名复制到char*中
	// 分配内存存储结果字符串
	char* result = (char*)malloc((filename_len + 1) * sizeof(char));
	if (result == NULL) {
		return NULL; // 内存分配失败，返回NULL
	}

	// 将TCHAR字符串转换为char字符串
	wcstombs(result, filename, filename_len + 1);

	// 确保字符串以'\0'结尾
	result[filename_len] = '\0'; // 手动添加字符串结束符

	return result;
}

//检查是否有浮点数 无返回0
float floNum(float Flot)
{
	long int integer_part = (long int)Flot;
	if (Flot != (double)integer_part)
		return Flot - floor(Flot);
	else
		return 0;
}
