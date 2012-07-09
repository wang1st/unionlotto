#include "stdafx.h"
#include "LottoData.h"
#include <ShellAPI.h>
#include <exdisp.h>
#include <comdef.h>
#include <GdiPlus.h>
#include "ControlEx.h"
#include "TwoColorBall.h"
#include "resource1.h"
#include "splashwnd.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}


// This is the only header we need to include for LuaBind to work
#include <luabind/luabind.hpp>

// Some other C++ headers we're most likely going to use
#include <iostream>
#include <string>

// We don't want to write std:: every time we're displaying some debug output
using namespace std;

// --------------------------------------------------------------------------------------------- //
namespace luabind  
{  
//std::vector<T> 与 table 转换  
using std::vector;  
template <class T>  
struct default_converter<vector<T> >  
	: native_converter_base<vector<T> >  
{  
	static int compute_score(lua_State* L, int index)  
	{  
		return (lua_type(L, index) == LUA_TTABLE )? 0 : -1;  
	}  

	vector<T>  from(lua_State* L, int index)  
	{  
		vector<T> container;   
		luabind::object tbl(from_stack(L, index));   

		for (luabind::iterator itr(tbl), end; itr != end; ++itr)   
		{   
			boost::optional<T> v = object_cast_nothrow<T>(*itr);   
			if (v){   
				container.push_back(*v);   
			}   
		}   

		return container;   
	}  

	void to(lua_State* L, vector<T>  const& container)  
	{  
		lua_createtable(L, container.size(), 0);   

		luabind::object tbl(from_stack(L, -1));   
		int n = 0;   

		for (vector<T>::const_iterator itr = container.begin(); itr != container.end(); ++itr)   
		{   
			tbl[++n] = *itr;   
		}  
	}  
};  

template <class T>  
struct default_converter<vector<T>  const&>  
	: default_converter<vector<T> >  
{};
}

class CLottoFrameWnd : public CWindowWnd, public INotifyUI
{
public:
	CLottoFrameWnd() { };
	LPCTSTR GetWindowClassName() const { return _T("UIMainFrame"); };
	UINT GetClassStyle() const { return CS_DBLCLKS; };
	void OnFinalMessage(HWND /*hWnd*/) { delete this; };

