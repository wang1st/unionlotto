#pragma once

class CMyContainer1UI : public CContainerUI
{
public:
	CMyContainer1UI()
	{
	}
	void DoPaint(HDC hDC, const RECT& rcPaint);
};

class CMyContainer2UI : public CContainerUI
{
public:
	CMyContainer2UI()
	{
	}
	void DoPaint(HDC hDC, const RECT& rcPaint);
};

class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CControlUI* CreateControl(LPCTSTR pstrClass) 
	{
		if( _tcscmp(pstrClass, _T("MyContainer1")) == 0 ) return new CMyContainer1UI;
		else if( _tcscmp(pstrClass, _T("MyContainer2")) == 0 ) return new CMyContainer2UI;
		return NULL;
	}
};