// main.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Promensa.h"
#include "TableView.h"
#include "DataProcessor.h"
#include "DialogInvoker.h"

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

HWND hEditable = NULL;	//	Editable box handle
unsigned uCurrentItem;
unsigned uCurrentSubItem;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

TableView* tv = nullptr;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);


	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PROMENSA, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROMENSA));

	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROMENSA));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PROMENSA);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	int nWidth = 800;
	int nHeight = 500;

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, nWidth, nHeight, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_LBUTTONDOWN:	/*	If mouse click occours outside the listview
							I abort the operation restoring the old text	*/
		if (hEditable != NULL)
			DestroyWindow(hEditable);
		break;
	case WM_NOTIFY:
	{
		LPNMHDR lpnmhdr = (LPNMHDR)lParam;
		if (lpnmhdr->idFrom == IDC_LISTVIEW &&
			lpnmhdr->code == LVN_COLUMNCLICK)
			tv->OnColumnClick(lParam);
		switch (((LPNMHDR)lParam)->code)
		{
		case NM_CLICK:		/*	If mouse click occours inside the listview but outside
								the edit box I abort the operation restoring the old text	*/
			if (hEditable != NULL)
				DestroyWindow(hEditable);
			break;
		case NM_DBLCLK:		//	I have to subclass!!!
			//SubClass_ListView_Editable(_hWndDlg, (NMLISTVIEW*)lParam);
		{
			RECT r;
			NMLISTVIEW* pLV = (NMLISTVIEW*)lParam;
			uCurrentItem = pLV->iItem;
			uCurrentSubItem = pLV->iSubItem;

			ListView_GetSubItemRect(tv->hWndList, uCurrentItem, uCurrentSubItem, LVIR_LABEL, &r);

			//Use the following conversion if you want the dialog to be the parent of the edit

			//POINT pt;
			//pt.x = r.left;
			//pt.y = r.top;
			//ClientToScreen(hListView, &pt);
			//ScreenToClient(_hWndDlg, &pt);

			hEditable = CreateWindowEx(0, WC_EDIT, L"Edit me", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT/*|ES_MULTILINE*/,
				r.left, r.top, r.right - r.left, r.bottom - r.top, tv->hWndList, NULL, hInst, 0);
			SendMessage(hEditable, EM_LIMITTEXT, 8, 0);		//	It accepts no more than 8 chars
			SendMessage(hEditable, EM_SETSEL, 0, 8);		//	Text selected
			SetFocus(hEditable);							//	Focus to the new box
			break;
		}
		}
		break;
	}
	case WM_CREATE:
		tv = new TableView(hWnd);
		break;
	case WM_SIZE:
		tv->OnSize(hWnd);
		break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case ID_QUIT:
			if (hEditable != NULL && GetFocus() == hEditable)
			{
				//	When ENTER is pressed I have to store the inserted text in the subitem
				LV_ITEM LvItem;
				WCHAR text[9] = L"";//Prepare your program for buffer overrun exploit here :) :)

				LvItem.iItem = uCurrentItem;			//	Item changed
				LvItem.iSubItem = uCurrentSubItem;	//	SubItem changed
				LvItem.pszText = text;								//	Where to store the new text
				GetWindowText(hEditable, text, sizeof(text));		//	Get new text and set it in the subitem
				SendMessage(tv->hWndList, LVM_SETITEMTEXT, uCurrentItem, (LPARAM)&LvItem);
				DestroyWindow(hEditable);//	Now I can destroy the edit box
			}
		case IDM_FILE_OPEN:
			if (tv) tv->OnFileOpen(hWnd);
			break;
		case IDM_FILE_SAVE:
			if (tv) tv->OnFileSave();
			break;
		case IDM_FILE_SAVEAS:
			if (tv) tv->OnFileSaveAs(hWnd);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
