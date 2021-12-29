#include "ToolbarProcessor.h"
#include "Promensa.h"


ToolBarProcessor::ToolBarProcessor()
{
	hWndToolBar = NULL;
}

void ToolBarProcessor::OnCreate(HWND hWndParent)
{
	auto hInst = (HINSTANCE)GetWindowLong(hWndParent, GWL_HINSTANCE);
	TBBUTTON tbb[3];

	ZeroMemory(tbb, sizeof(tbb));
	tbb[0].iBitmap = STD_FILEOPEN;
	tbb[0].fsState = TBSTATE_ENABLED;
	tbb[0].fsStyle = TBSTYLE_BUTTON;
	tbb[0].idCommand = ID_TOOL_OPEN;

	tbb[1].iBitmap = STD_FILESAVE;
	tbb[1].fsState = TBSTATE_ENABLED;
	tbb[1].fsStyle = TBSTYLE_BUTTON;
	tbb[1].idCommand = ID_TOOL_SAVE;

	tbb[2].iBitmap = STD_COPY;
	tbb[2].fsState = TBSTATE_ENABLED;
	tbb[2].fsStyle = TBSTYLE_BUTTON;
	tbb[2].idCommand = ID_TOOL_SAVEAS;


	SendMessage(hWndToolBar, TB_ADDBUTTONS, 3, (LPARAM)&tbb);

	hWndToolBar = CreateToolbarEx(hWndParent,
		WS_CHILD | WS_VISIBLE | WS_BORDER
		| WS_CLIPSIBLINGS | CCS_TOP,
		IDC_TOOLBAR,
		3, HINST_COMMCTRL, IDB_STD_LARGE_COLOR,
		tbb,
		3,
		24, 24, 24, 24,
		sizeof(TBBUTTON));
}

void ToolBarProcessor::OnSize(HWND hWnd)
{
	RECT rc, rw;
	if (hWndToolBar)
	{
		GetClientRect(hWnd, &rc);
		GetWindowRect(hWndToolBar, &rw);
		MoveWindow(hWndToolBar, 0, 0, rc.right - rc.left, rw.bottom - rw.top, FALSE);
	}
}

void ToolBarProcessor::OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	auto pnmh = (LPNMHDR)lParam;
	if (pnmh->code == TTN_NEEDTEXT)
	{
		auto lpttt = (LPTOOLTIPTEXT)lParam;
		switch (lpttt->hdr.idFrom)
		{
		case ID_TOOL_OPEN:
			lstrcpy(lpttt->szText, L"Відкрити");
			break;
		case ID_TOOL_SAVE:
			lstrcpy(lpttt->szText, L"Зберегти");
			break;
		case ID_TOOL_SAVEAS:
			lstrcpy(lpttt->szText, L"Зберегти як");
			break;
		default: lstrcpy(lpttt->szText, L"Щось невідоме");
		}
	}
}
