﻿// Project.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "framework.h"
#include "RemoteCtrl.h"
#include "SeverSocket.h"
#include <direct.h> //查分区
#include <atlimage.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;
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

std::string MakeDriverInfo() //1=>A2=>B
{
    std::string result;
    for (int i = 1; i <= 26; i++)
    {
        if (_chdrive(i) == 0)
        {
            if (result.size() > 0)
                result += ',';
            result += 'A' + i - 1;

        }
    }
    CPacket pack(1, (BYTE*)result.c_str(), result.size());
    Dump((BYTE*)pack.Data(), pack.Size());
    //CServerSocket::getInstance()->Send(CPacket(1,(BYTE*) result. c_str(), result.size()));
    return 0;
}
#include<stdio.h>
#include<io.h>
#include<list>
typedef struct file_info
{
    file_info()
    {
        IsInvalid = FALSE;
        IsDirectory = -1;
        HasNext = TRUE;
        memset(szFileName, 0, sizeof(szFileName));
    }
    BOOL IsInvalid;//是否有效
    BOOL IsDirectory;//是否为目录 0是，1否
    BOOL HasNext;//是否有后续 0 没有 1 有
    char szFileName[256];//文件名

}FILEINFO, * PFILEINFO;

int MakeDirectoryInfo()
{
    std::string strPath;
    //std::list<FILEINFO> listFileInfos;
    if (CServerSocket::getInstance()->GetFilePath(strPath) == false)
    {
        OutputDebugString(_T("当前的命令，不是获取文件列表，命令解析错误！！"));
        return -1;
    }
    if (_chdir(strPath.c_str()) != 0)
    {
        FILEINFO finfo;
        finfo.IsInvalid = TRUE;
        finfo.IsDirectory = TRUE;
        finfo.HasNext = FALSE;
        memcpy(finfo.szFileName, strPath.c_str(), strPath.size());
        //   listFileInfos.push_back(finfo);
        CPacket pack(2, (BYTE*)&finfo, sizeof(finfo));
        CServerSocket::getInstance()->Send(pack);
        OutputDebugString(_T("没有权限，访问目录！！"));
        return -2;
    }
    _finddata_t fdata;
    int hfind = 0;
    if ((hfind = _findfirst("*", &fdata)) == 1)
    {
        OutputDebugString(_T("没有找到任何文件！"));
        return -3;
    }
    do
    {
        FILEINFO finfo;
        finfo.IsDirectory = (fdata.attrib & _A_SUBDIR) != 0;
        memcpy(finfo.szFileName, fdata.name, strlen(fdata.name));
        //   listFileInfos.push_back(finfo);
        CPacket pack(2, (BYTE*)&finfo, sizeof(finfo));
        CServerSocket::getInstance()->Send(pack);
    } while (!_findnext(hfind, &fdata));
    //发信息到控制端
    FILEINFO finfo;
    finfo.HasNext = FALSE;
    CPacket pack(2, (BYTE*)&finfo, sizeof(finfo));
    CServerSocket::getInstance()->Send(pack);
    return 0;
}

