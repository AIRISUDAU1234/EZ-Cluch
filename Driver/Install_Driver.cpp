#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <filesystem> 

#include "Global.h"
using namespace std;
namespace fs = std::filesystem; 

void SetConsoleColor(int textColor);
void clearConsole();
void DisplayWarning();
void silent_install();
void time_call();
void time_hostedit(int minutes, int seconds);
void Host_Edit();
void Installer_Delete();

// ホストファイル管理クラス
class HostFileManager {
public:
    static std::wstring GetHostFilePath() {
        return L"C:\\Windows\\System32\\drivers\\etc\\hosts";
    }

    static bool AddEntries(const std::wstring& entries) {
        std::wstring hostFilePath = GetHostFilePath();
        std::wofstream outFile(hostFilePath, std::ios::app);
        if (!outFile.is_open()) {
            std::wcout << "ホストファイルを開けませんでした";
            return false;
        }
        outFile << entries;
        outFile.close();
        return true;
    }
};

size_t Driver_WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::ofstream* output_file = static_cast<std::ofstream*>(userp);
    size_t total_size = size * nmemb;
    output_file->write(static_cast<char*>(contents), total_size);
    return total_size;
}

void Get_Driver() 
{
    if (atexit(Installer_Delete) != 0) {
        SetConsoleColor(4);
        std::cerr << "atexitの登録に失敗しました。\n";
        return;
    }
    CURL* curl;
    CURLcode res;

    const char* url = "https://www.dropbox.com/scl/fi/9ylnzh649g1xh1b74cce6/lghub_installer.exe?rlkey=ttzr01up3prlg58jvgjehulnr&st=xa2gl8gl&dl=1";

    try {
        // カレントディレクトリを取得
        std::string current_path = fs::current_path().string();
        std::string output_filename = current_path + "/lghub_installer.exe"; // 保存先ファイル名

        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();

        if (curl) {
            std::ofstream output_file(output_filename, std::ios::binary);

            if (!output_file.is_open()) {
                std::cerr << "Error: Driver設定用ファイルを書き込めませんでした.: " << output_filename << std::endl;
                curl_easy_cleanup(curl);
                curl_global_cleanup();
            }

            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Driver_WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output_file);

            res = curl_easy_perform(curl);

            if (res != CURLE_OK) {
                std::cerr << "Error: curl_easy_perform() が失敗しました: " << curl_easy_strerror(res) << std::endl;
            }
            else {
                std::cout << "Setupファイルが正常にダウンロードされました。インストールを開始します。" << output_filename << std::endl;
                Sleep(2000);
                silent_install();
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

void silent_install() {
    try {
        std::string current_fpath = fs::current_path().string();
        std::string exe_path = current_fpath + "/lghub_installer.exe";

        if (!fs::exists(exe_path)) {
            std::cerr << "Error: Driver Setupファイルが見つかりません。: " << exe_path << std::endl;
            return;
        }

        std::string arg1 = "--silent"; // 引数
        std::string command = "\"" + exe_path + "\" " + arg1;

        int result = system(command.c_str());

        // 実行結果を確認
        if (result == 0) {
            std::cout << "セットアップを実行中です。しばらくお待ちください。" << std::endl;
            Sleep(900);
            thread t1(time_call);
        }
        else {
            std::cerr << "Error: コードによりプログラムの実行が失敗しました: " << result << std::endl;
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void time_call()
{
    time_hostedit(1, 30); //1分30秒
}

void time_hostedit(int minutes, int seconds)
{
    int total_seconds = minutes * 60 + seconds;

    std::cout << "Driverのセットアップが終了するまでお待ちください。(推定1分30秒)" << std::endl;

    auto start_time = std::chrono::steady_clock::now();

    while (true) {
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();

        if (elapsed_time >= total_seconds) {
            break; 
        }
        if (elapsed_time % 10 == 0) { 
            std::cout << "経過時間 約: " << elapsed_time << " 秒" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
    }
    Host_Edit();
}

void Host_Edit()
{
    std::wstring hostEntries = L"\n127.0.0.1 updates.ghub.logitechg.com\n127.0.0.1 util.logitech.io\n";
    if (!HostFileManager::AddEntries(hostEntries)) {
        std::wcout << "ホストファイルの編集に失敗しました。";
    }
    DisplayWarning();
}

void DisplayWarning() {
    SetConsoleColor(4);
    std::wcout << L"[!] 警告: ドライバーはこのバージョンのみ動作します。GHUBアプリケーションを更新しないでください。\n";
    Sleep(2000);
    std::wcout << L"[!] 使用中はGHUBアプリケーションを終了しないでください。\n";
    Sleep(2000);
    std::wcout << L"[!] ドライバーの設定が完了したらアプリケーションを再起動してください。\n";
    Sleep(2000);
    clearConsole();
}

void Installer_Delete()
{
    std::string current_path2 = fs::current_path().string();
    std::string deletefile = current_path2 + "/lghub_installer.exe";

    if (fs::exists(deletefile))
    {
        if (fs::remove(deletefile)) {}
        else {
            std::cerr << "インストーラーの削除に失敗しました: " << deletefile << std::endl;
        }
    }
    else {
        std::cerr << "ファイルが存在しません: " << deletefile << std::endl;
    }
}