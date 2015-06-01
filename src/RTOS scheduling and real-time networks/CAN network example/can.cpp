/*
 * can.cpp
 *
 *  Created on: May 9, 2015
 *      Author: Avees Lab.
 */
#include "mcp_can.h"
#include "can.h"

extern MCP_CAN CAN;

unsigned long id;


/*******************************************************************************
CAN_initialize()

Baudrate :

 CAN_100KBPS, CAN_125KBPS, CAN_200KBPS, CAN_250KBPS, CAN_500KBPS, CAN_1000KBPS

********************************************************************************/

void CAN_initialize(byte Baudrate)
{
	CAN.begin(Baudrate);
}


/*******************************************************************************
CAN_write()

ID : CC = 0x50F // LKAS = 0x50E


********************************************************************************/

void CAN_write(unsigned long id, byte len, unsigned char *buf)
{
	CAN.sendMsgBuf(id, 0, 8, buf);
}


/*******************************************************************************
CAN_read()

Read the message of buffer.

ID for LKAS : 0x50C // for CC : 0x50B, 0x50D

********************************************************************************/

void CAN_read(unsigned char *len, unsigned char *buf)
{

	CAN.readMsgBuf(len, buf);
	id = CAN.getCanId();
}