int RunFlie()
{
    std::string strPath;
    CServerSocket::getInstance()->GetFilePath(strPath);
    ShellExecuteA(NULL, NULL, strPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
    CPacket pack(3, NULL, 0);
    CServerSocket::getInstance()->Send(pack);
    return 0;
}
#pragma warning(disable:4996)  //open sprintf strcpy strstr 
int DownloadFile()
{
    std::string strPath;
    CServerSocket::getInstance()->GetFilePath(strPath);
    long long data = 0;
    FILE* pFile = NULL;
    errno_t err = fopen_s(&pFile, strPath.c_str(), "rb");
    if (err != 0)
    {
        CPacket pack(4, (BYTE*)&data, 8);
        CServerSocket::getInstance()->Send(pack);
        return -1;
    }
    if (pFile != NULL)
    {

        fseek(pFile, 0, SEEK_END);
        data = _ftelli64(pFile);
        CPacket head(4, (BYTE*)&data, 8);
        fseek(pFile, 0, SEEK_SET);
        char buffer[1024] = "";
        size_t rlen = 0;
        do
        {
            fread(buffer, 1, 1024, pFile);
            CPacket pack(4, (BYTE*)buffer, rlen);
            CServerSocket::getInstance()->Send(pack);
        } while (rlen >= 1024);

        fclose(pFile);
    }
    CPacket pack(4, NULL, 0);
    CServerSocket::getInstance()->Send(pack);
    return 0;
}

int MouseEvent()
{
    MOUSEEVENT mouse;
    if (CServerSocket::getInstance()->GetMouseEvent(mouse))
    {
        DWORD nFlages = 0;

        switch (mouse.nButton)
        {
        case 0://左键
            nFlages = 1;
            break;
        case 1://右键
            nFlages = 2;
            break;
        case 2://中键
            nFlages = 4;
            break;
        case 4://没有按键
            nFlages = 8;
            break;
        }
        if (nFlages != 8) SetCursorPos(mouse.ptXY.x, mouse.ptXY.y);
        switch (mouse.nAction)
        {
        case 0://单击
            nFlages |= 0x10;
            break;
        case 1://双击
            nFlages |= 0x20;
            break;
        case 2://按下
            nFlages |= 0x40;
            break;
        case 3://放开
            nFlages |= 0x80;
            break;
        default:
            break;
        }
        switch (nFlages)
        {
        case 0x21://左键双击
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
        case 0x11://左键单击
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
            break;
        case 0x41://左键按下
            mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
            break;
        case 0x81://左键放开
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
            break;
        case 0x22://右侧双击
            mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
            mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
        case 0x12://右侧单击
            mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
            mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
            break;
        case 0x42://右侧按下
            mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
            break;
        case 0x82://右侧放开
            mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
            break;
        case 0x24://中键双击
            mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, GetMessageExtraInfo());
            mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, GetMessageExtraInfo());
        case 0x14://中键单击
            mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, GetMessageExtraInfo());
            mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, GetMessageExtraInfo());
            break;
        case 0x44://中键按下
            mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, GetMessageExtraInfo());
            break;
        case 0x84://中键放开
            mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, GetMessageExtraInfo());
            break;
        case 0x08://单纯的鼠标移动
            mouse_event(MOUSEEVENTF_MOVE, mouse.ptXY.x, mouse.ptXY.y, 0, GetMessageExtraInfo());
            break;
        }
        CPacket pack(4, NULL, 0);
        CServerSocket::getInstance()->Send(pack);
    }
    else
    {
        OutputDebugString(_T("获取鼠标操作失败！！"));
        return -1;
    }
    return 0;
}

int SendScreen()
{
    CImage screen;//GDI
    HDC hScreen=::GetDC(NULL);//获取设备上下文
    int nBitPerPixel = GetDeviceCaps(hScreen, BITSPIXEL);//24 argb88832
    int nWidth = GetDeviceCaps(hScreen, HORZRES);
    int nheight = GetDeviceCaps(hScreen, VERTRES);
    screen.Create(nWidth, nheight, nBitPerPixel);
    BitBlt(screen.GetDC(), 0, 0, 2256*1.5, 1504*1.5, hScreen, 0, 0, SRCCOPY);
    ReleaseDC(NULL, hScreen);
    HGLOBAL hMem=GlobalAlloc(GMEM_MOVEABLE, 0);
    if (hMem == NULL)return -1;
    IStream* pStream = NULL;
    HRESULT ret =CreateStreamOnHGlobal(hMem, TRUE, &pStream);
    if (ret ==S_OK)
    {   
        screen.Save(pStream, Gdiplus::ImageFormatPNG);
        LARGE_INTEGER bg = { 0 };
        pStream->Seek(bg, STREAM_SEEK_SET, NULL);
        PBYTE pData =(PBYTE)GlobalLock(hMem);
        SIZE_T nSize = GlobalSize(hMem);
        CPacket pack(6, pData, nSize);
        CServerSocket::getInstance()->Send(pack);
        GlobalUnlock(hMem);
        
    }
  
    /*DWORD tick = GetTickCount();
    TRACE("png %d\r\n", GetTickCount() - tick);
    screen.Save(_T("test2023.png"), Gdiplus::ImageFormatPNG);
    DWORD tick = GetTickCount();
    screen.Save(_T("test2023.jpg"), Gdiplus::ImageFormatJPEG);
    TRACE("jpg %d\r\n", GetTickCount() - tick)*/;
    pStream->Release();
    GlobalFree(hMem);
    screen.ReleaseDC();
    
    return 0;
}
#include"LockDialog.h"
CLockDialog dlg;
unsigned threadid;
unsigned __stdcall threadLockDlg(void* arg)
{
   
    dlg.Create(IDD_DIALOG_INFO, NULL);
    dlg.ShowWindow(SW_SHOW);//非模态
    //屏蔽后台窗口
    CRect rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = GetSystemMetrics(SM_CXFULLSCREEN);
    rect.right *= 1.5;
    rect.bottom = GetSystemMetrics(SM_CYFULLSCREEN);
    rect.bottom *= 1.5;
    TRACE("right =%d bottom =%d\r\n", rect.right, rect.bottom);

    //窗口置顶
    dlg.SetWindowPos(&dlg.wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    ShowCursor(false);//限制鼠标功能
    //隐藏任务栏
    ::ShowWindow(::FindWindow(_T("Shell_TrayWind"), NULL), SW_HIDE);
    //dlg.GetWindowRect(rect);
    rect.left = 0;
    rect.top = 0;
    rect.right = 1;
    rect.bottom = 1;
    ClipCursor(rect);//限制鼠标范围

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_KEYDOWN)
        {
            TRACE("msg:%08X wparam:%08x lparam:%08X\r\n", msg.message, msg.wParam, msg.lParam);
            if (msg.wParam == 0x1B)//按下esc退出
            {
                break;
            }
        }
    }
   
    ShowCursor(true);
    ::ShowWindow(::FindWindow(_T("Shell_TrayWind"), NULL), SW_SHOW);
    dlg.DestroyWindow();
    _endthread();
    return 0;
}

