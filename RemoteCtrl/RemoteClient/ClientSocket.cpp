#include "pch.h"
#include "ClientSocket.h"

//CServerSocket server;

//error LNK2001: 无法解析的外部符号 "private: static class CServerSocket * CServerSocket::m_instance"
CClientSocket* CClientSocket::m_instance = NULL; //显式的初始化
CClientSocket::CHelper CClientSocket::m_helper;

CClientSocket* pserver = CClientSocket::getInstance();
std::string GetErrInfo(int wsaErrCode)
{
	std::string ret;
	LPVOID ipMsgBuf = NULL;
	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL,
		wsaErrCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&ipMsgBuf, 0, NULL);
	ret = (char*)ipMsgBuf;
	LocalFree(ipMsgBuf);
	return ret;
}