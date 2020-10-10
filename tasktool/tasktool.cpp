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
		cout << "没有找到工作文件目录，需要执行程序目录下有files目录！" << endl;
		getchar();
		return 0;
	}

	cout << "正在读取files目录下的文件！" << endl;
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
		cout << "没有找到names.txt文件，或者文件内容为空！" << endl;
		getchar();
		return 0;
	}

	std::vector<CString> names;
	CommonHelper::SplitCStringToArray(strNames,"\r\n", names );

	if ( names.size() > workFiles.size() )
	{
		cout << "文件个数比人数少，不做分配！" << endl;
		getchar();
		return 0;
	}
	int average = workFiles.size()/names.size();
	int reminder = workFiles.size()%names.size();

	cout << "正在做文件分配，请稍后！" << endl;
	::Sleep(100);

	//string nowTime =  CommonHelper::ToString( CPcTime::Now().ToLocalDateTime().ToString( EDateFormat::PYYYYMMDDHHmmss ) );
	string resultPath = exePath  + "\\" + nowTime;
	::CreateDirectoryA( resultPath.c_str(),NULL );

	string showInfo;
	char csvtask[200];

	for ( int i = 0; i< names.size(); i ++ )
	{	
		showInfo = "正在给" + CommonHelper::ToString( names[i]) + "分配任务";
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
		cout << "正在分配剩余的任务" << endl;
		for ( int i = 0;i<reminder; i++ )
		{
			showInfo = "给" + CommonHelper::ToString( names[i]) + "分配了一个多余的任务";
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
	sprintf_s( total, "一共%d个任务，%d个人员，平均分配%d个任务，多余%d个任务", workFiles.size(), names.size(), average,reminder );
	cout <<  total << endl;

	cout <<  "任务分配目录:" << endl;
	cout <<   resultPath.c_str() << endl;
	cout <<  "csv文件路径:" << endl;
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
		cout <<  "哇，原来是猪头！" << endl;
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
		cout <<  "没有找到tasks.csv文件，跳过任务过滤" << endl;
		return;
	}
	if ( !ATLPath::IsDirectory( tasksPath.c_str() ))
	{
		cout <<  "没有找到tasks目录，跳过任务过滤" << endl;
		return;
	}

	CSVFile CSVRead; 
	if (!CSVRead.Open(true, csvPath.c_str() ))   
	{
		cout <<  "tasks.csv文件读取失败，请确认文件格式是否正确" << endl;
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
	string reaultInListPath = resultPath + "\\在任务列表的文件";
	string reaultNotInListPath = resultPath + "\\不在任务列表的文件";
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
		cout << "没有找到工作文件目录，需要执行程序目录下有tasks目录！" << endl;
		return;
	}

	cout << "正在读取tasks目录下的文件！" << endl;
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
	string taskinfo = "文件比较完成，目录：" + resultPath + "\r\n\r\n";
	cout << taskinfo.c_str() << endl;
	::FindClose(hp);
}
//#define CREATE_TEMP_FILE

//分析CSV文件，生成新的CSV
void analyzeCSV( string nowTime ){

	cout <<  "请按回车获得结果数据，输入0，表示只输出结果列，其他表示同时输出原始数据 " << endl;

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
		cout <<  "没有找到files.csv文件，跳过任务过滤" << endl;
		return;
	}

	CSVFile CSVRead; 
	if (!CSVRead.Open(true, csvPath.c_str() ))   
	{
		cout <<  "tasks.csv文件读取失败，请确认文件格式是否正确" << endl;
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

	cout << "输入重文件夹的路径，直接回车表示当前目录：" << endl;

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
		cout << "没有找到工作文件目录" << endl;
		return;
	}

	char renameDictory[500];
	sprintf_s(renameDictory, "%s\\RenameFiles", tasksPath.c_str());
	::CreateDirectoryA(renameDictory,NULL);

	cout << "正在读取tasks目录下的文件！" << endl;
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
				cout << "文件处理失败" << info.cFileName << error << endl;
			}
			else
			{
				cout << "复制文件文件：" << info.cFileName << "到:" << fileDesct << endl;
			}
		}
	} while (FindNextFileA(hp, &info));
	
	cout << "完成" << endl;
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
	cout <<  "开始比较tasks.csv文件和tasks目录下的文件：" << endl;
	ReadTask( nowTime );

	cout <<  "开始任务分配" << endl;
	ArrangeTask( nowTime );
#endif

#if TASK_ANALYZE
	analyzeCSV(nowTime);
#endif
	
	return 0;
}

