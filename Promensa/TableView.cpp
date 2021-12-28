#include "TableView.h"
#include "Promensa.h"
#include <fstream>
#include "DataProcessor.h"


static TableView* thisPtr = nullptr;
static int CALLBACK CallBackSortAsc(LPARAM, LPARAM, LPARAM);
static int CALLBACK CallBackSortDesc(LPARAM, LPARAM, LPARAM);

TableView::TableView()
{
	this->rcl = { };
	this->hWndList = NULL;
	this->selectedCol = 0;
	this->order = SortState::Unsorted;
	thisPtr = this;
}

TableView::TableView(HWND hWndParent)
{
	InitCommonControls();
	const auto hInst = (HINSTANCE)GetWindowLong(hWndParent, GWL_HINSTANCE);

	int x = 0, y = 0;
	GetClientRect(hWndParent, &rcl);
	int listWidth = rcl.right - rcl.left;
	int listHeight = rcl.bottom - rcl.top;

	this->hWndList = CreateWindow(WC_LISTVIEW, L"",
		WS_VISIBLE | WS_BORDER | WS_CHILD | LVS_REPORT | LVS_EDITLABELS,
		x, y, listWidth, listHeight,
		hWndParent, (HMENU)IDC_LISTVIEW, hInst, 0);
	this->selectedCol = 0;
	this->order = SortState::Unsorted;
	thisPtr = this;
}

TableView::~TableView()
{
}

void TableView::AddColumn(int columnIndex, wstring value, int width)
{
	const int maxLen = 128;

	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = width;
	lvc.iSubItem = columnIndex;
	lvc.pszText = const_cast<LPWSTR>(value.c_str());
	lvc.cchTextMax = maxLen;

	ListView_InsertColumn(this->hWndList, columnIndex, &lvc);
}

void TableView::AddRow(int colsCount, int rowIndex, vector<wstring> row)
{
	const int maxLen = 128;

	for (int columnIndex = 0; columnIndex < colsCount; ++columnIndex)
	{
		wstring value = row[columnIndex];
		LVITEM lvi;

		lvi.mask = LVIF_TEXT;
		lvi.iItem = rowIndex;
		lvi.iSubItem = columnIndex;
		lvi.pszText = const_cast<LPWSTR>(value.c_str());
		lvi.cchTextMax = maxLen;

		if (columnIndex > 0) ListView_SetItem(hWndList, &lvi);
		else ListView_InsertItem(hWndList, &lvi);
	}
}

vector<wstring> TableView::Split(wstring str, wstring delim)
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

void TableView::ReadFile(LPWSTR fileInput)
{
	// костыль
	// {
	if (fileInput != DataProcessor::fileName) delete DataProcessor::fileName;
	DataProcessor::fileName = fileInput;
	// }
	columns.clear();
	rows.clear();

	vector<vector<wstring>> entities = { };
	wstring str;
	wifstream file(fileInput);
	while (getline(file, str))
	{
		auto attibutes = Split(str, L"\t");
		entities.push_back(attibutes);
	}

	this->columns = entities[0];
	entities.erase(entities.begin());
	this->rows = entities;
}

void TableView::SaveFile(LPWSTR fileSave)
{
	if (!fileSave) fileSave = DataProcessor::fileName;
	else DataProcessor::fileName = fileSave;

	wofstream outfile;
	outfile.open(fileSave, ios_base::out);

	//wstring outStr;
	//wstring delim = L"\t";
	//wstring eol = L"\n";

	auto entitiesStrs = GetEntitiesStrings();
	for (int i = 0; i < entitiesStrs.size(); ++i)
		outfile << entitiesStrs[i];
	//// write columns
	//for (int i = 0; i < columns.size(); ++i)
	//{
	//    if (i == columns.size() - 1)
	//    {
	//        outStr = outStr + columns[i] + eol;
	//        break;
	//    }
	//    outStr = outStr + columns[i] + delim;
	//}
	//outfile << outStr;
	//outStr = L"";

	//// write rows
	//for (int i = 0; i < rows.size(); ++i)
	//{
	//    auto row = rows[i];
	//    for (int j = 0; j < row.size(); ++j)
	//    {
	//        if (j == row.size() - 1)
	//        {
	//            outStr = outStr + row[j] + eol;
	//            break;
	//        }
	//        outStr = outStr + row[j] + delim;
	//    }
	//    outfile << outStr;
	//    outStr = L"";
	//}
}

void TableView::Clear()
{
	ListView_DeleteAllItems(this->hWndList);
	HWND hWndHeader = (HWND)::SendMessage(this->hWndList, LVM_GETHEADER, 0, 0);
	int columnCount = (int)::SendMessage(hWndHeader, HDM_GETITEMCOUNT, 0, 0L);
	for (int iCol = columnCount - 1; iCol >= 0; --iCol)
		ListView_DeleteColumn(this->hWndList, iCol);
}

