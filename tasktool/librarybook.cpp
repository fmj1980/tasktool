#include "stdafx.h"


#include "FileHelper.h"
#include "CommonHelper.h"
#include <Windows.h>
#include "AtlConv.h"
#include "PcTime.h"
#include "CSVFile.h"
#include <map>
using namespace pcutil;
using namespace std;


class CBook
{
public:
	string 序号;
	string 题名;
	string 索书号;
	string 条码号;
	string 作者;
	string 出版社;
	string 出版日期;
	string 单价;
	string ISBN;
	string 当前馆藏地点;
	string 馆藏状态;
	string 入藏日期;
};

map<CString, CBook*> g_allBooks;
map<CString, CBook*> g_alreadyFoundList;
int g_toalBookCount = 0;
int g_totalFoundCount = 0;
int g_totalNotFoundCount = 0;
string g_notFoundStringList;
string g_repeadStringList;
int g_repeadCount = 0;


void MakeCode(CString& barcode)
{
	barcode.MakeLower();
	barcode.Replace(L"+", L"");
	
	barcode.Replace(L"y", L"");
	barcode.Replace(L"b", L"");
}
int ReadAllBook(string fileName)
{
	string appPath = CT2A(CFileHelper::GetAppPath(), CP_ACP);
	string csvPath = appPath + fileName;

	CSVFile CSVRead;
	if (!CSVRead.Open(true, csvPath.c_str()))
	{
		cout << "tasks.csv文件读取失败，请确认文件格式是否正确" << endl;
		return 0;
	}
	while (CSVRead.CSVReadNextRow())
	{
		CBook* book = new CBook();
		CSVRead.CSVRead("序号", book->序号);
		CSVRead.CSVRead("题名", book->题名);
		CSVRead.CSVRead("索书号", book->索书号);
		CSVRead.CSVRead("条码号", book->条码号);
		CSVRead.CSVRead("作者", book->作者);
		CSVRead.CSVRead("出版社", book->出版社);
		CSVRead.CSVRead("出版日期", book->出版日期);
		CSVRead.CSVRead("单价", book->单价);
		CSVRead.CSVRead("ISBN", book->ISBN);
		CSVRead.CSVRead("当前馆藏地点", book->当前馆藏地点);
		CSVRead.CSVRead("馆藏状态", book->馆藏状态);
		CSVRead.CSVRead("入藏日期", book->入藏日期);
		CString barcode = CA2T(book->条码号.c_str(), CP_UTF8).m_psz;
		MakeCode(barcode);

		if (g_toalBookCount %10000 == 0)
		{
			wprintf(L"count:%d\n", g_toalBookCount);
		}

		if (barcode == L"[0030330]")
		{
			wprintf(L"[0030330]");
		}
		
		g_allBooks.insert(make_pair( barcode, book));
		g_toalBookCount++;
	}
	return g_toalBookCount;
}

void RemoveBarCode(CString &strCode, string fileName)
{
	if (strCode.GetLength() < 7)
	{
		int length = 7 - strCode.GetLength();
		for (int i = 0; i < length; i++)
		{
			strCode = L"0" + strCode;
		}
	}
	strCode = "[" + strCode + "]";

	if (strCode == L"[0030330]")
	{
		printf("[0030330]");
	}
	if (g_alreadyFoundList.find(strCode) != g_alreadyFoundList.end())
	{
		g_repeadCount++;
		g_repeadStringList += fileName + "--" + string(CT2A(strCode, CP_UTF8).m_psz) + "\n";
		return;
	}
	auto it = g_allBooks.find(strCode);
	if (it != g_allBooks.end())
	{
		g_totalFoundCount++;
		g_allBooks.erase(it);
	}
	else
	{
		g_totalNotFoundCount++;
		if (g_totalNotFoundCount % 1000 == 0)
		{
			wprintf(L"not found%d\n", g_totalNotFoundCount);
		}
		g_notFoundStringList += fileName + "--" + string(CT2A(strCode, CP_UTF8).m_psz) + "\n";
	}
	g_alreadyFoundList.emplace(strCode,new CBook);

}

int RemoveBookData(string fileName)
{
	string appPath = CT2A(CFileHelper::GetAppPath(), CP_ACP);
	string csvPath = appPath + fileName;

	CSVFile CSVRead;
	if (!CSVRead.Open(true, csvPath.c_str()))
	{
		cout << "tasks.csv文件读取失败，请确认文件格式是否正确" << endl;
		return 0;
	}
	
	while (CSVRead.CSVReadNextRow())
	{
		string barcode;
		CSVRead.CSVRead("barcode", barcode);
		if (barcode.length() ==0)
		{
			continue;
		}
		CString strCode = CA2T(barcode.c_str(), CP_UTF8).m_psz;
		MakeCode(strCode);

		if (strCode.GetLength() == 14)
		{
			RemoveBarCode(strCode.Mid(0, 7), fileName);
			RemoveBarCode(strCode.Mid(7, 7), fileName);
		}
		else
		{
			RemoveBarCode(strCode, fileName);
		}
	}

	return S_OK;
}

void CaculateLibaryBook()
{
	/*CString code = L"30330";
	RemoveBarCode(code, "");*/
	ReadAllBook("all1.csv");
	ReadAllBook("all.csv");

	g_skipCount = 3;
	for (int i = 1;i<=8; i++)
	{
		if (i > 1)
		{
			g_skipCount = 0;
		}
		RemoveBookData(CommonHelper::IntToString(i) + ".csv");
	}
	RemoveBookData("notreturn.csv");

	g_skipCount = 0;
	string csvPath = CT2A(CFileHelper::GetAppPath() + L"result.csv", CP_ACP);

	CSVFile writer;
	if (!writer.Open(false, csvPath.c_str()))
	{
		cout << "tasks.csv文件读取失败，请确认文件格式是否正确" << endl;
		return;
	}
	for (auto book : g_allBooks)
	{
		writer.CSVWrite("序号", book.second->序号);
		writer.CSVWrite("题名", book.second->题名);
		writer.CSVWrite("索书号", book.second->索书号);
		writer.CSVWrite("条码号", book.second->条码号);
		writer.CSVWrite("作者", book.second->作者);
		writer.CSVWrite("出版社", book.second->出版社);
		writer.CSVWrite("出版日期", book.second->出版日期);
		writer.CSVWrite("单价", book.second->单价);
		writer.CSVWrite("ISBN", book.second->ISBN);
		writer.CSVWrite("当前馆藏地点", book.second->当前馆藏地点);
		writer.CSVWrite("馆藏状态", book.second->馆藏状态);
		writer.CSVWrite("入藏日期", book.second->入藏日期);

		writer.CSVWriteNextRow();
	}
	
	wprintf(L"total:%d, left:%d foundcont:%d  notfound:%d  repeatcount:%d\n ", g_toalBookCount, g_allBooks.size(), g_totalFoundCount, g_totalNotFoundCount, g_repeadCount);

	CString notfoundPath = CFileHelper::GetAppPath() + L"notfound.csv";
	CFileHelper::Delete(notfoundPath);
	CFileHelper::WriteAllBinary(notfoundPath, (LPVOID)g_notFoundStringList.c_str(), g_notFoundStringList.length());


	CString repeatPath = CFileHelper::GetAppPath() + L"repeat.csv";
	CFileHelper::Delete(repeatPath);
	CFileHelper::WriteAllBinary(repeatPath, (LPVOID)g_repeadStringList.c_str(), g_repeadStringList.length());
	printf("end...");
}