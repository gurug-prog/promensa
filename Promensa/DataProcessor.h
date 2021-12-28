#pragma once
#include "framework.h"
#include <vector>
#include <string>

using namespace std;

class DataProcessor
{
private:
	DataProcessor() { };
	~DataProcessor() { };
public:
	static LPWSTR fileName;
	static vector<vector<wstring>> ReadFile(LPWSTR);
	static void SaveFile(LPWSTR, vector<wstring>);
	static vector<wstring> Split(wstring, wstring);
	static bool DoubleTryParse(wstring, double*);
};
