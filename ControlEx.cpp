#include "StdAfx.h"
#include "controlex.h"
#include <GdiPlus.h>
#include <vector>
#include "LottoData.h"


void CMyContainer2UI::DoPaint(HDC hDC, const RECT& rcPaint)
{

	CContainerUI::DoPaint(hDC, rcPaint);
	using namespace Gdiplus;

	CLottoData* D = CLottoData::Instance();

	Bitmap* bmp = D->GetHistBmp();
	/*Drawing on DC*/
		Graphics graphics(hDC);
	/*Important! Create a CacheBitmap object for quick drawing*/
//		CachedBitmap cachedBmp(bmp,&graphics);


	//	int _offset = this->GetY() - GetParent()->GetY();
	//graphics.DrawCachedBitmap(&cachedBmp,0,this->GetHeight());
	//	graphics.DrawImage(bmp,0,0 - static_cast<CContainerUI*>(GetParent())->GetScrollPos().cy + 125 );//当有tab页的时候，需要加125的偏移量
	graphics.DrawImage(bmp,0,0 - static_cast<CContainerUI*>(GetParent())->GetScrollPos().cy + 88);

}

void CMyContainer1UI::DoPaint(HDC hDC, const RECT& rcPaint)
{

	CContainerUI::DoPaint(hDC, rcPaint);
	using namespace Gdiplus;

	CLottoData* D = CLottoData::Instance();

	Bitmap* bmp = D->GetInfoBmp();
	///*Drawing on DC*/
	Graphics graphics(hDC);
	///*Important! Create a CacheBitmap object for quick drawing*/
	//		CachedBitmap cachedBmp(bmp,&graphics);


	////	int _offset = this->GetY() - GetParent()->GetY();
	////graphics.DrawCachedBitmap(&cachedBmp,0,this->GetHeight());
	//graphics.DrawImage(bmp,0,0 - static_cast<CContainerUI*>(GetParent())->GetScrollPos().cy + 125 ); //当有tab页的时候，需要加125的偏移量


	graphics.DrawImage(bmp,0,0 - static_cast<CContainerUI*>(GetParent())->GetScrollPos().cy  + 88); 

}