#include "stdafx.h"
#include "OperatingSystem.h"
#include "WinCred.h"
namespace pcutil
{

COperatingSystem::COperatingSystem()
{

}

COperatingSystem::COperatingSystem( const COperatingSystem::PlatformID id , const CVersion &ver , const std::pair<DWORD,DWORD> &pairServicePackageNo )
{
	m_Version = ver;
	m_PlatformID = id;
	m_pairServicePackageNo = pairServicePackageNo;
}

COperatingSystem::~COperatingSystem()
{


}

CVersion& COperatingSystem::GetVersion()
{
	return m_Version;
}

COperatingSystem::PlatformID COperatingSystem::GetPlatformID()
{
	return m_PlatformID;
}

std::pair<DWORD,DWORD> COperatingSystem::GetServicePackageNo()
{
	return m_pairServicePackageNo;
}


CString COperatingSystem::ToString()
{
    CString str;
    if (m_PlatformID == COperatingSystem::Win32NT)
    {
        str = _T( "Microsoft Windows NT" );
    }
    else if (m_PlatformID == COperatingSystem::Win32Windows)
    {
        if ((m_Version.GetMajor() > 4) || ((m_Version.GetMajor() == 4) && (m_Version.GetMinor() > 0)))
        {
            str = _T( "Microsoft Windows 98" );
        }
        else
        {
            str = _T( "Microsoft Windows 95" );
        }
    }
    else if (m_PlatformID == COperatingSystem::Win32S)
    {
        str = _T("Microsoft Win32S");
    }
    else if (m_PlatformID == COperatingSystem::WinCE)
    {
        str = _T("Microsoft Windows CE");
    }
    else
    {
        str = _T("<unknown>");
    }
    return ( str + _T(" ") + m_Version.ToString() );
}


COperatingSystem::EOSVersion COperatingSystem::GetCurrentOSVersion()
{
    OSVERSIONINFO osvi;
    ::ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if(GetVersionEx(&osvi))
    {
        if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
            return Windows2000;
        else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
            return WindowsXP;
        else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
            return  Windows2003;
        else if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0)
            return  WindowsVista;
        else if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1)
            return  Windows7;
		else if( osvi.dwMajorVersion >= 6 )
			return  Windows7High;
        else
            return EOSVersion::Unknown;
    }
    return EOSVersion::Unknown;
}


BOOL COperatingSystem::CurrentIsWinxpOrLater()
{
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;
	int op = VER_GREATER_EQUAL;

	// Initialize the OSVERSIONINFOEX structure.

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 5;
	osvi.dwMinorVersion = 1;
	osvi.wServicePackMajor = 0;
	osvi.wServicePackMinor = 0;

	// Initialize the condition mask.

	VER_SET_CONDITION( dwlConditionMask, VER_MAJORVERSION, op );
	VER_SET_CONDITION( dwlConditionMask, VER_MINORVERSION, op );
	VER_SET_CONDITION( dwlConditionMask, VER_SERVICEPACKMAJOR, op );
	VER_SET_CONDITION( dwlConditionMask, VER_SERVICEPACKMINOR, op );

	// Perform the test.

	return VerifyVersionInfo(
		&osvi, 
		VER_MAJORVERSION | VER_MINORVERSION | 
		VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR,
		dwlConditionMask);
}

//copy from msdn
typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

BOOL IsWow64()
{
#if defined(_WIN64)
	return TRUE; 
#elif defined(_WIN32)
	BOOL bIsWow64 = FALSE;
	LPFN_ISWOW64PROCESS fnIsWow64Process=NULL;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
		GetModuleHandle(_T("kernel32")),"IsWow64Process");

	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
		{
			// handle error
		}
	}
	return bIsWow64;
#endif
}

bool COperatingSystem::Is64BitOS()
{
	static int pa = 3;
	if (3==pa)
	{
		pa = IsWow64();
	}
	return 1==pa;
}


}