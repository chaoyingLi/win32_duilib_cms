#include "CMS-db.h"

DBgo::DBgo()
{
	//数据库操作三剑客
	henv = SQL_NULL_HENV;
	hdbc1 = SQL_NULL_HDBC;
	hstmt1 = SQL_NULL_HSTMT;
	//登录验证
	dbDSN = L"SQL Server";
	dbUID = L"sa";
	dbPWD = L"hitimc@wh";
	//dbSemicolon = L";";
	//连接数据库
	isConnected = this->ConnectDB();

}

DBgo::~DBgo()
{
	//4.断开数据源
	/*
	1.断开与数据源的连接.
	2.释放连接句柄.
	3.释放环境句柄 (如果不再需要在这个环境中作更多连接)
	*/
	SQLDisconnect(hdbc1);
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc1);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);

}

bool DBgo::QueryPlan(wchar_t* QueryStr, int QP_Method, struct CMS_DB_HEADER* cdh)
{
	if (isConnected == false)
		return false;
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1);

	if (QP_Method == QP_BY_PLANNAME)
	{
		dbSQL = QUERY_CALLPLAN_BY_PLANNAME;
		dbSQL = dbSQL + QueryStr;
		dbSQL = dbSQL + L"%';";
	}
	else if (QP_Method == QP_BY_PLANNO)
	{
		dbSQL = QUERY_CALLPLAN_BY_PLANNO;
		dbSQL = dbSQL + QueryStr;
		dbSQL = dbSQL + L" ;";
	}

	SQLExecDirectW(hstmt1, (SQLWCHAR*)dbSQL.c_str(), dbSQL.length());

	struct CallPlan* cp = new struct CallPlan;
	memset(cp->PlanName, 0, PLANNAME_LEN);

	SQLBindCol(hstmt1, 1, SQL_C_LONG, &cp->PlanNo, 0, 0);
	SQLBindCol(hstmt1, 2, SQL_C_WCHAR, cp->PlanName, PLANNAME_LEN, 0);
	SQLBindCol(hstmt1, 3, SQL_C_DOUBLE, &cp->RemoteFee, 0, 0);
	SQLBindCol(hstmt1, 4, SQL_C_DOUBLE, &cp->LocalFee, 0, 0);
	SQLBindCol(hstmt1, 5, SQL_C_DOUBLE, &cp->RoamingFee, 0, 0);
	
	do {
		retcode = SQLFetch(hstmt1);
		if (retcode == SQL_ERROR)
		{
#ifdef CMS_DEBUG
			//std::wcout << L"SQL错误" << std::endl;
			//
			//std::wcout << dbSQL << std::endl;
#endif
			break;
		}
		else if (retcode == SQL_NO_DATA)
		{
#ifdef CMS_DEBUG
			//std::wcout << L"查询结束" << std::endl;
#endif
			break;
		}
		AddToCDH(cdh, cp, sizeof(struct CallPlan));
	} while (true);

	if (cp)
	{
		delete cp;
		cp = NULL;
	}
	SQLCloseCursor(hstmt1);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);
	return true;
}

