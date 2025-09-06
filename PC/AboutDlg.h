/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   + LunaSolCal - (c) 2009 by Volker Voecking Software Engineering
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

#pragma once
#include "LocalizableDialog.h"
#include "afxwin.h"

class CAboutDlg : public CLocalizableDialog
{
public:
	CAboutDlg();

    void SetProductName( const CString& productName ) {
        m_ProductName = productName;
    }

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


// Implementation
protected:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

    void ShowUrl( const TCHAR* url );
    void DrawHyperlink( CPaintDC& dc, UINT id );
    bool IsOverHyperlink( UINT id, const CPoint& point );

	CBrush  m_brush;
    HCURSOR m_hHlinkCursor;
    HCURSOR m_hOldCursor;
    CString m_ProductName;

    CString m_ContactLabelText;
    CString m_ImageSrcLabelText;
    CString m_IconsLabelText;
    CString m_LocationSrcLabelText;
    CString m_IconSourceText;
public:
    afx_msg void OnBnClickedCloseAbout();
    CButton mCloseButton;
};
