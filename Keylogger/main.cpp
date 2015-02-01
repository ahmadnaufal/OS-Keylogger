#include <QcoreApplication>
#include <QDebug>
#include <QTime>
#include <QChar>
#include <iostream>
#include <Windows.h>
#pragma comment(lib, "user32.lib")

HHOOK hHook = NULL;

using namespace std;

LRESULT CALLBACK MyLowLevelKeyBoardProc (int nCode, WPARAM wParam, LPARAM lParam){

    qDebug() << "Key Pressed";
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

wchar_t *convertCharArrayToLPCWSTR(const char* charArray)
{
    wchar_t* wString=new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
    return wString;
}

int main (int argc, char *argv[]){

    QCoreApplication a(argc, argv);

    SYSTEMTIME time;

    GetSystemTime(&time);

    char fileName[256];
    sprintf(fileName, "%04d%02d%02d-%02d%02d%02d",time.wYear,time.wMonth,time.wDay,time.wHour, time.wMinute,time.wSecond);

    cout << "File Name : " << fileName << endl;

    LPCWSTR fn;
    fn= convertCharArrayToLPCWSTR(fileName);

    HANDLE hf= CreateFile(fn, GENERIC_READ | GENERIC_WRITE, (DWORD) 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);

    if (hf == INVALID_HANDLE_VALUE){

        qDebug() << "File failed to create.";
    }else{

        qDebug() << "File created.";
    }

        hHook = SetWindowsHookEx(WH_KEYBOARD_LL, MyLowLevelKeyBoardProc, NULL, 0);
        if(hHook == NULL){
            qDebug() << "Hook Failed";
        }
    return a.exec();
}
