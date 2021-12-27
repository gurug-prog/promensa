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
	LPWSTR fileName;
private:
	void AddColumn(int, wstring, int);
	void AddRow(int, int, vector<wstring>);
	bool DoubleTryParse(wstring, double*);
	void Clear();
	wstring GetCell(int, int);
	void ReadFile(LPWSTR);
	void HandleSortState(LPARAM);
	vector<wstring> Split(wstring, wstring);
	vector<wstring> GetEntitiesStrings();
public:
	TableView();
	TableView(HWND);
	~TableView();
	void OnColumnClick(LPARAM);
	void FillTable(LPWSTR);
	void SaveFile(LPWSTR);
	int CompareListItemsAsc(LPARAM, LPARAM);
	int CompareListItemsDesc(LPARAM, LPARAM);
};
