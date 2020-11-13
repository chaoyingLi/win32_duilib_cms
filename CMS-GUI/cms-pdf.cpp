#include "cms-pdf.h"
#include "CMS-utils.h"

void String2text(PDF *p, int &TextFlow, CString &mPagenameAndDate, int fontsize)
{
	char characteristic_cn[100];
	int mPagenameAndDateSize = mPagenameAndDate.GetLength();//字符串长度
	PDF_set_parameter(p, "FontOutline", "a=../Resource/MSYH.TTF");//这里的“./font/STXINGKA.TTF”为字体文件的存放路径，也就是说需要将字库文件放到相应的位置，不然会造成错误
	sprintf_s(characteristic_cn, "fontname=a:0 fontsize = %d encoding = unicode textlen = %d", fontsize, mPagenameAndDateSize * 2);//中文字符
	PDF_add_textflow(p, TextFlow, (char *)(mPagenameAndDate).AllocSysString(), mPagenameAndDateSize * 2, characteristic_cn);//添加字符段内容
}

void OutputStringToPDF(PDF *p, CString &mPagenameAndDate, int Left, int &Top, int Right, int Reduction, int fontsize)//数据输出
{
	//该函数输出数据到PDF中，需要注意的是在输出到页未的时候会自行进行换页
	if (Top<130)
	{
		Top = 740;
		PDF_end_page_ext(p, ""); // 关闭页面并开启新的页面
		PDF_begin_page_ext(p, a4_width, a4_height, "");
	}
	int TextFlow = PDF_create_textflow(p, "", 0, "");//创建字符段
	String2text(p, TextFlow, mPagenameAndDate, fontsize);
	while (true)
	{
		const char *returnstring = PDF_fit_textflow(p, TextFlow, Left, Top, Right, Top - Reduction, "");
		Top -= Reduction;
		if (strcmp(returnstring, "_boxfull") != 0)//对象满了
		{
			break;
		}
		if (Top<130)
		{
			Top = 740;
			PDF_end_page_ext(p, ""); // 关闭页面并开启新的页面
			PDF_begin_page_ext(p, a4_width, a4_height, "");
		}
	}
	PDF_delete_textflow(p, TextFlow);//释放字符串对象
}

bool CreatePDF(CString Content)
{
	PDF *p = NULL;
	int Left = 85, Top = 740, Right = a4_width - 85;
	//pdf对象创建
	if ((p = PDF_new()) == (PDF *)0)
	{
#ifdef CMS_DEBUG
		::MessageBox(NULL, L"Couldn't create PDFlib object (out of memory)!n", L"PDF error", MB_OK);

#endif // CMS_DEBUG

		return false;
	}
	PDF_TRY(p)
	{
		//先创建PDF文档，名为outout.pdf
		if (PDF_begin_document(p, PDF_FILE_PATH, 0, "") == -1)
		{
#ifdef CMS_DEBUG
			CString ws;
			std::string str = PDF_get_errmsg(p);
			ws.Format(L"Error: %sn", s2ws(str).c_str());
			//printf("Error: %sn", PDF_get_errmsg(p));
			::MessageBox(NULL, ws, L"PDF error", MB_OK);
#endif // CMS_DEBUG
			return false;
		}
		//再创建PDF文档里面的页面，并填入页面的内容，这里本人将其封装成了两个函数
		PDF_begin_page_ext(p, a4_width, a4_height, "");
		//CString mPagenameAndDate = L"君不见黄河之水天上来  奔流到海不复回\n";
		//mPagenameAndDate += L"君不见高堂明镜悲白发  朝如青丝暮成雪\n";
		//mPagenameAndDate += L"人生得意须尽欢  莫使金樽空对月\n";
		//mPagenameAndDate += L"天生我材必有用  千金散尽还复来\n";
		//mPagenameAndDate += L"烹羊宰牛且为乐  会须一饮三百杯\n";
		//mPagenameAndDate += L"岑夫子  丹丘生  将进酒  杯莫停\n";
		//mPagenameAndDate += L"与君歌一曲  请君为我倾耳听\n";
		//mPagenameAndDate += L"钟鼓馔玉不足贵  但愿长醉不复醒\n";
		//mPagenameAndDate += L"古来圣贤皆寂寞  惟有饮者留其名\n";
		//mPagenameAndDate += L"陈王昔时宴平乐  斗酒十千恣欢谑\n";
		//mPagenameAndDate += L"主人何为言少钱  径须沽取对君酌\n";
		//mPagenameAndDate += L"五花马  千金裘\n";
		//mPagenameAndDate += L"呼儿将出换美酒  与尔同销万古愁\n";
		//将文字输出到PDF，第三个参数为输出的左边距，第四个参数为输出的高度，第五个参数为输出的右边距，第六个参数为行距，第七个则是字体的大小
		OutputStringToPDF(p, Content, Left, Top, Right, 45, 24);
		//页结束
		PDF_end_page_ext(p, "");
		//文档结束
		PDF_end_document(p, "");
	}
	PDF_CATCH(p)//错误信息输出
	{
#ifdef CMS_DEBUG
		//printf("[%d] %s: %sn", PDF_get_errnum(p), PDF_get_apiname(p), PDF_get_errmsg(p));
		CString ws;
		ws.Format(L"[%d] %s: %sn", PDF_get_errnum(p), PDF_get_apiname(p), PDF_get_errmsg(p));
		//printf("Error: %sn", PDF_get_errmsg(p));
		::MessageBox(NULL, ws, L"PDF error", MB_OK);
		PDF_delete(p);
#endif // CMS_DEBUG
		return false;
	}
	PDF_delete(p);
	//打开pdf
	system(PDF_FILE_PATH);
	return true;
}



