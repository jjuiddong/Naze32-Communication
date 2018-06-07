
#include "stdafx.h"
#include "launcher.h"

using namespace network;
using namespace common;


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool	network::LaunchTCPClient(const std::string &ip, const int port, OUT SOCKET &out, const bool isLog)
{
	const string tmpIp = ip; // thread safety

	// ���� ������ Ȯ�� �մϴ�.
	//WORD wVersionRequested = MAKEWORD(1,1);
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	int nRet = WSAStartup(wVersionRequested, &wsaData);
	if (wsaData.wVersion != wVersionRequested)
	{
		//clog::Error( clog::ERROR_CRITICAL, "���� ������ Ʋ�Ƚ��ϴ�\n" );
		if (isLog)
			dbg::ErrLog("���� ������ Ʋ�Ƚ��ϴ�\n");
		return false;
	}

	LPHOSTENT lpHostEntry;
	lpHostEntry = gethostbyname(tmpIp.c_str());
	if(lpHostEntry == NULL)
	{
		//clog::Error( clog::ERROR_CRITICAL, "gethostbyname() error\n" );
		if (isLog)
			dbg::ErrLog("gethostbyname() error\n");
		return false;
	}

	// TCP/IP ��Ʈ�� ������ �����մϴ�.
	// socket(�ּ� �迭, ���� ����, ��������
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		//clog::Error( clog::ERROR_CRITICAL, "socket() error\n" );
		if (isLog)
			dbg::ErrLog("socket() error\n");
		return false;
	}

	// �ּ� ����ü�� ä��ϴ�.
	SOCKADDR_IN saServer;
	saServer.sin_family = AF_INET;
	saServer.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list); // ���� �ּ�
	saServer.sin_port = htons(port);

	// ������ �����մϴ�
	// connect(����, ���� �ּ�, ���� �ּ��� ����
	nRet = connect(clientSocket, (LPSOCKADDR)&saServer, sizeof(struct sockaddr) );
	if (nRet == SOCKET_ERROR)
	{
		//clog::Error( clog::ERROR_CRITICAL, "connect() error ip=%s, port=%d\n", ip.c_str(), port );
		if (isLog)
			dbg::ErrLog("connect() error ip=%s, port=%d\n", ip.c_str(), port);
		closesocket(clientSocket);
		return false;
	}

	out = clientSocket;

	return true;
}



//------------------------------------------------------------------------
// ���� ����
//------------------------------------------------------------------------
bool network::LaunchTCPServer(const int port, OUT SOCKET &out, const bool isLog)
{
	// ������ �����ϰ� ������ Ȯ���մϴ�
	WORD wVersionRequested = MAKEWORD(1, 1);
	WSADATA wsaData;
	int nRet = WSAStartup(wVersionRequested, &wsaData);
	if (wsaData.wVersion != wVersionRequested)
	{
		//clog::Error( clog::ERROR_CRITICAL,  "���� ������ Ʋ�Ƚ��ϴ�\n" );
		if (isLog)
			dbg::ErrLog("���� ������ Ʋ�Ƚ��ϴ�\n");
		return false;
	}

	// socket(�ּҰ迭, ���� ����, ��������)
	SOCKET svrSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(svrSocket == INVALID_SOCKET)
	{
		//clog::Error( clog::ERROR_CRITICAL, "socket() error\n" );
		if (isLog)
			dbg::ErrLog("socket() error\n");
		return false;
	}

	// �ּ� ����ü�� ä��ϴ�.
	SOCKADDR_IN saServer;
	saServer.sin_family = AF_INET;
	saServer.sin_addr.s_addr = INADDR_ANY;    // ������ �����ϰ� �Ӵϴ�.
	saServer.sin_port = htons(port);		// ����ٿ��� ���� ��Ʈ�� �ֽ��ϴ�.

	// ���ϰ� listensocket �� bind(����) �մϴ�.
	// bind(����, ���� �ּ�, �ּ� ����ü�� ����
	nRet = bind(svrSocket, (LPSOCKADDR)&saServer, sizeof(struct sockaddr) );
	if (nRet == SOCKET_ERROR)
	{
		//clog::Error( clog::ERROR_CRITICAL,  "bind() error port: %d\n", port );
		if (isLog)
			dbg::ErrLog("bind() error port: %d\n", port);
		closesocket(svrSocket);
		return false;
	}

	char szBuf[256];
	nRet = gethostname( szBuf, sizeof(szBuf) );
	if (nRet == SOCKET_ERROR)
	{
		//clog::Error( clog::ERROR_CRITICAL, "gethostname() error\n" );
		if (isLog)
			dbg::ErrLog("gethostname() error\n");
		closesocket(svrSocket);
		return false;
	}

	// listen(���� ����, ��û ���� ������ �뷮)
	nRet = listen(svrSocket, SOMAXCONN);

	if (nRet == SOCKET_ERROR)
	{
		//clog::Error( clog::ERROR_CRITICAL, "listen() error\n" );
		if (isLog)
			dbg::ErrLog("listen() error\n");
		closesocket(svrSocket);
		return false;
	}

	out = svrSocket;

	return true;
}



