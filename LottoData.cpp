#include "StdAfx.h"
#include "LottoData.h"
#include "myhelper.h"

std::auto_ptr<CLottoData> CLottoData::_instance;

CLottoData::CLottoData(void):_infobmp(0),_histbmp(0),user_code(0)
{

}


CLottoData::~CLottoData(void)
{
//if(bmp) delete bmp; //��	Gdiplus::GdiplusShutdown(m_gdiplusToken);֮��ִ�У����Բ���Ҫ�ˡ�
}

long CLottoData::GetUserCode()
{
	if(!user_code){
		HMODULE hDllLib = LoadLibrary(_T("myhelper.dll"));
		if(hDllLib){
			FN_GETHDCODE GetHdCode = (FN_GETHDCODE)GetProcAddress(hDllLib,"GetHdCode");
			if(GetHdCode){
				user_code = GetHdCode();
			}
			FreeLibrary(hDllLib);
		}
	}
	return user_code;
}

void CLottoData::CreateHistBmp()
{
	using namespace Gdiplus;
	if(_histbmp) delete _histbmp;
	int _size = draws.size();
	int _rows = _size; //100 > _size? _size : 100;
	const int _rowwidth = 11;
	const int _rowheight = _rowwidth;
	const int _drawwidth = 25;
	const int _ballcount = 49;
	const int _top = 5;
	const int _left = 5;
	const int _bottom = 5;
	const int _right = 5;
	const int _height = _top + _bottom + _rows * _rowheight ; 
	const int _width = _left + _right + _drawwidth + _ballcount * _rowwidth;

	_histbmp = new Bitmap(_width, _height + 406);//������˿ռ䣬�õײ�����գ������û��������ۿ�
	Graphics bmpGraphics(_histbmp);

	FontFamily fontFamily(L"MS Gothic");
	Font font(&fontFamily, 8, FontStyleRegular, UnitPixel);
	SolidBrush brushDraw(Color(255,0,0,0));
	SolidBrush brushBall(Color(255,255,255,255));

	for(int i=0; i<_rows; i++){
		CLottoDraw draw = draws[_size - _rows + i];	
		RectF rt;
		WCHAR s[32];
		rt.X = _left;
		rt.Y = _top + i * _rowheight;
		rt.Width = _drawwidth;
		rt.Height = _rowheight;
		wsprintfW(s, L"%05d", draw.draw);
		StringFormat stringFormat;
		// Center-justify each line of text.
		stringFormat.SetAlignment(StringAlignmentCenter);
		stringFormat.SetLineAlignment(StringAlignmentCenter);
		LinearGradientBrush linGrBrush(rt,Color(255,255,255,255),Color(255,192,192,192),60.0f,true);
		bmpGraphics.FillRectangle(&linGrBrush, rt);
		bmpGraphics.DrawString(s,-1,&font,rt,&stringFormat,&brushDraw);

		for(int j = 0; j < draw.balls.size(); j ++){ //���ݿ�����Ϣ�����
			if(j < 6){ //����
				rt.X = _left + _drawwidth + (draw.balls[j] - 1) * _rowwidth ;
				//rt.Y = rt.Y; //���øı�
				rt.Width = _rowwidth;
				rt.Height = _rowheight;
				wsprintfW(s, L"%d", draw.balls[j]);
				LinearGradientBrush linGrBrush(rt,Color(255,255,0,0),Color(255,160,0,0),45.0f,true);
				bmpGraphics.FillRectangle(&linGrBrush, rt);
				bmpGraphics.DrawString(s,-1,&font,rt,&stringFormat,&brushBall);
			}
			else{ //����
				rt.X = _left + _drawwidth + (draw.balls[j] + 33 - 1) * _rowwidth;
				//rt.Y = rt.Y; //���øı�
				rt.Width = _rowwidth;
				rt.Height = _rowheight;
				wsprintfW(s, L"%d", draw.balls[j]);
				LinearGradientBrush linGrBrush(rt,Color(255,0,0,255),Color(255,0,0,160),45.0f,true);
				bmpGraphics.FillRectangle(&linGrBrush, rt);
				bmpGraphics.DrawString(s,-1,&font,rt,&stringFormat,&brushBall);
			}

		}
	}
	Pen pen(Color(128, 192, 192, 192));
	Pen penStroke(Color(255,128,128,128));
	for(int i=0; i <= _size; i++){ //������
		bmpGraphics.DrawLine(&pen, _left, _top + i * _rowheight, _width - _right , _top + i * _rowheight );
	}
	bmpGraphics.DrawLine(&pen,_left,_top,_left,_height - _bottom);
	for(int i=0; i<=_ballcount; i++){ //������
	//	if(i==0 || i==11 || i==22 || i==33 || i == 41){
	//		bmpGraphics.DrawLine(&penStroke, _drawwidth + _left + i * _rowwidth, _top, _drawwidth + _left + i * _rowwidth, _height - _bottom);

	//	}
	//	else{
			bmpGraphics.DrawLine(&pen, _drawwidth + _left + i * _rowwidth, _top, _drawwidth + _left + i * _rowwidth, _height - _bottom);

	//	}
	}
	RectF rtFill;
	rtFill.X = _drawwidth + _left + 11 * _rowwidth;
	rtFill.Width = 11 * _rowwidth;
	rtFill.Y = _top;
	rtFill.Height = _size * _rowwidth;
	SolidBrush brushFill(Color(64,208,208,208));
	bmpGraphics.FillRectangle(&brushFill, rtFill);
	
	rtFill.X = _drawwidth + _left + 33 * _rowwidth;
	rtFill.Width = 8 * _rowwidth;
		bmpGraphics.FillRectangle(&brushFill, rtFill);


}

