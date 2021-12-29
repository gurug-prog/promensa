#pragma once
#include "framework.h"
#include <commctrl.h>
#pragma comment (lib, "comctl32.lib")
#include <string>
#include <vector>
#include "SortState.h"

using namespace std;

class TableView
{
private:
	HWND hWndList;
	SortState order;
	int selectedCol;
	vector<wstring> columns;
	vector<vector<wstring>> rows;
	RECT rcl;
private:
	void SetFont(HWND);
	void AddColumn(int, wstring, int);
	void AddRow(int, int, vector<wstring>);
	wstring GetCell(int, int);
	void FillTable(LPWSTR);
	void Clear();
	void HandleSortState(LPARAM);
	vector<wstring> GetEntitiesStrings();
public:
	TableView();
	TableView(HWND);
	~TableView();
	void OnColumnClick(LPARAM);
	void OnSize(HWND);
	void OnFileOpen(HWND);
	void OnFileSave();
	void OnFileSaveAs(HWND);
	int CompareListItemsAsc(LPARAM, LPARAM);
	int CompareListItemsDesc(LPARAM, LPARAM);
};
