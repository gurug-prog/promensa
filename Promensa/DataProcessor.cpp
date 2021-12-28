#include "DataProcessor.h"
#include <fstream>


LPWSTR DataProcessor::fileName = NULL;


vector<vector<wstring>> DataProcessor::ReadFile(LPWSTR fileInput)
{
	if (fileInput != DataProcessor::fileName) delete DataProcessor::fileName;
	DataProcessor::fileName = fileInput;

	vector<vector<wstring>> entities = { };
	wstring str;
	wifstream file(fileInput);
	while (getline(file, str))
	{
		auto attibutes = Split(str, L"\t");
		entities.push_back(attibutes);
	}

	return entities;
}

void DataProcessor::SaveFile(LPWSTR fileSave, vector<wstring> lines)
{
	if (!fileSave) fileSave = DataProcessor::fileName;
	else DataProcessor::fileName = fileSave;

	wofstream outfile;
	outfile.open(fileSave, ios_base::out);

	for (int i = 0; i < lines.size(); ++i)
		outfile << lines[i];
}

vector<wstring> DataProcessor::Split(wstring str, wstring delim)
{
	vector<wstring> splittedValues;
	size_t pos = 0;
	wstring token;
	while (pos != wstring::npos)
	{
		pos = str.find(delim);
		token = str.substr(0, pos);
		splittedValues.push_back(token);
		str.erase(0, pos + delim.length());
	}

	return splittedValues;
}

bool DataProcessor::DoubleTryParse(wstring str, double* out)
{
	try
	{
		*out = stod(str);
		return true;
	}
	catch (const std::exception&)
	{
		return false;
	}
}