//------------------------------------------------------------------------
// ���� ����
//------------------------------------------------------------------------
bool network::LaunchUDPServer(const int port, OUT SOCKET &out, const bool isLog)
{
	// ������ �����ϰ� ������ Ȯ���մϴ�
	WORD wVersionRequested = MAKEWORD(1, 1);
	WSADATA wsaData;
	int nRet = WSAStartup(wVersionRequested, &wsaData);
	if (wsaData.wVersion != wVersionRequested)
	{
		//clog::Error( clog::ERROR_CRITICAL,  "���� ������ Ʋ�Ƚ��ϴ�\n" );
		if (isLog)
			dbg::ErrLog("���� ������ Ʋ�Ƚ��ϴ�\n");
		return false;
	}

	// socket(�ּҰ迭, ���� ����, ��������)
	//	SOCKET svrSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKET svrSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (svrSocket == INVALID_SOCKET)
	{
		//clog::Error( clog::ERROR_CRITICAL, "socket() error\n" );
		if (isLog)
			dbg::ErrLog("socket() error\n");
		return false;
	}

	// �ּ� ����ü�� ä��ϴ�.
	SOCKADDR_IN saServer;
	saServer.sin_family = AF_INET;
	saServer.sin_addr.s_addr = INADDR_ANY;    // ������ �����ϰ� �Ӵϴ�.
	saServer.sin_port = htons(port);		// ����ٿ��� ���� ��Ʈ�� �ֽ��ϴ�.

	// ���ϰ� listensocket �� bind(����) �մϴ�.
	// bind(����, ���� �ּ�, �ּ� ����ü�� ����
	nRet = bind(svrSocket, (LPSOCKADDR)&saServer, sizeof(struct sockaddr));
	if (nRet == SOCKET_ERROR)
	{
		//clog::Error( clog::ERROR_CRITICAL,  "bind() error port: %d\n", port );
		if (isLog)
			dbg::ErrLog("bind() error port: %d\n", port);
		closesocket(svrSocket);
		return false;
	}

	char szBuf[256];
	nRet = gethostname(szBuf, sizeof(szBuf));
	if (nRet == SOCKET_ERROR)
	{
		//clog::Error( clog::ERROR_CRITICAL, "gethostname() error\n" );
		if (isLog)
			dbg::ErrLog("gethostname() error\n");
		closesocket(svrSocket);
		return false;
	}


	out = svrSocket;

	return true;
}



//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool network::LaunchUDPClient(const std::string &ip, const int port, OUT SOCKADDR_IN &sockAddr, OUT SOCKET &out, const bool isLog)
{
	// ���� ������ Ȯ�� �մϴ�.
	WORD wVersionRequested = MAKEWORD(1, 1);
	WSADATA wsaData;
	int nRet = WSAStartup(wVersionRequested, &wsaData);
	if (wsaData.wVersion != wVersionRequested)
	{
		//clog::Error( clog::ERROR_CRITICAL, "���� ������ Ʋ�Ƚ��ϴ�\n" );
		if (isLog)
			dbg::ErrLog("���� ������ Ʋ�Ƚ��ϴ�\n");
		return false;
	}

	LPHOSTENT lpHostEntry;
	lpHostEntry = gethostbyname(ip.c_str());
	if (lpHostEntry == NULL)
	{
		//clog::Error( clog::ERROR_CRITICAL, "gethostbyname() error\n" );
		if (isLog)
			dbg::ErrLog("gethostbyname() error\n");
		return false;
	}

	// TCP/IP ��Ʈ�� ������ �����մϴ�.
	// socket(�ּ� �迭, ���� ����, ��������
	SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		//clog::Error( clog::ERROR_CRITICAL, "socket() error\n" );
		if (isLog)
			dbg::ErrLog("socket() error\n");
		return false;
	}

	// �ּ� ����ü�� ä��ϴ�.
	//SOCKADDR_IN saServer;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list); // ���� �ּ�
	sockAddr.sin_port = htons(port);

	// ������ �����մϴ�
	// connect(����, ���� �ּ�, ���� �ּ��� ����
	nRet = connect(clientSocket, (LPSOCKADDR)&sockAddr, sizeof(struct sockaddr));
	if (nRet == SOCKET_ERROR)
	{
		//clog::Error( clog::ERROR_CRITICAL, "connect() error ip=%s, port=%d\n", ip.c_str(), port );
		if (isLog)
			dbg::ErrLog("connect() error ip=%s, port=%d\n", ip.c_str(), port);
		closesocket(clientSocket);
		return false;
	}

	out = clientSocket;

	return true;
}