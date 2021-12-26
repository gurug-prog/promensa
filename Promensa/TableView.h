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
	vector<wstring> columns;
	vector<vector<wstring>> rows;
	//vector<vector<wstring>> entities;
	//private:
		//void NextState();
public:
	vector<wstring> Split(wstring, wstring);
	void ReadFile(LPWSTR);
	void SaveFile();
public:
	TableView();
	TableView(HWND);
	~TableView();
	void AddColumn(int, wstring, int);
	void AddRow(int, int, vector<wstring>);
	void Clear();
	void FillTable(HWND);
};

