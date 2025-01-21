#ifndef UTILS_H
#define UTILS_H

#include <Windows.h>
#include <iostream>
#include "Capture.h"

extern HWND hwnd;

void Draw_FOV();
void Radius_Reload();
void ExitOverlay();
void Exit(HWND hwnd);

extern int CrossRadius;

inline void SetConsoleColor(int textColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, textColor);
}

inline void clearConsole()
{
    std::system("cls");
}

int load();
void TriggerBot();
bool ReadConfig();
void CreateDefaultConfig();
void Call_AimBOT();

extern WinRTScreenCapture capture;

extern int screenWidth;
extern int screenHeight;

extern float MouseSensitivity;
extern int AimSpeed;

extern const float constant;
extern float modifier;

extern bool suspendThreads;
extern int threads;

extern int centreScreenX, centreScreenY;

extern bool Radius;
extern bool Triggerbot;
extern bool AimAssist;
extern bool outlawmode;
extern bool marshallmode;
extern bool operatormode;
extern bool vandalmode1;
extern bool vandalmode2;
extern bool pistolmode;
extern bool sheriffmode;

extern bool head;
extern bool body;

extern bool RED;
extern bool PURPLE;
extern bool YELLOW;

extern int shadeVariation;

extern int erode_iterations;
extern int dilate_iterations;

#endif