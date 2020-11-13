#include "CMS-utils.h"

bool InitCDH(CMS_DB_HEADER * cdh)
{
		if (cdh == NULL)
		{
#ifdef CMS_DEBUG
			std::wcerr << L"InitCDH: cdh is NULL?" << std::endl;
#endif // CMS_DEBUG
			return false;
		}
		cdh->n = 0;
		cdh->next = NULL;
		return true;
}

bool DeleteCDH(CMS_DB_HEADER * cdh)
{
	if (cdh == NULL || cdh->n == 0 || cdh->next == NULL)
	{
#ifdef CMS_DEBUG
		std::wcerr << L"DeleteCDH: cdh is null or the chain is empty?" << std::endl;
#endif // CMS_DEBUG
		return false;
	}

	struct CMS_DB_NODE* pNext = cdh->next;
	while (pNext != NULL)
	{
		cdh->next = pNext->next;
		delete pNext;
		pNext = cdh->next;
		--cdh->n;
	}

	if (cdh->n != 0)
	{
#ifdef CMS_DEBUG
		std::wcerr << L"DeleteCDH: cdh->n != 0." << std::endl;
#endif // CMS_DEBUG
		return false;
	}

	delete cdh;
	cdh = NULL;

	return true;
}

bool AddToCDH(CMS_DB_HEADER * cdh, void * content, size_t ContentLen)
{
	if (cdh == NULL)
	{
#ifdef CMS_DEBUG
		std::wcerr << L"AddToCDH: cdh is NULL?" << std::endl;
#endif // CMS_DEBUG
		return false;
	}
	else if (content == NULL)
	{
#ifdef CMS_DEBUG
		std::wcerr << L"AddToCDH: content is NULL?" << std::endl;
#endif // CMS_DEBUG
		return false;
	}

	struct CMS_DB_NODE* pNew = new struct CMS_DB_NODE;
	memcpy_s(pNew->content, CONTENT_LEN, content, ContentLen);

	//头插法将结点加入链表
	pNew->next = cdh->next;
	cdh->next = pNew;
	++cdh->n;

	return false;
}

std::string WChar2Ansi(LPCWSTR pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);

	if (nLen <= 0) return std::string("");

	char* pszDst = new char[nLen];
	if (NULL == pszDst) return std::string("");

	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen - 1] = 0;

	std::string strTemp(pszDst);
	delete[] pszDst;

	return strTemp;
}

std::wstring Ansi2WChar(LPCSTR pszSrc, int nLen)

{
	int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);
	if (nSize <= 0) return NULL;

	WCHAR *pwszDst = new WCHAR[nSize + 1];
	if (NULL == pwszDst) return NULL;

	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, pwszDst, nSize);
	pwszDst[nSize] = 0;

	if (pwszDst[0] == 0xFEFF) // skip Oxfeff  
		for (int i = 0; i < nSize; i++)
			pwszDst[i] = pwszDst[i + 1];

	std::wstring wcharString(pwszDst);
	delete pwszDst;

	return wcharString;
}

std::string ws2s(std::wstring& inputws) { return WChar2Ansi(inputws.c_str()); }
std::wstring s2ws(const std::string& s) { return Ansi2WChar(s.c_str(), s.size()); }

std::string ConvertMoneyCaps(long double moneySum)
{
	long int temp_i = (long int)moneySum;  /**//* 整数部分 */
	float temp_f = moneySum - temp_i; /**//* 小数部分 */
	int digit = 0, i, j, k, num_i;
	std::string money("");
	char num[20], *p;
	char name[][3] = { "元","拾","佰","仟","万","亿" };
	char numchar[][3] = { "零","壹","贰","叁","肆","伍","陆","柒","捌","玖" };
	ltoa(temp_i, num, 10);  /**//* 整数部分转换成字符串后在处理 */
	p = num;
	digit = strlen(num);            /**//* 整数部分位数 */
										/**//*--------处理整数部分 start--------*/
	for (i = 1; i <= digit; i++)
	{
		k = (digit - i) % 4;
		if (isdigit(*p))
		{
			num_i = *p & 0xF; /* 把字符转换成数字,比如 '0'-> 0,'1' -> 1*/
							  /**//*--------转换数字开始---------*/
			if (num_i)
			{
				money = money + numchar[num_i];
			}
			else
			{
				if (k && (*(p + 1) & 0xF))
					money += "零";
			}
			/**//*--------转换数字结束-------*/
				/**//*---------添加计数单位开始----*/
			if (k)
			{
				if (num_i)
					money = money + name[k];
			}
			else
			{
				j = digit - i;
				if (j)
					money = money + name[j / 4 + 3];
				else
					money += "元";
			}
			/**//*--------添加计数单位结束--------*/
			p++;
		}
		else
		{
			money = "遇到非数字退出!";
			return money;
		}
	}
	/**//*--------处理整数部分 End --------*/
		/**//*--------处理小数部分 start--------*/
	if (temp_f > 0.01)
	{
		if ((int)(temp_f * 10)) money = money + numchar[(int)(temp_f * 10)] + "角";
		if ((int)(temp_f * 100) % 10) money = money + numchar[(int)(temp_f * 100) % 10] + "分";
	}
	/**//*--------处理小数部分 End--------*/
	money += "整";

	return money;
}
