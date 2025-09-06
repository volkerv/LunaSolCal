/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   + LunaSolCal - (c) 2009 by Volker Voecking Software Engineering
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

#include "stdafx.h"
#include <winver.h>
#include "ProductVersion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// create an instance of a version object
ProductVersion::ProductVersion( LPCTSTR pFilePath )
: m_pVerInfoBuf(NULL), m_dwFileInfoSize(0)
{ 
    const int cMaxPathSize = MAX_PATH;
    TCHAR buf[cMaxPathSize * sizeof(TCHAR)];

    if (!pFilePath)
    { 
        // get file path of own module
        GetModuleFileName(AfxGetApp()->m_hInstance, buf, cMaxPathSize);
        pFilePath = buf;
    }

    DWORD dwVerHandle = 0;
    m_dwFileInfoSize = GetFileVersionInfoSize( (LPTSTR)pFilePath, &dwVerHandle);

    if (m_dwFileInfoSize)
    { 
        m_pVerInfoBuf = new TCHAR[m_dwFileInfoSize * sizeof(TCHAR)];
        if (!m_pVerInfoBuf)
            return;
        memset( m_pVerInfoBuf, 0, m_dwFileInfoSize * sizeof(TCHAR));
        GetFileVersionInfo((LPTSTR)pFilePath, dwVerHandle, m_dwFileInfoSize, m_pVerInfoBuf);
    }  
}

ProductVersion::~ProductVersion()
{
    delete m_pVerInfoBuf;
}  

// get the version information from the current module
// return a pointer to the VERSIONINFO of this module
// return NULL if no version-info is available
const void *ProductVersion::GetVersionOfModule()
{ 
    return m_pVerInfoBuf;
}

// return the binary version-information of a file
// return -1 for both values, if no version-information is available
void ProductVersion::GetFileVersionNumber(DWORD& dwVersionNumber, DWORD& dwBuildNumber)
{ 
    unsigned int wSize = (unsigned int) m_dwFileInfoSize; // VerQueryValue requires int* in parameter 4
    if (!m_pVerInfoBuf)
    { 
        dwVersionNumber = (UINT) -1;
        dwBuildNumber = (UINT) -1;
        return;
    }

    VS_FIXEDFILEINFO* pFixedInfo = NULL;
    VerQueryValue(m_pVerInfoBuf, _T("\\"), (void**)&pFixedInfo, &wSize);
    dwVersionNumber = pFixedInfo->dwFileVersionMS;
    dwBuildNumber = pFixedInfo->dwFileVersionLS;
}

void ProductVersion::GetProductVersionString(CString& productVersion)
{
    if (!m_pVerInfoBuf)
    { 
        productVersion.Empty();
        return;
    }

    CString infoPart("\\StringFileInfo\\040904E4\\ProductVersion");
    TCHAR *tcTmp = new TCHAR[ infoPart.GetLength() + 1];
    _tcscpy( tcTmp, infoPart );

    unsigned int wSize = 0;
    void* pProductVersion = NULL; 
    VerQueryValue(m_pVerInfoBuf, tcTmp, &pProductVersion, &wSize);

    delete [] tcTmp;
    productVersion = (LPCTSTR )pProductVersion;
}

void ProductVersion::GetProductNameString(CString& productName)
{
    if (!m_pVerInfoBuf)
    { 
        productName.Empty();
        return; 
    }

    const CString infoPart("\\StringFileInfo\\040904E4\\ProductName");
    TCHAR *tcTmp = new TCHAR[ infoPart.GetLength() + 1];
    _tcscpy( tcTmp, infoPart );

    unsigned int wSize = 0;
    void* pProductName = NULL; 
    VerQueryValue(m_pVerInfoBuf, tcTmp, &pProductName, &wSize);

    delete [] tcTmp;
    productName = (LPCTSTR )pProductName;
}

