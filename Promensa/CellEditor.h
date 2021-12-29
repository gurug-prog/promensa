#pragma once
#include "TableView.h"
#include "framework.h"
#include "TextChange.h"

typedef unsigned int uint;

class CellEditor
{
	HWND hEditable;
	TableView* table;
	uint uCurrentItem;
	uint uCurrentSubItem;
	vector<TextChange> changes;
public:
	CellEditor();
	~CellEditor();
	CellEditor(TableView*);
	LRESULT EditBoxWndProc(HWND, UINT, WPARAM, LPARAM);
	void CreateEditBox(HWND, NMLISTVIEW*);
	void OnDestroyWindow();
	void OnUndo();
};
