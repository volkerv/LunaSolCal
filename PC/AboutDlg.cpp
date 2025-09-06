/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   + LunaSolCal - (c) 2009 by Volker Voecking Software Engineering
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

#include "stdafx.h"
#include "lunasol.h"
#include "AboutDlg.h"
#include "customer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CAboutDlg::CAboutDlg() : CLocalizableDialog(CAboutDlg::IDD)
, m_ContactLabelText(_T(""))
, m_ImageSrcLabelText(_T(""))
, m_IconsLabelText(_T(""))
, m_LocationSrcLabelText(_T(""))
, m_IconSourceText(_T(""))
{
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_brush.CreateSolidBrush(RGB(255, 255, 255)); // color white brush
    m_hHlinkCursor = theApp.LoadCursor( IDC_HYPERLINK );
    m_hOldCursor = 0;

    LocalizeTitle( IDS_ABOUT_TITLE );

    LocalizeString( m_ContactLabelText, IDS_ABOUT_CONTACT );
    LocalizeString( m_ImageSrcLabelText, IDS_ABOUT_IMAGE_SRC );
    LocalizeString( m_LocationSrcLabelText, IDS_ABOUT_LOCATION_SRC );
    LocalizeString( m_IconsLabelText, IDS_ABOUT_ICONS );
    LocalizeString( m_IconSourceText, IDS_ICON_SOURCE );

    CString closeButtonText;
    LocalizeString( closeButtonText, IDS_CLOSE );
    mCloseButton.SetWindowText(closeButtonText);

    UpdateData( FALSE );

	return TRUE;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_STATIC_PRODUCT_NAME, m_ProductName);
    DDX_Text(pDX, IDC_LABEL_CONTACT, m_ContactLabelText);
    DDX_Text(pDX, IDC_LABEL_IMAGE_SRC, m_ImageSrcLabelText);
    DDX_Text(pDX, IDC_LABEL_ICON_SRC, m_IconsLabelText);
    DDX_Text(pDX, IDC_LABEL_LOCATION_SRC, m_LocationSrcLabelText);
    DDX_Text(pDX, IDC_VALUE_ICON_SRC, m_IconSourceText);
    DDX_Control(pDX, IDC_CLOSE_ABOUT, mCloseButton);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_CTLCOLOR()
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_PAINT()
    ON_BN_CLICKED(IDC_CLOSE_ABOUT, &CAboutDlg::OnBnClickedCloseAbout)
END_MESSAGE_MAP()

HBRUSH CAboutDlg::OnCtlColor(CDC* /* pDC */, CWnd* /* pWnd */, UINT /* nCtlColor */)
{
	return m_brush;
}

void CAboutDlg::OnPaint()
{
    CPaintDC dc( this ); // device context for painting

    DrawHyperlink( dc, IDC_STATIC_COPYRIGHT );
    DrawHyperlink( dc, IDC_VALUE_LOCATION_SRC );
}

void CAboutDlg::DrawHyperlink( CPaintDC& dc, UINT id )
{
    CWnd * const pControl = GetDlgItem( id );
    if( pControl )
    {
        pControl->ShowWindow( SW_HIDE );

        const COLORREF bgColor = GetSysColor( COLOR_3DFACE );
        const int briteness = GetRValue( bgColor )
                              + GetGValue( bgColor )
                              + GetBValue( bgColor ) / 2;

        // draw text in blue, to appear as a hyperlink:
        CRect r;
        pControl->GetWindowRect( &r );
        ScreenToClient( &r );
        
        CString controlTxt;
        pControl->GetWindowText( controlTxt );

        const COLORREF darkBlue  = RGB(   0,  51, 255 );
        const COLORREF liteBlue  = RGB( 153, 153, 255 );
        const COLORREF linkColor( 
            briteness >= 300 ? darkBlue : liteBlue );
        CPen pen( PS_SOLID, 1, linkColor );
        CPen  * const pOldPen   = dc.SelectObject( &pen );
        CFont * const pFont     = pControl->GetFont();
        CFont * const pOldFont  = dc.SelectObject( pFont );
        const COLORREF oldColor = dc.SetTextColor( linkColor );
        const int oldBkMode     = dc.SetBkMode( TRANSPARENT );
        dc.DrawText( controlTxt, r, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_CALCRECT );
        dc.DrawText( controlTxt, r, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX );
        dc.MoveTo( r.left,  r.bottom );
        dc.LineTo( r.right, r.bottom );

        dc.SetBkMode( oldBkMode );
        dc.SetTextColor( oldColor );
        dc.SelectObject( pOldFont );
        dc.SelectObject( pOldPen );
    }
}

void CAboutDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    if ( IsOverHyperlink( IDC_STATIC_COPYRIGHT, point ) || 
         IsOverHyperlink( IDC_VALUE_LOCATION_SRC, point ))
    {
        m_hOldCursor = SetCursor( m_hHlinkCursor );
    }
    else
    {
        // reset cursor
        if ( m_hOldCursor != 0 )
        {
            SetCursor( m_hOldCursor );
            m_hOldCursor = 0; 
        }
    }

    CWnd::OnMouseMove(nFlags, point);
}

bool CAboutDlg::IsOverHyperlink( UINT id, const CPoint& point )
{
    bool hit = false;

    CWnd * const pControl = GetDlgItem( id );
    if ( pControl )
    {
        CRect r;
        pControl->GetWindowRect( &r );
        ScreenToClient( &r );
        hit = ( r.PtInRect( point ) == TRUE );
    }

    return hit;
}

void CAboutDlg::OnLButtonDown( UINT /* nFlags */, CPoint point )
{
    if ( IsOverHyperlink( IDC_STATIC_COPYRIGHT, point ) )
    {
        ShowUrl( CUSTOM_COMPANY_HOMEPAGE );
    }
    else
    {
        if ( IsOverHyperlink( IDC_VALUE_LOCATION_SRC, point ) )
        {
            ShowUrl( CUSTOM_OPENSTREETMAP_LICENSE );
        }
    }
}

void CAboutDlg::ShowUrl( const TCHAR* url )
{
    const HCURSOR hWait = ::LoadCursor( NULL, IDC_WAIT );
    const HCURSOR hOld  = SetCursor( hWait );

    ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_SHOWNORMAL);
    Sleep(2000);

    SetCursor( hOld );
}



void CAboutDlg::OnBnClickedCloseAbout()
{
    ::PostMessage(m_hWnd, WM_CLOSE,0,0);
}
