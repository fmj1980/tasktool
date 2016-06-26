#pragma once
/*********************************************************************
* ��Ȩ���� (C)2012, ���루�������������ι�˾��
*
* �ļ����ƣ� PJHandler.h
* �ļ���ʶ��
* ����ժҪ�� �ļ������Helper��
* �� �ߣ�	 ������
* �������ڣ� 2012��8��14��
*
* �޸ļ�¼��// �����޸����ڡ��޸��߼��޸�����

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
	//�ݹ�ɾ��Ŀ¼�����е��ļ� ������Ŀ¼�µ��ļ�
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

	//��ʱֻ֧��ANSI��ʽ
	static BOOL WriteAllText( CString filePath, CString& str, FileEncoding encoding = FILE_ENCODING_ANSI );
	//��ʱֻ֧��ANSI��ʽ
	static CString ReadAllText( CString filePath, FileEncoding encoding = FILE_ENCODING_ANSI );

	static CString ReadAllText( char* &pBuffer, UINT64& buffSize, FileEncoding encoding = FILE_ENCODING_ANSI );
	static BOOL ReadAllBinary( CString filePath, OUT char*& buffer, OUT UINT64& buffSize );
	static BOOL WriteAllBinary( CString filePath, LPVOID buf, UINT64 len);
	//��ȡӦ�ó���Ŀ¼
	static CString GetAppPath();
	static CVersion GetProductVersion();
};
	
}