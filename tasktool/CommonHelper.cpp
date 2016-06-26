#include "StdAfx.h"
#include "CommonHelper.h"
#include "OperatingSystem.h"
#include <Iphlpapi.h>

#pragma comment(lib, "Iphlpapi.lib")

namespace pcutil
{

namespace detail_stringhelper
{
static CString DecToHex( TCHAR num, int radix )
{
    int temp = 0;
    CString csTmp;
    int num_char = ( int )num;
    if ( num_char < 0 )
    {
        num_char = 256 + num_char;
    }

    const TCHAR hexVals[16] = { _T( '0' ), _T( '1' ), _T( '2' ), _T( '3' ), _T( '4' ), _T( '5' ), _T( '6' ), _T( '7' ), _T( '8' ), _T( '9' ), _T( 'A' ), _T( 'B' ), _T( 'C' ), _T( 'D' ), _T( 'E' ), _T( 'F' ) };
    while ( num_char >= radix )
    {
        temp = num_char % radix;
        num_char = /*floor( */num_char / radix /*)*/;
        csTmp = hexVals[temp];
    }

    csTmp += hexVals[num_char];

    if ( csTmp.GetLength() < 2 )
    {
        csTmp += _T( '0' );
    }

    // Reverse the String
    CString strdecToHex( csTmp );
    strdecToHex.MakeReverse();

    return strdecToHex;
}

static char CharToInt(char ch)
{
	if(ch>='0' && ch<='9')return (char)(ch-'0');
	if(ch>='a' && ch<='f')return (char)(ch-'a'+10);
	if(ch>='A' && ch<='F')return (char)(ch-'A'+10);
	return -1;
}
static char StrToBin(char *str)
{
	char tempWord[2];
	char chn;

	tempWord[0] = CharToInt(str[0]);                         //make the B to 11 -- 00001011
	tempWord[1] = CharToInt(str[1]);                         //make the 0 to 0  -- 00000000

	chn = (tempWord[0] << 4) | tempWord[1];                //to change the BO to 10110000

	return chn;
}
static bool isSafe( char ch )
{
	if ((((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z'))) || ((ch >= '0') && (ch <= '9')))
	{
		return true;
	}
	switch (ch)
	{
	case '\'':
	case '(':
	case ')':
	case '*':
	case '-':
	case '.':
	case '_':
	case '!':
		return true;
	}
	return false;
}

static bool isSafeEx( char ch )
{
	if ((((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z'))) || ((ch >= '0') && (ch <= '9')))
	{
		return true;
	}
	switch (ch)
	{
	case '-':
	case '.':
	case '_':
	case '~':
		return true;
	}
	return false;
}

}
CString CommonHelper::URLEncodeEx( const CString& str )
{
	std::string tt = ConvertLocalToUtf8( str );
	std::string dd;
	size_t len=tt.length();
	for (size_t i=0;i<len;i++)
	{
		if( detail_stringhelper::isSafe((BYTE)tt.at(i)))
		{
			char tempbuff[2]={0};
			sprintf(tempbuff,"%c",(BYTE)tt.at(i));
			dd.append(tempbuff);
		}
		else if (isspace((BYTE)tt.at(i)))
		{
			dd.append("+");
		}
		else
		{
			char tempbuff[4];
			sprintf(tempbuff,"%%%X%X",((BYTE)tt.at(i)) >>4,((BYTE)tt.at(i)) %16);
			dd.append(tempbuff);
		}

	}
	CString result = CA2T( dd.c_str() );
	return result;


}


CString CommonHelper::URLEncodeForIKan( const CString& str )
{
	std::string tt = ConvertLocalToUtf8( str );
	std::string dd;
	size_t len=tt.length();
	for (size_t i=0;i<len;i++)
	{
		if( detail_stringhelper::isSafeEx((BYTE)tt.at(i)))
		{
			char tempbuff[2]={0};
			sprintf(tempbuff,"%c",(BYTE)tt.at(i));
			dd.append(tempbuff);
		}
		else if (isspace((BYTE)tt.at(i)))
		{
			dd.append("%20");
		}
		else
		{
			char tempbuff[4];
			sprintf(tempbuff,"%%%X%X",((BYTE)tt.at(i)) >>4,((BYTE)tt.at(i)) %16);
			dd.append(tempbuff);
		}

	}
	CString result = CA2T( dd.c_str() );
	return result;
}

CString CommonHelper::URLDecodeEx( const CString& str )
{
	std::string inputstr = ConvertLocalToUtf8( str );
	std::string output="";
	char tmp[2];
	int i=0,idx=0,len=inputstr.length();

	while(i<len){
		if(inputstr[i]=='%'){
			tmp[0]=inputstr[i+1];
			tmp[1]=inputstr[i+2];
			output += detail_stringhelper::StrToBin(tmp);
			i=i+3;
		}
		else if(inputstr[i]=='+'){
			output+=' ';
			i++;
		}
		else{
			output+=inputstr[i];
			i++;
		}
	}
	CString result = ConvertUtf8ToLocal( output );
	return result;

}
int CommonHelper::ComputeDimNumber( const CString &str, const CString &strDim )
{
	int nDimLen = strDim.GetLength();
	if ( nDimLen <= 0 ) return FALSE;
	std::auto_ptr<CString> pString( new CString( str ) );
	if ( pString.get() == NULL ) return FALSE;
	CString &strSrc = *pString;
	strSrc += strDim;
	DWORD nLoop = 0;
	do
	{
		int iRet = strSrc.Find( strDim );
		if ( iRet >= 0 )
		{
			int iLeft = strSrc.GetLength() - iRet - nDimLen;
			if ( iLeft <= 0 )
				break;
			strSrc = strSrc.Right( iLeft );
		}
		if ( ++nLoop > 500000 ) //add this avoid dead loop
			return FALSE;
	}
	while ( pString->Find( strDim ) >= 0 );
	return nLoop;
}

CString CommonHelper::FormatTipText(const CString& tipText)
{
    CString newtips = _T("");
    std::vector<CString> tipArray;
    CommonHelper::SplitCStringToArray( tipText, _T("\r\n"), tipArray);
    for ( int i = 0; i < tipArray.size(); i++ )
    {
        CString oldTips = tipArray[i] ;
        while( !oldTips.IsEmpty() )
        {
            int lineCharCount = 0;
            do 
            {
                if( oldTips[0] >= 0 && oldTips[0] < 0x80 ) 
                { 
                    lineCharCount ++; 
                }
                else 
                { 
                    if (lineCharCount == 39)
                    {
                        break;
                    }
                    lineCharCount += 2; 
                }
                newtips.Append( oldTips.Left(1) );
                oldTips = oldTips.Right( oldTips.GetLength()-1 );
            } while( lineCharCount < 40 );

            if( oldTips.GetLength() > 0 ) 
            { 
                newtips.Append( _T("\r\n") ); 
            }
        }
        if ( i < tipArray.size()-1 )
            newtips.Append( _T("\r\n") ); 
    }
    return newtips;
}

BOOL CommonHelper::SplitCStringToArrayeEx( const CString &str, const CString &strDim, std::vector<CString> &arrRet) /*增加了默认项*/
{
 
    int startPos = 0;
    if( !str.IsEmpty() )
    {
        int pos = -1;
        int lastPos = -1;
        int strDimLength = strDim.GetLength();
        while((pos = str.Find(strDim, startPos)) != -1)
        {
            CString file = str.Left(pos).Right(pos - startPos);

            if(!file.IsEmpty())
                arrRet.push_back(file);
            startPos = pos + strDimLength;
        }
        arrRet.push_back(str.Right(str.GetLength() - startPos));
    }
    return TRUE;
}

BOOL CommonHelper::SplitCStringToArray( const CString &str,
                                        const CString &strDim,
                                        std::vector<CString> &arrRet,
										BOOL bAddZeroLengthItem )
{
    arrRet.clear();
	//CString resToken;
	//int curPos = 0;
	//resToken= str.Tokenize( strDim ,curPos );
	//while( resToken != _T("") )
	//{
	//   arrRet.push_back( resToken );
	//   resToken = str.Tokenize( strDim, curPos );
	//}   
	//return TRUE;

    int nDimLen = strDim.GetLength();
    if ( nDimLen <= 0 ) return FALSE;
    std::auto_ptr<CString> pString( new CString( str ) );
    if ( pString.get() == NULL ) return FALSE;
    CString &strSrc = *pString;
    strSrc += strDim;
    DWORD nLoop = 0;
    do
    {
        int iRet = strSrc.Find( strDim );
        if ( iRet >= 0 )
        {
            CString strPart = strSrc.Left( iRet ) ;
            if ( strPart.GetLength() > 0 || bAddZeroLengthItem )
            {
                arrRet.push_back( ( LPCTSTR )strPart );
            }

            int iLeft = strSrc.GetLength() - iRet - nDimLen;
            if ( iLeft <= 0 )
                break;
            strSrc = strSrc.Right( iLeft );
        }
        if ( ++nLoop > 500000 ) //add this avoid dead loop
            return FALSE;
    }
    while ( pString->Find( strDim ) >= 0 );
    return TRUE;
}


BOOL CommonHelper::SplitCStringToArray2( const CString &str,
									   const CString &strDim,
									   std::vector<CString> &arrRet,
									   BOOL bAddZeroLengthItem )//联系人分组使用
{
	arrRet.clear();;

	int nDimLen = strDim.GetLength();
	if ( nDimLen <= 0 ) return FALSE;
	std::auto_ptr<CString> pString( new CString( str ) );
	if ( pString.get() == NULL ) return FALSE;
	CString &strSrc = *pString;
	strSrc += strDim;
	DWORD nLoop = 0;
	do
	{
		int iRet = strSrc.Find( strDim );
		if ( iRet >= 0 )
		{
			CString strPart = strSrc.Left( iRet ) ;
			if ( strPart.GetLength() >= 0 || bAddZeroLengthItem )//=0的时候是默认分组，转换为空
			{
				arrRet.push_back( ( LPCTSTR )strPart );
			}

			int iLeft = strSrc.GetLength() - iRet - nDimLen;
			if ( iLeft <= 0 )
				break;
			strSrc = strSrc.Right( iLeft );
		}
		if ( ++nLoop > 500000 ) //add this avoid dead loop
			return FALSE;
	}
	while ( pString->Find( strDim ) >= 0 );
	return TRUE;
}

std::string CommonHelper::ConvertLocalToUtf8( LPCTSTR lpLocal )
{
	// 对输入参数的有效性进行判断，如果输入NULL，返回
	if ( !lpLocal || 0 == wcslen( lpLocal ) )
		return std::string();
	CAutoBuffer szUtf8;
	int len = 0;
	len = WideCharToMultiByte( CP_UTF8, 0, lpLocal, -1, NULL, 0, NULL, NULL );
	if ( !szUtf8.AllocateBuffer( len - 1 ) ) return std::string();
	WideCharToMultiByte ( CP_UTF8, 0, lpLocal, -1, szUtf8.GetBuffer() , len - 1, NULL, NULL );
	return szUtf8.ToString();
}

std::string CommonHelper::ConvertLocalToUtf8( const CString& strLocal )
{
	
	if ( strLocal.IsEmpty() )
		return std::string();
	else
		return CommonHelper::ConvertLocalToUtf8( ( LPCTSTR )strLocal );
}


CString CommonHelper::ConvertUtf8ToLocal( const std::string& strUtf8 )
{
	return CommonHelper::ConvertUtf8ToLocal( strUtf8.c_str() );
}

CString CommonHelper::ConvertUtf8ToLocal( const char* strUtf8 )
{
	int len = MultiByteToWideChar( CP_UTF8, 0, strUtf8, -1, NULL, 0 );
	if ( len == 0 )
		return CString();
	CString ret;
	LPWSTR lpBuffer = ret.GetBuffer( len );
	MultiByteToWideChar( CP_UTF8, 0, strUtf8, -1, lpBuffer, len );
	ret.ReleaseBuffer();
	return ret;
}


void CommonHelper::Trim( CString& strSrc , const CString &strDim )
{
    int nDimLen = strDim.GetLength();
    if ( nDimLen == 0 ) return ;

    //trim left
    int pos = strSrc.Find( strDim );
    while ( pos == 0 )
    {
        strSrc = strSrc.Mid( pos + 1 );
        pos = strSrc.Find( strDim );
    }

	// 2010.6.12
	// 替换完成左侧之后，需要判断此时字符串长度是否还大于替换字符串，避免std异常
	if (strSrc.GetLength() < strDim.GetLength())
		return;

    //trim right
    pos = strSrc.Find( strDim , strSrc.GetLength() - nDimLen );
    while ( pos == strSrc.GetLength() - nDimLen )
    {
        strSrc = strSrc.Left( pos );
        pos = strSrc.Find( strDim , strSrc.GetLength() - nDimLen );
    }

}

CString CommonHelper::URLEncode( const CString &strURL )
{
	// 由于此方法没有考虑到unicode转换的问题，所以逻辑上改为URLEncodeEx
	// 在空格的处理上，保留原来URLEncode的方式，省略空格字符
	std::string tt = ConvertLocalToUtf8( strURL );
	std::string dd;
	size_t len=tt.length();
	for (size_t i=0;i<len;i++)
	{
		if( detail_stringhelper::isSafe((BYTE)tt.at(i)))
		{
			char tempbuff[2]={0};
			sprintf(tempbuff,"%c",(BYTE)tt.at(i));
			dd.append(tempbuff);
		}
		else if (isspace((BYTE)tt.at(i)))
		{
			dd.append("%20");
		}
		else
		{
			char tempbuff[4];
			sprintf(tempbuff,"%%%X%X",((BYTE)tt.at(i)) >>4,((BYTE)tt.at(i)) %16);
			dd.append(tempbuff);
		}

	}
	CString result = CA2T( dd.c_str() );
	return result;
}

CString CommonHelper::HtmlEncode( const CString &strHtml )
{
    CString strEncode = strHtml;
    strEncode.Replace( _T( "&" ), _T( "&amp;" ) );
    strEncode.Replace( _T( "\"" ), _T( "&quot;" ) );
    strEncode.Replace( _T( "<" ), _T( "&lt;" ) );
    strEncode.Replace( _T( ">" ), _T( "&gt;" ) );
    //strEncode.Replace( _T( " " ), _T( "&nbsp;" ) );
    return strEncode;
}

CString CommonHelper::HtmlDecode( const CString &strHtml )
{
    CString strDecode = strHtml;
    strDecode.Replace(  _T( "&quot;" ) , _T( "\"" ) );
    strDecode.Replace(  _T( "&lt;" ) ,_T( "<" ) );
    strDecode.Replace(	_T( "&gt;" ) , _T( ">" ) );
    //strDecode.Replace(  _T( "&nbsp;" ) , _T( " " ) );
	strDecode.Replace(  _T( "&amp;" ) , _T( "&" ) );
    return strDecode;
}
CString CommonHelper::HtmlDecodeForBlog( const CString &strHtml )
{
	CString strDecode = strHtml;
	strDecode.Replace(  _T( "&quot;" ) , _T( "\"" ) );
	strDecode.Replace(  _T( "&lt;" ) ,_T( "<" ) );
	strDecode.Replace(	_T( "&gt;" ) , _T( ">" ) );
	//strDecode.Replace(  _T( "&nbsp;" ) , _T( " " ) );
	strDecode.Replace(  _T( "&amp;" ) , _T( "&" ) );
	strDecode.Replace(  _T( "&#183;" ) , _T( "·" ) );
	return strDecode;
}
static int vsnprintfT( TCHAR* buffer,
                       size_t size,
                       const TCHAR* format,
                       va_list arguments )
{

#ifndef _UNICODE
	int length = vsnprintf_s(buffer, size, size - 1, format, arguments);
	if (length < 0)
		return _vscprintf(format, arguments);
	return length;
#else
	int length = _vsnwprintf_s(buffer, size, size - 1, format, arguments);
	if (length < 0)
		return _vscwprintf(format, arguments);
	return length;
#endif
}

CString CommonHelper::StringFormat( const TCHAR* format , va_list ap )
{
	CString strRet;    
	TCHAR stack_buf[1024 * 4];

	// 验证输入参数的有效性
	if (!format)
		return strRet;

    va_list backup_ap = ap;
    int result = vsnprintfT( stack_buf, ARRAY_SIZE( stack_buf ), format, backup_ap );
    va_end( backup_ap );
    if ( result >= 0 && result < static_cast<int>( ARRAY_SIZE( stack_buf ) ) )
    {
        strRet.Append( stack_buf, result );
        return strRet;
    }

    // Repeatedly increase buffer size until it fits.
    int mem_length = ARRAY_SIZE( stack_buf );
    while ( true )
    {
        if ( result < 0 )
        {
			ATLASSERT( FALSE );
			return strRet;
        }
        else
        {
            mem_length *= 2;
        }
        if ( mem_length > 10 * 1024 * 1024 )
        {
            return strRet;
        }
        std::vector<TCHAR> mem_buf( mem_length );
        backup_ap = ap ;
        result = vsnprintfT( &mem_buf[0], mem_length, format, ap );
        va_end( backup_ap );
        if ( ( result >= 0 ) && ( result < mem_length ) )
        {
            strRet.Append( &mem_buf[0], result );
            return strRet;
        }
    }
}

CString CommonHelper::LoadString( UINT id , HINSTANCE hRes )
{
	CString strText;
	if( hRes != NULL )
	{
		strText.LoadString( hRes , id );
	}
	else 
	{
		strText.LoadString( id );
	}
	return strText;
}

int CommonHelper::ComputeStringLen( const CString& str )
{
    int i;
    int j;
    j = 0;
    for(i = 0; i < str.GetLength(); i++)
    {
        if( str.GetAt( i ) > 127 )
        {
            j = j + 2;
        }
        else
        {
            j++;
        }
    }
    return j; 
}

void CommonHelper::LeftString(CString& str,const int count)
{
    int index = str.GetLength();
    if (CommonHelper::ComputeStringLen(str) > count)
    {
        int j = 0;
        for(int i = 0; i < str.GetLength(); i++)
        {
            if( str.GetAt( i ) > 127 )
            {
                j = j + 2;
            }
            else
            {
                j++;
            }
            if (j > count)
            {
                index = i;
                break;
            }
        }
        str = str.Left(index);
    }
}

CString CommonHelper::GetPaddingStringEndEllipsis(CString text, int maxlen)
{
	CString padding = _T("...");
	if (text.GetLength() <=0 || text.GetLength() <= maxlen)
		return text;

	int n = maxlen - padding.GetLength();
	if (n <= 0)
		return text.Left(maxlen);

	return text.Left(n)+ padding;
}

void CommonHelper::SBCToDBC(CString &string)
{
	int nLen = string.GetLength();
	for (int i=0; i<nLen; i++)
	{
		wchar_t c = string.GetAt(i);
		// 有空格
		if(c==12288){
			c = (wchar_t)32;
		}
		else if (c > 65280 && c < 65375)    
		{    
			c = (wchar_t)(c - 65248);                    
		}  
		else
		{
			continue;
		}
		string.SetAt(i,c);
	}
} 

void CommonHelper::DeleteSpace(CString &string)
{
	string.TrimLeft();   
	string.TrimRight();
}
BOOL CommonHelper::IsPasswordStr(CString string)
{
	int i = 0;
	while(string[i])
	{
		int nKeyValue = (int)string[i];
		// 超出范围的字符串返回错误，修改判断&&为||
		if (nKeyValue < 0x21 || nKeyValue >= 0x7f)
		{
			return FALSE;
		}
		i++;
	}
	return TRUE;
}

BOOL CommonHelper::IsDigitStr(const CString &string)
{
	int i = 0;
	while(string[i])
	{
		if ( isdigit(string[i]) == 0 )
		{
			return FALSE;
		}
		i++;
	}
	return TRUE;
}

BOOL CommonHelper::WCharToMByte( LPCWSTR lpcwszStr, LPSTR& lpszStr, DWORD WChardwSize )
{
	DWORD dwMinSize;
	dwMinSize = WideCharToMultiByte(CP_OEMCP,NULL,lpcwszStr,-1,NULL,0,NULL,FALSE);

	if(lpszStr!=NULL)
	{
		delete [](lpszStr);
		lpszStr = NULL;
	}
	lpszStr = new char[dwMinSize];
	WideCharToMultiByte(CP_OEMCP,NULL,lpcwszStr,-1,lpszStr,dwMinSize,NULL,FALSE);
	return TRUE;
}

BOOL CommonHelper::MByteToWChar( LPCSTR lpcszStr, LPWSTR* lpwszStr, DWORD MBytedwSize )
{
	// Get the required size of the buffer that receives the Unicode 
	// string. 
	DWORD dwMinSize;
	dwMinSize = MultiByteToWideChar (CP_ACP, 0, lpcszStr, -1, NULL, 0);

	if(*lpwszStr!=NULL)
	{
		delete [](*lpwszStr);
		*lpwszStr = NULL;
	}
	*lpwszStr = new wchar_t[dwMinSize];

	// Convert headers from ASCII to Unicode.
	MultiByteToWideChar (CP_ACP, 0, lpcszStr, -1, *lpwszStr, dwMinSize);  
	return TRUE;
}

CString CommonHelper::TranslateFetionGameParam(CString param)
{
    CString res = param;
    res.Replace(_T("\\"),_T("\\\\"));
    res.Replace(_T("|"),_T("\\|"));
    return res;
}

void CommonHelper::CRToLF( CString& body )
{
	if ( body.IsEmpty() )
	{
		return;
	}
	int i = body.Find('\r');
	if ( i <= 0 )
	{
		return;
	}
	else
	{
		if ( i < body.GetLength() && i > 0 )
		{
			body.SetAt(i,'\n');
		}
		CRToLF( body );
	}
}

CString CommonHelper::GetFileNameFromPath( CString fileFullPath )
{
	int index;
	index = fileFullPath.ReverseFind( _T('\\') );
	if( index == -1 )
	{
		return fileFullPath;
	}
	else
	{
		CString result;
		result = fileFullPath.Right( fileFullPath.GetLength() - index - 1 );
		return result;
	}
}

void CommonHelper::CStringTostring( const CString& src, std::string& des )
{
	int i;
	for( i = 0;i < src.GetLength(); i++ )
	{
		char ch;
		ch = src.GetAt( i );
		des.push_back( ch );
	}
}

CString CommonHelper::GetWidthFixedString( CString src,int width,HFONT font)
{
	return _T("");
}

CString CommonHelper::GetCutShortFileName(const CString& strFileName, int iLengthLimit/* = 32*/, int iLeftLimit/* = 20*/, int iRightLimit/* = 4*/)
{
	int iPos = strFileName.ReverseFind('.');
	if (iPos < 0)
	{
		//兼容没有后缀的文件名
		iPos = strFileName.GetLength();
	}
	int iLength = iLeftLimit + iRightLimit;
	int iLimit = iLengthLimit/2;
	if ((iPos <= iLimit)
		|| (iLengthLimit < 5)
		|| (iLength >= iLengthLimit))
	{
		return strFileName;
	}

	CString strName = strFileName.Left(iPos);
	int iSize = strName.GetLength();

	CString strLeft;
	int iCharCount = 0;
	for (int i=0; i<iSize; i++)
	{
		if(strName[i] >= 0 && strName[i] < 0x80)
		{
			if (iCharCount == iLeftLimit)
			{
				strLeft = strName.Left(i);
			}
			++iCharCount;
		}
		else
		{
			if ((iCharCount == (iLeftLimit-1)) || (iCharCount == iLeftLimit))
			{
				strLeft = strName.Left(i);
			}
			iCharCount += 2; 
		}
	}

	if (iCharCount <= iLengthLimit)
	{
		return strFileName;
	}

	CString strRight;
	int iRigthCount = 0;
	for (int j=iSize-1; j>=0; j--)
	{
		if(strName[j] >= 0 && strName[j] < 0x80)
		{
			if (iRigthCount == iRightLimit)
			{
				strRight = strFileName.Mid(j+1);
				break;
			}
			++iRigthCount;
		}
		else
		{
			if ((iRigthCount == (iRightLimit-1)) || (iRigthCount == iRightLimit))
			{
				strRight = strFileName.Mid(j+1);
				break;
			}
			iRigthCount += 2; 
		}
	}

	CString strDest;
	strDest.Format(_T("%s%s%s"), strLeft, _T("…"), strRight);
	return strDest;
}

CString CommonHelper::GetRenameRiskNameFile(const ATL::CString &strRiskFile)
{
	CString strNewName;// = strRiskFile  + CResourceManager::LoadString( _T("IDS_CONVERSATION_FILE_RENAME"));
	return strNewName;
}

BOOL CommonHelper::IsRenameRiskNameFile(const CString& strRiskFile)
{
	BOOL bFound = FALSE;
	//int nIndex = strRiskFile.ReverseFind(_T('.'));
	//if (-1 != nIndex)
	//{
	//	CString strExt = strRiskFile.Right(strRiskFile.GetLength() - nIndex);
	//	if (strExt == CResourceManager::LoadString( _T("IDS_CONVERSATION_FILE_RENAME")))
	//	{
	//		bFound = TRUE;
	//	}
	//}

	return bFound;
}

CString CommonHelper::GetGroupShareFileInfo(const CString& strFileName, const CString& strFileSize)
{
	CString strInfo;
	CString strShortFileName = CommonHelper::GetCutShortFileName(strFileName);
	//strInfo.Format(_T("%s(%s)"), strShortFileName, FileSysHelper::ConvertUlongFileSizeToShowString(_ttoi64(strFileSize)));
	return strInfo;
}

CString CommonHelper::ToString( int arg , int radix  )
{
	TCHAR buf[68] = {_T( '\0' )};
	_itot( arg , buf , radix );
	CString strRet = buf;
	return strRet;
}

CString CommonHelper::ToString( __int64 arg , int radix )
{
	TCHAR buf[68] = {_T( '\0' )};
	_i64tot( arg , buf , radix );
	CString strRet = buf;
	return strRet;
}


CString CommonHelper::UInt32ToString( unsigned int arg , int radix  )
{
	CString strRet;
	if ( radix == 10 )
	{
		strRet.Format( _T( "%u" ) , arg );
	}
	else if ( radix == 16 )
	{
		strRet.Format( _T( "%x" ) , arg );
	}
	else
	{
		ATLASSERT( FALSE );
	}
	return strRet;
}


CString CommonHelper::UInt64ToString( unsigned __int64 arg , int radix )
{
	TCHAR buf[68] = {_T( '\0' )};
	_ui64tot( arg , buf , radix );
	CString strRet = buf;
	return strRet;
}

BOOL CommonHelper::ToBOOL( const CString& str )
{
	CString strTemp = str;
	strTemp.Trim();
	strTemp.MakeUpper();
	if ( strTemp == _T( "TRUE" ) || strTemp == _T( "1" ) )
	{
		return TRUE;
	}
	return FALSE;

}



DWORD CommonHelper::TimeDiff( DWORD dwCur , DWORD dwStart )
{
	if( dwCur >= dwStart )
	{
		return dwCur - dwStart   ;
	}
	else
	{
		return INFINITE - dwStart  + dwCur ;
	}
	return 0;
}

UUID CommonHelper::GenerateUUID()
{
	UUID uuid = {0};
	if( UuidCreate( &uuid ) != RPC_S_OK )
	{
		ZeroMemory( &uuid , sizeof( UUID ) );
		uuid.Data1 = GetTickCount();
	}
	return uuid;
}

UUID CommonHelper::GenerateUUID( std::string &result )
{
	UUID uuid = {0};
	if( UuidCreate( &uuid ) != RPC_S_OK )
	{
		ZeroMemory( &uuid , sizeof( UUID ) );
		uuid.Data1 = GetTickCount();
	}
	unsigned char* str = NULL;
	UuidToStringA( &uuid, &str );
	if( str != NULL )
	{
		result = reinterpret_cast<char*>( str );
	}
	RpcStringFreeA( &str );
	return uuid;
}

UUID CommonHelper::GenerateUUID( CString &result )
{
	UUID uuid = {0};
	if( UuidCreate( &uuid ) != RPC_S_OK )
	{
		ZeroMemory( &uuid , sizeof( UUID ) );
		uuid.Data1 = GetTickCount();
	}
	unsigned short* str = NULL;
	UuidToStringW( &uuid, &str );
	if( str != NULL )
	{
		result = reinterpret_cast<wchar_t*>( str );
	}
	RpcStringFreeW( &str );
	return uuid;
}

CString CommonHelper::ToCString( std::string& str )
{
	CString result;
	LPWSTR buff  = NULL;
	if( CommonHelper::MByteToWChar(str.c_str(), &buff, 0 ) )
	{
		result = buff;
		return result;
	}
	result = CA2T(str.c_str()).m_psz;
	return result;
}

std::string CommonHelper::ToString( CString & str )
{	
	char* buff = NULL;
	if( CommonHelper::WCharToMByte( str.GetBuffer(), buff, str.GetLength() ) )
	{
		return buff;
	}
	std::string result;	
	result = CT2A( str.GetBuffer() ).m_psz;
	return result;
}

std::string CommonHelper::IntToString(int arg)
{
	std::stringstream ss;
	std::string str;
	ss << arg;
	ss >> str;

	return str;
}

CString CommonHelper::UInt64ToCString(UINT64 arg)
{
	CString s;
	s.Format( _T("%I64u"), arg);
	
	return s;
}

INT64 CommonHelper::CStringToInt64(CString str)
{
	char* p = NULL;

	if(WCharToMByte(str.GetBuffer(0),p,str.GetLength()))
	{
		//return 18446744073709551615;	// 暂且返回UINT64最大值;
	}

	return _atoi64(p);
}

int CommonHelper::CStringToInt(CString str)
{
	return _ttoi(str);
}

BOOL CommonHelper::IsTextUTF8( LPSTR lpBuffer, int iBufSize )
{
	/*
	0zzzzzzz;
	110yyyyy, 10zzzzzz
	1110xxxx, 10yyyyyy, 10zzzzzz
	11110www, 10xxxxxx, 10yyyyyy, 10zzzzzz
	*/

	int iLeftBytes = 0;
	BOOL bUtf8 = FALSE;
	if( iBufSize <= 0 )
		return FALSE;

	for( int i = 0;i < iBufSize;i++ )
	{
		char c = lpBuffer[i];
		if( c < 0 )       //至少有一个字节最高位被置位
			bUtf8 = TRUE;
		if( iLeftBytes == 0 )//之前尚无UTF-8编码的字符的前导字节，或者是下个字符。
		{
			if ( c >= 0 ) //0000 0000 - 0100 0000
				continue;
			do//统计出高位连续的的个数
			{
				c <<= 1;
				iLeftBytes++;
			}while ( c < 0 );
			iLeftBytes--;    //表示本字符的剩余字节的个数；
			if ( iLeftBytes == 0 )//最高位是10，不能作为UTF-8编码的字符的前导字节。
				return FALSE;
		}
		else
		{
			c &= 0xC0;       //1100 0000
			if ( c != ( char )0x80 )//1000 0000 对于合法的UTF-8编码，非前导字节的前两位必须为。
				return 0;
			else
				iLeftBytes--;
		}
	}
	if ( iLeftBytes )
		return FALSE;
	return bUtf8;
}

COperatingSystem::EOSVersion CommonHelper::GetOSVersion()
{
	return COperatingSystem::GetCurrentOSVersion();
}


CString CommonHelper::GetRegKeyVal(HKEY key,LPCTSTR subkey,LPTSTR valName)
{
	HKEY hkey;
	LONG retval = RegOpenKeyEx( key, subkey, 0, KEY_QUERY_VALUE, &hkey );
	CString strReturn(_T(""));
	if ( retval == ERROR_SUCCESS )
	{


		DWORD dwLen = 1024 ;
		CString strRegValue ;
		LONG lRet =  RegQueryValueEx(	hkey,
			valName,
			NULL,
			NULL,
			( LPBYTE )strRegValue.GetBuffer( dwLen ) ,
			( LPDWORD ) & dwLen );
		strRegValue.ReleaseBuffer();
		if ( lRet == ERROR_SUCCESS )
		{
			strReturn = strRegValue;
		}
		RegCloseKey( hkey );
	}
	return strReturn;
}

LONG CommonHelper::GetRegKey( HKEY key, LPCTSTR subkey, LPTSTR retdata )
{
	HKEY hkey;
	LONG retval = RegOpenKeyEx( key, subkey, 0, KEY_QUERY_VALUE, &hkey );

	if ( retval == ERROR_SUCCESS )
	{
		long datasize = MAX_PATH;
		TCHAR data[MAX_PATH] = {_T( '\0' )};
		RegQueryValue( hkey, NULL, data, &datasize );
		lstrcpy( retdata, data );
		RegCloseKey( hkey );
	}

	return retval;
}

HINSTANCE CommonHelper::GotoURL( LPCTSTR url, int showcmd )
{
	BOOL bOK = FALSE;
	TCHAR key[MAX_PATH + MAX_PATH] = {_T('\0')};
	CString addr;
	HINSTANCE result = NULL;
	{
		if ( GetRegKey( HKEY_CLASSES_ROOT, _T( "http\\shell\\open\\command" ), key ) == ERROR_SUCCESS )
		{
			//lstrcat( key, _T( "\\shell\\open\\command" ) );

			//if ( GetRegKey( HKEY_CLASSES_ROOT, key, key ) == ERROR_SUCCESS )
			{
				TCHAR *pos = NULL;
				pos = _tcsstr( key, _T( "\"%1\"" ) );
				if ( pos == NULL )
				{                     // No quotes found
#ifndef _UNICODE
					pos = strstr( key, _T( "%1" ) );   // Check for %1, without quotes
#else
					pos = wcsstr( key, _T( "%1" ) );   // Check for %1, without quotes
#endif
					if ( pos == NULL )                 // No parameter at all...
						pos = key + lstrlen( key ) - 1;
					else
						*pos = _T( '\0' );                 // Remove the parameter
				}
				else
				{
					*pos = _T( '\0' );                     // Remove the parameter
				}

				CString strExe = key;
				strExe.MakeLower();
				strExe.Trim();
				if( strExe.Find( _T("\"") ) != 0 )
				{
					strExe = CString( _T("\"") ) + strExe + CString( _T("\"") );
				}
				addr.Format( _T( "%s %s" ), key, url );
				result = ( HINSTANCE ) WinExec( CT2A( addr ).m_psz, showcmd );
				bOK = ( (int)result >= 32 );
			}
		}
	}

	if( !bOK )
	{
		if ( GetRegKey( HKEY_CLASSES_ROOT, _T( "Applications\\iexplore.exe\\shell\\open\\command" ), key ) == ERROR_SUCCESS )
		{
			TCHAR *pos = NULL;
			pos = _tcsstr( key, _T( "\"%1\"" ) );
			if ( pos == NULL )
			{                     // No quotes found
#ifndef _UNICODE
				pos = strstr( key, _T( "%1" ) );   // Check for %1, without quotes
#else
				pos = wcsstr( key, _T( "%1" ) );   // Check for %1, without quotes
#endif
				if ( pos == NULL )                 // No parameter at all...
					pos = key + lstrlen( key ) - 1;
				else
					*pos = _T( '\0' );                 // Remove the parameter
			}
			else
			{
				*pos = _T( '\0' );                     // Remove the parameter
			}

			CString strExe = key;
			strExe.MakeLower();
			strExe.Trim();
			if( strExe.Find( _T("\"") ) != 0 )
			{
				strExe = CString( _T("\"") ) + strExe + CString( _T("\"") );
			}
			addr.Format( _T( "%s %s" ), key, url );
			result = ( HINSTANCE ) WinExec( CT2A( addr ).m_psz, showcmd );
			bOK = ( (int)result >= 32 );
		}
	}
	return result;
}

BOOL CommonHelper::EncryptFile( tstring srcfile,tstring dstfile,tstring password,BOOL bEncrypt /*= TRUE*/ )
{

	return TRUE;
}

std::string CommonHelper::WToA( const wchar_t* pszW, unsigned int uCodePage )
{
	std::string strA;
	int n = WideCharToMultiByte(uCodePage, 0, pszW, -1, NULL, 0, 0, 0);
	char* pA = new char[n];
	WideCharToMultiByte(uCodePage, 0, pszW, -1, pA, n, 0, 0);
	strA = pA;
	delete[] pA;
	return strA;
}

tstring CommonHelper::AToW( const char* pszA, unsigned int uCodePage )
{
	tstring ss;
	return ss;
}

CString CommonHelper::DecodeString( const CString& str )
{
	CString outString=str;		
	outString.Replace(_T("&lt;"), _T("<"));
	outString.Replace(_T("&gt;"),_T(">"));
	outString.Replace(_T("&gt;"),_T(">"));
	outString.Replace(_T("&apos;"),_T("&apos;"));
	outString.Replace(_T("&amp;"),_T("&"));
	return outString;

}
CString CommonHelper::EncodeString( const CString& str )
{
	CString outString;
	int i = 0;
	int strLength = ( int ) str.GetLength();
	while ( i < strLength )
	{
		wchar_t c = str[i];
		if ( c == L'&' )
		{
			outString.Append( L"&amp;" );
			++i;
		}
		else if ( c == L'<' )
		{
			outString.Append( L"&lt;" );
			++i;
		}
		else if ( c == L'>' )
		{
			outString.Append(  L"&gt;" );
			++i;
		}
		else if ( c == L'\"' )
		{
			outString.Append( L"&quot;" );
			++i;
		}
		else if ( c == L'\'' )
		{
			outString.Append( L"&apos;" );
			++i;
		}
		else if ( c < 32 )
		{
			wchar_t buf[ 32 ];
			_snwprintf_s( buf, sizeof( buf ), L"&#x%02X;", ( unsigned ) ( c & 0xff ) );
			outString.Append( buf, ( int )wcslen( buf ) );
			++i;
		}
		else
		{
			outString += c;
			++i;
		}
	}
	return outString;
}

BOOL  CommonHelper::GetMacAddress( CString& mac )  
{  
	PIP_ADAPTER_INFO pAdapterInfo,pNext;  
	DWORD AdapterInfoSize;  
	TCHAR szMac[32]   =   {0};  
	DWORD Err;    
	AdapterInfoSize   =   0;  
	Err   =   GetAdaptersInfo(NULL,   &AdapterInfoSize);  
	if((Err   !=   0)   &&   (Err   !=   ERROR_BUFFER_OVERFLOW))
	{  
		return   FALSE;  
	}  
	//   分配网卡信息内存  
	pAdapterInfo   =   (PIP_ADAPTER_INFO)   GlobalAlloc(GPTR,   AdapterInfoSize);  
	if(pAdapterInfo   ==   NULL)
	{  
		return   FALSE;  
	}    
	if(GetAdaptersInfo(pAdapterInfo,   &AdapterInfoSize)   !=   0)
	{  
		GlobalFree(pAdapterInfo);  
		return   FALSE;  
	} 
	pNext=pAdapterInfo;
	while(pNext)
	{
		MIB_IFROW mib = {0};
		mib.dwIndex = pAdapterInfo->Index;
		if(mib.dwType != IF_TYPE_OTHER && mib.dwType != IF_TYPE_SOFTWARE_LOOPBACK)
		{
			mac.Format(  _T("%0XX-%02X-%02X-%02X-%02X-%02X"),pNext->Address[0],  
				pNext->Address[1],  
				pNext->Address[2],  
				pNext->Address[3],  
				pNext->Address[4],  
				pNext->Address[5] );
			break;
		}
		pNext=pNext->Next;
	}

	GlobalFree(pAdapterInfo);  
	return   TRUE;  
}

}
