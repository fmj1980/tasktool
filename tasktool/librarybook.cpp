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
	string ���;
	string ����;
	string �����;
	string �����;
	string ����;
	string ������;
	string ��������;
	string ����;
	string ISBN;
	string ��ǰ�ݲصص�;
	string �ݲ�״̬;
	string �������;
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
		cout << "tasks.csv�ļ���ȡʧ�ܣ���ȷ���ļ���ʽ�Ƿ���ȷ" << endl;
		return 0;
	}
	while (CSVRead.CSVReadNextRow())
	{
		CBook* book = new CBook();
		CSVRead.CSVRead("���", book->���);
		CSVRead.CSVRead("����", book->����);
		CSVRead.CSVRead("�����", book->�����);
		CSVRead.CSVRead("�����", book->�����);
		CSVRead.CSVRead("����", book->����);
		CSVRead.CSVRead("������", book->������);
		CSVRead.CSVRead("��������", book->��������);
		CSVRead.CSVRead("����", book->����);
		CSVRead.CSVRead("ISBN", book->ISBN);
		CSVRead.CSVRead("��ǰ�ݲصص�", book->��ǰ�ݲصص�);
		CSVRead.CSVRead("�ݲ�״̬", book->�ݲ�״̬);
		CSVRead.CSVRead("�������", book->�������);
		CString barcode = CA2T(book->�����.c_str(), CP_UTF8).m_psz;
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
		cout << "tasks.csv�ļ���ȡʧ�ܣ���ȷ���ļ���ʽ�Ƿ���ȷ" << endl;
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
		cout << "tasks.csv�ļ���ȡʧ�ܣ���ȷ���ļ���ʽ�Ƿ���ȷ" << endl;
		return;
	}
	for (auto book : g_allBooks)
	{
		writer.CSVWrite("���", book.second->���);
		writer.CSVWrite("����", book.second->����);
		writer.CSVWrite("�����", book.second->�����);
		writer.CSVWrite("�����", book.second->�����);
		writer.CSVWrite("����", book.second->����);
		writer.CSVWrite("������", book.second->������);
		writer.CSVWrite("��������", book.second->��������);
		writer.CSVWrite("����", book.second->����);
		writer.CSVWrite("ISBN", book.second->ISBN);
		writer.CSVWrite("��ǰ�ݲصص�", book.second->��ǰ�ݲصص�);
		writer.CSVWrite("�ݲ�״̬", book.second->�ݲ�״̬);
		writer.CSVWrite("�������", book.second->�������);

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