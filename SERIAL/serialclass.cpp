// serialclass.cpp : ���� DLL Ӧ�ó���ĺ�����

#include "stdafx.h"
#include<windows.h>
#include<stdio.h>
#include<tchar.h>
#include"serialclass.h"

HANDLE SerialPort::Open_driver_Y(TCHAR *name)
{
	//�򿪴���
	HANDLE m_hCom = CreateFile(name,
		GENERIC_READ | GENERIC_WRITE, //����д
		0,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		NULL);
	if (m_hCom == INVALID_HANDLE_VALUE)
	{
		printf("Create File faile\n");
		return NULL;
	}

	int buffer_size;
	buffer_size = 1024;
	//���û�������С
	if (!SetupComm(m_hCom, buffer_size, buffer_size))
	{
		printf("SetupComm fail!\n");
		CloseHandle(m_hCom);
		return NULL;
	}

	//���ó�ʱ
	COMMTIMEOUTS TimeOuts;
	memset(&TimeOuts, 0, sizeof(TimeOuts));
	TimeOuts.ReadIntervalTimeout = 100;
	TimeOuts.ReadTotalTimeoutConstant = 1000;
	TimeOuts.ReadTotalTimeoutMultiplier = 100;
	TimeOuts.WriteTotalTimeoutConstant = 2000;
	TimeOuts.WriteTotalTimeoutMultiplier = 50;
	SetCommTimeouts(m_hCom, &TimeOuts);

	PurgeComm(m_hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);//��ջ�����

													 //���ô��ڲ���
	DCB dcb = { 0 };

	if (!GetCommState(m_hCom, &dcb))
	{
		printf("GetCommState fail\n");
		return NULL;
	}

	dcb.DCBlength = sizeof(dcb);
	if (!BuildCommDCB(TEXT("9600,n,8,1"), &dcb))//���ģãµ����ݴ����ʡ���żУ�����͡�����λ��ֹͣλ
	{
		printf("BuileCOmmDCB fail\n");
		CloseHandle(m_hCom);
		return NULL;
	}

	if (SetCommState(m_hCom, &dcb))
	{
		printf("SetCommState OK!\n");
	}
	//��������ʼ���첽��ʽ
	ZeroMemory(&wrOverlapped, sizeof(wrOverlapped));

	if (wrOverlapped.hEvent != NULL)
	{
		ResetEvent(wrOverlapped.hEvent);
		wrOverlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	}
	return m_hCom;
}

bool SerialPort::Send_driver_Y(HANDLE fd, BYTE *data, DWORD dwDataLen)
{
	DWORD dwError;
	DWORD dwExpectSend = dwDataLen;
	DWORD dwRealSend = 0;
	BYTE *pSendBuffer;

	pSendBuffer = data;

	if (ClearCommError(fd, &dwError, NULL))
	{
		PurgeComm(fd, PURGE_TXABORT | PURGE_TXCLEAR);
	}

	if (!WriteFile(fd, pSendBuffer, dwExpectSend, &dwRealSend, &wrOverlapped))
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			while (!GetOverlappedResult(fd, &wrOverlapped, &dwRealSend, FALSE))
				//�ɹ����ط�0��ʧ�ܷ���0
			{
				if (GetLastError() == ERROR_IO_INCOMPLETE)
				{
					continue;
				}
				else
				{
					printf("send Fail!\n");
					ClearCommError(fd, &dwError, NULL);
					return true;
				}
			}
		}
	}
	return false;
}

bool SerialPort::Receive_driver_Y(HANDLE fd, BYTE *data)
{
	DWORD dwError;
	DWORD dwWantRead = 20;
	DWORD dwRealRead = 0;
	BYTE* pReadBuf;
	unsigned int i;
	///	BYTE crc;
	pReadBuf = data;
	if (ClearCommError(fd, &dwError, NULL))
	{
		PurgeComm(fd, PURGE_TXABORT | PURGE_TXCLEAR);
	}
	if (!ReadFile(fd, pReadBuf, dwWantRead, &dwRealRead, &wrOverlapped))
	{
		dwError = GetLastError();
		if (dwError == ERROR_IO_PENDING)
		{
			while (!GetOverlappedResult(fd, &wrOverlapped, &dwRealRead, TRUE))
				//�ɹ����ط�0, ʧ�ܷ���0
			{
				return true;
			}
		}
	}
	if (dwRealRead>0) {
		printf("recv_len = %d\n", dwRealRead);
	}
	printf("��������\n");

	for (i = 0; i < dwRealRead; i++)
	{
		printf("%.2x ", data[i]);
	}
	printf("\r\n");
	return false;
}

void SerialPort::Close_driver(void *fd)
{
	CloseHandle(fd);
}
