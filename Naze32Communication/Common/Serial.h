//
// 2018-06-07, jjuiddong
// Serial Communication class
// 
// 2018-06-07
//	- refactoring
//
#pragma once


namespace common
{
	class cSerial
	{
	public:
		cSerial();
		virtual ~cSerial();

		bool Open( int nPort = 2, int nBaud = 9600 );
		int ReadData(void *buffer, int size);
		int SendData(const char *buffer, int size);
		int ReadDataWaiting();
		bool IsOpened() const;
		bool Close();


	protected:
		bool WriteCommByte(unsigned char ucByte);


	public:
		bool m_isOpened;
		HANDLE m_hIDComDev;
		OVERLAPPED m_overlappedRead;
		OVERLAPPED m_overlappedWrite;
	};


	inline bool cSerial::IsOpened() const { return(m_isOpened); }
}
