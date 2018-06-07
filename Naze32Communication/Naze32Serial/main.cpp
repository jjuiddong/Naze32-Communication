// Naze32Serial.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "../Common/naze32serial.h"

using namespace std;

int main()
{
	cNaze32Serial naze;
	if (!naze.Open(4))
	{
		cout << "Error Open Serial Port" << endl;
		return 0;
	}

	int oldRcvCnt = 0;
	int oldErrCnt = 0;
	while (1)
	{
		if (oldRcvCnt != naze.m_rcvCount)
		{
			//cout << "rcv cnt = " << naze.m_rcvCount << endl;
			oldRcvCnt = naze.m_rcvCount;
		}

		if (oldErrCnt != naze.m_errCount)
		{
			//cout << "err cnt = " << naze.m_errCount << endl;
			oldErrCnt = naze.m_errCount;
		}

		int rcData[cNaze32Serial::MAX_RC];
		if (naze.ReadRCData(rcData))
		{
			for (int i = 0; i < cNaze32Serial::MAX_RC; ++i)
				cout << rcData[i] << " ";
			cout << endl;
		}
	}

	return 0;
}

