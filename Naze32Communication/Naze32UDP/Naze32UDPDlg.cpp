
// Naze32UDPDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Naze32UDP.h"
#include "Naze32UDPDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CNaze32UDPDlg dialog
CNaze32UDPDlg::CNaze32UDPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_NAZE32UDP_DIALOG, pParent)
	, m_loop(true)
	, m_udpPort(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNaze32UDPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_slider1);
	DDX_Control(pDX, IDC_SLIDER2, m_slider2);
	DDX_Control(pDX, IDC_SLIDER3, m_slider3);
	DDX_Control(pDX, IDC_SLIDER4, m_slider4);
	DDX_Control(pDX, IDC_SLIDER5, m_slider5);
	DDX_Control(pDX, IDC_SLIDER6, m_slider6);
	DDX_Control(pDX, IDC_STATIC_RC1, m_staticRC1);
	DDX_Control(pDX, IDC_STATIC_RC2, m_staticRC2);
	DDX_Control(pDX, IDC_STATIC_RC3, m_staticRC3);
	DDX_Control(pDX, IDC_STATIC_RC4, m_staticRC4);
	DDX_Control(pDX, IDC_STATIC_RC5, m_staticRC5);
	DDX_Control(pDX, IDC_STATIC_RC6, m_staticRC6);
	DDX_Control(pDX, IDC_COMBO_COM, m_comboDev);
	DDX_Control(pDX, IDC_IPADDRESS_UDP_IP, m_ipAddressUDP);
	DDX_Text(pDX, IDC_EDIT_UDP_PORT, m_udpPort);
}

BEGIN_MESSAGE_MAP(CNaze32UDPDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CNaze32UDPDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CNaze32UDPDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CNaze32UDPDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_UDP_SEND, &CNaze32UDPDlg::OnBnClickedButtonUdpSend)
END_MESSAGE_MAP()


// CNaze32UDPDlg message handlers

BOOL CNaze32UDPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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
	m_timer.Create();

	const int LOWER = 990;
	const int UPPER = 2000;
	m_slider1.SetRange(LOWER, UPPER);
	m_slider2.SetRange(LOWER, UPPER);
	m_slider3.SetRange(LOWER, UPPER);
	m_slider4.SetRange(LOWER, UPPER);
	m_slider5.SetRange(LOWER, UPPER);
	m_slider6.SetRange(LOWER, UPPER);

	m_comboDev.InitList();

	m_ipAddressUDP.SetAddress(127, 0, 0, 1);
	m_udpPort = 1000;
	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNaze32UDPDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNaze32UDPDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNaze32UDPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CNaze32UDPDlg::Run()
{
	while (m_loop)
	{
		MSG msg;
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage(&msg, NULL, 0, 0))
			{
				break;
			}
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		const double curT = m_timer.GetMilliSeconds();
		static double oldT = curT;
		const double deltaT = curT - oldT;
		const float t = (float)(deltaT * 0.001f);
		oldT = curT;

		Process(t);
	}
}


void CNaze32UDPDlg::Process(const float deltaSeconds)
{
	if (!m_naze.IsOpen())
		return;

	int rcData[cNaze32Serial::MAX_RC];
	if (m_naze.ReadRCData(rcData))
	{
		m_slider1.SetPos(rcData[0]);
		m_slider2.SetPos(rcData[1]);
		m_slider3.SetPos(rcData[2]);
		m_slider4.SetPos(rcData[3]);
		m_slider5.SetPos(rcData[4]);
		m_slider6.SetPos(rcData[5]);

		TCHAR text[64];
		wsprintf(text, L"%d", rcData[0]);
		m_staticRC1.SetWindowTextW(text);
		wsprintf(text, L"%d", rcData[1]);
		m_staticRC2.SetWindowTextW(text);
		wsprintf(text, L"%d", rcData[2]);
		m_staticRC3.SetWindowTextW(text);
		wsprintf(text, L"%d", rcData[3]);
		m_staticRC4.SetWindowTextW(text);
		wsprintf(text, L"%d", rcData[4]);
		m_staticRC5.SetWindowTextW(text);
		wsprintf(text, L"%d", rcData[5]);
		m_staticRC6.SetWindowTextW(text);


		static double oldT = m_timer.GetMilliSeconds();
		const double curT = m_timer.GetMilliSeconds();
		if (m_udpClient.IsConnect() && (curT - oldT > 33))
		{
			oldT = curT;

			sUDP_Packet packet;
			memcpy(packet.rc, rcData, sizeof(rcData));
			m_udpClient.SendData((BYTE*)&packet, sizeof(packet));
		}
	}
}


void CNaze32UDPDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}


void CNaze32UDPDlg::OnBnClickedCancel()
{
	//CDialog::OnCancel();
	m_loop = false;
}


void CNaze32UDPDlg::OnBnClickedButtonConnect()
{
	const int portNum = m_comboDev.GetPortNum();
	if (!m_naze.Open(portNum))
	{
		::AfxMessageBox(L"Error!! Open Serial Port");
	}
}


void CNaze32UDPDlg::OnBnClickedButtonUdpSend()
{
	UpdateData();

	BYTE address[4];
	m_ipAddressUDP.GetAddress(address[0], address[1], address[2], address[3]);

	char ip[32];
	sprintf(ip, "%d.%d.%d.%d", address[0], address[1], address[2], address[3]);

	if (m_udpClient.Init(ip, m_udpPort))
	{
		SetWindowText(L"Naze32UDP - Send UDP");
	}
	else
	{
		::AfxMessageBox(L"Error Open UDP Port");
	}
}
