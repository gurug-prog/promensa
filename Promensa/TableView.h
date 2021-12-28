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
	void AddColumn(int, wstring, int);
	void AddRow(int, int, vector<wstring>);
	wstring GetCell(int, int);
	void Clear();
	void HandleSortState(LPARAM);

	// DataProcessor
	void ReadFile(LPWSTR);
	bool DoubleTryParse(wstring, double*);
	vector<wstring> Split(wstring, wstring);
	vector<wstring> GetEntitiesStrings();
public:
	TableView();
	TableView(HWND);
	~TableView();
	void OnColumnClick(LPARAM);
	void FillTable(LPWSTR);

	// DataProcessor
	void SaveFile(LPWSTR);
	// Unknown class
	int CompareListItemsAsc(LPARAM, LPARAM);
	int CompareListItemsDesc(LPARAM, LPARAM);
};
