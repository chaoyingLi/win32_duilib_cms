#ifndef _CMS_PDF_H_
#define _CMS_PDF_H_

#pragma once
#include <iostream>
#include <string>
#include <Windows.h>
#include <atlstr.h>
#include "pdflib.h"

#pragma comment(lib, "PDFLib.lib")

#define		PDF_FILE_PATH	"D:/receipt.pdf"

void String2text(PDF *p, int &TextFlow, CString &mPagenameAndDate, int fontsize);
void OutputStringToPDF(PDF *p, CString &mPagenameAndDate, int Left, int &Top, int Right, int Reduction, int fontsize);
bool CreatePDF(CString Content);

#endif // !_CMS_PDF_H_