//后续：支持按月查询
bool DBgo::QueryCDR(wchar_t * CallerMSISDN, struct CMS_DB_HEADER* cdh)
{
	if (isConnected == false)
		return false;
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1);


	dbSQL = QUERY_CDR_BY_CALLER;
	dbSQL = dbSQL + CallerMSISDN;
	dbSQL = dbSQL + L";";
	SQLExecDirectW(hstmt1, (SQLWCHAR*)dbSQL.c_str(), dbSQL.length());

	struct CDR* cdr = new struct CDR;
	//memset(cdr->CallTime, 0, TIME_LEN);
	//memset(cdr->caller, 0, MSISDN_LEN);
	//memset(cdr->called, 0, MSISDN_LEN);
	//memset(cdr->type, 0, TYPE_LEN);
	memset(cdr, 0, sizeof(struct CDR));

	SQLBindCol(hstmt1, 1, SQL_C_WCHAR, cdr->CallTime, TIME_LEN, 0);
	SQLBindCol(hstmt1, 2, SQL_C_WCHAR, cdr->caller, MSISDN_LEN, 0);
	SQLBindCol(hstmt1, 3, SQL_C_WCHAR, cdr->called, MSISDN_LEN, 0);
	SQLBindCol(hstmt1, 4, SQL_C_WCHAR, cdr->type, TYPE_LEN, 0);
	SQLBindCol(hstmt1, 5, SQL_C_LONG, &cdr->duration, 0, 0);
	
	do {
		retcode = SQLFetch(hstmt1);
		if (retcode == SQL_ERROR)
		{
#ifdef CMS_DEBUG
			std::wcout << L"SQL语句错误" << std::endl;
			std::wcout << dbSQL << std::endl;
#endif
			break;
		}
		else if (retcode == SQL_NO_DATA)
		{
#ifdef CMS_DEBUG
			std::wcout << L"查询结束" << std::endl;
#endif
			break;
		}
		//std::wcout << cdr->CallTime << L"\t" << cdr->caller << L"\t";
		//std::wcout << cdr->called << L"\t" << cdr->type << L"\t";
		//std::wcout << cdr->duration << std::endl;
		AddToCDH(cdh, cdr, sizeof(struct CDR));
	} while (true);

	if (cdr)
	{
		delete cdr;
		cdr = NULL;
	}
	SQLCloseCursor(hstmt1);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);

	return true;
}

bool DBgo::QueryCharge(wchar_t * MSISDN, struct CMS_DB_HEADER* cdh)
{
	if (isConnected == false)
		return false;
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1);

	dbSQL = QUERY_CHARGE_BY_MSISDN;
	dbSQL = dbSQL + MSISDN;
	dbSQL = dbSQL + L";";
	SQLExecDirectW(hstmt1, (SQLWCHAR*)dbSQL.c_str(), dbSQL.length());

	struct Charge* c = new struct Charge;
	//memset(c->PayTime, 0, TIME_LEN);
	//memset(c->MSISDN, 0, MSISDN_LEN);
	//memset(c->PayMethod, 0, TYPE_LEN);
	memset(c, 0, sizeof(struct Charge));

	SQLBindCol(hstmt1, 1, SQL_C_WCHAR, c->PayTime, TIME_LEN, 0);
	SQLBindCol(hstmt1, 2, SQL_C_WCHAR, c->MSISDN, MSISDN_LEN, 0);
	SQLBindCol(hstmt1, 3, SQL_C_DOUBLE, &c->PayAmount, 0, 0);
	SQLBindCol(hstmt1, 4, SQL_C_WCHAR, c->PayMethod, TYPE_LEN, 0);
	SQLBindCol(hstmt1, 5, SQL_C_LONG, &c->OperatorNo, 0, 0);

	do {
		retcode = SQLFetch(hstmt1);
		if (retcode == SQL_ERROR)
		{
#ifdef CMS_DEBUG
			std::wcout << L"SQL语句错误" << std::endl;
			std::wcout << dbSQL << std::endl;
#endif
			break;
		}
		else if (retcode == SQL_NO_DATA)
		{
#ifdef CMS_DEBUG
			std::wcout << L"查询结束" << std::endl;
#endif
			break;
		}
		//std::wcout << c->PayTime << L"\t" << c->MSISDN << L"\t";
		//std::wcout << c->PayAmount << L"\t" << c->PayMethod << L"\t";
		//std::wcout << c->OperatorNo << std::endl;
		AddToCDH(cdh, c, sizeof(struct Charge));
	} while (true);

	if (c)
	{
		delete c;
		c = NULL;
	}
	SQLCloseCursor(hstmt1);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);

	return true;
}

