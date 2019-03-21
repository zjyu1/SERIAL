#pragma once
#include<tchar.h>

class SerialPort
{
private:
	OVERLAPPED wrOverlapped;
public:
	HANDLE Open_driver_Y(TCHAR *name);

	bool Send_driver_Y(HANDLE fd, BYTE *data, DWORD dwDataLen);

	bool Receive_driver_Y(HANDLE fd, BYTE *data);

	void Close_driver(void *fd);

};