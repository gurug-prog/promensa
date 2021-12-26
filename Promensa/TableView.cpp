#include "TableView.h"
#include "Promensa.h"


TableView::TableView()
{
    hWndList = NULL;
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
}

TableView::~TableView()
{
}


