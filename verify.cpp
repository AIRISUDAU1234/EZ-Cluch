#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#include <iostream>
#include <string>
#include <thread>
#include <fstream>
#include <curl/curl.h>
#include <json/json.h>
#include <windows.h>
#include <stdio.h>
#include "resource.h"
#include "Global.h"

#define MAX_LOADSTRING 100
using namespace std;

bool ExtractAndLoadEmbeddedDLL(int resourceId, const std::string& tempFileName) {
    HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(resourceId), RT_RCDATA);
    if (!hResource) {
        SetConsoleColor(12);
        // std::cerr << "Resource not found."<< std::endl;
        return false;
    }
    HGLOBAL hResourceData = LoadResource(NULL, hResource);
    if (!hResourceData) {
        SetConsoleColor(12);
        // std::cerr << "Failed to load resource data."<< std::endl;
        return false;
    }
    DWORD resourceSize = SizeofResource(NULL, hResource);
    void* pResourceData = LockResource(hResourceData);

    if (!pResourceData || resourceSize == 0) {
        SetConsoleColor(12);
        // std::cerr << "Invalid resource data."<< std::endl;
        return false;
    }
    std::ofstream tempFile(tempFileName, std::ios::binary);
    if (!tempFile) {
        SetConsoleColor(12);
        // std::cerr << "Failed to create temporary file."<< std::endl;
        return false;
    }
    tempFile.write(reinterpret_cast<const char*>(pResourceData), resourceSize);
    tempFile.close();

    HMODULE hModule = LoadLibraryA(tempFileName.c_str()); // DLLをロード
    if (!hModule) {
        SetConsoleColor(12);
        std::cerr << "Failed to load main_dll file" << std::endl;
        return false;
    }
    return true;
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::string* str = static_cast<std::string*>(userp);
    size_t totalSize = size * nmemb;
    str->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

bool verifySerial(const std::string& serial) {
    CURL* curl;
    CURLcode res;
    std::string responseString;

    Json::Value jsonData;
    jsonData["serial"] = serial;
    Json::StreamWriterBuilder writer;
    std::string jsonString = Json::writeString(writer, jsonData);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://2258a9cc-ebf8-4680-9166-1f725334a4d3-00-3scu3gralrf0d.kirk.replit.dev/verify"); //認証サーバーURL
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonString.c_str());

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return false;
        }
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    Json::CharReaderBuilder reader;
    Json::Value responseJson;
    std::string errs;

    std::istringstream s(responseString);
    if (Json::parseFromStream(reader, s, &responseJson, &errs)) {

        if (responseJson["status"].asString() == "valid") {
            return true;
        }
    }
    else {
        std::cerr << "JSON parse error: " << errs << std::endl;
    }
    return false;
}

extern void SetConsoleColor(int textColor);

int main(void)
{
    SetConsoleTitle(L"EZ Cluch");
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.hInstance = GetModuleHandle(NULL);
    wcex.hIcon = (HICON)LoadImage(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), 0);

    if (ExtractAndLoadEmbeddedDLL(IDR_DLL1, "js.dll")) {}
    if (ExtractAndLoadEmbeddedDLL(IDR_DLL2, "lib.dll")) {}
    if (ExtractAndLoadEmbeddedDLL(IDR_DLL3, "zbd.dll")) {}

    HANDLE hStdout;
    DWORD ret;
    wchar_t strWcar[2048];

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    swprintf_s(strWcar, _countof(strWcar), L"                                                               \n"
        L"▓█████ ▒███████▒    ▄████▄   ██▓     █    ██  ▄████▄   ██░ ██ \n"
        L"▓█   ▀ ▒ ▒ ▒ ▄▀░   ▒██▀ ▀█  ▓██▒     ██  ▓██▒▒██▀ ▀█  ▓██░ ██▒\n"
        L"▒███   ░ ▒ ▄▀▒░    ▒▓█    ▄ ▒██░    ▓██  ▒██░▒▓█    ▄ ▒██▀▀██░\n"
        L"▒▓█  ▄   ▄▀▒   ░   ▒▓▓▄ ▄██▒▒██░    ▓▓█  ░██░▒▓▓▄ ▄██▒░▓█ ░██ \n"
        L"░▒████▒▒███████▒   ▒ ▓███▀ ░░██████▒▒▒█████▓ ▒ ▓███▀ ░░▓█▒░██▓\n");
    SetConsoleColor(5);
    for (int i = 0; i < wcslen(strWcar); i++) {
        WriteConsoleW(hStdout, &strWcar[i], 1, &ret, NULL);
        Sleep(1);
    }
    Sleep(1200);

    int result_system = load(); //仮

    cout << endl;

    SetConsoleColor(7);

    std::string inputSerial;
    bool isAuthenticated = false;
    Sleep(1000);

    while (!isAuthenticated) {
        std::cout << "\n";
        std::cout << "シリアルを入力してください: ";
        std::getline(std::cin, inputSerial);

        std::cout << "認証中..." << std::endl;
        Sleep(1700);

        if (verifySerial(inputSerial)) // 認証処理
        {
            SetConsoleColor(10);
            std::cout << "[!] 認証完了" << std::endl;
            SetConsoleColor(7);
            DeleteFileA("js.dll");
            DeleteFileA("lib.dll");
            DeleteFileA("zbd.dll");
            isAuthenticated = true;
            Sleep(1000);

            // int result_system = load();
        }
        else
        {
            SetConsoleColor(12);
            std::cout << "[!] 認証失敗" << std::endl;
            SetConsoleColor(7);
            Sleep(1800);
        }
        return 0;
    }
};