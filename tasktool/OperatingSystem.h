#pragma once

#include "Version.h"
namespace pcutil
{

class WEAVERLIB_API_UTILS COperatingSystem
{
public:
	enum PlatformID
	{
		Win32S,
		Win32Windows,
		Win32NT,
		WinCE
    };

    static enum EOSVersion
    {
        Unknown = 0x0,
        Windows2000 = 0x1,
        WindowsXP = 0x2,
        Windows2003 = 0x3,
        WindowsVista = 0x4,
        Windows7 = 0x5,
        Windows2008 = 0x6,
		Windows7High = 0x7
    };

private:
	COperatingSystem();

public:
	COperatingSystem( const PlatformID id , const CVersion &ver , const std::pair<DWORD,DWORD> &pairServicePackageNo );
	~COperatingSystem();

public:
	CVersion& GetVersion();
	PlatformID GetPlatformID();
	CString ToString();
	std::pair<DWORD,DWORD> GetServicePackageNo();

    static EOSVersion GetCurrentOSVersion();
	static BOOL CurrentIsWinxpOrLater();
	static bool Is64BitOS();

private:
	CVersion m_Version;
	PlatformID m_PlatformID;
	std::pair<DWORD,DWORD> m_pairServicePackageNo;

};


}