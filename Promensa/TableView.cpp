#include "TableView.h"
#include "Promensa.h"


TableView::TableView()
{
    this->hWndList = NULL;
    this->order = SortState::Unsorted;
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
}

TableView::~TableView()
{
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

vector<vector<wstring>> TableView::ReadFile()
{
	vector<vector<wstring>> entities = { };
	wstring str;
	wifstream file("./entities.tsv");
	while (getline(file, str))
	{
		auto attibutes = Split(str, L"\t");
		entities.push_back(attibutes);
		// TODO something
		// после заполнения вектора entities
		// в методе заполнения вектора
		// auto vector = ReadFile();
	}

	return entities;
}

void TableView::FillTable(HWND hWnd)
{
	ListView_DeleteAllItems(this->hWndList);

	auto rows = ReadFile();
	auto columns = rows[0];
	rows.erase(rows.begin());
	
	const int maxLen = 128;

	RECT rcl;
	GetClientRect(hWnd, &rcl);

    LVCOLUMN lvc;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;
    lvc.cx = (rcl.right - rcl.left) * 0.1; // ширина

    for (int index = 0; index < columns.size(); index++)
    {
        wstring textStr = columns[index];
        lvc.iSubItem = index;
        lvc.pszText = const_cast<LPWSTR>(textStr.c_str());
        lvc.cchTextMax = maxLen;
        ListView_InsertColumn(hWndList, (index), &lvc);
    }


    // insert rows
    LVITEM lvi;
    wstring text = L"";
    for (int i = 0; i < rows.size(); i++)
    {
        for (int j = 0; j < columns.size(); j++)
        {
            text = rows[i][j];

            lvi.mask = LVIF_TEXT;
            lvi.iItem = i;
            lvi.iSubItem = j;
            lvi.pszText = const_cast<LPWSTR>(text.c_str());
            lvi.cchTextMax = maxLen;

            if (j > 0) ListView_SetItem(hWndList, &lvi);
            else ListView_InsertItem(hWndList, &lvi);
        }
    }
}
