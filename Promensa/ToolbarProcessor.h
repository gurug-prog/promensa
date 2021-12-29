#pragma once
#include "framework.h"
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

class ToolBarProcessor
{
private:
	HWND hWndToolBar;
public:
	ToolBarProcessor();
	void OnCreate(HWND);
	void OnSize(HWND);
	void OnNotify(HWND, WPARAM, LPARAM);
};
