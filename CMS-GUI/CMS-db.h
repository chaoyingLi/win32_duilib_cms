/*
CMS数据库操作 头文件
*/
#ifndef _CMS_H_
#define _CMS_H_

//引用一堆头文件
#pragma once
#include <iostream>
#include <string>
#include <Windows.h>
#include <atlbase.h>
#include <atlstr.h>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <odbcss.h>
#include "CMS-utils.h"

//定义查询语句
#define		QUERY_OPERATOR_BY_NO							L"select * from Operator where OperatorNo = "
#define		QUERY_OPERATOR_BY_NAME					L"select * from Operator where OperatorName like '%"
#define		QUERY_USERINFO_BY_USERNAME				L"select * from UserInfo where Name "
#define		QUERY_USERINFO_BY_MSISDN					L"select * from UserInfo where MSISDN "
#define		QUERY_USERINFO_BY_PLANNO					L"select * from UserInfo where PlanNo "
#define		QUERY_CALLPLAN_BY_PLANNO					L"select * from CallPlan where PlanNo = "
#define		QUERY_CALLPLAN_BY_PLANNAME			L"select * from CallPlan where PlanName like '%"
#define		QUERY_CDR_BY_CALLER								L"select * from CDR where Caller = "
//select * from CDR where CallTime like '%/ 5/%'and Caller = 13800138000;
#define		QUERY_CDR_BY_MONTH_AND_CALLER		L"select * from CDR where CallTime like '%/ "
#define		QUERY_CHARGE_BY_MSISDN						L"select * from Charge where MSISDN = "
//定义插入语句
//insert into CallPlan values (5,'本地轻松聊',0.2,0.03,0.2);
#define		ADD_CALLPLAN											L"insert into CallPlan values ("
#define		ADD_CHARGE												L"insert into Charge values("
#define		ADD_OPERATOR											L"insert into Operator values("
#define		ADD_USERINFO											L"insert into UserInfo values("
//定义删除语句
#define		DROP_CALLPLAN_BY_PLANNO					L"delete from CallPlan where PlanNo = "
#define		DROP_OPERATOR_BY_OPERATORNO		L"delete from Operator where OperatorNo = "
#define		DROP_USERINFO_BY_MSISDN					L"delete from UserInfo where MSISDN = "
#define		DROP_USERINFO_ALL									L"delete from UserInfo;"
//定义更新语句
#define		UPDATE_USERINFO_ON_BALANCE				L"update UserInfo set Balance = "
#define		UPDATE_OPERATOR_ON_PASSWORD		L"update Operator set Password = "
//定义备份恢复语句
#define		BACKUP_CMS												L"backup database CMS to disk='D:/CMS.bak' ;"
#define		RESTORE_CMS												L"restore database CMS from disk='D:/CMS.bak';"
//定义错误信息
//#define		ERROR_DROPCALLPLAN_STILLUSED
//定义字符串数组长度
#define		GENDER_LEN		4
#define		STATUS_LEN		16
#define		NAME_LEN			16
#define		TYPE_LEN				16
#define		MSISDN_LEN		64
#define		PLANNAME_LEN	64
#define		PWD_LEN				66
#define		ADDR_LEN			64
#define		TIME_LEN				64
//定义用户信息查询方式（默认：按用户名精确查找）
#define		QUI_BY_USERNAME			0
#define		QUI_BY_MSISDN					2
#define		QUI_BY_PLANNO					4
#define		QUI_WAY_ACCURATE			0
#define		QUI_WAY_APPROXIMATE	1
//定义套餐查询方式
#define		QP_BY_PLANNO					0
#define		QP_BY_PLANNAME				2
//定义收费员查询方式
#define		QO_BY_OPERATORNO		0
#define		QO_BY_OPERATORNAME	2
//定义用户信息删除方式
#define		DUI_WAY_ALL						0
#define		DUI_WAY_BY_MSISDN		2
//调试开关
#define		CMS_DEBUG		1