bool DBgo::QueryOperator(LPCWSTR QueryStr, int QO_Method, struct CMS_DB_HEADER* cdh)
{
	if (isConnected == false)
		return false;
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1);

	CString cstr;
	//根据查询方式 赋予相应SQL语句
	if (QO_Method == QO_BY_OPERATORNO)
	{
		cstr.Format(L"%s%s ;", QUERY_OPERATOR_BY_NO, QueryStr);
	}
	else if (QO_Method == QO_BY_OPERATORNAME)
	{
		cstr.Format(L"%s%s", QUERY_OPERATOR_BY_NAME, QueryStr);
		cstr += L"%' ;";
	}
	else
	{
			SQLCloseCursor(hstmt1);
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);
			return false;
	}

	SQLExecDirectW(hstmt1, (SQLWCHAR*)cstr.GetBuffer(cstr.GetLength()), cstr.GetLength());

	struct Operator* op = new struct Operator;

	memset(op, 0, sizeof(struct Operator));

	SQLBindCol(hstmt1, 1, SQL_C_LONG, &op->OperatorNo, 0, 0);
	SQLBindCol(hstmt1, 2, SQL_C_WCHAR, op->OperatorName, NAME_LEN, 0);
	SQLBindCol(hstmt1, 3, SQL_C_WCHAR, op->OperatorGender, GENDER_LEN, 0);
	SQLBindCol(hstmt1, 4, SQL_C_LONG, &op->Class, 0, 0);
	SQLBindCol(hstmt1, 5, SQL_C_WCHAR, op->password, PWD_LEN, 0);
	//SQLBindCol(hstmt1, 6, SQL_C_LONG, &op->LoginStatus, 0, 0);

	do {
		retcode = SQLFetch(hstmt1);
		if (retcode == SQL_ERROR)
		{
#ifdef CMS_DEBUG
			//std::wcout << L"SQL语句错误" << std::endl;
			//std::wcout << dbSQL << std::endl;
#endif
			break;
		}
		else if (retcode == SQL_NO_DATA)
		{
#ifdef CMS_DEBUG
			//std::wcout << L"查询结束" << std::endl;
#endif
			break;
		}
		AddToCDH(cdh, op, sizeof(struct Operator));
	} while (true);

	if (op)
	{
		delete op;
		op = NULL;
	}
	SQLCloseCursor(hstmt1);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);
	return true;
}

