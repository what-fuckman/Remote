#include "pch.h"
#include "SeverSocket.h"
//CServerSocket server;

//error LNK2001: �޷��������ⲿ���� "private: static class CServerSocket * CServerSocket::m_instance"
CServerSocket* CServerSocket::m_instance = NULL; //��ʽ�ĳ�ʼ��
CServerSocket::CHelper CServerSocket::m_helper;

CServerSocket* pserver = CServe