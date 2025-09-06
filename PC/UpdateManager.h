#pragma once
#include <string>
#include "wininet.h"

typedef void (CALLBACK *DOWNLOAD_STATUS_CB)( int status, int detail );
class UpdateDownloadProgress;
class UpdateManager;

struct threadParams
{
    UpdateManager           *pUpdateManager;
    UpdateDownloadProgress  *pProgressDlg;
    std::wstring             downloadUrl;
    std::wstring             installerPath;
};

class UpdateManager
{
public:
    enum 
    {
        DOWNLOAD_STARTED,
        DOWNLOAD_PROGRESS,
        DOWNLOAD_FINISHED,
        DOWNLOAD_FAILED
    };

    UpdateManager( );
    ~UpdateManager( );
    bool Initialize( const std::wstring& versionURL, const std::wstring& product, const std::wstring& currentVersion );

    bool DoUpdateCheck( bool& updateAvailable, 
                        std::wstring& latestVersion );

    bool DoUpdate( UpdateDownloadProgress *pProgressDlg );

    DWORD GetLastError( ) { return m_LastError; }

    bool IsInstallerAvailable( ) {
        return m_InstallerAvailable; }
    void SetInstallerAvailable( bool value ) {
        m_InstallerAvailable = value; }

    bool RunInstaller( );

private:
    bool DownloadFile( const std::wstring& url, char** fileData, unsigned int& fileDataLen );
    bool DownloadVersionFile( );
    bool DownloadInstaller( UpdateDownloadProgress *pProgressDlg );
    bool ParseVersionFile( std::wstring& latestVersion );
    bool GetValue( const std::wstring& tag, std::wstring& value );
    void SplitVersionString( const std::wstring version, int& major, int& minor, int& week, int& build );

    std::wstring    m_VersionURL;
    std::wstring    m_Product;
    std::wstring    m_CurrentVersion;
    std::wstring    m_VersionFileContent;
    std::wstring    m_DownloadURL;

    DWORD           m_LastError;

    HINTERNET       m_hAsyncFileHandle;

    char*           m_fileData;
    unsigned int    m_fileDataLen;

    threadParams    m_ThreadParams;
    HANDLE          m_hDownloadThread;

    bool            m_InstallerAvailable;
};
