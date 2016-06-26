#pragma once
/*********************************************************************
* 版权所有 (C)2012, 联想（北京）有限责任公司。
*
* 文件名称： PJHandler.h
* 文件标识：
* 内容摘要： 文件管理的Helper类
* 作 者：	 樊名江
* 创建日期： 2012年8月14日
*
* 修改记录：// 包括修改日期、修改者及修改内容

**********************************************************************/

namespace pcutil
{

enum FileEncoding
{
	FILE_ENCODING_UNKNOWN,
	FILE_ENCODING_UNICODE,
	FILE_ENCODING_UTF8,
	FILE_ENCODING_ANSI
};

class WEAVERLIB_API_UTILS CFileHelper
{
public:
	static BOOL CreateDir( CString path );
	static BOOL CreateDir( char* path );

	static BOOL Delete( CString filePath );
	//递归删除目录下所有的文件 包含子目录下的文件
	static void DeleteFiles( CString folderPath,  BOOL deleteSubFolder = FALSE  );
	static void DeleteFiles( char* folderPath, BOOL deleteSubFolder = FALSE );
	//static bool SHDeleteFiles(CString strFrom);

	static BOOL IsFileExists( CString filePath );
	static CString GetFileName( CString filePath );
	static CString GetFileExtention( CString filePath );
	static void Move( CString sourceFilePath, CString destFilePath );
	static void Copy( CString sourceFilePath, CString destFilePath  );
	static UINT64 GetFileSize( CString filePath );

	static BOOL IsDictoryExists( CString path );

	static CString GetApplicationDataPath();
	static CString GetMyPicturePath();

	//暂时只支持ANSI格式
	static BOOL WriteAllText( CString filePath, CString& str, FileEncoding encoding = FILE_ENCODING_ANSI );
	//暂时只支持ANSI格式
	static CString ReadAllText( CString filePath, FileEncoding encoding = FILE_ENCODING_ANSI );

	static CString ReadAllText( char* &pBuffer, UINT64& buffSize, FileEncoding encoding = FILE_ENCODING_ANSI );
	static BOOL ReadAllBinary( CString filePath, OUT char*& buffer, OUT UINT64& buffSize );
	static BOOL WriteAllBinary( CString filePath, LPVOID buf, UINT64 len);
	//获取应用程序目录
	static CString GetAppPath();
	static CVersion GetProductVersion();
};
	
}