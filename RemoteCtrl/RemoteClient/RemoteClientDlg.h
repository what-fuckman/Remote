﻿
// RemoteClientDlg.h: 头文件
//

#pragma once

// CRemoteClientDlg 对话框
#include"ClientSocket.h"
#include "StatusDlg.h"

#define WM_SEND_PACKET (WM_USER+1)  //发送数据包消息

class CRemoteClientDlg : public CDialogEx
{
// 构造
public:
	
	CRemoteClientDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REMOTECLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	bool isFull()const
	{
		return m_isFull;
	}
	CImage GetImage()
	{
		return m_image;
	}
private:
	CImage m_image;//缓存
	bool m_isFull;//缓存是否有数据 
private:
	static void threadEntryForWatchData( void* arg);//静态函数不能用this指针
	void threadWatchData();//成员函数可以用this指针
	static void threadEntryForMownFile(void *arg);
	void threadDownFile();
	void LoadFileCurrent();
	void LoadFileInfo();
	CString GetPath(HTREEITEM hTree);
	void DeleteTreeChildrenItem(HTREEITEM hTree);
	//1.查看磁盘分区。。。。。。
	int SendCommandPacked(int nCmd,bool bAutoClose=true,BYTE*pData=NULL,size_t nLength=0);
// 实现
protected:
	HICON m_hIcon;
	CStatusDlg m_dlgStatus;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTest();
	DWORD m_server_address;
	CString m_nPort;
	afx_msg void OnBnClickedBtnFileinfo();
	CTreeCtrl m_Tree;
	afx_msg void OnDblclkTreeDir(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickTreeDir(NMHDR* pNMHDR, LRESULT* pResult);
	// 显示文件
	CListCtrl m_list;
	afx_msg void OnRclickListFile(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteFile();
	afx_msg void OnRunFile();
	afx_msg void OnDownloadFile();
	afx_msg LRESULT  OnSendPacket(WPARAM wParam,LPARAM lParam);

	afx_msg void OnBnClickedBtnStarteWatch();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
