#include "StdAfx.h"
#include "UpdateManager.h"
#include "UpdateDownloadProgress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

UpdateManager::UpdateManager( ) : 
m_LastError( 0 ), m_fileData( NULL ), m_fileDataLen( 0 ), m_hDownloadThread( NULL ), m_InstallerAvailable( false )
{
}

UpdateManager::~UpdateManager()
{
    if ( m_hDownloadThread )
    {
        CloseHandle( m_hDownloadThread );
    }
}

bool UpdateManager::Initialize( const std::wstring& versionURL,
                                const std::wstring& product, 
                                const std::wstring& currentVersion )
{
    m_VersionURL = versionURL;
    m_Product = product;
    m_CurrentVersion = currentVersion;

    return true;
}

bool UpdateManager::DoUpdateCheck( bool& updateAvailable, 
                                   std::wstring& latestVersion )
{
    bool success = false;

    success = DownloadVersionFile( );
    if ( success )
    {
        success = ParseVersionFile( latestVersion );
        if ( success )
        {
            // compare versions
            int majorCurrent = 0;
            int minorCurrent = 0;
            int weekCurrent = 0;
            int buildCurrent = 0;
            SplitVersionString( m_CurrentVersion, majorCurrent, minorCurrent, weekCurrent, buildCurrent );

            int majorLatest = 0;
            int minorLatest = 0;
            int weekLatest = 0;
            int buildLatest = 0;
            SplitVersionString( latestVersion, majorLatest, minorLatest, weekLatest, buildLatest );

            if ( ( majorLatest > majorCurrent ) ||
                 ( ( majorLatest == majorCurrent ) && ( minorLatest > minorCurrent ) ) ||
                 ( ( majorLatest == majorCurrent ) && ( minorLatest == minorCurrent ) && ( weekLatest > weekCurrent ) ) ||
                 ( ( majorLatest == majorCurrent ) && ( minorLatest == minorCurrent ) && ( weekLatest == weekCurrent ) && ( buildLatest > buildCurrent ) ) )
            {
                updateAvailable = true;
            }
        }
    }

    return success;
}

