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
void Dump(BYTE* pData, size_t nSize)
{
	std::string strOut;
	for (size_t i = 0; i < nSize; i++)
	{
		char buf[8] = "";
		if (i > 0 && (i % 16 == 0))strOut += "\n";
		snprintf(buf, sizeof(buf), "%02", pData[i] & 0xFF);
		strOut += buf;
	}
	strOut += "\n";
	OutputDebugStringA(strOut.c_str());
}