// tasktool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "FileHelper.h"
#include "CommonHelper.h"
#include <Windows.h>
#include "AtlConv.h"
#include "PcTime.h"
#include "CSVFile.h"
using namespace pcutil;

#define TASK_ARRANGE 0
#define TASK_ANALYZE 0
#define TASK_RENAME 1

int ArrangeTask( string nowTime ) 
{
	string exePath = CT2A( CFileHelper::GetAppPath() );
	string worksPath = exePath + "\\files";
	string namesPath = exePath + "\\names.txt";

#ifdef CREATE_TEMP_FILE
	for (int i= 0;i<1025;i++)
	{
		char str[4];
		string filepath = worksPath + "\\" + itoa(i,str,10) + ".txt" ;
		CString content = _T("");
		CFileHelper::WriteAllText( CommonHelper::ToCString(filepath), content);
	}
#endif	
	string csvContent = "\"file\",\"name\"\r\n";
	std::vector<string> workFiles;

	char fileFound[500];
	WIN32_FIND_DATAA info;
	HANDLE hp; 
	sprintf_s( fileFound, "%s\\*.*",worksPath.c_str() );
	hp = FindFirstFileA( fileFound, &info );
	if (!hp || hp == INVALID_HANDLE_VALUE )
	{
		cout << "û���ҵ������ļ�Ŀ¼����Ҫִ�г���Ŀ¼����filesĿ¼��" << endl;
		getchar();
		return 0;
	}

	cout << "���ڶ�ȡfilesĿ¼�µ��ļ���" << endl;
	do
	{
		if ( (( strcmp(info.cFileName, "." )==0)|| ( strcmp( info.cFileName, ".." )==0 )) )
		{
			continue;
		}
		if( ( info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY )
		{
			continue;		
		}
		else
		{
			sprintf_s( fileFound,"%s\\%s",worksPath.c_str() , info.cFileName );
			workFiles.push_back( fileFound );
		}
	}while( FindNextFileA( hp, &info ) ); 
	::FindClose(hp);

	CString cstrNamesPath  = CA2T(namesPath.c_str());
	CString strNames = CFileHelper::ReadAllText( cstrNamesPath );
	if (strNames.IsEmpty() )
	{
		cout << "û���ҵ�names.txt�ļ��������ļ�����Ϊ�գ�" << endl;
		getchar();
		return 0;
	}

	std::vector<CString> names;
	CommonHelper::SplitCStringToArray(strNames,"\r\n", names );

	if ( names.size() > workFiles.size() )
	{
		cout << "�ļ������������٣��������䣡" << endl;
		getchar();
		return 0;
	}
	int average = workFiles.size()/names.size();
	int reminder = workFiles.size()%names.size();

	cout << "�������ļ����䣬���Ժ�" << endl;
	::Sleep(100);

	//string nowTime =  CommonHelper::ToString( CPcTime::Now().ToLocalDateTime().ToString( EDateFormat::PYYYYMMDDHHmmss ) );
	string resultPath = exePath  + "\\" + nowTime;
	::CreateDirectoryA( resultPath.c_str(),NULL );

	string showInfo;
	char csvtask[200];

	for ( int i = 0; i< names.size(); i ++ )
	{	
		showInfo = "���ڸ�" + CommonHelper::ToString( names[i]) + "��������";
		cout <<  showInfo.c_str() << endl;
		string namePath = resultPath + "\\" + CommonHelper::ToString( names[i]) + "\\";
		::CreateDirectoryA( namePath.c_str(),NULL );
		for ( int j=i*average; j<i*average + average; j++)
		{					
			string targetPath =  namePath + ATLPath::FindFileName(workFiles[j].c_str());
			::CopyFileA( workFiles[j].c_str(),targetPath.c_str(),TRUE );

			ZeroMemory(csvtask,200);
			sprintf_s( csvtask,"\"%s\",\"%s\"\r\n",ATLPath::FindFileName(workFiles[j].c_str()),CommonHelper::ToString( names[i]).c_str() );
			csvContent += csvtask;
		}
		::Sleep(100);
	}
	if (reminder>0)
	{
		cout <<  "" << endl;
		cout << "���ڷ���ʣ�������" << endl;
		for ( int i = 0;i<reminder; i++ )
		{
			showInfo = "��" + CommonHelper::ToString( names[i]) + "������һ�����������";
			cout <<  showInfo.c_str() << endl;
			string namePath = resultPath + "\\" + CommonHelper::ToString( names[i]) + "\\";
			string targetPath =  namePath + ATLPath::FindFileName(workFiles[names.size()*average+i].c_str());
			::CopyFileA(workFiles[names.size()*average+i].c_str(),targetPath.c_str(),TRUE );

			ZeroMemory(csvtask,200);
			sprintf_s( csvtask,"\"%s\",\"%s\"\r\n",ATLPath::FindFileName(workFiles[names.size()*average+i].c_str()),CommonHelper::ToString( names[i]).c_str() );
			csvContent += csvtask;
		}
	}

	string outputFile = exePath + "\\" + nowTime + ".csv";
	CFileHelper::WriteAllText( CommonHelper::ToCString(outputFile), CommonHelper::ToCString(csvContent) );

	cout <<  "" << endl;

	char total[500];
	sprintf_s( total, "һ��%d������%d����Ա��ƽ������%d�����񣬶���%d������", workFiles.size(), names.size(), average,reminder );
	cout <<  total << endl;

	cout <<  "�������Ŀ¼:" << endl;
	cout <<   resultPath.c_str() << endl;
	cout <<  "csv�ļ�·��:" << endl;
	cout << outputFile.c_str() << endl;


	char buffer[10];
	ZeroMemory(buffer,10);
	int i, ch;

	for (i = 0; (i < 10) && ((ch = _getchar_nolock()) != EOF)
		&& (ch != '\n'); i++)
	{
		buffer[i] = (char) ch;
	}
	if ( strcmp(buffer,"liuliang") == 0)
	{	
		cout <<  "�ۣ�ԭ������ͷ��" << endl;
		getchar();
	}
}

void ReadTask( string nowTime )
{
	string exePath = CT2A( CFileHelper::GetAppPath() );
	string tasksPath = exePath + "\\tasks";
	string csvPath = exePath + "\\tasks.csv";
	string resultsPath = exePath + "\\resultsPath";

	if ( !ATLPath::FileExists( csvPath.c_str() ))
	{
		cout <<  "û���ҵ�tasks.csv�ļ��������������" << endl;
		return;
	}
	if ( !ATLPath::IsDirectory( tasksPath.c_str() ))
	{
		cout <<  "û���ҵ�tasksĿ¼�������������" << endl;
		return;
	}

	CSVFile CSVRead; 
	if (!CSVRead.Open(true, csvPath.c_str() ))   
	{
		cout <<  "tasks.csv�ļ���ȡʧ�ܣ���ȷ���ļ���ʽ�Ƿ���ȷ" << endl;
		return;
	}
	std::vector<string> m_tasks;
	while (CSVRead.CSVReadNextRow())   
	{
		string fileName;
		CSVRead.CSVRead("file", fileName);   
		if ( fileName.length() == 0 )
		{
			continue;;
		}
		m_tasks.push_back(fileName);
	}
	
	string resultPath = exePath  + "\\task_" + nowTime;
	string reaultInListPath = resultPath + "\\�������б���ļ�";
	string reaultNotInListPath = resultPath + "\\���������б���ļ�";
	::CreateDirectoryA( resultPath.c_str(),NULL );
	::CreateDirectoryA( reaultInListPath.c_str(),NULL );
	::CreateDirectoryA( reaultNotInListPath.c_str(),NULL );

	char fileFound[500];
	WIN32_FIND_DATAA info;
	HANDLE hp; 
	sprintf_s( fileFound, "%s\\*.*",tasksPath.c_str() );
	hp = FindFirstFileA( fileFound, &info );
	if (!hp || hp == INVALID_HANDLE_VALUE )
	{
		cout << "û���ҵ������ļ�Ŀ¼����Ҫִ�г���Ŀ¼����tasksĿ¼��" << endl;
		return;
	}

	cout << "���ڶ�ȡtasksĿ¼�µ��ļ���" << endl;
	do
	{
		if ( (( strcmp(info.cFileName, "." )==0)|| ( strcmp( info.cFileName, ".." )==0 )) )
		{
			continue;
		}
		if( ( info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY )
		{
			continue;		
		}
		else
		{
			sprintf_s( fileFound,"%s\\%s",tasksPath.c_str() , info.cFileName );			
			std::vector<string>::iterator it = m_tasks.begin();
			bool finded = false;
			string fileName = info.cFileName;
			while(it != m_tasks.end())
			{
				if (*it == info.cFileName )
				{
					finded = true;
					break;
				}
				it++;
			}
			string destFilePath = (finded?reaultInListPath:reaultNotInListPath) + "\\" + fileName;
			::CopyFileA( fileFound, destFilePath.c_str(), FALSE );
		}
	}while( FindNextFileA( hp, &info ) );
	string taskinfo = "�ļ��Ƚ���ɣ�Ŀ¼��" + resultPath + "\r\n\r\n";
	cout << taskinfo.c_str() << endl;
	::FindClose(hp);
}
//#define CREATE_TEMP_FILE

//����CSV�ļ��������µ�CSV
void analyzeCSV( string nowTime ){

	cout <<  "�밴�س���ý�����ݣ�����0����ʾֻ�������У�������ʾͬʱ���ԭʼ���� " << endl;

	char c1;
	cin.get(c1);

	bool includeSourceLine = true;
	if (c1=='0'){
		includeSourceLine = false;
	}

	string exePath = CT2A( CFileHelper::GetAppPath() );
	string csvPath = exePath + "\\files.csv";

	if ( !ATLPath::FileExists( csvPath.c_str() ))
	{
		cout <<  "û���ҵ�files.csv�ļ��������������" << endl;
		return;
	}

	CSVFile CSVRead; 
	if (!CSVRead.Open(true, csvPath.c_str() ))   
	{
		cout <<  "tasks.csv�ļ���ȡʧ�ܣ���ȷ���ļ���ʽ�Ƿ���ȷ" << endl;
		return;
	}
	std::string contents = "column1,column2,column3,column4\r\n";
	while (CSVRead.CSVReadNextRow())   
	{
		string column1;
		CSVRead.CSVRead("column1", column1);  
		string column2;
		CSVRead.CSVRead("column2", column2); 
		string column3;
		CSVRead.CSVRead("column3", column3); 

		if (includeSourceLine){
			contents+=column1+","+column2+","+column3+",\r\n";
		}
		contents+=",,,";

		int count = atoi(column1.c_str());
		for (int i =1; i<=count;i++)
		{
			stringstream ss;
			string str;
			ss<<i;
			ss>>str;

			string column4 = column3+column2+str;
			contents+=column4+"\r\n";

			if (i<count)
			{
				contents+=",,,";
			}
		}
	}
	string outputFile = exePath + "\\" + nowTime + ".csv";
	CFileHelper::WriteAllText( CommonHelper::ToCString(outputFile), CommonHelper::ToCString(contents) );
}

void RenameFiles(string time) {

	cout << "�������ļ��е�·����ֱ�ӻس���ʾ��ǰĿ¼��" << endl;

	char str[MAX_PATH] = { '\0' };
	cin.getline(str, MAX_PATH);

	string tasksPath;
	if (strlen(str) == 0)
	{
		tasksPath = CT2A(CFileHelper::GetAppPath());
	}
	else
	{
		tasksPath = str;
	}

	char fileFound[500];
	WIN32_FIND_DATAA info;
	HANDLE hp;
	sprintf_s(fileFound, "%s\\*.*", tasksPath.c_str());
	hp = FindFirstFileA(fileFound, &info);
	if (!hp || hp == INVALID_HANDLE_VALUE)
	{
		cout << "û���ҵ������ļ�Ŀ¼" << endl;
		return;
	}

	char renameDictory[500];
	sprintf_s(renameDictory, "%s\\RenameFiles", tasksPath.c_str());
	::CreateDirectoryA(renameDictory,NULL);

	cout << "���ڶ�ȡtasksĿ¼�µ��ļ���" << endl;
	do
	{
		if (((strcmp(info.cFileName, ".") == 0) || (strcmp(info.cFileName, "..") == 0)))
		{
			continue;
		}
		if ((info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;
		}
		else
		{
			char fileSrc[500];
			char fileDesct[500];
		
			sprintf_s(fileSrc, "%s\\%s", tasksPath.c_str(), info.cFileName);
			sprintf_s(fileDesct, "%s\\%s.%s", renameDictory,time.c_str(), PathFindFileNameA(info.cFileName));
			
			BOOL result = ::CopyFileA(fileSrc, fileDesct,FALSE);
			if (!result)
			{
				DWORD error = GetLastError();
				cout << "�ļ�����ʧ��" << info.cFileName << error << endl;
			}
			else
			{
				cout << "�����ļ��ļ���" << info.cFileName << "��:" << fileDesct << endl;
			}
		}
	} while (FindNextFileA(hp, &info));
	
	cout << "���" << endl;
	getchar();
	::FindClose(hp);
}

extern void CaculateLibaryBook();
using namespace pcutil;
int _tmain(int argc, _TCHAR* argv[])
{
	CaculateLibaryBook();

	string nowTime =  CommonHelper::ToString( CPcTime::Now().ToLocalDateTime().ToString( EDateFormat::PYYYYMMDDHHmmss) );

#if TASK_RENAME
	RenameFiles(nowTime);

#endif

#if TASK_ARRANGE
	cout <<  "��ʼ�Ƚ�tasks.csv�ļ���tasksĿ¼�µ��ļ���" << endl;
	ReadTask( nowTime );

	cout <<  "��ʼ�������" << endl;
	ArrangeTask( nowTime );
#endif

#if TASK_ANALYZE
	analyzeCSV(nowTime);
#endif
	
	return 0;
}