bool DBgo::QueryUserInfo(wchar_t* QueryStr, int QUI_Method, struct CMS_DB_HEADER* cdh)
{
	if (isConnected == false)
		return false;
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1);

	//查找方式
	switch ((QUI_Method>>1)<<1)
	{
	case QUI_BY_USERNAME:
		dbSQL = QUERY_USERINFO_BY_USERNAME;
		break;
	case QUI_BY_MSISDN:
		dbSQL = QUERY_USERINFO_BY_MSISDN;
		break;
	case QUI_BY_PLANNO:
		dbSQL = QUERY_USERINFO_BY_PLANNO;
		break;
	default:
#ifdef CMS_DEBUG
		std::wcerr << "Unknown Qui_method..." << std::endl;
#endif // CMS_DEBUG
		dbSQL = QUERY_USERINFO_BY_USERNAME;
	}

	//精确查找（偶数）、近似查找（奇数）
	if (QUI_Method % 2 == 0)
	{
		dbSQL = dbSQL + L"= '";
		dbSQL = dbSQL + QueryStr;
		dbSQL = dbSQL + L"' ;";
	}
	else
	{
		dbSQL = dbSQL + L"like '%";
		dbSQL = dbSQL + QueryStr;
		dbSQL = dbSQL + L"%' ;";
	}

	SQLExecDirectW(hstmt1, (SQLWCHAR*)dbSQL.c_str(), dbSQL.length());

	struct UserInfo* ui = new struct UserInfo;
	//memset(ui->MSISDN, 0, MSISDN_LEN);
	//memset(ui->status, 0, STATUS_LEN);
	//memset(ui->name, 0, NAME_LEN);
	//memset(ui->gender, 0, GENDER_LEN);
	//memset(ui->address, 0, ADDR_LEN);
	memset(ui, 0, sizeof(struct UserInfo));

	SQLBindCol(hstmt1, 1, SQL_C_WCHAR, ui->MSISDN, MSISDN_LEN, 0);
	SQLBindCol(hstmt1, 2, SQL_C_DOUBLE, &ui->balance, 0, 0);
	SQLBindCol(hstmt1, 3, SQL_C_LONG, &ui->PlanNo, 0, 0);
	SQLBindCol(hstmt1, 4, SQL_C_WCHAR, ui->status, STATUS_LEN, 0);
	SQLBindCol(hstmt1, 5, SQL_C_WCHAR, ui->name, NAME_LEN, 0);
	SQLBindCol(hstmt1, 6, SQL_C_WCHAR, ui->gender, GENDER_LEN, 0);
	SQLBindCol(hstmt1, 7, SQL_C_WCHAR, ui->address, ADDR_LEN, 0);


	do {
		retcode = SQLFetch(hstmt1);
		if (retcode == SQL_ERROR)
		{
#ifdef CMS_DEBUG
			std::wcout << L"SQL语句错误" << std::endl;
			std::wcout << dbSQL << std::endl;
#endif
			break;
		}
		else if (retcode == SQL_NO_DATA) 
		{
#ifdef CMS_DEBUG
			std::wcout << L"查询结束" << std::endl;
#endif
			break;
		}
		//std::wcout << ui->MSISDN << L"\t" << ui->balance << L"\t";
		//std::wcout << ui->PlanNo << L"\t" << ui->status << L"\t";
		//std::wcout << ui->name << L"\t" << ui->gender << L"\t";
		//std::wcout << ui->address << L"\t" << std::endl;
		AddToCDH(cdh, ui, sizeof(struct UserInfo));

	} while (true);

	if (ui)
	{
		delete ui;
		ui = NULL;
	}
	SQLCloseCursor(hstmt1);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);
	return true;
}

bool DBgo::AddPlan(CallPlan * cp)
{
	bool isAdded = false;

	if (isConnected == false)
		return false;
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1);

	//构造Insert语句
	//假设用户输入是合法的（合法性检查放在UI执行）
	CStringW wszStr;
	wszStr.Format(L"%s%d, '%s', %lf, %lf, %lf);", ADD_CALLPLAN,
		cp->PlanNo, cp->PlanName, cp->RemoteFee, cp->LocalFee, cp->RoamingFee);
	dbSQL = wszStr.GetBuffer(wszStr.GetLength());
	
	retcode = SQLExecDirectW(hstmt1, (SQLWCHAR*)dbSQL.c_str(), dbSQL.length());

	if (retcode != SQL_SUCCESS)
	{
#ifdef CMS_DEBUG
		//std::wcout << L"Failed to Insert..." << std::endl;
#endif // CMS_DEBUG
		isAdded = false;
	}
	else
		isAdded = true;

	retcode = SQLCloseCursor(hstmt1);
	retcode = SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);

	return isAdded;
}

bool DBgo::AddCharge(Charge * c)
{
	bool isAdded = false;
	
	if (isConnected == false)
		return false;
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1);

	//构造Insert语句
	//假设用户输入是合法的（合法性检查放在UI执行）
	SYSTEMTIME st;
	CStringW wszStr;
	GetLocalTime(&st);
	wszStr.Format(L"%s'%4d-%2d-%2d %2d:%2d:%2d', '%s', %lf, '%s', %d);",
		ADD_CHARGE, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,
		c->MSISDN, c->PayAmount, c->PayMethod, c->OperatorNo);
	dbSQL = wszStr.GetBuffer(wszStr.GetLength());
	retcode = SQLExecDirectW(hstmt1, (SQLWCHAR*)dbSQL.c_str(), dbSQL.length());

	if (retcode != SQL_SUCCESS)
	{
#ifdef CMS_DEBUG
		std::wcout << L"Failed to Insert..." << std::endl;
#endif // CMS_DEBUG
		isAdded = false;
	}
	else
		isAdded = true;

	retcode = SQLCloseCursor(hstmt1);
	retcode = SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);

	return isAdded;
}