int CALLBACK EnumFontsProc(const LOGFONT *lplf,const TEXTMETRIC *lptm,DWORD dwType,LPARAM lpData)
{
	return 10;//�ҵ�����,������10��ԭ�������ֻ��1�Ļ�������Ҳ���Ҳ�᷵��1����10�Ͳ���
}


void CLottoData::CreateInfoBmp()
{
	if(draws.size() == 0 ) return;

	using namespace Gdiplus;
	if(_infobmp) delete _infobmp;
	const int _top = 5;
	const int _left = 5;
	const int _bottom = 5;
	const int _right = 5;
	const int _tbwidth = 564;
	const int _height = _top + _bottom + 1200;
	const int _width = _left + _right + _tbwidth;
	const int _rowheight = 20;
	const int _ballwidth = 30;
	const int _ballspace = 8;
	_infobmp = new Bitmap(_width, _height);
	Graphics bmpGraphics(_infobmp);

	HFONT   hFont   =   (HFONT)GetStockObject(SYSTEM_FONT); 
	LOGFONT logFont;
	GetObject(hFont, sizeof(LOGFONT), &logFont);
	std::string sFacename(logFont.lfFaceName);
	std::wstring wsFacename = s2ws(sFacename);

	BOOL fontOK = FALSE;
	HDC hdc = GetDC(0);
	std::string sFont("΢���ź�");
	if ( 10 == EnumFonts(hdc,sFont.c_str(),EnumFontsProc,0)){ //�ҵ�����,������10��ԭ�������ֻ��1�Ļ�������Ҳ���Ҳ�᷵��1����10�Ͳ���
		fontOK = TRUE;
	}
	else{
		sFont = "����";
	}
	std::wstring wsFont = s2ws(sFont);

//	FontFamily fontFamily(wsFacename.c_str());
	FontFamily fontFamily(wsFont.c_str());
	Font font(&fontFamily, 9, FontStyleRegular, UnitPoint);
	SolidBrush brushDraw(Color(255,32,32,96));
	SolidBrush brushBall(Color(255,255,255,255));

	Pen pen(Color(128, 192, 192, 192));
	Pen penStroke(Color(128,128,128,128));

	bmpGraphics.DrawLine(&pen, _left, _top , _left + _tbwidth, _top);
	bmpGraphics.DrawLine(&pen, _left, _top + _rowheight, _left + _tbwidth, _top + _rowheight);
	bmpGraphics.DrawLine(&pen, _left, _top + 4 * _rowheight, _left + _tbwidth, _top + 4 * _rowheight);
	bmpGraphics.DrawLine(&pen, _left, _top + 5 * _rowheight, _left + _tbwidth, _top +5 * _rowheight);
	bmpGraphics.DrawLine(&pen, _left, _top , _left, _top + 5 * _rowheight);
	bmpGraphics.DrawLine(&pen, _left + _tbwidth, _top , _left + _tbwidth, _top +5 * _rowheight);

	CLottoDraw draw = draws[draws.size() - 1];	
	RectF rt;
	WCHAR s[256];
	rt.X = _left;
	rt.Y = _top;
	rt.Width = _tbwidth;
	rt.Height = _rowheight;

	wsprintfW(s, L"˫ɫ��� %05d ��", draw.draw );
	StringFormat stringFormat;
	// Center-justify each line of text.
	stringFormat.SetAlignment(StringAlignmentNear);
	stringFormat.SetLineAlignment(StringAlignmentCenter);
	if(fontOK)
		bmpGraphics.SetTextRenderingHint(TextRenderingHintAntiAlias); //�������
	bmpGraphics.DrawString(s,-1,&font,rt,&stringFormat,&brushDraw);


	rt.Y = _top + 3 + 3 * _rowheight;
	wsprintfW(s, L"����˳�� %d %d %d %d %d %d", draw.positions[0], draw.positions[1], draw.positions[2], draw.positions[3], draw.positions[4], draw.positions[5]);
	bmpGraphics.DrawString(s,-1,&font,rt,&stringFormat,&brushDraw);

	rt.Y = _top + 4 * _rowheight;
	wsprintfW(s, L"�������� %d��Ԫ ���ع��� %d��Ԫ", draw.sales/10000, draw.pools/10000 );
	bmpGraphics.DrawString(s,-1,&font,rt,&stringFormat,&brushDraw);
	
	Font font2(&fontFamily, 16, FontStyleBold, UnitPixel);
	stringFormat.SetAlignment(StringAlignmentCenter);

	for(int i=0; i<draw.balls.size(); i++){
		RectF rt(_left + 3 + i * (_ballwidth + _ballspace), _top + 3 + 1 * _rowheight, _ballwidth, _ballwidth);
		LinearGradientBrush linGrBrush1(rt,Color(255,255,0,0),Color(255,128,0,0),45.0f,true);
		LinearGradientBrush linGrBrush2(rt,Color(255,64,64,255),Color(255,64,64,64),45.0f,true);
		bmpGraphics.SetSmoothingMode(SmoothingModeAntiAlias);
		if(i == 6) //����
			bmpGraphics.FillEllipse(&linGrBrush2, rt);
		else
			bmpGraphics.FillEllipse(&linGrBrush1, rt);
		wsprintfW(s, L"%d", draw.balls[i]);
	//	rt.Offset(-3, -3);
		bmpGraphics.DrawString(s,-1,&font2,rt,&stringFormat,&brushBall);

	}

	const int _bounsY = _top + 3 + 6 *  _rowheight;
	for(int i=0; i<9; i++){
		int y = _bounsY + i * _rowheight;
		bmpGraphics.DrawLine(&pen, _left, y , _left + _tbwidth, y);
	}
	bmpGraphics.DrawLine(&pen, _left, _bounsY, _left, _bounsY + 8 * _rowheight);
	bmpGraphics.DrawLine(&pen, _left + _tbwidth, _bounsY, _left + _tbwidth, _bounsY + 8 * _rowheight);

	const int _col1width = 100;
	const int _col2width = 160;
	const int _col3width = _tbwidth - _col1width - _col2width;

	bmpGraphics.DrawLine(&pen, _left + _col1width, _bounsY + _rowheight, _left + _col1width, _bounsY + 8 * _rowheight);
	bmpGraphics.DrawLine(&pen, _left + _col1width + _col2width, _bounsY + _rowheight, _left + _col1width + _col2width, _bounsY + 8 * _rowheight);

	rt.Y = _bounsY ;
	rt.Width = _tbwidth; 
	stringFormat.SetAlignment(StringAlignmentNear);
	wsprintfW(s, L"%s", L"������Ϣ");
	bmpGraphics.DrawString(s,-1,&font,rt,&stringFormat,&brushDraw );

	stringFormat.SetAlignment(StringAlignmentCenter);
	rt.Y = _bounsY + _rowheight;
	rt.Width = _col1width;
	wsprintfW(s, L"%s", L"����ȼ�");
	bmpGraphics.DrawString(s,-1,&font,rt,&stringFormat,&brushDraw );

	rt.X = _left + _col1width;
	rt.Width = _col2width;
	wsprintfW(s, L"%s", L"�н�ע��");
	bmpGraphics.DrawString(s,-1,&font,rt,&stringFormat,&brushDraw );

	rt.X = _left + _col1width + _col2width;
	rt.Width = _col3width;
	wsprintfW(s, L"%s", L"��ע����(Ԫ)");
	bmpGraphics.DrawString(s,-1,&font,rt,&stringFormat,&brushDraw );

	for(int i=0; i<draw.bouns.size(); i++){
		rt.Y = _bounsY + 3 + (2 + i) * _rowheight;

		rt.X = _left;
		rt.Width = _col1width;
		wsprintfW(s, L"%d", i+1);
		bmpGraphics.DrawString(s,-1,&font,rt,&stringFormat,&brushDraw );

		rt.X = _left + _col1width;
		rt.Width = _col2width;
		wsprintfW(s, L"%d", draw.bouns[i].winners);
		bmpGraphics.DrawString(s,-1,&font,rt,&stringFormat,&brushDraw );

		rt.X = _left + _col1width + _col2width;
		rt.Width = _col3width;
		wsprintfW(s, L"%d", draw.bouns[i].bouns);
		bmpGraphics.DrawString(s,-1,&font,rt,&stringFormat,&brushDraw );
	}

	const int _predY = _bounsY + 9 *  _rowheight;
	for(int i=0; i<8; i ++)
	{
		int y =  _predY + i * _rowheight;
		bmpGraphics.DrawLine(&pen, _left, y , _left+_tbwidth,y );

	}

	bmpGraphics.DrawLine(&pen, _left, _predY, _left, _predY + 7 * _rowheight);
	bmpGraphics.DrawLine(&pen, _left+_tbwidth, _predY, _left+_tbwidth, _predY + 7 * _rowheight);

	int _colwidth1 = 260;
	int _colwidth2 = 120;
	int _colwidth3 = 80;
	int _colwidth4 = _tbwidth - _colwidth1 - _colwidth2 - _colwidth3;

	bmpGraphics.DrawLine(&pen, _left + _colwidth1, _predY + _rowheight, _left + _colwidth1, _predY + 7 * _rowheight);
	bmpGraphics.DrawLine(&pen, _left + _colwidth1 + _colwidth2, _predY + _rowheight, _left + _colwidth1+ _colwidth2, _predY + 7 * _rowheight);
	bmpGraphics.DrawLine(&pen, _left + _colwidth1 + _colwidth2 + _colwidth3, _predY + _rowheight,_left + _colwidth1 + _colwidth2 + _colwidth3,_predY + 7 * _rowheight  );

	wsprintfW(s, L"����(��%d��)���Ԥ����", this_draw[0].draw);
	rt.X = _left;
	rt.Y = _predY;
	rt.Width = _tbwidth;
	rt.Height = _rowheight;
	stringFormat.SetAlignment(StringAlignmentNear);
	bmpGraphics.DrawString(s,-1,&font,rt,&stringFormat,&brushDraw );
	stringFormat.SetAlignment(StringAlignmentCenter);

	wsprintfW(s, L"%s", L"Ԥ�����");
	rt.Y = _predY + _rowheight;
	rt.Width = _colwidth1;
	bmpGraphics.DrawString(s,-1,&font,rt,&stringFormat,&brushDraw );

	wsprintfW(s, L"%s", L"�������");
	rt.X = _left + _colwidth1;
	rt.Width = _colwidth2;
	bmpGraphics.DrawString(s,-1,&font,rt,&stringFormat,&brushDraw );


	wsprintfW(s, L"%s", L"����ȼ�");
	rt.X = _left + _colwidth1 + _colwidth2;
	rt.Width = _colwidth3;
	bmpGraphics.DrawString(s,-1,&font,rt,&stringFormat,&brushDraw );


	wsprintfW(s, L"%s", L"��ע����(Ԫ)");
	rt.X = _left + _colwidth1 + _colwidth2 + _colwidth3;
	rt.Width = _colwidth4;
	bmpGraphics.DrawString(s,-1,&font,rt,&stringFormat,&brushDraw );

	const WCHAR* sn5[9] = {L"��",L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��"};
//	const wchar_t* sn5[9] = {L"A",L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I"};
	for(int i=0; i<this_draw.size(); i++)
	{
		rt.X = _left + 20;
		rt.Y = _predY + (2 + i) * _rowheight;
		rt.Width = 24 ; //_rowheight * 1.5;
		wsprintfW(s, L"%s",sn5[i]);
		bmpGraphics.DrawString(s, -1,&font,rt,&stringFormat,&brushDraw );
		rt.X = rt.X + rt.Width;
		for(int j=0; j<this_draw[i].balls.size(); j++)
		{
			const int _space = 2;
			rt.X = rt.X + _space;

			wsprintfW(s, L"%d", this_draw[i].balls[j]);
			bmpGraphics.DrawString(s, -1,&font,rt,&stringFormat,&brushDraw );
			rt.X = rt.X + rt.Width;
		}

		wsprintfW(s, L"%d + %d", this_draw[i].redshoot, this_draw[i].blueshot);
		rt.X = _left + _colwidth1;
		rt.Width = _colwidth2;
		bmpGraphics.DrawString(s,-1,&font,rt,&stringFormat,&brushDraw );		

		wsprintfW(s, L"%d", this_draw[i].level);
		rt.X = _left + _colwidth1 + _colwidth2;
		rt.Width = _colwidth3;
		bmpGraphics.DrawString(s,-1,&font,rt,&stringFormat,&brushDraw );		

		wsprintfW(s, L"%d", this_draw[i].bouns);
		rt.X = _left + _colwidth1 + _colwidth2 + _colwidth3;
		rt.Width = _colwidth4;
		bmpGraphics.DrawString(s,-1,&font,rt,&stringFormat,&brushDraw );		

	}

	const int _rowheight2 = 20;
	const int _nextY = _predY + 8 *  _rowheight;
	
	for(int i=0; i<8; i++){
		bmpGraphics.DrawLine(&pen, _left, _nextY + i * _rowheight2 , _left+_tbwidth,_nextY + i * _rowheight2);

	} 

	bmpGraphics.DrawLine(&pen, _left, _nextY, _left, _nextY + 7 * _rowheight2);
	bmpGraphics.DrawLine(&pen, _left+_tbwidth, _nextY, _left+_tbwidth, _nextY + 7 * _rowheight2);
	bmpGraphics.DrawLine(&pen, _left+_colwidth1, _nextY + _rowheight2, _left + _colwidth1, _nextY + 7 * _rowheight2);

	rt.X = _left;
	rt.Y = _nextY;
	rt.Height = _rowheight2;
	rt.Width = _tbwidth;
	//Font font3(&fontFamily, 13, FontStyleBold, UnitPixel);
	Font font3(&fontFamily, 9, FontStyleRegular, UnitPoint);
	wsprintfW(s, L"����(��%d��)���Ԥ��", this_draw[0].draw + 1);
	bmpGraphics.DrawString(s,-1,&font3,rt,&stringFormat,&brushDraw );
	//stringFormat.SetAlignment(StringAlignmentNear);
	//stringFormat.SetAlignment(StringAlignmentCenter);

	wsprintfW(s, L"%s", L"Ԥ���㷨");
	rt.Y = _nextY + _rowheight2;
	rt.Width = _colwidth1;
	bmpGraphics.DrawString(s,-1,&font3,rt,&stringFormat,&brushDraw );
	wsprintfW(s, L"%s", L"Ԥ�����");
	rt.X = _colwidth1;
	rt.Width = _tbwidth - _colwidth1;
	bmpGraphics.DrawString(s,-1,&font3,rt,&stringFormat,&brushDraw );

	const WCHAR* al[5] = {L"����", L"����",L"����",L"����", L"ȫ��"};
	rt.X = _left;
	rt.Width = _colwidth1;
	for(int i=0; i<5; i++){
		rt.Y = _nextY + (i + 2) * _rowheight2;

		wsprintfW(s, L"������ - fann - %s����", al[i]);
		bmpGraphics.DrawString(s,-1,&font,rt,&stringFormat,&brushDraw );
	}

	int startX = _left + _colwidth1 + 40;
	int startY = _nextY + _rowheight2;
	const int _ballwidth2 = 20;
	const int _ballspace2 = 4;
	for(int i=0; i<next_draw.size(); i++){
		startY = startY + _rowheight2;
		rt.X = startX;
		rt.Y = startY;
		rt.Width = _ballwidth2;
		rt.Height = _ballwidth2;
		wsprintfW(s, L"%s", sn5[i]);
		bmpGraphics.DrawString(s,-1,&font3,rt,&stringFormat,&brushDraw);



		for(int j=0; j<next_draw[i].size(); j++){
			RectF rt(startX + (j + 1) * (_ballwidth2 + _ballspace2 ), startY, _ballwidth2, _ballwidth2);
			LinearGradientBrush linGrBrush1(rt,Color(255,255,0,0),Color(255,128,0,0),45.0f,true);
			LinearGradientBrush linGrBrush2(rt,Color(255,64,64,255),Color(255,64,64,64),45.0f,true);
			bmpGraphics.SetSmoothingMode(SmoothingModeAntiAlias);
			if(j == 6) //����
				bmpGraphics.FillEllipse(&linGrBrush2, rt);
			else
				bmpGraphics.FillEllipse(&linGrBrush1, rt);
			wsprintfW(s, L"%d", next_draw[i][j]);
			//	rt.Offset(-3, -3);
			bmpGraphics.DrawString(s,-1,&font3,rt,&stringFormat,&brushBall);
		}

	}




}
void CLottoData::AddDraw(const std::vector<int>& v){

	CLottoDraw myDraw;
	myDraw.draw = v[0];
	myDraw.balls.push_back(v[1]);
	myDraw.balls.push_back(v[2]);
	myDraw.balls.push_back(v[3]);
	myDraw.balls.push_back(v[4]);
	myDraw.balls.push_back(v[5]);
	myDraw.balls.push_back(v[6]);
	myDraw.balls.push_back(v[7]);
	myDraw.positions.push_back(v[8]);
	myDraw.positions.push_back(v[9]);
	myDraw.positions.push_back(v[10]);
	myDraw.positions.push_back(v[11]);
	myDraw.positions.push_back(v[12]);
	myDraw.positions.push_back(v[13]);
	myDraw.sales = v[14];
	myDraw.pools = v[15];
	CLottoBouns myBouns;
	myBouns.winners = v[16];
	myBouns.bouns = v[17];
	myDraw.bouns.push_back(myBouns);
	myBouns.winners = v[18];
	myBouns.bouns = v[19];
	myDraw.bouns.push_back(myBouns);
	myBouns.winners = v[20];
	myBouns.bouns = v[21];
	myDraw.bouns.push_back(myBouns);
	myBouns.winners = v[22];
	myBouns.bouns = v[23];
	myDraw.bouns.push_back(myBouns);
	myBouns.winners = v[24];
	myBouns.bouns = v[25];
	myDraw.bouns.push_back(myBouns);
	myBouns.winners = v[26];
	myBouns.bouns = v[27];
	myDraw.bouns.push_back(myBouns);

	draws.push_back(myDraw);
}

void CLottoData::SetPrediction(const std::vector<int>& params, const std::vector<int>& v)
{
	// ��Ϊluabind���ݲ�������5�����ϵ�ʱ�򣬻�������⡣����������params���ݲ��ֲ���
	if(params.size() == 0){
		next_draw.push_back(v);
	}
	else
	{
		CLottoPrediction predition;
		predition.draw = params[0];
		predition.balls = v;
		predition.redshoot = params[1];
		predition.blueshot = params[2];
		predition.level = params[3];
		predition.bouns = params[4];
		this_draw.push_back(predition);
	}
}