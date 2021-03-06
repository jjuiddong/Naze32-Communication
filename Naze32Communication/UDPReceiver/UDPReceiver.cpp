// UDPReceiver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../Common/udpserver.h"

#include <iostream>
using namespace std;


int main()
{
	network::cUDPServer udpServer;

	struct sUDP_Packet
	{
		int rc[6]; // cNaze32Serial::MAX_RC
	};

	if (!udpServer.Init(0, 1000))
	{
		cout << "error open udp port" << endl;
	}

	while (udpServer.IsConnect())
	{
		sUDP_Packet packet;
		if (udpServer.GetRecvData((BYTE*)&packet, sizeof(packet)) > 0)
		{
			for (int i = 0; i < 6; ++i)
			{
				cout << packet.rc[i] << " ";
			}
			cout << endl;
		}
	}

    return 0;
}

