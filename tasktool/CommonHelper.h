#pragma once
#include "OperatingSystem.h"

//#define  LITTLE_ENDIAN
//#if defined(BIG_ENDIAN) && !defined(LITTLE_ENDIAN)
//#define htons(A)   (A)
//#define htonl(A)     (A)
//#define ntohs(A)   (A)
//#define ntohl(A)    (A)
//
//#elif defined(LITTLE_ENDIAN) && !defined(BIG_ENDIAN)
//#define htons(A)     ((((uint16)(A) & 0xff00) >> 8) | \
//	(((uint16)(A) & 0x00ff) << 8))
//#define htonl(A)     ((((uint32)(A) & 0xff000000) >> 24) | \
//	(((uint32)(A) & 0x00ff0000) >> 8) | \
//	(((uint32)(A) & 0x0000ff00) << 8) | \
//	(((uint32)(A) & 0x000000ff) << 24))
//#define ntohs htons
//#define ntohl htohl

//#else
//#error "Either BIG_ENDIAN or LITTLE_ENDIAN must be #defined, but not both."
//#endif

namespace pcutil
{


typedef std::basic_string< TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > tstring; 
class WEAVERLIB_API_UTILS CommonHelper
{

public:
	static BOOL SplitCStringToArray( const CString &str, const CString &strDim, std::vector<CString> &arrRet,BOOL bAddZeroLengthItem = FALSE );
	static BOOL SplitCStringToArray2( const CString &str, const CString &strDim, std::vector<CString> &arrRet,BOOL bAddZeroLengthItem = FALSE );
    static BOOL SplitCStringToArrayeEx( const CString &str, const CString &strDim, std::vector<CString> &arrRet);
	static CString FormatTipText(const CString& tipText);
    
    static int ComputeDimNumber( const CString &str, const CString &strDim );
	static CString ConvertUtf8ToLocal(const std::string& strUtf8) ;
	static CString ConvertUtf8ToLocal( const char* strUtf8 );
	static std::string ConvertLocalToUtf8( LPCTSTR lpLocal );
	static std::string ConvertLocalToUtf8(const CString& strLocal) ;
	static void Trim( CString& strSrc , const CString &strDim ) ;
	static CString URLEncode( const CString &strURL );
	static CString HtmlEncode( const CString &strHtml );
	static CString HtmlDecode( const CString &strHtml );
	static CString HtmlDecodeForBlog( const CString &strHtml );
	static CString StringFormat( const TCHAR* format , va_list ap );
	static CString LoadString( UINT id , HINSTANCE hRes = NULL );
    static int ComputeStringLen( const CString& str );
    static void LeftString(CString& str,const int count);
	static CString GetPaddingStringEndEllipsis(CString text, int maxlen);
	static void SBCToDBC(CString &string);
	static void CRToLF( CString& body );
	static void DeleteSpace(CString &string);
	static BOOL IsPasswordStr(CString string);
	static BOOL IsDigitStr(const CString &string);
	inline static BOOL StartsWith(const CString& src,const TCHAR* des)
	{
		return NULL==des? FALSE : 0 == src.Find(des);
	}

    static CString TranslateFetionGameParam(CString param);

	static BOOL WCharToMByte(LPCWSTR lpcwszStr, LPSTR& lpszStr, DWORD WChardwSize);

	static BOOL MByteToWChar(LPCSTR lpcszStr, LPWSTR* lpwszStr, DWORD MBytedwSize);
	static CString URLEncodeEx( const CString& str );
	static CString URLDecodeEx( const CString& str  );
	static CString URLEncodeForIKan( const CString& str );
	static CString GetFileNameFromPath( CString fileFullPath );
	static void CStringTostring( const CString& src, std::string& des );
	static CString GetWidthFixedString( CString src,int width,HFONT font);

	static CString GetCutShortFileName(const CString& strFileName, int iLengthLimit = 32, int iLeftLimit = 20, int iRightLimit = 4);
	static CString GetRenameRiskNameFile(const CString& strRiskFile);
	static BOOL IsRenameRiskNameFile(const CString& strRiskFile);
	static CString GetGroupShareFileInfo(const CString& strFileName, const CString& strFileSize);

	static CString ToString( int arg , int radix = 10 );
	static CString UInt32ToString( unsigned int arg , int radix = 10 );
	static CString ToString( __int64 arg , int radix = 10 );
	static CString UInt64ToString( unsigned __int64 arg , int radix = 10 );
	static BOOL ToBOOL( const CString& str );

	static DWORD TimeDiff( DWORD dwCur , DWORD dwStart );
	static UUID GenerateUUID();
	static UUID GenerateUUID( std::string &result );
	static UUID GenerateUUID( CString &result );

	static CString ToCString( std::string& str );
	static std::string  ToString( CString & str );
	static std::string IntToString(int arg);
	static CString UInt64ToCString(UINT64 arg);
	static INT64 CStringToInt64(CString str);
	static int CStringToInt(CString str);

	static BOOL IsTextUTF8( LPSTR lpBuffer, int iBufSize );
	static COperatingSystem::EOSVersion GetOSVersion();

	static LONG CommonHelper::GetRegKey( HKEY key, LPCTSTR subkey, LPTSTR retdata );
	static CString CommonHelper::GetRegKeyVal( HKEY key, LPCTSTR subkey, LPTSTR valName );
	static HINSTANCE CommonHelper::GotoURL( LPCTSTR url, int showcmd );

	
	static tstring		AToW(const char* pszA, unsigned int uCodePage);
	static std::string	WToA(const wchar_t* pszW, unsigned int uCodePage);
	static BOOL	EncryptFile(tstring srcfile,tstring dstfile,tstring password,BOOL bEncrypt = TRUE);
	static CString CommonHelper::DecodeString( const CString& str );
	static CString EncodeString( const CString& str );

	static BOOL  GetMacAddress( CString& mac );

private:
	DISALLOW_IMPLICIT_CONSTRUCTORS(CommonHelper);

};






}