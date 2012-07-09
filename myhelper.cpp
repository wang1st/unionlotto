#include "myhelper.h"
#include <comutil.h>

std::string ws2s(const std::wstring& ws)
{
_bstr_t t = ws.c_str();
char* pchar = (char*)t;
std::string result = pchar;
return result;
}
std::wstring s2ws(const std::string& s)
{
_bstr_t t = s.c_str();
wchar_t* pwchar = (wchar_t*)t;
std::wstring result = pwchar;
return result;
}