bool DBgo::AddOperator(Operator * op)
{
	bool isAdded = false;

	if (isConnected == false)
		return false;
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1);

	//构造Insert语句
	//假设用户输入是合法的（合法性检查放在UI执行）
	CStringW wszStr;
	//wszStr.Format(L"%s%d, '%s', '%s', %d, '%s', %d);", ADD_OPERATOR,
	//	op->OperatorNo, op->OperatorName, op->OperatorGender, op->Class, op->password, op->LoginStatus);
	wszStr.Format(L"%s%d, '%s', '%s', %d, '%s');", ADD_OPERATOR,
		op->OperatorNo, op->OperatorName, op->OperatorGender, op->Class, op->password);
	dbSQL = wszStr.GetBuffer(wszStr.GetLength());

	retcode = SQLExecDirectW(hstmt1, (SQLWCHAR*)dbSQL.c_str(), dbSQL.length());

	if (retcode != SQL_SUCCESS)
	{
#ifdef CMS_DEBUG
		std::wcout << L"Failed to Insert..." << std::endl;
#endif // CMS_DEBUG
		isAdded = false;
	}
	else
		isAdded = true;

	retcode = SQLCloseCursor(hstmt1);
	retcode = SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);

	return isAdded;
}

bool DBgo::AddUserInfo(UserInfo * ui)
{
	bool isAdded = false;

	if (isConnected == false)
		return false;
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1);

	//构造Insert语句
	//假设用户输入是合法的（合法性检查放在UI执行）
	CStringW wszStr;
	wszStr.Format(L"%s'%s', %lf, %d, '%s', '%s', '%s', '%s');", ADD_USERINFO,
		ui->MSISDN, ui->balance, ui->PlanNo, ui->status, ui->name, ui->gender, ui->address);
	dbSQL = wszStr.GetBuffer(wszStr.GetLength());

	retcode = SQLExecDirectW(hstmt1, (SQLWCHAR*)dbSQL.c_str(), dbSQL.length());

	if (retcode != SQL_SUCCESS)
	{
#ifdef CMS_DEBUG
		std::wcout << L"Failed to Insert..." << std::endl;
#endif // CMS_DEBUG
		isAdded = false;
	}
	else
		isAdded = true;

	retcode = SQLCloseCursor(hstmt1);
	retcode = SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);

	return isAdded;
}

bool DBgo::DropPlan(int PlanNo)
{
	bool isAdded = false;

	if (isConnected == false)
		return false;
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1);

	//构造delete语句
	//假设用户输入是合法的（合法性检查放在UI执行）
	CStringW wszStr;
	wszStr.Format(L"%d;", PlanNo);
	dbSQL = DROP_CALLPLAN_BY_PLANNO;
	dbSQL = dbSQL + wszStr.GetBuffer(wszStr.GetLength());

	retcode = SQLExecDirectW(hstmt1, (SQLWCHAR*)dbSQL.c_str(), dbSQL.length());

	if (retcode != SQL_SUCCESS)
	{
#ifdef CMS_DEBUG
		std::wcout << L"Failed to delete..." << std::endl;
#endif // CMS_DEBUG
		isAdded = false;
	}
	else
		isAdded = true;

	retcode = SQLCloseCursor(hstmt1);
	retcode = SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);

	return isAdded;
}

