#include <iostream>
#include <windows.h> 
#include <gdiplus.h>
#include <vector>

#include "Global.h"
#include "ToggleKey.h"
#include "ColorFilter.h"

// 0xb41515 = Red
// 0xaf2eaf = purple
// 0xc8c722 = yellow

void ToggleRadius() {
    Radius = !Radius;
    if (Radius)
    {
        Draw_FOV();
    }
    else
    {
        EXIT();
    }
    Beep(1000, 10);
}

void ToggleTriggerbot() {
    Triggerbot = !Triggerbot;
    if (Triggerbot)
    {
        CrossRadius = 20;
        Sleep(10);
        Radius_Reload();
    }
    Beep(1500, 10);
}

void ToggleAimAssist() {
    AimAssist = !AimAssist;
    if (AimAssist)
    {
        CrossRadius = 60;
        Sleep(10);
        Radius_Reload();
    }
    Beep(3500, 10);
}

int SniperCounter = 0;

void ToggleSniper() {//スナイパーモード切り替え
    SniperCounter = (SniperCounter % 3) + 1;

    outlawmode = false;
    marshallmode = false;
    operatormode = false;

    switch (SniperCounter) {
    case 1:
        outlawmode = true;
        marshallmode = false;
        operatormode = false;
        break;
    case 2:
        marshallmode = true;
        operatormode = false;
        outlawmode = false;
        break;
    case 3:
        operatormode = true;
        outlawmode = false;
        marshallmode = false;
        break;
    }
    Beep(1000, 100);
}

int VandalCounter = 0;

void ToggleVandal() {//ヴァンダルモード切り替え
    VandalCounter = (VandalCounter % 2) + 1;

    vandalmode1 = false;
    vandalmode2 = false;

    switch (VandalCounter)
    {
    case 1:
        vandalmode1 = true;
        vandalmode2 = false;
        break;
    case 2:
        vandalmode2 = true;
        vandalmode1 = false;
        break;
    }
    Beep(1000, 100);
}

int SidearmCounter = 0;

void ToggleSidearm() {//ピストルモード切替え
    SidearmCounter = (SidearmCounter % 2) + 1;

    pistolmode = false;
    sheriffmode = false;

    switch (SidearmCounter)
    {
    case 1:
        pistolmode = true;
        sheriffmode = false;
        break;
    case 2:
        sheriffmode = true;
        pistolmode = false;
        break;
    }
    Beep(1000, 100);
}

int HebeCounter;

void ToggleHeBd() { //aim head body 切り替え
    HebeCounter = (HebeCounter % 2) + 1;

    head = false;
    body = false;

    switch (HebeCounter)
    {
    case 1:
        head = true;
        body = false;
        break;
    case 2:
        head = false;
        body = true;
        break;
    }
    Beep(1000, 100);
}

int ColorCounter;

void ToggleColor() { //敵Color切り替え
    ColorCounter = (ColorCounter % 3) + 1;

    RED = false;
    PURPLE = false;
    YELLOW = false; 

    int redHex = 0xb41515;
    int purpleHex = 0xaf2eaf;
    int yellowHex = 0xc8c722;

    cv::Scalar base_color;

    switch (ColorCounter)
    {
    case 1:
        RED = true;
        PURPLE = false;
        YELLOW = false;
        base_color = hexToScalar(redHex);
        break;
    case 2:
        RED = false;
        PURPLE = true;
        YELLOW = false;
        base_color = hexToScalar(purpleHex);
        break;
    case 3:
        RED = false;
        PURPLE = false;
        YELLOW = true;
        base_color = hexToScalar(yellowHex);
        break;
    }
    Beep(1000, 100);
}

#define HOTKEY_ID_F6 4
int run = true;

void EXIT()
{
    std::cout << "終了しています..." << std::endl;
    SetConsoleColor(7);
    run = false; //app exit
    Sleep(800);
    exit(1);
}