#include "Stdafx.h"
#include "FileHelper.h"
#include "atlfile.h"
#include "atlpath.h"
#include "CommonHelper.h"
#include <WinVer.h>
#pragma comment( lib , "Version.lib" ) 
namespace pcutil
{

BOOL CFileHelper::IsFileExists( CString filePath )
{
	return ATLPath::FileExists( filePath.GetBuffer() );
}

CString CFileHelper::GetFileName( CString filePath )
{
	return ATLPath::FindFileName( filePath.GetBuffer() );
}

BOOL CFileHelper::IsDictoryExists( CString path )
{
	return ATLPath::IsDirectory( path.GetBuffer() );

	DWORD dwAttr = ::GetFileAttributes( path );
	if ( dwAttr == 0xFFFFFFFF )
	{
		return FALSE;
	}
	if ( ( dwAttr & FILE_ATTRIBUTE_DIRECTORY ) > 0 )
	{
		return TRUE;
	}
	return FALSE;	
}

CString CFileHelper::GetFileExtention( CString filePath )
{
	return ATLPath::FindExtension( filePath );
}

BOOL CFileHelper::Delete( CString filePath )
{
	return DeleteFile( filePath );
}

void CFileHelper::DeleteFiles( CString folderPath, BOOL deleteSubFolder  )
{
	std::string str = CommonHelper::ToString( folderPath );
	const char* tmp = str.c_str() ;

	return ( DeleteFiles( const_cast<char*>( tmp ), deleteSubFolder ));
}

void CFileHelper::DeleteFiles( char* folderPath, BOOL deleteSubFolder  )
{
	char fileFound[256];
	WIN32_FIND_DATAA info;
	HANDLE hp; 
	sprintf_s( fileFound, "%s\\*.*", folderPath );
	hp = FindFirstFileA( fileFound, &info );
	do
	{
		if ( !(( strcmp(info.cFileName, "." )==0)|| ( strcmp( info.cFileName, ".." )==0 )) )
		{
			if( ( info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY )
			{
				if ( deleteSubFolder )
				{
					string subFolder = folderPath;
					subFolder.append( "\\" );
					subFolder.append( info.cFileName );
					DeleteFiles( (char*)subFolder.c_str(), deleteSubFolder );
					::RemoveDirectoryA( subFolder.c_str() );
				}				
			}
			else
			{
				sprintf_s( fileFound,"%s\\%s", folderPath, info.cFileName );
				BOOL retVal = ::DeleteFileA( fileFound );
			}
		}

	}while( FindNextFileA( hp, &info ) ); 
	::FindClose(hp);
}

//bool CFileHelper::SHDeleteFiles(CString strFrom)
//{
//	TCHAR pFromTemp[MAX_PATH] = {0};  
//	_tcscpy(pFromTemp,strFrom.GetBuffer());  
//
//	SHFILEOPSTRUCT FileOp = {0};  
//	FileOp.fFlags = FOF_SILENT|FOF_NOCONFIRMATION|FOF_MULTIDESTFILES;  
//	if(false)  
//	{  
//		FileOp.fFlags |= FOF_ALLOWUNDO;  
//	}  
//	FileOp.hNameMappings = NULL;  
//	FileOp.hwnd = NULL;  
//	FileOp.lpszProgressTitle = NULL;  
//	FileOp.pFrom = pFromTemp;  
//	FileOp.pTo = NULL;  
//	FileOp.wFunc = FO_DELETE;  
//
//	return SHFileOperation(&FileOp) == 0;
//}

void CFileHelper::Move( CString sourceFilePath, CString destFilePath )
{
	::DeleteFile( destFilePath );
	::MoveFile( sourceFilePath, destFilePath );
}

void CFileHelper::Copy( CString sourceFilePath, CString destFilePath )
{
	::CopyFile( sourceFilePath, destFilePath, FALSE );
}


UINT64 CFileHelper::GetFileSize( CString filePath )
{
	CAtlFile file;
	HRESULT ret = file.Create( filePath, FILE_READ_ATTRIBUTES , FILE_SHARE_READ, OPEN_EXISTING );

	if ( !SUCCEEDED(ret) )
		return 0;

	UINT64 size;
	if ( file.GetSize( size ) == S_OK )
		return size;
	else
		return 0;
}

CString CFileHelper::GetApplicationDataPath()
{
	CString strPath;
	HRESULT hr = SHGetFolderPath( NULL , CSIDL_APPDATA , NULL , SHGFP_TYPE_CURRENT , strPath.GetBuffer( MAX_PATH + 1 ) );
	strPath.ReleaseBuffer();
	ATLASSERT( hr == S_OK );
	if( strPath.Right(1) !=  _T("\\") )
	{
		strPath.Append( _T("\\") );
	}
	return strPath;
}

CString CFileHelper::GetMyPicturePath()
{
	CString strPath;
	HRESULT hr = SHGetFolderPath( NULL , CSIDL_MYPICTURES , NULL , SHGFP_TYPE_CURRENT , strPath.GetBuffer( MAX_PATH + 1 ) );
	strPath.ReleaseBuffer();
	ATLASSERT( hr == S_OK );
	if( strPath.Right(1) !=  _T("\\") )
	{
		strPath.Append( _T("\\") );
	}
	return strPath;
}

BOOL CFileHelper::CreateDir( CString path )
{
	std::string str = CommonHelper::ToString( path );
	const char* tmp = str.c_str() ;
	//return CreateDir("c:\\fmj\\fmj1\\fmj2");
	return ( CreateDir( const_cast<char*>( tmp ) ) );
}


BOOL CFileHelper::CreateDir( char* path )
{
	char DirName[256];
	char* p = path;
	char* q = DirName; 
	while(*p)
	{
		if (('\\' == *p) || ('/' == *p))
		{
			if (':' != *(p-1))
			{
				if( !ATLPath::IsDirectory( DirName ) )
				{
					::CreateDirectoryA(DirName, NULL);
				}				
			}
		}
		*q++ = *p++;
		*q = '\0';
	}
	::CreateDirectoryA(DirName, NULL);
	return TRUE;
}


//utf8和unicode格式尚未单元测试 FMJ
BOOL CFileHelper::WriteAllText( CString filePath, CString& str, FileEncoding encoding /*= FILE_ENCODING_ANSI */ )
{
	CAtlFile file;
	HRESULT ret = file.Create( filePath, FILE_WRITE_DATA, FILE_SHARE_WRITE,CREATE_ALWAYS );
	if ( !SUCCEEDED( ret ) )
		return FALSE;	
	switch ( encoding )
	{
	case FILE_ENCODING_UNICODE:
		{
			//初始化UNICODE bom
			unsigned char bom[2] = {  0xff, 0xfe };
			file.Write( &bom, 2 );
			file.Write( str.GetBuffer(), str.GetLength()*2 );
		}
		break;
	case FILE_ENCODING_UTF8:
		{
			std::string utf = CommonHelper::ConvertLocalToUtf8( str );
			//初始化utf8bom
			unsigned char bom[3] = { 0xef, 0xbb, 0xbf };
			file.Write( &bom, 3 );
			file.Write( utf.c_str(), strlen( utf.c_str() ));
		}
		break;
	default:	//默认为ANSI格式
		{
			int length = str.GetLength()*2 + 1;
			char* pBuffer = new char[length]();
			ZeroMemory( pBuffer,length );
			memcpy( pBuffer, CT2A( str ).m_psz, length  );
			ret = file.Write( pBuffer, strlen(pBuffer) );
			delete pBuffer;
		}
	}

	if ( !SUCCEEDED( ret ) )
		return FALSE;

	return ret;
}


CString CFileHelper::ReadAllText( CString filePath, FileEncoding encoding /*= FILE_ENCODING_ANSI */ )
{
	char* pBuffer = NULL;
	ULONGLONG buffSize;
	if( !ReadAllBinary( filePath, pBuffer, buffSize ) )
	{
		return _T("");
	}
	return ReadAllText( pBuffer, buffSize, encoding );
}

//utf8和unicode格式尚未单元测试 FMJ
CString CFileHelper::ReadAllText( char* &pBuffer,ULONGLONG& buffSize, FileEncoding encoding )
{
	CString result;
	switch ( encoding )
	{
	case FILE_ENCODING_UNICODE:
		{
			ATLASSERT( pBuffer[0] == 0xff && pBuffer[1] == 0xfe );
			wchar_t* pwstr = reinterpret_cast<wchar_t*>( pBuffer );
			//此处应该需要做LITTLE_ENDIAN的判断 否则可能忽悠bug
			if ( *pwstr == 0xfeff )
				return ( pwstr + 1 );
			else
				return pwstr;
		}
		break;
	case FILE_ENCODING_UTF8:
		{
			const unsigned char* pUBuffer = (const unsigned char*)pBuffer;
			if(  pUBuffer[0] == 0xef && pUBuffer[1] == 0xbb && pUBuffer[2] == 0xbf )
			{
				pBuffer += 3;
			}
			return CommonHelper::ConvertUtf8ToLocal( pBuffer );
		}
		break;
	default:	//默认为ANSI格式
		{
			if( CommonHelper::IsTextUTF8( pBuffer , (int)buffSize ) )
			{
				return ReadAllText( pBuffer, FILE_ENCODING_UTF8);
			}
			result = CA2T(pBuffer).m_psz;
		}
	}
	delete pBuffer;
	return result;
}

BOOL CFileHelper::ReadAllBinary( CString filePath, OUT char*& buffer, OUT ULONGLONG& buffSize )
{
	if( !IsFileExists( filePath ) )
	{
		return NULL;
	}

	CAtlFile file;
	HRESULT ret = file.Create( filePath, FILE_READ_DATA, FILE_SHARE_READ,OPEN_EXISTING );
	if( ret != S_OK )
	{
		return NULL;
	}

	if ( S_OK != file.GetSize( buffSize ))
	{
		file.Close();
		return NULL;
	}
	if( buffer != NULL)
	{
		delete buffer;
	}
	//bufferSize 以0结尾
	buffer = new char[(int)buffSize + 1]();
	ZeroMemory( buffer,(int)buffSize + 1 );

	DWORD readSize = 0;
	file.Read( buffer, (DWORD)buffSize, readSize );
	ATLASSERT( buffSize == readSize );

	return TRUE;
}

BOOL CFileHelper::WriteAllBinary( CString filePath, LPVOID buf, UINT64 len)
{
	CAtlFile file;
	HRESULT ret = file.Create( filePath, FILE_WRITE_DATA, FILE_SHARE_WRITE, CREATE_ALWAYS );
	if ( !SUCCEEDED( ret ) )
		return FALSE;	
	try
	{
		ret = file.Write(buf, (DWORD)len);
	}
	catch (...)
	{
		return FALSE;
	}

	if ( !SUCCEEDED( ret ) )
		return FALSE;

	return TRUE;
}

CString CFileHelper::GetAppPath()
{
	CString strPath;
	TCHAR *szBuffer = strPath.GetBuffer( MAX_PATH + 1 );
	::GetModuleFileName( NULL, szBuffer, MAX_PATH );
	strPath.ReleaseBuffer();
	int nFind = strPath.ReverseFind( '\\' );
	return strPath.Left( nFind + 1 );
}

pcutil::CVersion CFileHelper::GetProductVersion()
{
	CString strPath;
	TCHAR *szBuffer = strPath.GetBuffer( MAX_PATH + 1 );
	::GetModuleFileName( NULL, szBuffer, MAX_PATH );
	strPath.ReleaseBuffer();
	DWORD fileInfoSize = ::GetFileVersionInfoSize( strPath, NULL );

	char* fileInfo = new char[fileInfoSize];
	if( ::GetFileVersionInfo( strPath, NULL, fileInfoSize, fileInfo ) != 0 )
	{
		VS_FIXEDFILEINFO *versionInfo;
		unsigned int len;
		if(::VerQueryValue(fileInfo, _T("\\"), (LPVOID*)&versionInfo, &len) != 0)
		{
			WORD major = HIWORD(versionInfo->dwProductVersionMS);
			WORD minor = LOWORD(versionInfo->dwProductVersionMS);
			WORD build = HIWORD(versionInfo->dwProductVersionLS);

			WORD branch = LOWORD(versionInfo->dwProductVersionLS);

			return CVersion( major, minor, build, branch );
		}
	}
	return CVersion( 0,0,0 );
}


}