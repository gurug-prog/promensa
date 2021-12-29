#pragma once
#include "framework.h"
#include "commdlg.h"


class DialogInvoker
{
public:
	static LPWSTR ProcessOpenDlg(HWND);
	static LPWSTR ProcessSaveAsDlg(HWND);

};
