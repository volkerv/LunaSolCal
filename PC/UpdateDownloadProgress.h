/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   + LunaSolCal - (c) 2009 by Volker Voecking Software Engineering
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

#pragma once
#include "resource.h"
#include "LocalizableDialog.h"

const int WM_PROGRESS_UPDATE    = WM_USER+1;
const int WM_CLOSE_PROGRESS_DLG = WM_USER+2;

const int SET_PROGRESS_MSG      = 1;
const int UPDATE_PROGRESS_BAR   = 2;

class UpdateDownloadProgress : public CLocalizableDialog
{
	DECLARE_DYNAMIC( UpdateDownloadProgress )

public:
	UpdateDownloadProgress( CWnd* pParent = NULL );   // standard constructor
	virtual ~UpdateDownloadProgress( );
// Dialog Data
	enum { IDD = IDD_UPDATE_DOWNLOAD };

    bool IsCanceled() {
        return m_bCanceled; }

    static void CALLBACK DownloadStatusCB( int status, int detail );

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange( CDataExchange* pDX );    // DDX/DDV support
    
    afx_msg void OnBnClickedCancel( );
    afx_msg long OnProgressUpdate( WPARAM wParam, LPARAM lParam );
    afx_msg long OnCloseProgressDlg( WPARAM wParam, LPARAM lParam );

	DECLARE_MESSAGE_MAP( )

private:
    CString m_DownloadingLabelText;
    bool    m_bCanceled;
};
