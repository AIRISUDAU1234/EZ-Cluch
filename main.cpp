#include <functional>
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <future>
#include <curl/curl.h>
#include <cstdlib>

#include "Global.h"
#include "ToggleKey.h"
#include "Driver.h"
#include "Mouse.h"

#include "Install_Driver.h"

#pragma comment(lib, "urlmon.lib")

using namespace std;
namespace fs = std::filesystem;

GhubMouse* gHub = nullptr;
Mouse* globalMouseThread = nullptr;

HANDLE hStdout;
DWORD ret;

wchar_t strWcar1[256];
wchar_t strWcar2[256];
wchar_t strWcar3[256];
wchar_t strWcar4[256];
wchar_t strWcar5[256];
wchar_t strWcar6[256];
wchar_t strWcar7[256];
wchar_t strWcar8[256];
wchar_t strWcar9[256];
wchar_t strWcar10[256];

void Call_Ghub();
void DriverC();
void DLL_download();
void DLL_Delete();
void run_menu();
void EZ_Title();
void DisplayStatus();
void prg_call();
void prg_call2();
void Setting_display();
void GHUB_version();

void TriggerBot();
void Call_AimBOT();
extern void SetConsoleColor(int textColor);
extern void clearConsole();

size_t DriverDLL_WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::ofstream* outputdll_file = static_cast<std::ofstream*>(userp);
    size_t total_size = size * nmemb;
    outputdll_file->write(static_cast<char*>(contents), total_size);
    return total_size;
}

int load()
{
    clearConsole();
    Sleep(600);

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    swprintf_s(strWcar1, _countof(strWcar1), L"                                                               \n");
    swprintf_s(strWcar2, _countof(strWcar2), L"▓█████ ▒███████▒    ▄████▄   ██▓     █    ██  ▄████▄   ██░ ██ \n");
    swprintf_s(strWcar3, _countof(strWcar3), L"▓█   ▀ ▒ ▒ ▒ ▄▀░   ▒██▀ ▀█  ▓██▒     ██  ▓██▒▒██▀ ▀█  ▓██░ ██▒\n");
    swprintf_s(strWcar4, _countof(strWcar4), L"▒███   ░ ▒ ▄▀▒░    ▒▓█    ▄ ▒██░    ▓██  ▒██░▒▓█    ▄ ▒██▀▀██░\n");
    swprintf_s(strWcar5, _countof(strWcar5), L"▒▓█  ▄   ▄▀▒   ░   ▒▓▓▄ ▄██▒▒██░    ▓▓█  ░██░▒▓▓▄ ▄██▒░▓█ ░██ \n");
    swprintf_s(strWcar6, _countof(strWcar6), L"░▒████▒▒███████▒   ▒ ▓███▀ ░░██████▒▒▒█████▓ ▒ ▓███▀ ░░▓█▒░██▓\n");
    swprintf_s(strWcar7, _countof(strWcar7), L"░░ ▒░ ░░▒▒ ▓░▒░▒   ░ ░▒ ▒  ░░ ▒░▓  ░░▒▓▒ ▒ ▒ ░ ░▒ ▒  ░ ▒ ░░▒░▒\n");
    swprintf_s(strWcar8, _countof(strWcar8), L"  ░   ░ ░ ░ ░ ░   ░          ░ ░    ░░░ ░ ░ ░         ░  ░░ ░\n");
    swprintf_s(strWcar9, _countof(strWcar9), L"   ░  ░  ░ ░       ░ ░          ░  ░   ░     ░ ░       ░  ░  ░\n");
    swprintf_s(strWcar10, _countof(strWcar10), L"                                                               created by あいりす\n");

    SetConsoleColor(11);
    cout << endl;
    Sleep(1000);
    DLL_download();

    cout << "configを取得しています..." << endl;
    std::string name, value;
    if (!ReadConfig()) {
        SetConsoleColor(4);
        clearConsole();
        Sleep(500);
        cout << "configの読み取りに失敗しました" << endl;
        Sleep(600);
        char response;
        clearConsole();
        std::cout << "設定ファイルが損傷、または存在しない可能性があります。新しい設定ファイルを作成しますか？ (y/n): ";
        std::cin >> response;

        if (response == 'y' || response == 'Y') {
            CreateDefaultConfig();
            SetConsoleColor(10);
            clearConsole();
            cout << "configを作成しました。再起動してください。" << endl;
            Sleep(1200);
            return 1;
        }
        else {
            SetConsoleColor(4);
            clearConsole();
            cout << "config作成ができません。" << endl;
            return 0;
        }
    }
    else {
        SetConsoleColor(10);
        clearConsole();
        cout << "Loaded Config!" << endl;
        Sleep(1000);
        //thread gc(Call_Ghub);
    }
    clearConsole();
    SetConsoleColor(7);
    Setting_display();
    Sleep(1000);
    clearConsole();
    Sleep(10);
    DriverC(); //ドライバー確認処理
    Beep(100, 10);
    thread t1(prg_call);
    t1.detach();
    Sleep(1);
    thread t2(prg_call2);
    t2.detach();
    Sleep(1200);
    run_menu();
    Sleep(1);
    return 0;
}

