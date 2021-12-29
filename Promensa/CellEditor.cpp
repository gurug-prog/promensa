#include "CellEditor.h"
#include "Promensa.h"


static CellEditor* thisPtr = NULL;
static LRESULT APIENTRY CallEditBoxWndProc(HWND, UINT, WPARAM, LPARAM);

CellEditor::CellEditor()
{
	this->table = nullptr;
	this->hEditable = NULL;
	this->uCurrentItem = 0;
	this->uCurrentSubItem = 0;
	thisPtr = this;
}

CellEditor::CellEditor(TableView* tv)
{
	this->table = tv;
	this->hEditable = NULL;
	this->uCurrentItem = 0;
	this->uCurrentSubItem = 0;
	thisPtr = this;
}

CellEditor::~CellEditor()
{
}

void CellEditor::CreateEditBox(HWND hWnd, NMLISTVIEW* _lParam)
{
	const auto hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	RECT r;
	WNDPROC wpOld;
	const int bufferSize = 256;
	
	/*	Create the edit box inside the cell I want to modify. I need to get
		the coordinates of the upper-left and lower-right corners of the cell	*/
	ListView_GetSubItemRect(table->hWndList, _lParam->iItem, _lParam->iSubItem, LVIR_LABEL, &r);


	this->hEditable = CreateWindowEx(0, WC_EDIT,
		table->GetCell(_lParam->iItem, _lParam->iSubItem).c_str(),
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
		r.left, r.top,
		r.right - r.left,
		r.bottom - r.top,
		table->hWndList,
		NULL, hInst, 0);

	SendMessage(this->hEditable, EM_LIMITTEXT, bufferSize, 0);		//	It accepts no more than bufferSize chars
	SendMessage(this->hEditable, EM_SETSEL, 0, bufferSize);			//	Text selected
	SetFocus(this->hEditable);										//	Focus to the new box

	//Replace the window procedure, new one is SubClass_ListView_WndProc
	wpOld = (WNDPROC)SetWindowLong(this->hEditable, GWL_WNDPROC, (LONG)CallEditBoxWndProc);

	//	Save some useful things...
	SetProp(this->hEditable, L"WP_OLD", (HANDLE)wpOld);
	SetProp(this->hEditable, L"ITEM", (HANDLE)_lParam->iItem);
	SetProp(this->hEditable, L"SUBITEM", (HANDLE)_lParam->iSubItem);
}


LRESULT CellEditor::EditBoxWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (LOWORD(wParam) == VK_RETURN)
		{
			//	When ENTER is pressed I have to store the inserted text in the subitem
			LV_ITEM LvItem;
			WCHAR text[257] = L"";

			LvItem.iItem = (int)GetProp(thisPtr->hEditable, L"ITEM");			//	Item changed
			LvItem.iSubItem = (int)GetProp(thisPtr->hEditable, L"SUBITEM");	//	SubItem changed
			LvItem.pszText = text;								//	Where to store the new text
			GetWindowText(thisPtr->hEditable, text, sizeof(text));		//	Get new text and set it in the subitem
			SendMessage(thisPtr->table->hWndList, LVM_SETITEMTEXT, (WPARAM)GetProp(hEditable, L"ITEM"), (LPARAM)&LvItem);
			DestroyWindow(hEditable);							//	Now I can destroy the edit box
		}
		else if (LOWORD(wParam) == VK_ESCAPE)
			DestroyWindow(thisPtr->hEditable);							//	Abort operation...
		break;
	case WM_DESTROY:
		RemoveProp(thisPtr->hEditable, L"WP_OLD");
		RemoveProp(thisPtr->hEditable, L"ITEM");
		RemoveProp(thisPtr->hEditable, L"SUBITEM");
		//	Restore the original window procedure
		SetWindowLong(thisPtr->hEditable, GWL_WNDPROC, (LONG)GetProp(thisPtr->hEditable, L"WP_OLD"));
		hEditable = NULL;
		break;
	default:
		//	Default messages are passed to the original window procedure
		return CallWindowProc((WNDPROC)GetProp(hEditable, L"WP_OLD"), hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

static LRESULT APIENTRY CallEditBoxWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (thisPtr) return thisPtr->EditBoxWndProc(hwnd, uMsg, wParam, lParam);
}

void CellEditor::OnDestroyWindow()
{
	if (this->hEditable) DestroyWindow(this->hEditable);
}
