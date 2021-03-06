
#pragma once

#include "../Common/udpclient.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "ComPortCombo.h"

struct sUDP_Packet
{
	int rc[6]; // cNaze32Serial::MAX_RC
};


class CNaze32UDPDlg : public CDialog
{
public:
	CNaze32UDPDlg(CWnd* pParent = NULL);	// standard constructor
	enum { IDD = IDD_NAZE32UDP_DIALOG };

	void Run();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void Process(const float deltaSeconds);


protected:
	HICON m_hIcon;
	bool m_loop;
	common::cTimer m_timer;
	cNaze32Serial m_naze;
	network::cUDPClient m_udpClient;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnSelchangeComboCom();
	afx_msg void OnBnClickedButtonConnect();
	CSliderCtrl m_slider1;
	CSliderCtrl m_slider2;
	CSliderCtrl m_slider3;
	CSliderCtrl m_slider4;
	CSliderCtrl m_slider5;
	CSliderCtrl m_slider6;
	CStatic m_staticRC1;
	CStatic m_staticRC2;
	CStatic m_staticRC3;
	CStatic m_staticRC4;
	CStatic m_staticRC5;
	CStatic m_staticRC6;
	CComPortCombo m_comboDev;
	CIPAddressCtrl m_ipAddressUDP;
	int m_udpPort;
	afx_msg void OnBnClickedButtonUdpSend();
};
