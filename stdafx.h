// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


#include <objbase.h>
#include <zmouse.h>

#include <UIlib.h>

#pragma comment(lib, "lua5.1.lib")
#ifdef _DEBUG
#pragma comment(lib, "luabindd.lib")
#else
#pragma comment(lib, "luabind.lib")
#endif
#pragma comment(lib, "gdiplus.lib")

using namespace DuiLib;

#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_ud.lib")
#   else
#       pragma comment(lib, "DuiLib_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_u.lib")
#   else
#       pragma comment(lib, "DuiLib.lib")
#   endif
#endif

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#pragma comment(lib, "comsuppw.lib") 