//定义一堆数据结构
//套餐
struct CallPlan
{
	int						PlanNo;										//套餐编号
	wchar_t				PlanName[PLANNAME_LEN];		//套餐名称
	double					RemoteFee;									//长途费率
	double					LocalFee;										//市话费率
	double					RoamingFee;								//漫游费率
};

//通话详单 CallDetailRecords
struct CDR		
{
	wchar_t		CallTime[TIME_LEN];			//通话时间
	wchar_t		caller[MSISDN_LEN];			//主叫
	wchar_t		called[MSISDN_LEN];			//被叫
	wchar_t		type[TYPE_LEN];					//通话类型
	int				duration;								//通话时长
};

//收费
struct Charge
{
	wchar_t			PayTime[TIME_LEN];			//缴费时间
	wchar_t			MSISDN[MSISDN_LEN];		//用户（电话号码）
	double				PayAmount;							//缴费金额
	wchar_t			PayMethod[TYPE_LEN];		//缴费方式
	int					OperatorNo;						//收费员工号
};

//收费员
struct Operator
{
	int						OperatorNo;									//工号
	wchar_t				OperatorName[NAME_LEN];			//姓名
	wchar_t				OperatorGender[GENDER_LEN];		//性别
	int						Class;												//权限等级
	wchar_t				password[PWD_LEN];						//密码（哈希）
	//int						LoginStatus;										//登录状态
};

//用户数据
struct UserInfo
{
	wchar_t				MSISDN[MSISDN_LEN];				//电话号码
	double					balance;										//余额
	int						PlanNo;										//套餐编号
	wchar_t				status[STATUS_LEN];					//用户状态
	wchar_t				name[NAME_LEN];						//姓名
	wchar_t				gender[GENDER_LEN];				//性别
	wchar_t				address[ADDR_LEN];					//住址
};


//数据库操作类 声明
class DBgo
{
public:
	//一堆可供内外部使用的变量...

	//一堆可供内外部调用的函数...
	DBgo();
	~DBgo();
	//查询各表
	bool QueryPlan(wchar_t* QueryStr, int QP_Method, struct CMS_DB_HEADER* cdh);
	bool QueryCDR(wchar_t* CallerMSISDN, struct CMS_DB_HEADER* cdh);
	bool QueryCharge(wchar_t* MSISDN, struct CMS_DB_HEADER* cdh);
	bool QueryOperator(LPCWSTR QueryStr, int QO_Method, struct CMS_DB_HEADER* cdh);
	bool QueryUserInfo(wchar_t* QueryStr, int QUI_Method, struct CMS_DB_HEADER* cdh);
	//新增套餐、收费记录、收费员、用户
	bool AddPlan(struct CallPlan* cp);
	bool AddCharge(struct Charge* c);
	bool AddOperator(struct Operator* op);
	bool AddUserInfo(struct UserInfo* ui);
	//删除套餐、收费员、用户	
	bool DropPlan(int PlanNo);
	bool DropOperator(int OperatorNo);
	bool DropUserInfo(wchar_t* MSISDN, int Dui_Way);
	//操作员更改密码
	bool OnOperatorChangePwd(int OperatorNo, LPCWSTR NewPwd);
	//用户缴费
	bool OnUserCharge(struct Charge* c, double dBalance);
	//登录
	bool Login(struct Operator* op, int & LoginResult);
	//获取编号最大值，参数：列、表名
	int GetMaxNo(LPCWSTR MaxItem, LPCWSTR SheetName);
private:
	//数据库操作三剑客
	SQLHENV			henv;				//环境句柄
	SQLHDBC		hdbc1;				//连接句柄
	SQLHSTMT		hstmt1;			//游标
	//登录验证
	std::wstring		dbDSN;			//数据库名
	std::wstring		dbUID;				//登录用户名
	std::wstring		dbPWD;			//登录用户密码
	//杂项
	std::wstring		dbSQL;				//SQL语句
	RETCODE			retcode;			//返回值
	bool					isConnected;	//连接成功？

	//连接数据库
	bool ConnectDB();

public:
	// 备份还原数据库
	bool BackupAndRestore(bool isBackup);
};

#endif