bool UpdateManager::DownloadFile( const std::wstring& url, char** fileData, unsigned int& fileDataLen )
{
    bool success = false;

    HINTERNET hInternet = InternetOpen( L"LunaSolCalUpdater", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
    if ( hInternet )
    {
        DWORD context = 0;
        HINTERNET hUrl = InternetOpenUrl( hInternet, url.c_str( ), NULL, 0, INTERNET_FLAG_NO_CACHE_WRITE, (DWORD_PTR)&context );
        if ( hUrl )
        {
            const unsigned int CHUNK_SIZE = 1024;
            char *buffer = (char *)malloc( CHUNK_SIZE );
            if ( !buffer) return false;

            unsigned int buflen = CHUNK_SIZE;

            fileDataLen = 0;
            DWORD bytesRead = 0;
            BOOL cont = TRUE;

            while ( cont )
            {
                cont = InternetReadFile( hUrl, buffer + fileDataLen, CHUNK_SIZE, &bytesRead );
                if ( bytesRead == 0 && cont )
                {
                    success = true;
                    break;
                }
                else
                {
                    if ( cont )
                    {
                        fileDataLen += bytesRead;
                        buflen = buflen + bytesRead;
                        char* newBuffer = (char *) realloc( buffer, buflen );
                        if ( newBuffer )
                        {
                            buffer = newBuffer;
                        }
                    }
                    else
                    {
                        success = false;
                    }
                }
            }

            InternetCloseHandle( hUrl );

            if ( success )
            {
                *fileData = buffer;
            }
        }
        else
        {
            m_LastError = ::GetLastError( );
        }

        InternetCloseHandle( hInternet );
    }   

    return success;
}

static DWORD WINAPI DownloadThread( LPVOID pParam )
{
    threadParams *pThreadParams = (threadParams *) pParam;
    UpdateManager *pUpdateManager = pThreadParams->pUpdateManager;
    UpdateDownloadProgress *pDlg = pThreadParams->pProgressDlg;
    const std::wstring& url = pThreadParams->downloadUrl;
    std::wstring& installerPath = pThreadParams->installerPath;

    if ( pDlg ) 
    {
        while ( !pDlg->m_hWnd )
            Sleep( 10 );

        HINTERNET hInternet = InternetOpen( L"LunaSolCalUpdater", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
        if ( hInternet )
        {
            DWORD context = 0;
            HINTERNET hUrl = InternetOpenUrl( hInternet, url.c_str( ), NULL, 0, INTERNET_FLAG_NO_CACHE_WRITE, (DWORD_PTR)&context );
            if ( hUrl )
            {
                DWORD index = 0;
                DWORD contentLength = 0; 
                DWORD clSize = sizeof( DWORD );
                HttpQueryInfo( hUrl, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &contentLength, &clSize, &index );
    
                bool success = false;
                const unsigned int CHUNK_SIZE = 102400;
                char *buffer = (char *)malloc( CHUNK_SIZE );
                if ( !buffer) return false;

                unsigned int buflen = CHUNK_SIZE;
    
                unsigned int fileDataLen = 0;
                DWORD bytesRead = 0;
                BOOL cont = TRUE;

                while ( cont )
                {
                    cont = InternetReadFile( hUrl, buffer + fileDataLen, CHUNK_SIZE, &bytesRead );
                    if ( bytesRead == 0 && cont )
                    {
                        success = true;
                        break;
                    }
                    else
                    {
                        if ( cont )
                        {
                            fileDataLen += bytesRead;
                            buflen = buflen + bytesRead;
                            char* newBuffer = (char *) realloc( buffer, buflen );
                            if ( newBuffer )
                            {
                                buffer = newBuffer;
                            }
                        }
                        else
                        {
                            success = false;
                        }
                    }

                    if ( contentLength && !pDlg->IsCanceled( ) )
                    {
                        static int lastPercentage = 0;
                        const int percentage = 100 * fileDataLen / contentLength;

                        if ( ( percentage - lastPercentage ) >= 25 )
                        {
                            lastPercentage = percentage;
                            pDlg->SendMessage( WM_PROGRESS_UPDATE, UPDATE_PROGRESS_BAR, percentage );
                            Sleep( 300 );
                        }
                    }

                    cont &= !pDlg->IsCanceled( );
                }

                if ( contentLength && !pDlg->IsCanceled( ) )
                {
                    const int percentage = 100 * fileDataLen / contentLength;
                    pDlg->PostMessage( WM_PROGRESS_UPDATE, UPDATE_PROGRESS_BAR, percentage );
                    Sleep( 500 );
                }

                InternetCloseHandle( hUrl );

                if ( success )
                {
                    // write to file
                    // Get the temp path
                    const unsigned int BUFSIZE = 4096;

                    DWORD bufSize = BUFSIZE;
                    TCHAR tempPath[ BUFSIZE ];
                    GetTempPath( bufSize, tempPath );

                    // Create a temporary file. 
                    TCHAR tempName[ MAX_PATH ];
                    _tcscpy( tempName, tempPath );
                    _tcscat( tempName, L"LunaSolCal_Update.exe" );

                    HANDLE hTempFile = CreateFile((LPTSTR) tempName, 
                        GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL, NULL);

                    if ( hTempFile != INVALID_HANDLE_VALUE ) 
                    { 
                        DWORD bytesWritten = 0;
                        WriteFile( hTempFile, buffer, fileDataLen, &bytesWritten, NULL );
                        CloseHandle( hTempFile );

                        installerPath = tempName;
                        pUpdateManager->SetInstallerAvailable( true );

                        success = true;
                    } 
                    else
                    {
//                        m_LastError = ::GetLastError( );
                    }
                }

                free( buffer );
            }
            else
            {
//                m_LastError = ::GetLastError( );
            }

            InternetCloseHandle( hInternet );
        }  

        if ( pDlg && !pDlg->IsCanceled( ) && pDlg->m_hWnd )
        {
            pDlg->PostMessage( WM_CLOSE_PROGRESS_DLG );
        }
    }

    return 0;
}

bool UpdateManager::DownloadInstaller( UpdateDownloadProgress *pProgressDlg )
{
    m_ThreadParams.pProgressDlg = pProgressDlg;
    m_ThreadParams.pUpdateManager = this;
    m_ThreadParams.downloadUrl = m_DownloadURL;

    DWORD dwId = 0;
    if ( m_hDownloadThread )
    {
        CloseHandle( m_hDownloadThread );
    }

    m_hDownloadThread = ::CreateThread(NULL, 0, DownloadThread, (LPVOID)&m_ThreadParams, 0, &dwId);

    return ( m_hDownloadThread != NULL );
}


bool UpdateManager::DownloadVersionFile( )
{
    bool success = false;
    char* fileData = NULL;
    unsigned int fileDataLen = 0;
    std::wstring url = m_VersionURL + m_Product;

    if ( DownloadFile( url, &fileData, fileDataLen ) && fileData )
    {
        const int sizeNeeded = MultiByteToWideChar( 
            CP_UTF8, MB_ERR_INVALID_CHARS, 
            (LPCSTR)fileData, fileDataLen, NULL, 0 );

        if ( sizeNeeded > 0 )
        {
            wchar_t* wBuffer = (wchar_t *) malloc( ( sizeNeeded + 1 )* sizeof( wchar_t ) );
            if ( wBuffer )
            {
                wBuffer[ sizeNeeded ] = 0;

                const int numChars = MultiByteToWideChar( 
                    CP_UTF8, MB_ERR_INVALID_CHARS, 
                    (LPCSTR)fileData, fileDataLen, 
                    wBuffer, sizeNeeded );

                if ( numChars != 0 )
                {
                    m_VersionFileContent = wBuffer;
                    success = true;
                }

                free( wBuffer );
            }
        }

        free( fileData );
    }

    return success;
}

bool UpdateManager::ParseVersionFile( std::wstring& latestVersion )
{
    bool success = false;

    success = GetValue( L"version", latestVersion );
    if ( success )
    {
        success = GetValue( L"url", m_DownloadURL );
    }

    return success;
}

bool UpdateManager::GetValue( const std::wstring& tag, std::wstring& value )
{
    bool success = false;

    std::wstring beginTag = std::wstring( L"<" ) + tag + std::wstring( L">" );
    std::wstring endTag = std::wstring( L"</" ) + tag + std::wstring( L">" );

    size_t startPos = m_VersionFileContent.find( beginTag );
    if ( startPos != std::wstring::npos )
    {
        startPos += beginTag.length( );

        size_t endPos = m_VersionFileContent.find( endTag, startPos );
        if ( endPos != std::wstring::npos )
        {
            value = m_VersionFileContent.substr( startPos, endPos - startPos );
            success = true;
        }
    }

    return success;
}

void UpdateManager::SplitVersionString( const std::wstring version, int& major, int& minor, int& week, int& build )
{
    int num[4] = {0,0,0,0};

    size_t startPos = 0;

    for ( int i = 0; i < 4; i++ )
    {
        size_t pos = version.find( L".", startPos );
        num[i] = _wtoi( version.substr( startPos, pos ).c_str( ) );

        if ( pos != std::string::npos )
        {
            startPos = pos + 1;
        }
    }

    major = num[0];
    minor = num[1];
    week  = num[2];
    build = num[3];
}


bool UpdateManager::DoUpdate( UpdateDownloadProgress *pProgressDlg )
{
    return DownloadInstaller( pProgressDlg );
}


bool UpdateManager::RunInstaller( )
{
    const std::wstring& installerPath = m_ThreadParams.installerPath;
    if ( installerPath.length( ) == 0 )
    {
        return false;
    }

    SHELLEXECUTEINFO execInfo;
    memset( &execInfo, 0, sizeof( SHELLEXECUTEINFO ) );
    execInfo.cbSize = sizeof( SHELLEXECUTEINFO );
    execInfo.fMask =  SEE_MASK_FLAG_DDEWAIT ;
    execInfo.lpVerb = L"open";
    execInfo.lpFile = installerPath.c_str( );
    execInfo.nShow = SW_NORMAL;

    BOOL bRes = ShellExecuteEx( &execInfo );
    if ( bRes == FALSE )
    {
        m_LastError = ::GetLastError( );
    }

    return ( bRes == TRUE );
}
