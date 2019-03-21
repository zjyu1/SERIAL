#include "stdafx.h"
#include "serialclass.h"

static SerialPort serialport;

bool OpenDriver(TCHAR * port)
{
	HANDLE hdl;
	hdl = serialport.Open_driver_Y(port);
	if (hdl == NULL)
	{
		return false;
	}
	return true;
}

bool SendData(HANDLE fd, BYTE *data, DWORD dwDataLen)
{
	bool err;
	err = serialport.Send_driver_Y(fd, data, dwDataLen);
	if (err)
	{
		return false;
	}
	return true;
}

bool ReceiveData(HANDLE fd, BYTE *data)
{
	bool err;
	err = serialport.Receive_driver_Y(fd, data);
	if (err)
	{
		return false;
	}
	return true;
}

void ClosePort(HANDLE fd)
{
	serialport.Close_driver(fd);
}