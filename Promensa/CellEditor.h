#pragma once
#include "TableView.h"
#include "framework.h"
#include <commctrl.h>
#pragma comment (lib, "comctl32.lib")

typedef unsigned int uint;

class CellEditor
{
public:
	HWND hEditable;
private:
	TableView* table;
	uint uCurrentItem;
	uint uCurrentSubItem;
public:
	CellEditor();
	~CellEditor();
	CellEditor(TableView*);
	LRESULT EditBoxWndProc(HWND, UINT, WPARAM, LPARAM);
	void CreateEditBox(HWND, NMLISTVIEW*);
	void OnDestroyWindow();
};
