#pragma once
#include "framework.h"
#include <commctrl.h>
#pragma comment (lib, "comctl32.lib")
//#include <string>

class TableView
{
private:
	HWND hWndList;
public:
	TableView();
	TableView(HWND);
	~TableView();
};

