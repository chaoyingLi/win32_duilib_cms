/*
CMS逻辑操作 头文件
*/
#ifndef _CMS_UTILS_H_
#define _CMS_UTILS_H_

#pragma once
#include <iostream>
#include <string>
#include <Windows.h>
#include "CMS-md5.h"

#define CONTENT_LEN	512

//定义链表结点、链表头
struct CMS_DB_NODE
{
	char content[CONTENT_LEN];
	struct CMS_DB_NODE* next;
};
struct CMS_DB_HEADER
{
	int n;
	struct CMS_DB_NODE* next;
};

//struct CMS_HWND_NODE
//{
//	HWND hwnd;
//	struct CMS_HWND_NODE* next;
//};
//
//struct CMS_HWND_HEADER
//{
//	int n;
//	struct CMS_HWND_NODE* next;
//};

//初始化链表头
bool InitCDH(struct CMS_DB_HEADER* cdh);
//删除整条链表
bool DeleteCDH(struct CMS_DB_HEADER* cdh);
//向链表头挂载新的结点
bool AddToCDH(struct CMS_DB_HEADER* cdh, void* content, size_t ContentLen);

std::string WChar2Ansi(LPCWSTR pwszSrc);
std::wstring Ansi2WChar(LPCSTR pszSrc, int nLen);
std::string ws2s(std::wstring& inputws);
std::wstring s2ws(const std::string& s);

//金额转人民币大写
std::string ConvertMoneyCaps(long double moneySum);


#endif // !_CMS_UTILS_H_