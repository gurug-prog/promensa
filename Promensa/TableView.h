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
	//SortState order;
	int order;
	int prevColumn;
	vector<wstring> columns;
	vector<vector<wstring>> rows;
	RECT rcl;
	LPWSTR fileName;
private:
	vector<wstring> Split(wstring, wstring);
	//void SaveFile();
	//void GetCell();
	//int CALLBACK CompareListItemsAsc(LPARAM, LPARAM, LPARAM);
	//int CALLBACK CompareListItemsDesc(LPARAM, LPARAM, LPARAM);
	//void NextState();
public:
	TableView();
	TableView(HWND);
	~TableView();
	void AddColumn(int, wstring, int);
	void AddRow(int, int, vector<wstring>);
	void Clear();
	void FillTable(LPWSTR);
	void OnColumnClick(LPARAM);
	wstring GetItem(int, int);
	void ReadFile(LPWSTR);
};