void DriverC()
{
    GHUB_version();
    Sleep(600);
    {
        if (gHub)
        {
            gHub->mouse_close();
            delete gHub;
            gHub = nullptr;
        }
    }
    SetConsoleColor(10);
    std::cout << "[+] Ghub Driver Loaded." << std::endl;
    Sleep(3000);
    Beep(1000,100);
    gHub = new GhubMouse();
    if (!gHub->mouse_xy(0, 0))
    {
        SetConsoleColor(4);
        std::cerr << "[!] Error with opening mouse." << std::endl;
        Sleep(3000);
        Beep(1000, 100);
        delete gHub;
        gHub = nullptr;
    }
} 

void DLL_download()
{
    if (atexit(DLL_Delete) != 0) {
        SetConsoleColor(4);
        std::cerr << "atexitの登録に失敗しました。\n";
        return;
    }
    CURL* curl;
    CURLcode res;

    const char* url = "https://www.dropbox.com/scl/fi/akffssomoaap07zjh2z2b/ghub_mouse.dll?rlkey=ndej6qqogaqydbi7szbuhrwxb&st=7lefylij&dl=1";

    try {
        // カレントディレクトリを取得
        std::string ghubDLL_path = fs::current_path().string();
        std::string GHUBDLL = ghubDLL_path + "/ghub_mouse.dll"; 

        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();

        if (curl) {
            std::ofstream output_file(GHUBDLL, std::ios::binary);

            if (!output_file.is_open()) {
                std::cerr << "Error: DriverDLLを書き込めませんでした.: " << GHUBDLL << std::endl;
                curl_easy_cleanup(curl);
                curl_global_cleanup();
            }

            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, DriverDLL_WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output_file);

            res = curl_easy_perform(curl);

            if (res != CURLE_OK) {
                std::cerr << "Error: curl_easy_perform() が失敗しました: " << curl_easy_strerror(res) << std::endl;
            }
            else {
                std::cout << "DriverDLLインストール完了" << endl;
            }
            output_file.close();
            curl_easy_cleanup(curl);
        }
        else {
            std::cerr << "Error: CURL を初期化できませんでした" << std::endl;
        }
        curl_global_cleanup();
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
}

void DLL_Delete()
{
    std::string current_path3 = fs::current_path().string();
    std::string deletefile1 = current_path3 + "/ghub_mouse.dll";

    if (fs::exists(deletefile1))
    {
        if (fs::remove(deletefile1)) {}
        else {
            std::cerr << "DLLの削除に失敗しました: " << deletefile1 << std::endl;
        }
    }
    else {
        std::cerr << "ファイルが存在しません: " << deletefile1 << std::endl;
    }
}

void EZ_Title()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hStdout, &csbi);
    int consoleWidth = csbi.dwSize.X;

    SetConsoleColor(11);
    WriteConsoleW(hStdout, strWcar1, wcslen(strWcar1), &ret, NULL);
    WriteConsoleW(hStdout, strWcar2, wcslen(strWcar2), &ret, NULL);
    WriteConsoleW(hStdout, strWcar3, wcslen(strWcar3), &ret, NULL);
    SetConsoleColor(3);
    WriteConsoleW(hStdout, strWcar4, wcslen(strWcar4), &ret, NULL);
    WriteConsoleW(hStdout, strWcar5, wcslen(strWcar5), &ret, NULL);
    WriteConsoleW(hStdout, strWcar6, wcslen(strWcar6), &ret, NULL);
    SetConsoleColor(9);
    WriteConsoleW(hStdout, strWcar7, wcslen(strWcar7), &ret, NULL);
    WriteConsoleW(hStdout, strWcar8, wcslen(strWcar8), &ret, NULL);
    SetConsoleColor(1);
    WriteConsoleW(hStdout, strWcar9, wcslen(strWcar9), &ret, NULL);

    SetConsoleColor(15);
    WriteConsoleW(hStdout, L"                                               created by ", wcslen(L"                                               created by "), &ret, NULL);
    SetConsoleColor(13);
    WriteConsoleW(hStdout, L"あい", wcslen(L"あい"), &ret, NULL);
    SetConsoleColor(9);
    WriteConsoleW(hStdout, L"りす", wcslen(L"りす"), &ret, NULL);
    WriteConsoleW(hStdout, L"\n", 1, &ret, NULL);
}

