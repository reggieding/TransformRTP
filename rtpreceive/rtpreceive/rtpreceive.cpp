// rtpreceive.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"


/*=============================================================================
*     FileName: jrtplib_receive.cpp
*         Desc: receive packet and print out the payloaddata
*       Author: licaibiao
*   LastChange: 2017-04-10
* =============================================================================*/
#include <rtpsession.h>
#include <rtpudpv4transmitter.h>
#include <rtpipv4address.h>
#include <rtpsessionparams.h>
#include <rtperrors.h>
#include <rtplibraryversion.h>
#include <rtppacket.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>



using namespace jrtplib;


int main(void)
{

	RTPSession session;
	WSADATA dat;
	WSAStartup(MAKEWORD(2, 2), &dat);
	RTPSessionParams sessionparams;
	sessionparams.SetOwnTimestampUnit(1.0 / 8000.0);
	RTPUDPv4TransmissionParams transparams;
	transparams.SetPortbase(6000);
	int oldBufSize = transparams.GetRTPReceiveBuffer();
	transparams.SetRTPReceiveBuffer(oldBufSize * 2);
	int status = session.Create(sessionparams, &transparams);
	int newBufSize = transparams.GetRTPReceiveBuffer();
	int oldBufSizec = transparams.GetRTCPReceiveBuffer();
	transparams.SetRTCPReceiveBuffer(oldBufSizec * 2);
	int newBufSizec = transparams.GetRTCPReceiveBuffer();

	BYTE *pfBuffer;
	BYTE *pBuffer;
	uint32_t destip, destip1;



	destip = ntohl(inet_addr("192.168.1.158"));
	destip1 = ntohl(inet_addr("192.168.1.159"));

	RTPIPv4Address addr(destip, 7000);
	RTPIPv4Address addr1(destip1, 7000);

	session.AddDestination(addr);
	session.AddDestination(addr1);

	/*session.SetDefaultPayloadType(96);
	session.SetDefaultMark(false);
	session.SetDefaultTimestampIncrement(160);*/
	bool g_RTPThreadRun = true;
	while (g_RTPThreadRun)
	{

		session.BeginDataAccess();
		if (session.GotoFirstSourceWithData())
		{
			do
			{
				RTPPacket *pack;

				while ((pack = session.GetNextPacket()) != NULL)
				{
				    uint8_t t = pack->GetPayloadType();
					bool mark = pack->HasMarker();
					uint32_t timestam = pack->GetTimestamp();

					int nLen = pack->GetPayloadLength();

					pfBuffer = (unsigned char*)pack->GetPayloadData();
					pBuffer = new BYTE[nLen + 1];
					memcpy(pBuffer, pfBuffer, nLen);
					pBuffer[nLen] = 0;

					//session.SendPacket((void *)pBuffer, nLen);
					session.SendPacket((void *)pBuffer, nLen, t, mark, timestam);

					session.DeletePacket(pack);
				}
			} while (session.GotoNextSourceWithData());
		}
		session.EndDataAccess();
		session.Poll(); //查看数据是否到来
		Sleep(1);
	}
	session.Destroy();
	WSACleanup();

	return 0;
}
