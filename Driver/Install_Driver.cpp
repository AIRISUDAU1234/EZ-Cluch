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

// �z�X�g�t�@�C���Ǘ��N���X
class HostFileManager {
public:
    static std::wstring GetHostFilePath() {
        return L"C:\\Windows\\System32\\drivers\\etc\\hosts";
    }

    static bool AddEntries(const std::wstring& entries) {
        std::wstring hostFilePath = GetHostFilePath();
        std::wofstream outFile(hostFilePath, std::ios::app);
        if (!outFile.is_open()) {
            std::wcout << "�z�X�g�t�@�C�����J���܂���ł���";
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
        std::cerr << "atexit�̓o�^�Ɏ��s���܂����B\n";
        return;
    }
    CURL* curl;
    CURLcode res;

    const char* url = "https://www.dropbox.com/scl/fi/9ylnzh649g1xh1b74cce6/lghub_installer.exe?rlkey=ttzr01up3prlg58jvgjehulnr&st=xa2gl8gl&dl=1";

    try {
        // �J�����g�f�B���N�g�����擾
        std::string current_path = fs::current_path().string();
        std::string output_filename = current_path + "/lghub_installer.exe"; // �ۑ���t�@�C����

        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();

        if (curl) {
            std::ofstream output_file(output_filename, std::ios::binary);

            if (!output_file.is_open()) {
                std::cerr << "Error: Driver�ݒ�p�t�@�C�����������߂܂���ł���.: " << output_filename << std::endl;
                curl_easy_cleanup(curl);
                curl_global_cleanup();
            }

            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Driver_WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output_file);

            res = curl_easy_perform(curl);

            if (res != CURLE_OK) {
                std::cerr << "Error: curl_easy_perform() �����s���܂���: " << curl_easy_strerror(res) << std::endl;
            }
            else {
                std::cout << "Setup�t�@�C��������Ƀ_�E�����[�h����܂����B�C���X�g�[�����J�n���܂��B" << output_filename << std::endl;
                Sleep(2000);
                silent_install();
            }
            output_file.close();
            curl_easy_cleanup(curl);
        }
        else {
            std::cerr << "Error: CURL ���������ł��܂���ł���" << std::endl;
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
            std::cerr << "Error: Driver Setup�t�@�C����������܂���B: " << exe_path << std::endl;
            return;
        }

        std::string arg1 = "--silent"; // ����
        std::string command = "\"" + exe_path + "\" " + arg1;

        int result = system(command.c_str());

        // ���s���ʂ��m�F
        if (result == 0) {
            std::cout << "�Z�b�g�A�b�v�����s���ł��B���΂炭���҂����������B" << std::endl;
            Sleep(900);
            thread t1(time_call);
        }
        else {
            std::cerr << "Error: �R�[�h�ɂ��v���O�����̎��s�����s���܂���: " << result << std::endl;
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
    time_hostedit(1, 30); //1��30�b
}

void time_hostedit(int minutes, int seconds)
{
    int total_seconds = minutes * 60 + seconds;

    std::cout << "Driver�̃Z�b�g�A�b�v���I������܂ł��҂����������B(����1��30�b)" << std::endl;

    auto start_time = std::chrono::steady_clock::now();

    while (true) {
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();

        if (elapsed_time >= total_seconds) {
            break; 
        }
        if (elapsed_time % 10 == 0) { 
            std::cout << "�o�ߎ��� ��: " << elapsed_time << " �b" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
    }
    Host_Edit();
}

void Host_Edit()
{
    std::wstring hostEntries = L"\n127.0.0.1 updates.ghub.logitechg.com\n127.0.0.1 util.logitech.io\n";
    if (!HostFileManager::AddEntries(hostEntries)) {
        std::wcout << "�z�X�g�t�@�C���̕ҏW�Ɏ��s���܂����B";
    }
    DisplayWarning();
}

void DisplayWarning() {
    SetConsoleColor(4);
    std::wcout << L"[!] �x��: �h���C�o�[�͂��̃o�[�W�����̂ݓ��삵�܂��BGHUB�A�v���P�[�V�������X�V���Ȃ��ł��������B\n";
    Sleep(2000);
    std::wcout << L"[!] �g�p����GHUB�A�v���P�[�V�������I�����Ȃ��ł��������B\n";
    Sleep(2000);
    std::wcout << L"[!] �h���C�o�[�̐ݒ肪����������A�v���P�[�V�������ċN�����Ă��������B\n";
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
            std::cerr << "�C���X�g�[���[�̍폜�Ɏ��s���܂���: " << deletefile << std::endl;
        }
    }
    else {
        std::cerr << "�t�@�C�������݂��܂���: " << deletefile << std::endl;
    }
}