#pragma once

#include<vector>
#include <GdiPlus.h>

class CLottoBouns
{

public:
	int winners;
	int bouns;
};

class CLottoDraw
{

public:
	int draw;
	std::vector<int> balls;
	std::vector<int> positions;
	int sales;
	int pools;
	std::vector<CLottoBouns> bouns;
};

class CLottoPrediction
{
public:
	int draw;
	std::vector<int> balls;
	int redshoot;
	int blueshot;
	int level;
	int bouns;
};

class CLottoData
{
public:
	static CLottoData* Instance(){
		if( 0 == _instance.get())
		{
			_instance.reset(new CLottoData);
		}
		return _instance.get();
	}
	void AddDraw(const std::vector<int>& v);
	void SetPrediction(const std::vector<int>& params, const std::vector<int>& v);
	std::vector<CLottoDraw>& GetDraws(){ return draws; }
	void CreateHistBmp();
	Gdiplus::Bitmap* GetHistBmp() {return _histbmp;}
	void CreateInfoBmp();
	Gdiplus::Bitmap* GetInfoBmp() {return _infobmp;}
	long GetUserCode();
	void SetExpireDate(int d){expire_date = d;} //设置过期日期，由lua脚本调用
	int GetExpireDate(){return expire_date;}
protected:
	friend class std::auto_ptr<CLottoData>;
	static std::auto_ptr<CLottoData> _instance;

	std::vector<CLottoDraw> draws;
	std::vector<std::vector<int> > next_draw;
	std::vector<CLottoPrediction> this_draw;

	long user_code;
	int expire_date;

	Gdiplus::Bitmap* _histbmp; //走势图像
	Gdiplus::Bitmap* _infobmp; //信息图像
	CLottoData(void);
	~CLottoData(void);
};



typedef long (CALLBACK* FN_GETHDCODE) ();
int CALLBACK EnumFontsProc(const LOGFONT *lplf,const TEXTMETRIC *lptm,DWORD dwType,LPARAM lpData);