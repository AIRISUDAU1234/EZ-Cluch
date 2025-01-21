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
        "# 設定\n"
        "MouseSensitivity = 0.4 #感度\n"
        "AimSpeed = 7 # AIMする速さ\n"
        "shadeVariation = 28 #色の閾値 (基本的にいじらなくても大丈夫)\n";

    std::ofstream outFile("config.txt");
    if (outFile.is_open()) {
        outFile << defaultConfig;
        outFile.close();
        std::cout << "新しい設定ファイル 'config.txt' を作成しました。\n";
    }
    else {
        std::cerr << "設定ファイルの作成に失敗しました。\n";
    }
}

bool ReadConfig() {
    std::ifstream cFile("config.txt");
    std::string line;

    if (cFile.is_open()) {
        while (std::getline(cFile, line)) {
            // 空白文字を削除
            line.erase(std::remove_if(line.begin(), line.end(),
                [](unsigned char c) { return std::isspace(c); }),
                line.end());
            if (line.empty() || line[0] == '#') continue; // コメントや空行をスキップ

            auto delimiterPos = line.find("=");
            if (delimiterPos == std::string::npos) {
                std::cerr << "無効な設定行: " << line << std::endl;
                continue;
            }

            auto name = line.substr(0, delimiterPos);
            auto value = line.substr(delimiterPos + 1);

            try {
                // 設定値を割り当て
                if (name == NAMEOF(MouseSensitivity)) {
                    MouseSensitivity = std::stof(value); // マウス感度
                }
                else if (name == NAMEOF(AimSpeed)) {
                    AimSpeed = std::stoi(value); // AIMスピード
                }
                else if (name == NAMEOF(shadeVariation)) {
                    shadeVariation = std::stoi(value); // 色の許容される変動範囲
                }
                else {
                    std::cerr << "未知の設定項目: " << name << std::endl;
                }
            }
            catch (const std::exception& e) {
                std::cerr << "例外: " << e.what() << ", 設定項目: " << name << ", 値: " << value << std::endl;
            }
        }
        cFile.close();
        return true;
    }
    else {
        std::cerr << "設定ファイルを開けませんでした: config.txt" << std::endl;
        return false;
    }
}