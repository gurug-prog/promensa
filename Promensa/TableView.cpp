#include "TableView.h"
#include "Promensa.h"
#include "commdlg.h"


TableView::TableView()
{
    this->hWndList = NULL;
    this->order = SortState::Unsorted;
	this->prevColumn = 0;
	// this->columns = ; how to init vector???
	// this->rows = ; how to init vector???
}

TableView::TableView(HWND hWndParent)
{
	InitCommonControls();
	const auto hInst = (HINSTANCE)GetWindowLong(hWndParent, GWL_HINSTANCE);

	RECT rcl;
	GetClientRect(hWndParent, &rcl);
	int x = 0, y = 0;
	int listWidth = rcl.right - rcl.left;
	int listHeight = rcl.bottom - rcl.top;

	this->hWndList = CreateWindow(WC_LISTVIEW, L"",
		WS_VISIBLE | WS_BORDER | WS_CHILD | LVS_REPORT | LVS_EDITLABELS,
		x, y, listWidth, listHeight,
		hWndParent, (HMENU)IDC_LISTVIEW, hInst, 0);
    this->order = SortState::Unsorted;
	this->prevColumn = 0;
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

void TableView::ReadFile(LPWSTR fileName)
{
	columns.clear();
	rows.clear();

	vector<vector<wstring>> entities = { };
	wstring str;
	wifstream file(L"./entities.tsv");
	while (getline(file, str))
	{
		auto attibutes = Split(str, L"\t");
		entities.push_back(attibutes);
		// TODO something
		// после заполнения вектора entities
		// в методе заполнения вектора
		// auto vector = ReadFile();
	}

	this->columns = entities[0];
	entities.erase(entities.begin());
	this->rows = entities;
	delete fileName;
}

void TableView::Clear()
{
	ListView_DeleteAllItems(this->hWndList);
	HWND hWndHeader = (HWND)::SendMessage(this->hWndList, LVM_GETHEADER, 0, 0);
	int columnCount = (int)::SendMessage(hWndHeader, HDM_GETITEMCOUNT, 0, 0L);
	for (int iCol = columnCount - 1; iCol >= 0; --iCol)
		ListView_DeleteColumn(this->hWndList, iCol);
}

void TableView::FillTable(HWND hWnd)
{
	this->Clear();
	RECT rcl;
	GetClientRect(hWnd, &rcl);

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
