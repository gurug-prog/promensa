#pragma once
#include "framework.h"
#include <commctrl.h>
#pragma comment (lib, "comctl32.lib")
#include <string>
#include <fstream>
#include <vector>
#include "SortState.h"

using namespace std;

class TableView
{
private:
	HWND hWndList;
	SortState order;
	int prevColumn;
public:
	vector<wstring> Split(wstring, wstring);
	vector<vector<wstring>> ReadFile();
	void SaveFile();
public:
	TableView();
	TableView(HWND);
	~TableView();
	void AddColumn();
	void AddRow();
	void FillTable(HWND);
};