void DisplayStatus() {
    clearConsole();
    EZ_Title();
    SetConsoleColor(4);
    std::cout << "=== モード選択 ===" << std::endl;
    SetConsoleColor(13);
    std::cout << "[F8]Radius: " << (Radius ? "ON" : "OFF") << std::endl;
    SetConsoleColor(10);
    std::cout << "[F3]Triggerbot: " << (Triggerbot ? "ON" : "OFF") << std::endl;
    std::cout << "[F7]AimAssist: " << (AimAssist ? "ON" : "OFF") << std::endl;
    SetConsoleColor(9);
    if (Triggerbot)
    {
        std::cout << "[F4]Sniper Mode: " << (outlawmode ? "Outlaw" :
            marshallmode ? "Marshall" :
            operatormode ? "Operator" : "None") << std::endl;
        std::cout << "[F5]Vandal Mode: " << (vandalmode1 ? "Mode1" :
            vandalmode2 ? "Mode2" : "None") << std::endl;
        std::cout << "[F6]Sidearm Mode: " << (pistolmode ? "Pistol" :
            sheriffmode ? "Sheriff" : "None") << std::endl;
    }
    if (AimAssist)
    {
        std::cout << "[F9]AimMode: " << (head ? "Head" :
            body ? "Body" : "None") << std::endl;
    }
    std::cout << "[F10]Enemy Color: " << (RED ? "RED" :
        PURPLE ? "PURPLE" :
        YELLOW ? "YELLOW" : "None") << std::endl;
    SetConsoleColor(15);
    std::cout << "[F12]終了" << std::endl;
}

void run_menu()
{
    bool run = true;

    std::map<int, std::function<void()>> toggleFunctions = {
       {VK_F3, ToggleTriggerbot},
       {VK_F4, ToggleSniper},
       {VK_F5, ToggleVandal},
       {VK_F6, ToggleSidearm},
       {VK_F7, ToggleAimAssist},
       {VK_F8, ToggleRadius},
       {VK_F9, ToggleHeBd},
       {VK_F10, ToggleColor},
       {VK_F12, EXIT}
    };

    DisplayStatus();
    Sleep(10);

    while (run)
    {
        Sleep(10);

        for (auto it = toggleFunctions.begin(); it != toggleFunctions.end(); ++it) {
            int key = it->first;
            auto toggleFunc = it->second;

            if (GetAsyncKeyState(key) & 1) {
                toggleFunc();
                DisplayStatus();
                Sleep(1);
            }
        }
    }
}

void prg_call()
{
    TriggerBot();
}

void prg_call2()
{
    Call_AimBOT();
}

void Setting_display()
{
    std::cout << "       設定\n";
    std::cout << "MouseSensitivity = " << MouseSensitivity << std::endl;
    std::cout << "\n";
    std::cout << "AimSpeed = " << AimSpeed << std::endl;
    std::cout << "\n";
    std::cout << "shadeVariation = " << shadeVariation << std::endl;
    std::cout << "\n";
    Sleep(2000);
}

std::string get_ghub_version()
{
    std::string line;
    std::ifstream in("C:\\Program Files\\LGHUB\\version");
    if (in.is_open())
    {
        while (std::getline(in, line)) {}
    }
    in.close();

    if (line.data())
    {
        return line;
    }
}

void GHUB_version()
{
    std::string ghub_version = get_ghub_version();

    if (ghub_version == "13.1.4")
    {
        std::string ghub_version_label = "正常なバージョンがインストールされています: " + ghub_version;
    }
    else
    {
        Sleep(600);
        SetConsoleColor(4);
        std::cerr << "[!] ドライバーがインストールされていません。インストールしてから再度実行してください。 " << std::endl;
        //std::future<void> result = std::async(std::launch::async, Get_Driver);
        //result.get();
        Sleep(1000);
        clearConsole();
        if (ghub_version == "")
        {
            ghub_version = "unknown";
        }
        std::string ghub_version_label = "[!]現在のGHUBのバージョン: " + ghub_version;
        SetConsoleColor(4);
        std::cerr << "[!] 間違ったバージョンの Ghub がインストールされているか、Ghub へのパスがデフォルトで設定されていません。Default system path: C:\\Program Files\\LGHUB" << std::endl;
    }
    Sleep(2000);
    SetConsoleColor(4);
    std::cerr << "[!] この方法は検出される可能性があります。自己責任で使用してください." << std::endl;
    Sleep(2000);
    clearConsole();
}

void Call_Ghub()
{
    LPCWSTR appPath = L"C:\\Program Files\\LGHUB\\system_tray\\lghub_system_tray.exe";

    if (!std::filesystem::exists(appPath)) {
        std::cerr << "Warning: GHUBが見つからない、またはインストールされていません。" << std::endl;
        return;
    }

    HINSTANCE result = ShellExecute(NULL, L"open", appPath, NULL, NULL, SW_MINIMIZE);

    if ((int)result <= 32) {}
}