bool DBgo::DropOperator(int OperatorNo)
{
	bool isAdded = false;

	if (isConnected == false)
		return false;
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1);

	//构造delete语句
	//假设用户输入是合法的（合法性检查放在UI执行）
	CStringW wszStr;
	wszStr.Format(L"%d;", OperatorNo);
	dbSQL = DROP_OPERATOR_BY_OPERATORNO;
	dbSQL = dbSQL + wszStr.GetBuffer(wszStr.GetLength());

	retcode = SQLExecDirectW(hstmt1, (SQLWCHAR*)dbSQL.c_str(), dbSQL.length());

	if (retcode != SQL_SUCCESS)
	{
#ifdef CMS_DEBUG
		std::wcout << L"Failed to delete..." << std::endl;
#endif // CMS_DEBUG
		isAdded = false;
	}
	else
		isAdded = true;

	retcode = SQLCloseCursor(hstmt1);
	retcode = SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);

	return isAdded;
}

bool DBgo::DropUserInfo(wchar_t * MSISDN, int Dui_Way)
{
	bool isAdded = false;

	if (isConnected == false)
		return false;
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1);

	//构造delete语句
	//假设用户输入是合法的（合法性检查放在UI执行）
	if (Dui_Way == DUI_WAY_BY_MSISDN)
	{
		dbSQL = DROP_USERINFO_BY_MSISDN;
		dbSQL = dbSQL + L"'";
		dbSQL = dbSQL + MSISDN;
		dbSQL = dbSQL + L"';";
	}
	else if (Dui_Way == DUI_WAY_ALL)
		dbSQL = DROP_USERINFO_ALL;
	
	retcode = SQLExecDirectW(hstmt1, (SQLWCHAR*)dbSQL.c_str(), dbSQL.length());

	if (retcode != SQL_SUCCESS)
	{
#ifdef CMS_DEBUG
		std::wcout << L"Failed to delete..." << std::endl;
#endif // CMS_DEBUG
		isAdded = false;
	}
	else
		isAdded = true;

	retcode = SQLCloseCursor(hstmt1);
	retcode = SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);

	return isAdded;
}

bool DBgo::OnOperatorChangePwd(int OperatorNo, LPCWSTR NewPwd)
{
	bool isAdded = false;
	CString wszStr;
	std::wstring szStr;
	wszStr.Format(L"%s", NewPwd);
	szStr = wszStr.GetBuffer(wszStr.GetLength());
	//计算新密码MD5
	MD5 * md5 = new MD5(ws2s(szStr));
	szStr = s2ws(md5->md5());
	//构造SQL语句
	wszStr.Format(L"%s'%s' where OperatorNo = %d;", 
		UPDATE_OPERATOR_ON_PASSWORD,
		szStr.c_str(),
		OperatorNo);

	//准备SQL执行环境
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1);
	//执行SQL语句
	retcode = SQLExecDirectW(hstmt1, (SQLWCHAR*)wszStr.GetBuffer(wszStr.GetLength()), wszStr.GetLength());

	if (retcode != SQL_SUCCESS)
		isAdded = false;
	else
		isAdded = true;

	retcode = SQLCloseCursor(hstmt1);
	retcode = SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);

	if (md5)
	{
		delete md5;
		md5 = NULL;
	}

	return isAdded;
}

bool DBgo::OnUserCharge(struct Charge* c, double dBalance)
{
	bool isAdded = false;
	//写入Charge
	this->AddCharge(c);

	//获取用户当前余额
	double dBalanceNow = dBalance + c->PayAmount;
	//构造SQL语句
	CString wszStr;
	wszStr.Format(L"%s%lf where MSISDN = '%s' ;", 
		UPDATE_USERINFO_ON_BALANCE, dBalanceNow, c->MSISDN);
	//准备SQL执行环境
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1);
	//执行SQL语句
	retcode = SQLExecDirectW(hstmt1, (SQLWCHAR*)wszStr.GetBuffer(wszStr.GetLength()), wszStr.GetLength());

	if (retcode != SQL_SUCCESS)
		isAdded = false;
	else
		isAdded = true;

	retcode = SQLCloseCursor(hstmt1);
	retcode = SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);

	return isAdded;
}

