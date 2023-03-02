#include "pch.h"
#include "SeverSocket.h"
//CServerSocket server;

//error LNK2001: 无法解析的外部符号 "private: static class CServerSocket * CServerSocket::m_instance"
CServerSocket* CServerSocket::m_instance = NULL; //显式的初始化
CServerSocket::CHelper CServerSocket::m_helper;

CServerSocket* pserver = CServe