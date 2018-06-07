
#include "stdafx.h"
#include "udpclient.h"
#include "launcher.h"

using namespace std;
using namespace network;

unsigned WINAPI UDPClientThreadFunction(void* arg);


cUDPClient::cUDPClient() : 
	m_isConnect(false)
	, m_threadLoop(true)
	, m_sleepMillis(30)
	, m_maxBuffLen(BUFFER_LENGTH)
{
}

cUDPClient::~cUDPClient()
{
	Close();
}


// UDP Ŭ���Ʈ ����, ip, port �� ������ �õ��Ѵ�.
bool cUDPClient::Init(const string &ip, const int port, const int sleepMillis, const bool isIgnoreHeader) 
//sleepMillis=30, isIgnoreHeader=true
{
	Close();

	m_ip = ip;
	m_port = port;
	m_sleepMillis = sleepMillis;

	if (network::LaunchUDPClient(ip, port, m_sockaddr, m_socket))
	{
		dbg::Log("Connect UDP Client ip=%s, port=%d\n", ip.c_str(), port);

		if (!m_sndQueue.Init(m_maxBuffLen, 512, isIgnoreHeader))
		{
			Close();
			return false;
		}

		m_threadLoop = false;
		if (m_thread.joinable())
			m_thread.join();

		m_isConnect = true;
		m_threadLoop = true;
		m_thread = std::thread(UDPClientThreadFunction, this);
	}
	else
	{
		dbg::Log("Error!! Connect UDP Client ip=%s, port=%d\n", ip.c_str(), port);
		return false;
	}
	
	return true;
}


// ������ ������ �����Ѵ�.
void cUDPClient::SendData(const BYTE *buff, const int buffLen)
{
	m_sndQueue.Push(m_socket, buff, buffLen);
}


void cUDPClient::SetMaxBufferLength(const int length)
{
	m_maxBuffLen = length;
}


// ������ ���´�.
void cUDPClient::Close()
{
	m_isConnect = false;
	m_threadLoop = false;
	if (m_thread.joinable())
		m_thread.join();

	Sleep(100);
	closesocket(m_socket);
	m_socket = INVALID_SOCKET;
}


// UDP ��Ʈ��ũ ������.
unsigned WINAPI UDPClientThreadFunction(void* arg)
{
	cUDPClient *udp = (cUDPClient*)arg;

	while (udp->m_threadLoop)
	{
		if (!udp->m_isConnect || (INVALID_SOCKET == udp->m_socket))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(udp->m_sleepMillis));
			continue;
		}

		udp->m_sndQueue.SendAll(udp->m_sockaddr);

		std::this_thread::sleep_for(std::chrono::milliseconds(udp->m_sleepMillis));
	}

	return 0;
}