bool DBgo::Login(Operator * op, int & LoginResult)
{
	int retval = 0;
	bool isLoginOK = false;
	//查询该工号
	struct CMS_DB_HEADER* cdh = new struct CMS_DB_HEADER;
	InitCDH(cdh);

	CString wszStr;
	wszStr.Format(L"%d", op->OperatorNo);

	retval = QueryOperator(wszStr.GetBuffer(wszStr.GetLength()),QO_BY_OPERATORNO, cdh);
	if (retval == false || cdh->n == 0 || cdh->next == NULL)
		isLoginOK = false;
	//检查是否匹配
	else
	{
		retval = _wcsnicmp(((struct Operator*)(cdh->next->content))->password, op->password, PWD_LEN);
		if (retval == 0)
			isLoginOK = true;
		else
			isLoginOK = false;
	}
	//填写提示信息
	if (isLoginOK)
		LoginResult = ((struct Operator*)(cdh->next->content))->Class;
	else
		LoginResult = -1;
	//释放链表
	DeleteCDH(cdh);
	
	return isLoginOK;
}

int DBgo::GetMaxNo(LPCWSTR MaxItem, LPCWSTR SheetName)
{
	bool isAdded = false;

	if (isConnected == false)
		return false;
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1);

	//构造查询语句
	//假设用户输入是合法的（合法性检查放在UI执行）
	CString wszStr;
	wszStr.Format(L"select max(%s) as 'MaxNo' from %s;", MaxItem, SheetName);

	retcode = SQLExecDirectW(hstmt1, (SQLWCHAR*)wszStr.GetBuffer(wszStr.GetLength()), wszStr.GetLength());

	int MaxNo = 0;
	SQLBindCol(hstmt1, 1, SQL_C_LONG, &MaxNo, 0, 0);

	retcode = SQLFetch(hstmt1);

	if (retcode != SQL_SUCCESS)
	{
#ifdef CMS_DEBUG
		//std::wcout << L"Failed to delete..." << std::endl;
#endif // CMS_DEBUG
		MaxNo = -1;
	}

	retcode = SQLCloseCursor(hstmt1);
	retcode = SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);
	return MaxNo;
}

bool DBgo::ConnectDB()
{
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);
	retcode = SQLSetEnvAttr(henv, 
											SQL_ATTR_ODBC_VERSION,
											(SQLPOINTER)SQL_OV_ODBC3,
											SQL_IS_INTEGER);
	retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc1);
	//1.连接数据源
	retcode = SQLConnect(hdbc1, 
										(SQLWCHAR*)dbDSN.c_str(), 
										dbDSN.length(), 
										(SQLWCHAR*)dbUID.c_str(), 
										dbUID.length(), 
										(SQLWCHAR*)dbPWD.c_str(), 
										(SQLSMALLINT)dbPWD.length());
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
	{
#ifdef CMS_DEBUG
		std::wcout << L"连接失败!" << std::endl;
#endif // CMS_DEBUG
		return false;
	}
#ifdef CMS_DEBUG
	std::wcout << L"连接成功" << std::endl;
#endif // CMS_DEBUG
	return true;
}


// 备份还原数据库
bool DBgo::BackupAndRestore(bool isBackup)
{
	CString wszStr;
	bool isAdded = false;
	if (isBackup)
	{
		wszStr = BACKUP_CMS;
	}
	else
	{
		wszStr = RESTORE_CMS;
	}

	if (isConnected == false)
		return false;

	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1);

	retcode = SQLExecDirectW(hstmt1, 
		(SQLWCHAR*)wszStr.GetBuffer(wszStr.GetLength()), 
		wszStr.GetLength());

	if (retcode != SQL_SUCCESS ||
		retcode != SQL_SUCCESS_WITH_INFO)
	{
#ifdef CMS_DEBUG
		//std::wcout << L"Failed to delete..." << std::endl;
#endif // CMS_DEBUG
		isAdded = false;
	}
	else
		isAdded = true;

	retcode = SQLCloseCursor(hstmt1);
	retcode = SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);

	return isAdded;
}