int LockMachine()
{
    if ((dlg.m_hWnd == NULL) || (dlg.m_hWnd == INVALID_HANDLE_VALUE))
    {
       // _beginthread(threadLockDlg, 0, NULL);
        _beginthreadex(NULL, 0, threadLockDlg, NULL, 0, &threadid);
    }
    CPacket pack(7, NULL, 0);
    CServerSocket::getInstance()->Send(pack);
    return 0;
}
int UnLockMachine()
{
   // dlg.SendMessage(WM_KEYDOWN, 0x1B, 0x01E0001);
    //::SendMessage(dlg.m_hWnd, WM_KEYDOWN, 0x1B, 0x01E0001);
    PostThreadMessage(threadid, WM_KEYDOWN, 0x1B, 0);
    CPacket pack(7, NULL, 0);
    CServerSocket::getInstance()->Send(pack);
    return 0;
}
int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // 初始化 MFC 并在失败时显示错误
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: 在此处为应用程序的行为编写代码。
            wprintf(L"错误: MFC 初始化失败\n");
            nRetCode = 1;
        }
        else
        {

            //1.进度可控
            //2.方便对接
            //3.可行性评估，提前暴露风险
            // TODO: socket，bind，listen，accept，read，writer，close，
            // 套接字初始化
            //server;
            //CServerSocket* pserver = CServerSocket::getInstance(); //单例
            //int count = 0;

            //if (pserver->InitSocket() == false)
            //{
            //    MessageBox(NULL, _T("网络初始化失败，未能成功初始化，请检查网络状态"), _T("网络初始化失败"), MB_OK | MB_ICONERROR);
            //    exit(0);
            //}
            //while (CServerSocket::getInstance() != NULL)
            //{
            //    if (pserver->AcceptClient() == false)
            //    {
            //        if (count > 0)
            //        {
            //            MessageBox(NULL, _T("多次正常接入用户，结束程序"), _T("接入用户失败"), MB_OK | MB_ICONERROR);
            //            exit(0);
            //        }
            //        MessageBox(NULL, _T("无法正常接入用户，自动重试"), _T("接入用户失败"), MB_OK | MB_ICONERROR);
            //    }
            //}
            //int ret = pserver->Dealcommand();
            //TODO
           
            //全局静态变量
            int nCmd = 7;
            switch (nCmd)
            {
            case 1://查看磁盘分区
                MakeDriverInfo();
                break;
            case 2://查看指定目录下的文件
                MakeDirectoryInfo();
                break;
            case 3://打开文件
                RunFlie();
                break;
            case 4://下载文件
                DownloadFile();
                break;
            case 5://鼠标操作
                MouseEvent();
                break;
            case 6://发送屏幕内容=》发送屏幕截图
                SendScreen();
            case 7://锁机
                LockMachine();
                //Sleep(500);
               // LockMachine();
                break;
            case 8:
                UnLockMachine();
                break;

            }
            Sleep(5000);
            UnLockMachine();
            //while ((dlg.m_hWnd != NULL) && (dlg.m_hWnd != INVALID_HANDLE_VALUE)) Sleep(100);
            TRACE("m_hWind =%08\r\n", dlg.m_hWnd);
        }
    }
    else
    {
        // TODO: 更改错误代码以符合需要
        wprintf(L"错误: GetModuleHandle 失败\n");
        nRetCode = 1;
    }

    return nRetCode;
}
