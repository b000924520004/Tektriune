
// MFCApplicationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFCApplication.h"
#include "MFCApplicationDlg.h"
#include "afxdialogex.h"
//#include "XeoVMWare.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CImage img;
CString strPrjPath ;	//設定檔路徑(包含檔名)
BOOL ButtonClick = FALSE;
CBitmap bmp; 


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCApplicationDlg dialog



CMFCApplicationDlg::CMFCApplicationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFCApplicationDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplicationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCApplicationDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_Ellipse, &CMFCApplicationDlg::OnBnClickedEllipse)
	ON_BN_CLICKED(IDC_Open, &CMFCApplicationDlg::OnBnClickedOpen)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CMFCApplicationDlg message handlers

BOOL CMFCApplicationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	GetDlgItem(IDC_Ellipse)->ShowWindow(SW_HIDE);
	GetWindowRect(m_rect);
	m_nScrollPos = 0;
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCApplicationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFCApplicationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		//CRect rect;
		GetClientRect(&m_rect);
		int x = (m_rect.Width() - cxIcon + 1) / 2;
		int y = (m_rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this); // device context for painting
		
		if(!img.IsNull()) 
		{
			img.BitBlt(dc,0,0,img.GetWidth(),img.GetHeight(),0,0,SRCCOPY);
			CSize DCSize(img.GetWidth(),img.GetHeight());
		}
		
		if(ButtonClick)  
		{ 
			//ButtonClick=FALSE;
			CPen pen(PS_INSIDEFRAME, 10, RGB(255,0,0));
			dc.SelectObject(pen);
			dc.SelectStockObject(NULL_BRUSH);
			dc.Ellipse(50,50,150,150);  
			//img.Save(_T("Output.jpg"));
		}
		dc.SetWindowOrg(m_nHScrollPos,m_nVScrollPos);
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFCApplicationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFCApplicationDlg::OnBnClickedOpen()
{
	// TODO: Add your control notification handler code here
	TCHAR szFilters[] = _T("(*.jpg)|(*.bmp)|All Files (*.*)|*.*||") ;
	CFileDialog fileDlg(TRUE, _T("jpg"), _T("*.jpg"), NULL, szFilters, this);

	if(fileDlg.DoModal() == IDOK)
	{	
		strPrjPath = fileDlg.GetPathName() ;
		img.Load(strPrjPath);
		//this->Invalidate(FALSE) ;
		//this->RedrawWindow();
	
		GetDlgItem(IDC_Ellipse)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_Open)->ShowWindow(SW_HIDE);

		RedrawWindow();

		/*HBITMAP hBitmap=img.Detach();
		CBitmap bmp;                    
		bmp.Attach(hBitmap);*/
	}
}

void CMFCApplicationDlg::OnBnClickedEllipse()
{
	// TODO: Add your control notification handler code here
	ButtonClick=TRUE;
	GetDlgItem(IDC_Ellipse)->ShowWindow(SW_HIDE);
	RedrawWindow();
}


void CMFCApplicationDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default.
	int nDelta;
	int nMaxPos = m_rect.Height() - m_nCurHeight;

	switch (nSBCode)
	{
	case SB_LINEDOWN:
		if (m_nScrollPos >= nMaxPos)
			return;
		nDelta = min(nMaxPos/100,nMaxPos-m_nScrollPos);
		break;

	case SB_LINEUP:
		if (m_nScrollPos <= 0)
			return;
		nDelta = -min(nMaxPos/100,m_nScrollPos);
		break;

    case SB_PAGEDOWN:
		if (m_nScrollPos >= nMaxPos)
			return;
		nDelta = min(nMaxPos/10,nMaxPos-m_nScrollPos);
		break;

	case SB_THUMBPOSITION:
		nDelta = (int)nPos - m_nScrollPos;
		break;

	case SB_PAGEUP:
		if (m_nScrollPos <= 0)
			return;
		nDelta = -min(nMaxPos/10,m_nScrollPos);
		break;
	
    default:
		return;
	}
	m_nScrollPos += nDelta;
	SetScrollPos(SB_VERT,m_nScrollPos,TRUE);
	ScrollWindow(0,-nDelta);
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CMFCApplicationDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)        
{
	int nDelta;
	int nMaxPos = m_rect.Height() - m_nCurHeight;

	switch (nSBCode)
	{
	case SB_LINELEFT:
		if (m_nScrollPos >= nMaxPos)
			return;
		nDelta = min(nMaxPos/100,nMaxPos-m_nScrollPos);
		break;

	case SB_LINERIGHT:
		if (m_nScrollPos <= 0)
			return;
		nDelta = -min(nMaxPos/100,m_nScrollPos);
		break;

    case SB_PAGELEFT:
		if (m_nScrollPos >= nMaxPos)
			return;
		nDelta = min(nMaxPos/10,nMaxPos-m_nScrollPos);
		break;

	case SB_THUMBPOSITION:
		nDelta = (int)nPos - m_nScrollPos;
		break;

	case SB_PAGERIGHT:
		if (m_nScrollPos <= 0)
			return;
		nDelta = -min(nMaxPos/10,m_nScrollPos);
		break;
	
    default:
		return;
	}
	m_nScrollPos += nDelta;
	SetScrollPos(SB_HORZ,m_nScrollPos,TRUE);
	ScrollWindow(-nDelta,0);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CMFCApplicationDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here.
	m_nCurHeight = cy;
	int nScrollMax;
	if (cy < m_rect.Height())
	{
	     nScrollMax = m_rect.Height() - cy;
	}
	else
	     nScrollMax = 0;

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL; // SIF_ALL = SIF_PAGE | SIF_RANGE | SIF_POS;
	si.nMin = 0;
	si.nMax = nScrollMax;
	si.nPage = si.nMax/10;
	si.nPos = 0;
        SetScrollInfo(SB_VERT, &si, TRUE); 
}