	void Init() {
		this->SetIcon(IDI_ICON1);
		m_pCloseBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("closebtn")));
		m_pMaxBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("maxbtn")));
		m_pRestoreBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("restorebtn")));
		m_pMinBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("minbtn")));
		m_pLssjBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("tab_draw")));
		CContainerUI* pControl = static_cast<CContainerUI*>(m_pm.FindControl(_T("mydrawarea2")));
		m_pStatusBar = static_cast<CLabelUI*>(m_pm.FindControl(_T("msginfo")));
		CLottoData* D = CLottoData::Instance();
		long user_code = D->GetUserCode();
		CHAR s[256];
		int expire_date = D->GetExpireDate();
		wsprintf(s, _T("机器码: %u  到期时间: %d年%d月%d日"), user_code, expire_date/10000, expire_date/100 % 100, expire_date % 100);
		m_pStatusBar->SetText(s);

		Gdiplus::Bitmap* bmp = D->GetHistBmp();
		pControl->SetFixedHeight(bmp->GetHeight());
	}

	void OnPrepare() {
	}

	void BtnLssjClicked(){
		CContainerUI* pControl = static_cast<CContainerUI*>(m_pm.FindControl(_T("mydrawarea2")));
		
		CContainerUI* pParent = static_cast<CContainerUI*>(pControl->GetParent());
		SIZE sz;
		sz.cx = 0; sz.cy=pControl->GetFixedHeight();
		pParent->SetScrollPos(sz);
		sz = pParent->GetScrollPos();

	}


	void Notify(TNotifyUI& msg) //按钮事件派发
	{
		if( msg.sType == _T("windowinit") ) OnPrepare();
		else if( msg.sType == _T("click") ) {
			if( msg.pSender == m_pCloseBtn ) {
				PostQuitMessage(0);
				return; 
			}
			else if( msg.pSender == m_pMinBtn ) { 
				SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); return; }
			else if( msg.pSender == m_pMaxBtn ) { 
				SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); return; }
			else if( msg.pSender == m_pRestoreBtn ) { 
				SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); return; }
			else if( msg.pSender == m_pLssjBtn) {
				//BtnLssjClicked();
			}
		}
		else if(msg.sType==_T("selectchanged"))
		{
			CStdString name = msg.pSender->GetName();
			CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("switch")));
			if(name==_T("tab_news"))
				pControl->SelectItem(0); //开奖信息
			else if(name==_T("tab_draw")){
				pControl->SelectItem(1); //历史走势
			}
			else if(name == _T("tab_man")){
				pControl->SelectItem(2); //软件说明 
			    CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(m_pm.FindControl(_T("view_richedit")));
				HANDLE m_hFileIn = ::CreateFile(_T("license.rtf"),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,NULL);
				if(m_hFileIn == INVALID_HANDLE_VALUE)
					return;	
				EDITSTREAM es;
				es.dwCookie = (DWORD_PTR)m_hFileIn;
				es.dwError = 0;
				es.pfnCallback = MyStreamInCallback;	
				pRichEdit->StreamIn(SF_RTF,es);
				CloseHandle(m_hFileIn);
			}
			else if(name == _T("tab_post")){
				CLottoData* D = CLottoData::Instance();
				long user_code = D->GetUserCode();
				CHAR s[256];
				wsprintf(s, _T("http://unionlotto.sinaapp.com/Post/index/user_code/%u"),user_code);
				ShellExecute(NULL, _T("open"),s, NULL, NULL, SW_SHOWNORMAL);
			}
			else if(name == _T("tab_reg")){
				CLottoData* D = CLottoData::Instance();
				long user_code = D->GetUserCode();
				CHAR s[256];
				wsprintf(s, _T("http://www.sharebank.com.cn/soft/softbuy.php?soid=44685&oraddcode=%u"),user_code);
				ShellExecute(NULL, _T("open"),s, NULL, NULL, SW_SHOWNORMAL);
			}
		}
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;
		::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		m_pm.Init(m_hWnd);
		CDialogBuilder builder;
		CDialogBuilderCallbackEx cb;
		CControlUI* pRoot = builder.Create(_T("skin.xml"), (UINT)0,  &cb, &m_pm);
		ASSERT(pRoot && "Failed to parse XML");
		m_pm.AttachDialog(pRoot);
		m_pm.AddNotifier(this);

		Init();
		return 0;
	}

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::PostQuitMessage(0L);

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if( ::IsIconic(*this) ) bHandled = FALSE;
		return (wParam == 0) ? TRUE : FALSE;
	}

	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(*this, &pt);

		RECT rcClient;
		::GetClientRect(*this, &rcClient);

		// 		if( !::IsZoomed(*this) ) {
		// 			RECT rcSizeBox = m_pm.GetSizeBox();
		// 			if( pt.y < rcClient.top + rcSizeBox.top ) {
		// 				if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
		// 				if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
		// 				return HTTOP;
		// 			}
		// 			else if( pt.y > rcClient.bottom - rcSizeBox.bottom ) {
		// 				if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
		// 				if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
		// 				return HTBOTTOM;
		// 			}
		// 			if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
		// 			if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
		// 		}

		RECT rcCaption = m_pm.GetCaptionRect();
		if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
				if( pControl && _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
					_tcscmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
					_tcscmp(pControl->GetClass(), _T("TextUI")) != 0 )
					return HTCAPTION;
		}

		return HTCLIENT;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SIZE szRoundCorner = m_pm.GetRoundCorner();
		if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
			CRect rcWnd;
			::GetWindowRect(*this, &rcWnd);
			rcWnd.Offset(-rcWnd.left, -rcWnd.top);
			rcWnd.right++; rcWnd.bottom++;
			HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
			::SetWindowRgn(*this, hRgn, TRUE);
			::DeleteObject(hRgn);
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		CRect rcWork = oMonitor.rcWork;
		rcWork.Offset(-rcWork.left, -rcWork.top);

		LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
		lpMMI->ptMaxPosition.x	= rcWork.left;
		lpMMI->ptMaxPosition.y	= rcWork.top;
		lpMMI->ptMaxSize.x		= rcWork.right;
		lpMMI->ptMaxSize.y		= rcWork.bottom;

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
		if( wParam == SC_CLOSE ) {
			::PostQuitMessage(0L);
			bHandled = TRUE;
			return 0;
		}
		BOOL bZoomed = ::IsZoomed(*this);
		LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		if( ::IsZoomed(*this) != bZoomed ) {
			if( !bZoomed ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
				if( pControl ) pControl->SetVisible(false);
				pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
				if( pControl ) pControl->SetVisible(true);
			}
			else {
				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
				if( pControl ) pControl->SetVisible(true);
				pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
				if( pControl ) pControl->SetVisible(false);
			}
		}
		return lRes;
	}

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch( uMsg ) {
		case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
		case WM_CLOSE:         lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
		case WM_DESTROY:       lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
		case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
		case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
		case WM_GETMINMAXINFO: lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
		case WM_SYSCOMMAND:    lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
		default:
			bHandled = FALSE;
		}
		if( bHandled ) return lRes;
		if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

public:
	CPaintManagerUI m_pm;

