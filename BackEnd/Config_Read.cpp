#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <cctype>
#include "Global.h"

using namespace std;

#define NAMEOF(name) #name

void CreateDefaultConfig() {
    const std::string defaultConfig =
        "# �ݒ�\n"
        "MouseSensitivity = 0.4 #���x\n"
        "AimSpeed = 7 # AIM���鑬��\n"
        "shadeVariation = 28 #�F��臒l (��{�I�ɂ�����Ȃ��Ă����v)\n";

    std::ofstream outFile("config.txt");
    if (outFile.is_open()) {
        outFile << defaultConfig;
        outFile.close();
        std::cout << "�V�����ݒ�t�@�C�� 'config.txt' ���쐬���܂����B\n";
    }
    else {
        std::cerr << "�ݒ�t�@�C���̍쐬�Ɏ��s���܂����B\n";
    }
}

bool ReadConfig() {
    std::ifstream cFile("config.txt");
    std::string line;

    if (cFile.is_open()) {
        while (std::getline(cFile, line)) {
            // �󔒕������폜
            line.erase(std::remove_if(line.begin(), line.end(),
                [](unsigned char c) { return std::isspace(c); }),
                line.end());
            if (line.empty() || line[0] == '#') continue; // �R�����g���s���X�L�b�v

            auto delimiterPos = line.find("=");
            if (delimiterPos == std::string::npos) {
                std::cerr << "�����Ȑݒ�s: " << line << std::endl;
                continue;
            }

            auto name = line.substr(0, delimiterPos);
            auto value = line.substr(delimiterPos + 1);

            try {
                // �ݒ�l�����蓖��
                if (name == NAMEOF(MouseSensitivity)) {
                    MouseSensitivity = std::stof(value); // �}�E�X���x
                }
                else if (name == NAMEOF(AimSpeed)) {
                    AimSpeed = std::stoi(value); // AIM�X�s�[�h
                }
                else if (name == NAMEOF(shadeVariation)) {
                    shadeVariation = std::stoi(value); // �F�̋��e�����ϓ��͈�
                }
                else {
                    std::cerr << "���m�̐ݒ荀��: " << name << std::endl;
                }
            }
            catch (const std::exception& e) {
                std::cerr << "��O: " << e.what() << ", �ݒ荀��: " << name << ", �l: " << value << std::endl;
            }
        }
        cFile.close();
        return true;
    }
    else {
        std::cerr << "�ݒ�t�@�C�����J���܂���ł���: config.txt" << std::endl;
        return false;
    }
}