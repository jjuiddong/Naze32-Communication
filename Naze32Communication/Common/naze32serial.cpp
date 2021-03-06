
#include "stdafx.h"
#include "naze32serial.h"
#include "protocol.h"
#include "timer.h"


//
// Naze32 Telemetry Protocol
//
// Naze32 send protocol
// $M>
// data length (1 byte)
// protocol id (1 byte)
// data (data length byte)
// checksum $M> 를 제외한 나머지 byte xor 값
//
void NazeSerialProc(cNaze32Serial *naze);


cNaze32Serial::cNaze32Serial()
	: m_state(eState::CLOSE)
	, m_rcvCount(0)
	, m_errCount(0)
{
	ZeroMemory(m_rcData, sizeof(m_rcData));
}

cNaze32Serial::~cNaze32Serial()
{
	Close();
}


bool cNaze32Serial::Open(
	int nPort //= 2
	, int nBaud //= 115200
)
{
	if (m_serial.Open(nPort, nBaud))
	{
		m_state = eState::OPEN;
		m_thread = std::thread(NazeSerialProc, this);
		return true;
	}

	return false;
}


// cmd : protocol.h definition
bool cNaze32Serial::SendCommand(const unsigned char cmd)
{
	unsigned char packet[64];
	int checksum = 0;
	int idx = 0;
	packet[idx++] = '$';
	packet[idx++] = 'M';
	packet[idx++] = '<';
	packet[idx++] = 0;
	checksum ^= 0;
	packet[idx++] = cmd;
	checksum ^= cmd;
	packet[idx++] = checksum;
	
	const int result = m_serial.SendData((char*)packet, idx);
	return result == idx;
}


bool cNaze32Serial::ReadRCData(int out[MAX_RC])
{
	if (m_state != eState::OPEN)
		return false;

	common::AutoCSLock cs(m_cs);
	memcpy(out, m_rcData, sizeof(m_rcData));
	return true;
}


bool cNaze32Serial::IsOpen()
{
	return m_state == eState::OPEN;
}


bool cNaze32Serial::Close()
{
	m_state = eState::CLOSE;
	if (m_thread.joinable())
		m_thread.join();

	m_serial.Close();

	return true;
}


// thread function
void NazeSerialProc(cNaze32Serial *naze)
{
	common::cTimer timer;
	timer.Create();

	while (naze->m_state == cNaze32Serial::eState::OPEN)
	{
		//1. send rc request packet
		if (!naze->SendCommand(MSP_RC))
			continue;

		//2. delay, 33 millisecond (FPS: 30)
		std::this_thread::sleep_for( std::chrono::duration<double, std::milli>(33) );

		//3. receive rc ack packet
		int state = 0;
		int dataLen = 0;
		int readLen = 0;
		int msp = 0;
		int readData = 0;
		int curByteSize = 0;
		int readRcIndex = 0;
		int checkSum = 0;
		int rcData[cNaze32Serial::MAX_RC];
		ZeroMemory(rcData, sizeof(rcData));

		//
		// Naze32 Telemetry Protocol
		//
		// Naze32 send protocol
		// $M>
		// data length (1 byte)
		// protocol id (1 byte)
		// data (data length byte)
		// checksum $M> 를 제외한 나머지 byte xor 값
		//
		const double beginT = timer.GetMilliSeconds();
		bool finishRead = false;
		while (!finishRead && (naze->m_state == cNaze32Serial::eState::OPEN))
		{
			char rcvBuff[64];
			const int len = naze->m_serial.ReadData(rcvBuff, sizeof(rcvBuff));
			if (len <= 0)
			{
				std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(10));
				continue;
			}

			// check timeout
			if (!finishRead)
				finishRead = ((timer.GetMilliSeconds() - beginT) > 500);

			// parsing packet
			for (int i = 0; i < len; ++i)
			{
				const unsigned char c = rcvBuff[i];
				switch (state)
				{
				case 0: state = (c == '$') ? 1 : 0; break;
				case 1: state = (c == 'M') ? 2 : 0; break;
				case 2: state = (c == '>') ? 3 : 0; break;
				case 3: 
					dataLen = c;
					checkSum ^= c;
					state = 4;
					break;
				case 4:
					msp = c;
					checkSum ^= c;
					readData = 0;
					state = 5;
					break;

				case 5:
					if (readLen < dataLen)
					{
						checkSum ^= c;
						readData |= c << (8 * curByteSize++);
						if (curByteSize >= 2) // 2 byte read
						{
							curByteSize = 0;
							if (ARRAYSIZE(rcData) > readRcIndex)
								rcData[readRcIndex++] = readData;
							readData = 0;
						}
					}
					else
					{
						if (checkSum == c)
						{
							// success read
							common::AutoCSLock cs(naze->m_cs);
							memcpy(naze->m_rcData, rcData, sizeof(rcData));
							naze->m_rcvCount++;
							finishRead = true;
						}
						else
						{
							// fail read
							naze->m_errCount++;
							finishRead = true;
						}
					}

					++readLen;
					break;

				default: 
					assert(0); 
					break;
				}
			} // end for
		} // end while
	} // end while
}