private:
	CButtonUI* m_pCloseBtn;
	CButtonUI* m_pMaxBtn;
	CButtonUI* m_pRestoreBtn;
	CButtonUI* m_pMinBtn;
	CButtonUI* m_pLssjBtn;
	CLabelUI*  m_pStatusBar;
	//...
};

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{

	WinExec("AUClient.exe /tray", SW_SHOWNORMAL);
	ULONG_PTR m_gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	CSplashWnd splash;
	splash.SetImage(Gdiplus::Image::FromFile(L"splash.jpg"));
	splash.Show();
	splash.SetProgressBarColor(RGB(192,192,192));
	splash.SetProgress(0, "程序启动...");
	lua_State *myLuaState = luaL_newstate();
	luaL_openlibs(myLuaState);

		// Connect LuaBind to this lua state
	luabind::open(myLuaState);


	// Define a lua function that we can call

	
/*	luabind::module(myLuaState) [
		luabind::class_<CLottoData>("LottoData")
			.def(luabind::constructor<>())
			.def("add_new_draw", &CLottoData::AddDraw),
		
		luabind::def("Singleton", &CLottoData::Instance)
	];
	*/
	CLottoData* D = CLottoData::Instance();

	 // (return_arg-type (classname::*)(arg1-type,grg2-type,...))&classname::class_memberfunction
	luabind::module(myLuaState) [
		luabind::class_<CLottoData>("LottoData")
			.def("add_new_draw", (void (CLottoData::*)(const std::vector<int>&))&CLottoData::AddDraw)
			.def("set_predition",(void (CLottoData::*)(const std::vector<int>&, const std::vector<int>&)) &CLottoData::SetPrediction)
			.def("get_user_code",(long (CLottoData::*)(void))&CLottoData::GetUserCode)
			.def("set_expire_date",(void(CLottoData::*)(int))&CLottoData::SetExpireDate),
			luabind::def("Draws", &CLottoData::Instance)
	];

	long user_code = D->GetUserCode();
	CHAR s[32];
	wsprintf(s,_T("%u"),user_code);

	splash.SetProgress(10, "正在验证用户注册信息...");
	int error = luaL_dofile(myLuaState, "validation.lua");
	if(error){
		_bstr_t bs1(lua_tostring(myLuaState, -1));
		MessageBox(NULL,bs1,_T("错误"),MB_OK);
		TRACE(_T("%s"), bs1);
	}

	splash.SetProgress(40, "正在同步往期开奖信息...");
	error = luaL_dofile(myLuaState, "syncdata.lua");
	if(error){
		_bstr_t bs1(lua_tostring(myLuaState, -1));
		MessageBox(NULL,bs1,_T("错误"),MB_OK);
		TRACE(_T("%s"), lua_tostring(myLuaState, -1));
	}

	splash.SetProgress(70, "正在读取往期开奖信息...");
	error = luaL_dofile(myLuaState, "loaddata.lua");
	if(error){
		_bstr_t bs1(lua_tostring(myLuaState, -1));
		MessageBox(NULL,bs1,_T("错误"),MB_OK);
		TRACE(_T("%s"), lua_tostring(myLuaState, -1));
	}

	splash.SetProgress(90, "正在计算下期开奖预测...");
	error = luaL_dofile(myLuaState, "fanntest.lua");
	if(error){
		_bstr_t bs1(lua_tostring(myLuaState, -1));
		MessageBox(NULL,bs1,_T("错误"),MB_OK);
		TRACE(_T("%s"), bs1);
	}

	D->CreateInfoBmp();
	D->CreateHistBmp();

	splash.SetProgress(100, "完成...");

	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin"));
//	CPaintManagerUI::SetResourceZip(_T("360SafeRes.zip"));

//	CPaintManagerUI::SetResourcePath(_T("D:\\lotto2C\\Skin"));

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;


	CLottoFrameWnd* pFrame = new CLottoFrameWnd();
	if( pFrame == NULL ) return 0;
	pFrame->Create(NULL, _T("双色球预测大师 2013"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 800, 572);
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(pFrame->GetHWND(), MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CRect rcWork = oMonitor.rcWork;
	rcWork.Offset(-rcWork.left, -rcWork.top);
	RECT rt;
	::GetWindowRect(pFrame->GetHWND(),&rt);
	::SetWindowPos(pFrame->GetHWND(),NULL,rt.left,0,rt.right-rt.left,rcWork.GetHeight()>722?722:rcWork.GetHeight(),0);

	pFrame->CenterWindow();
	splash.Hide();

	::ShowWindow(*pFrame, SW_SHOW);

	CPaintManagerUI::MessageLoop();

	::CoUninitialize();



	//int error = luaL_dostring(
	//	myLuaState,
	//	"io.input(\"shuangseqiu.txt\") \
	//	local s = io.read(\"*all\") \
	//	local D = Draws() \
	//	for d, r1, r2, r3, r4, r5, r6, b in string.gmatch(s, \"(%d+),(%d+),(%d+),(%d+),(%d+),(%d+),(%d+),(%d+)\") do \
 //       D:add_new_draw({d, r1, r2, r3, r4, r5, r6, b}) \
	//	end"
	//	);
	//luaL_dofile(myLuaState, "loaddata.lua");
	lua_close(myLuaState);
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
	return 0;
}


DWORD CALLBACK MyStreamInCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	HANDLE hFile = (HANDLE)dwCookie;
	BOOL bRet = !::ReadFile(hFile,pbBuff,cb,(DWORD *)pcb,NULL);	
	return bRet;
}
