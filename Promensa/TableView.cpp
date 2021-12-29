#include "TableView.h"
#include "Promensa.h"
#include "DataProcessor.h"
#include "DialogInvoker.h"


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

	int x = 0, y = 37;
	GetClientRect(hWndParent, &rcl);
	int listWidth = rcl.right - rcl.left;
	int listHeight = rcl.bottom - rcl.top;

	this->hWndList = CreateWindow(WC_LISTVIEW, L"",
		WS_VISIBLE | WS_BORDER | WS_CHILD | LVS_REPORT,
		x, y, listWidth, listHeight,
		hWndParent, (HMENU)IDC_LISTVIEW, hInst, 0);
	ListView_SetExtendedListViewStyle(this->hWndList, LVS_EX_FULLROWSELECT);
	this->selectedCol = 0;
	this->order = SortState::Unsorted;
	thisPtr = this;
	this->SetFont(hWndParent);
}

TableView::~TableView()
{
}

HWND TableView::GetHWND()
{
	return this->hWndList;
}

void TableView::SetFont(HWND hWnd)
{
	LOGFONT logFont;
	HFONT hFont;
	memset(&logFont, 0, sizeof(LOGFONT));
	logFont.lfHeight = 20;
	wcscpy_s(logFont.lfFaceName, L"Consolas");
	hFont = CreateFontIndirect(&logFont);

	PostMessage(this->hWndList, WM_SETFONT, (WPARAM)hFont, TRUE);
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

void TableView::SetText(int iCol, int iRow, wstring text)
{
	ListView_SetItemText(this->hWndList, iRow,
		iCol, const_cast<LPWSTR>(text.c_str()));
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
	columns.clear();
	rows.clear();
	this->Clear();
	auto entities = DataProcessor::ReadFile(fileInput);

	this->columns = entities[0];
	entities.erase(entities.begin());
	this->rows = entities;

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

void TableView::OnColumnClick(LPARAM lParam)
{
	auto pLVInfo = (LPNMLISTVIEW)lParam;
	const int nSortColumn = pLVInfo->iSubItem;

	if (this->selectedCol != nSortColumn) this->order = SortState::Ascending;
	this->selectedCol = nSortColumn;

	// sort list
	this->HandleSortState(lParam);
}

void TableView::OnSize(HWND hWnd)
{
	RECT rc;
	if (this->hWndList)
	{
		GetClientRect(hWnd, &rc);
		MoveWindow(this->hWndList, 0, 37, rc.right - rc.left, rc.bottom - rc.top - 37, FALSE);
	}
}

void TableView::OnFileOpen(HWND hWnd)
{
	auto fileName = DialogInvoker::ProcessOpenDlg(hWnd);
	if (fileName) this->FillTable(fileName);
}

void TableView::OnFileSave()
{
	auto lines = this->GetEntitiesStrings();
	DataProcessor::SaveFile(nullptr, lines);
}

void TableView::OnFileSaveAs(HWND hWnd)
{
	auto fileName = DialogInvoker::ProcessSaveAsDlg(hWnd);
	auto lines = this->GetEntitiesStrings();
	if (fileName) DataProcessor::SaveFile(fileName, lines);
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

int TableView::CompareListItemsAsc(LPARAM lParam1, LPARAM lParam2)
{
	wstring strItem1 = this->GetCell((int)lParam1, this->selectedCol);
	wstring strItem2 = this->GetCell((int)lParam2, this->selectedCol);

	double numItem1, numItem2;
	bool isNumberSorting = DataProcessor::DoubleTryParse(strItem1, &numItem1)
		&& DataProcessor::DoubleTryParse(strItem2, &numItem2);

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
	bool isNumberSorting = DataProcessor::DoubleTryParse(strItem1, &numItem1)
		&& DataProcessor::DoubleTryParse(strItem2, &numItem2);

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

static int CALLBACK CallBackSortAsc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	if (thisPtr) return thisPtr->CompareListItemsAsc(lParam1, lParam2);
}

static int CALLBACK CallBackSortDesc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	if (thisPtr) return thisPtr->CompareListItemsDesc(lParam1, lParam2);
}
