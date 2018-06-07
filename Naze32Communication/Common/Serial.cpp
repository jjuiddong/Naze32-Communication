#include "stdafx.h"
#include "Serial.h"


#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04
#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13
using namespace common;


cSerial::cSerial()
{
	ZeroMemory( &m_overlappedRead, sizeof( OVERLAPPED ) );
	ZeroMemory( &m_overlappedWrite, sizeof( OVERLAPPED ) );
	m_hIDComDev = NULL;
	m_isOpened = false;

}

cSerial::~cSerial()
{
	Close();
}


bool cSerial::Open( 
	int nPort // = 2
	, int nBaud  // = 9600
)
{
	if (m_isOpened) 
		return true;

	TCHAR szPort[15];
	TCHAR szComParams[50];
	DCB dcb;

	wsprintf(szPort, TEXT("\\\\.\\COM%d"), nPort);
	m_hIDComDev = CreateFile( szPort, GENERIC_READ | GENERIC_WRITE, 0
		, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL );

	if (m_hIDComDev == NULL) 
		return false;

	memset( &m_overlappedRead, 0, sizeof( OVERLAPPED ) );
 	memset( &m_overlappedWrite, 0, sizeof( OVERLAPPED ) );

	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 5000;
	SetCommTimeouts( m_hIDComDev, &CommTimeOuts );

	wsprintf( szComParams, TEXT("COM%d:%d,n,8,1"), nPort, nBaud );

	m_overlappedRead.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_overlappedWrite.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

	dcb.DCBlength = sizeof( DCB );
	GetCommState( m_hIDComDev, &dcb );
	dcb.BaudRate = nBaud;
	dcb.ByteSize = 8;
	unsigned char ucSet;
	ucSet = (unsigned char) ( ( FC_RTSCTS & FC_DTRDSR ) != 0 );
	ucSet = (unsigned char) ( ( FC_RTSCTS & FC_RTSCTS ) != 0 );
	ucSet = (unsigned char) ( ( FC_RTSCTS & FC_XONXOFF ) != 0 );
	if (!SetCommState(m_hIDComDev, &dcb) ||
		!SetupComm( m_hIDComDev, 10000, 10000 ) ||
		m_overlappedRead.hEvent == NULL ||
		m_overlappedWrite.hEvent == NULL)
	{
		DWORD dwError = GetLastError();
		if( m_overlappedRead.hEvent != NULL ) 
			CloseHandle( m_overlappedRead.hEvent );
		if( m_overlappedWrite.hEvent != NULL ) 
			CloseHandle( m_overlappedWrite.hEvent );
		CloseHandle( m_hIDComDev );
		return false;
	}

	m_isOpened = true;

	return true;
 }


bool cSerial::Close()
{
	if( !m_isOpened || m_hIDComDev == NULL ) 
		return true;

	if( m_overlappedRead.hEvent != NULL ) 
		CloseHandle( m_overlappedRead.hEvent );
	if( m_overlappedWrite.hEvent != NULL ) 
		CloseHandle( m_overlappedWrite.hEvent );
	CloseHandle( m_hIDComDev );
	m_isOpened = false;
	m_hIDComDev = NULL;

	return true;
}


bool cSerial::WriteCommByte( unsigned char ucByte )
{
	bool bWriteStat;
	DWORD dwBytesWritten;

	bWriteStat = WriteFile( m_hIDComDev, (LPSTR) &ucByte, 1, &dwBytesWritten, &m_overlappedWrite );
	if( !bWriteStat && ( GetLastError() == ERROR_IO_PENDING ) )
	{
		if( WaitForSingleObject( m_overlappedWrite.hEvent, 1000 ) ) 
		{ 
			dwBytesWritten = 0;
		}
		else
		{
			GetOverlappedResult( m_hIDComDev, &m_overlappedWrite, &dwBytesWritten, FALSE );
			m_overlappedWrite.Offset += dwBytesWritten;
		}
	}
	return true;
}


int cSerial::SendData( const char *buffer, int size )
{
	if( !m_isOpened || m_hIDComDev == NULL ) 
		return( 0 );

	DWORD dwBytesWritten;
	const int bWriteStat = WriteFile(m_hIDComDev, buffer, size, &dwBytesWritten, &m_overlappedWrite);
	if (!bWriteStat && (GetLastError() == ERROR_IO_PENDING))
	{
		if (WaitForSingleObject(m_overlappedWrite.hEvent, 1000))
		{
			dwBytesWritten = 0;
		}
		else
		{
			GetOverlappedResult(m_hIDComDev, &m_overlappedWrite, &dwBytesWritten, FALSE);
			m_overlappedWrite.Offset += dwBytesWritten;
		}
	}
	return (int)dwBytesWritten;
}


int cSerial::ReadDataWaiting()
{
	if (!m_isOpened || m_hIDComDev == NULL) 
		return 0;

	DWORD dwErrorFlags;
	COMSTAT ComStat;
	ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );
	return (int)ComStat.cbInQue;
}


int cSerial::ReadData( void *buffer, int size )
{
	if (!m_isOpened || m_hIDComDev == NULL)
		return 0;

	bool bReadStatus;
	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;

	const bool isSucceed = ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );
	if (!ComStat.cbInQue) 
		return 0;
	if (!isSucceed)
	{
		m_isOpened = false;
		return -1;
	}

	dwBytesRead = (DWORD) ComStat.cbInQue;
	if (size < (int) dwBytesRead) 
		dwBytesRead = (DWORD)size;

	bReadStatus = ReadFile( m_hIDComDev, buffer, dwBytesRead, &dwBytesRead, &m_overlappedRead );
	if (!bReadStatus)
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			WaitForSingleObject( m_overlappedRead.hEvent, 2000 );
			return (int)dwBytesRead;
		}
		return 0;
	}

	return (int)dwBytesRead;
}

