/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   + LunaSolCal - (c) 2009 by Volker Voecking Software Engineering
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

#include "stdafx.h"
#include "lunasol.h"
#include "UpdateDownloadProgress.h"
#include "UpdateManager.h"


// UpdateDownloadProgress dialog

IMPLEMENT_DYNAMIC(UpdateDownloadProgress, CDialog)

UpdateDownloadProgress::UpdateDownloadProgress(CWnd* pParent /*=NULL*/)
: CLocalizableDialog(UpdateDownloadProgress::IDD, pParent), m_bCanceled( false )
    , m_DownloadingLabelText(_T(""))
{

}

UpdateDownloadProgress::~UpdateDownloadProgress()
{
}

BOOL UpdateDownloadProgress::OnInitDialog()
{
	CDialog::OnInitDialog();

    LocalizeString( m_DownloadingLabelText,IDS_UPDATE_DOWNLOADING );
    LocalizeTitle( IDS_UPDATE_TITLE );

    UpdateData( FALSE );

	return TRUE;
}

void UpdateDownloadProgress::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_STATIC_PRODUCT_NAME, m_DownloadingLabelText);
}


BEGIN_MESSAGE_MAP(UpdateDownloadProgress, CDialog)
    ON_MESSAGE(WM_PROGRESS_UPDATE, OnProgressUpdate)
    ON_MESSAGE(WM_CLOSE_PROGRESS_DLG, OnCloseProgressDlg)
    ON_BN_CLICKED(IDCANCEL, &UpdateDownloadProgress::OnBnClickedCancel)
END_MESSAGE_MAP()


// UpdateDownloadProgress message handlers
void UpdateDownloadProgress::DownloadStatusCB( int status, int /*detail*/ )
{
    switch( status )
    {
    case UpdateManager::DOWNLOAD_FINISHED:
        {
            CWnd *pWnd = AfxGetApp( )->GetMainWnd( );
            if ( pWnd )
            {
                pWnd->SendMessage( WM_CLOSE, 0, 0 );
            }
        }
        break;
    }
}


long UpdateDownloadProgress::OnProgressUpdate( WPARAM wParam, LPARAM lParam )
{
    switch( wParam )
    {
    case SET_PROGRESS_MSG:
        {
            CWnd* pWnd = theApp.GetMainWnd();
            if ( pWnd )
            {
                pWnd->UpdateWindow( ) ;
            }
       //     SetDialogText( IDC_STATIC_CURRENT_ACTION, (UINT)lParam );
        }
        break;
    case UPDATE_PROGRESS_BAR:
        {
            CProgressCtrl * const pProgress = (CProgressCtrl*) GetDlgItem( IDC_PROGRESS );
            if ( pProgress )
            {
                pProgress->SetPos( (int)lParam );
            }

            CStatic * const pPercent = (CStatic*) GetDlgItem( IDC_STATIC_STATE );
            if ( pPercent )
            {
                CString text;
                text.Format(_T("%d %%"), lParam);
                pPercent->SetWindowText(text);
            }
        }
        break;
    default:
        break;
    }

    return 0;
}

long UpdateDownloadProgress::OnCloseProgressDlg( WPARAM /* wParam */, LPARAM /* lParam */       )
{
    CDialog::OnCancel( );

    return 0;
}

void UpdateDownloadProgress::OnBnClickedCancel()
{
    m_bCanceled = true;
    OnCancel( ); 
}
