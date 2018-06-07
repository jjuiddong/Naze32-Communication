//
// 2018-06-07, jjuiddong
// naze32 serial communication
// - only receive remote control information
//		- receive 30 per 1 second (FPS: 30)
//
//-------------------------------------
// Naze32 Telemetry Protocol
//
// Naze32 send protocol
// $M>
// data length (1 byte)
// protocol id (1 byte)
// data (data length byte)
// checksum $M> 를 제외한 나머지 byte xor 값
//
//-------------------------------------
// Naze32 receive protocol
// $M<
// data length (1 byte)
// protocol id (1 byte)
// data (data length byte)
// checksum $M> 를 제외한 나머지 byte xor 값
//
//
#pragma once

#include "Serial.h"
#include "autocs.h"


class cNaze32Serial
{
public:
	enum {MAX_RC = 6};

	cNaze32Serial();
	virtual ~cNaze32Serial();

	bool Open(int nPort = 2, int nBaud = 115200);
	bool SendCommand(const unsigned char cmd);
	bool ReadRCData(int out[MAX_RC]);
	bool IsOpen();
	bool Close();


public:
	struct eState { 
		enum Enum {
			CLOSE
			, OPEN
		};
	};

	eState::Enum m_state;
	common::cSerial m_serial;
	std::thread m_thread;
	common::CriticalSection m_cs;
	int m_rcData[MAX_RC];
	int m_rcvCount;
	int m_errCount;
};