void TableView::FillTable(LPWSTR fileInput)
{
	this->ReadFile(fileInput);
	this->Clear();

	// insert columns
	for (int index = 0; index < this->columns.size(); ++index)
	{
		wstring textStr = this->columns[index];
		int width = (rcl.right - rcl.left) * 0.1;
		this->AddColumn(index, textStr, width);
	}

	// insert rows
	for (int rowIndex = 0; rowIndex < this->rows.size(); ++rowIndex)
	{
		auto row = this->rows[rowIndex];
		this->AddRow(this->columns.size(), rowIndex, row);
	}
}

void TableView::HandleSortState(LPARAM lParam)
{
	auto pLVInfo = (LPNMLISTVIEW)lParam;
	auto lParamSort = 1 + pLVInfo->iSubItem;

	switch (this->order)
	{
	case SortState::Unsorted:
		FillTable(DataProcessor::fileName);
		this->order = SortState::Ascending;
		break;
	case SortState::Ascending:
		ListView_SortItemsEx(pLVInfo->hdr.hwndFrom, CallBackSortAsc, lParamSort);
		this->order = SortState::Descending;
		break;
	case SortState::Descending:
		ListView_SortItemsEx(pLVInfo->hdr.hwndFrom, CallBackSortDesc, lParamSort);
		this->order = SortState::Unsorted;
		break;
	default:
		string message = "Unknown sorting state: " + to_string((int)this->order);
		throw new exception(message.c_str());
		break;
	}
}


void TableView::OnColumnClick(LPARAM lParam)
{
	auto pLVInfo = (LPNMLISTVIEW)lParam;
	const int nSortColumn = pLVInfo->iSubItem;

	if (this->selectedCol != nSortColumn) this->order = SortState::Ascending;
	this->selectedCol = nSortColumn;

	// sort list
	this->HandleSortState(lParam);
}

int TableView::CompareListItemsAsc(LPARAM lParam1, LPARAM lParam2)
{
	wstring strItem1 = this->GetCell((int)lParam1, this->selectedCol);
	wstring strItem2 = this->GetCell((int)lParam2, this->selectedCol);

	double numItem1, numItem2;
	bool isNumberSorting = this->DoubleTryParse(strItem1, &numItem1)
		&& this->DoubleTryParse(strItem2, &numItem2);

	if (isNumberSorting)
	{
		if (numItem1 > numItem2) return 1;
		else if (numItem1 < numItem2) return -1;
		else return 0;
	}
	else
	{
		if (strItem1 > strItem2) return 1;
		else if (strItem1 < strItem2) return -1;
		else return 0;
	}
}

int TableView::CompareListItemsDesc(LPARAM lParam1, LPARAM lParam2)
{
	wstring strItem1 = this->GetCell((int)lParam1, this->selectedCol);
	wstring strItem2 = this->GetCell((int)lParam2, this->selectedCol);

	double numItem1, numItem2;
	bool isNumberSorting = this->DoubleTryParse(strItem1, &numItem1)
		&& this->DoubleTryParse(strItem2, &numItem2);

	if (isNumberSorting)
	{
		if (numItem1 < numItem2) return 1;
		else if (numItem1 > numItem2) return -1;
		else return 0;
	}
	else
	{
		if (strItem1 < strItem2) return 1;
		else if (strItem1 > strItem2) return -1;
		else return 0;
	}
}

wstring TableView::GetCell(int row, int col)
{
	const int maxLength = 256;
	WCHAR buffer[maxLength] = L"";

	LVITEM lvi;
	lvi.mask = LVIF_TEXT;
	lvi.iItem = row;
	lvi.pszText = buffer;
	lvi.iSubItem = col;
	lvi.cchTextMax = maxLength;
	ListView_GetItem(this->hWndList, &lvi);

	return wstring(buffer);
}

vector<wstring> TableView::GetEntitiesStrings()
{
	HWND hWndHeader = (HWND)::SendMessage(this->hWndList, LVM_GETHEADER, 0, 0);
	int columnsCount = (int)::SendMessage(hWndHeader, HDM_GETITEMCOUNT, 0, 0L);
	int rowsCount = ListView_GetItemCount(this->hWndList);
	wstring delim = L"\t";
	wstring eol = L"\n";

	vector<wstring> strEntities;
	wstring strCols;
	for (int iCol = 0; iCol < columnsCount; ++iCol)
	{
		if (iCol == columns.size() - 1)
		{
			strCols = strCols + columns[iCol] + eol;
			break;
		}
		strCols = strCols + columns[iCol] + delim;
	}
	strEntities.push_back(strCols);

	for (int iRow = 0; iRow < rowsCount; ++iRow)
	{
		wstring outStr;
		for (int iCol = 0; iCol < columnsCount; ++iCol)
		{
			if (iCol == columns.size() - 1)
			{
				outStr = outStr + this->GetCell(iRow, iCol) + eol;
				break;
			}
			outStr = outStr + this->GetCell(iRow, iCol) + delim;
		}
		strEntities.push_back(outStr);
	}

	return strEntities;
}

bool TableView::DoubleTryParse(wstring str, double* out)
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

static int CALLBACK CallBackSortAsc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	if (thisPtr) return thisPtr->CompareListItemsAsc(lParam1, lParam2);
}

static int CALLBACK CallBackSortDesc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	if (thisPtr) return thisPtr->CompareListItemsDesc(lParam1, lParam2);
}
