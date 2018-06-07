//
// 2015-11-29, jjuiddong
//
// - ��Ŷ�� �����Ѵ�. Thread Safe, �ִ��� �����ϰ� �������.
// - ť�� ��������, ���� ������ ��Ŷ�� �����ϰ�, �߰��Ѵ�.
// - �� ���ϸ��� packetSize ũ�⸸ŭ ä�� ������ �������� �Ѿ�� �ʴ´�.
// - ��Ŷ�� ť�� ������ ��, ���(sHeader)�� �߰��ȴ�.
// - ���ϸ��� �ϳ� �̻��� ť�� ���� �� �ִ�.
//
// 2016-06-29, jjuiddong
//		- sHeader�� ���� ��Ŷ�� ���� �� �ְ���
//		- Init(),  isIgnoreHeader �÷��� �߰�
//
// 2016-05-26, jjuiddong
//		- ū ��Ŷ�� �������� �� ��� sHeader ������ ����, ��Ŷ�� �������� �״´�.
//		- ��Ʈ��ũ�� ���� �� ��Ŷ�� 2�� �̻��� ���������� ������ ���, ������ ó���Ѵ�.
//
// 2016-09-24, jjuiddong
//		- �޸�Ǯ�� ��� ����, ������ ��Ŷ�� �����Ѵ�.
//
#pragma once


namespace network
{
	// �� ��Ŷ�� ������ �����Ѵ�.
	// �ϳ��� ��Ŷ�� �������� �������� ��, �� sSockBuffer �� ��� ����ȴ�.
	struct sSockBuffer
	{
		SOCKET sock; // ���� ����
		BYTE *buffer;
		int totalLen;
		bool full; // ���۰� �� ä������ true�� �ȴ�.
		int readLen;
		int actualLen; // ���� ��Ŷ�� ũ�⸦ ��Ÿ����. totalLen - sizeof(sHeader)
	};

	struct sSession;
	class cPacketQueue
	{
	public:
		cPacketQueue();
		virtual ~cPacketQueue();

		struct sHeader
		{
			BYTE head[3]; // $@
			BYTE protocol; // protocol id
			int length;	// packet length (byte)
		};

		bool Init(const int packetSize, const int maxPacketCount, const bool isIgnoreHeader=false);
		void Push(const SOCKET sock, const BYTE *data, const int len, const bool fromNetwork=false);
		bool Front(OUT sSockBuffer &out);
		void Pop();
		void SendAll();
		void SendAll(const sockaddr_in &sockAddr);
		void SendBroadcast(vector<sSession> &sessions, const bool exceptOwner = true);
		void Lock();
		void Unlock();
		int GetSize();
		bool IsIgnoreHeader();
		int GetPacketSize();
		int GetMaxPacketCount();

		vector<sSockBuffer> m_queue;

		// �ӽ� ����
		BYTE *m_tempHeaderBuffer; // �ӽ÷� Header �����ϴ� ����
		int m_tempHeaderBufferSize;
		bool m_isStoreTempHeaderBuffer; // �ӽ÷� �����ϰ� ���� �� true
		BYTE *m_tempBuffer; // �ӽ÷� ����� ����
		int m_tempBufferSize;
		bool m_isIgnoreHeader; // sHeader ����Ÿ�� ���� ��Ŷ�� ���� ��, true
		bool m_isLogIgnorePacket; // ������ ��Ŷ �α׸� ������ ����, default = false

	protected:
		sSockBuffer* FindSockBuffer(const SOCKET sock);
		int CopySockBuffer(sSockBuffer *dst, const BYTE *data, const int len);
		int AddSockBuffer(const SOCKET sock, const BYTE *data, const int len, const bool fromNetwork);

		//---------------------------------------------------------------------
		// Simple Queue Memory Pool
		BYTE* Alloc();
		void Free(BYTE*ptr);
		void ClearMemPool();
		void Clear();

		struct sChunk
		{
			bool used;
			BYTE *p;
		};
		vector<sChunk> m_memPool;
		BYTE *m_memPoolPtr;
		int m_packetBytes;	// ������ ��Ŷ ũ��
		int m_chunkBytes;	// sHeader ����� ������ ��Ŷ ũ��
		int m_totalChunkCount;
		CRITICAL_SECTION m_criticalSection;
	};


	inline bool cPacketQueue::IsIgnoreHeader() { return  m_isIgnoreHeader;  }
	inline int cPacketQueue::GetPacketSize() { return m_packetBytes;  }
	inline int cPacketQueue::GetMaxPacketCount() { return m_totalChunkCount; }
}
