/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   + HauteCapture - (c) 2005 by Volker Voecking Software Engineering
   +
   + $Source: C:\\CVS_Repository/CybonTailTNG/shared/RegAccess.h,v $
   + $Author: volker $
   + $Date: 2005/02/13 07:42:02 $
   + $Revision: 1.6 $
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

#pragma once

#include "ConfigKeys.h"
#include "BlowFish.h"

class CRegAccess
{
  public:
	CRegAccess();

    bool GetBoolValue( int nKeyID );
    void SetBoolValue( int nKeyID, bool bValue );

    DWORD GetDWORDValue( int nKeyID );
    void SetDWORDValue( int nKeyID, DWORD dwValue );

    CString GetStringValue( int nKeyID );
    void SetStringValue( int nKeyID, CString szValue);

    CString GetEncodedStringValue( int nKeyID );
    void SetEncodedStringValue( int nKeyID, CString szValue );

    bool GetBinaryValue( int nKeyID, CString szValueName, ULONG* ulBufSize, BYTE** pBuffer );
    bool GetBinaryValue( int nKeyID, ULONG* pulBufSize, BYTE** ppBuffer );
    void SetBinaryValue( int nKeyID, CString szValueName, ULONG ulBufSize, BYTE* pBuffer );
    void SetBinaryValue( int nKeyID, ULONG ulBufSize, BYTE* pBuffer );

    // caller must free memory
    LOGFONT* GetFont( int nKeyID );
    void SetFont( int nKeyID, LOGFONT lf );

    bool DeleteValue( int nKeyID );
    bool DeleteValue( int nKeyID, CString szValueName );
    bool DeleteKey( LPCTSTR lpSubKey );

    void GetListOfSubKeys( int nKeyID, CStringList& subKeys );
    void GetListOfSubKeys( HKEY hKey, LPCTSTR lpSubKey, CStringList& keyList );

    CString GetStringFromRegistry( LPCTSTR lpSubKey, LPCTSTR lpValueName, LPCTSTR lpDefault );
    CString GetStringFromRegistry( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, LPCTSTR lpDefault );
    bool GetStringFromRegistry( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, CString& szValue );
    void SetStringInRegistry( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, LPCTSTR lpValue );

    DWORD GetDWORDFromRegistry( LPCTSTR lpSubKey, LPCTSTR lpValueName, DWORD dwDefault );
    bool GetDWORDFromRegistry( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, DWORD& dwValue );
    void SetDWORDInRegistry( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, DWORD dwDefault );

    bool GetBinaryValueFromRegistry( LPCTSTR lpSubKey, LPCTSTR lpValueName, ULONG* pulBufSize, BYTE** ppBuffer );
    bool GetBinaryValueFromRegistry( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, ULONG* ulBufSize, BYTE** pBuffer );
    void SetBinaryValueInRegistry( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, ULONG ulBufSize, BYTE* pBuffer );

    bool DeleteValueFromRegistry( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName );

    bool GetRegKeyPath( int nKeyID, LPTSTR pBuffer, DWORD dwBufferLen );
    CString GetValueName( int nKeyID );

private:
    static REG_CONFIG_HIERARCHY Hierarchy_Table[];
    static REG_CONFIG_KEYDATA KeyData_Table[];

    // helper functions

    int GetLinkIDFromHierarchyID( int nID );
    LPCTSTR GetPathFromHierarchyID( int nID );
    void GetRegHierarchyKeyPath( int nKeyID, LPTSTR pBuffer, DWORD dwBufferLen );
    bool GetHierarchyKeyIDFromKeyID( int nID, int& nHierarchyKeyID );
    bool IsHierarchyKeyID( int nKeyID );

    bool GetDefaultValue( int nID, bool& bDefault );
    bool GetDefaultValue( int nID, DWORD& dwDefault );
    bool GetDefaultValue( int nID, CString& szDefault );

    bool GetRegPathAndKeyName( int nKeyID, LPTSTR pRegPath, DWORD dwMaxLen, LPCTSTR* ppKeyName );
    
    REG_CONFIG_HIERARCHY* GetHierarchyKeyData( int nID );
    REG_CONFIG_KEYDATA*   GetKeyData( int nID );

    CBlowFish m_BlowFish;
};

