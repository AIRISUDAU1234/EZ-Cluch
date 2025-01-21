#define WIN32_LEAN_AND_MEAN
#define _WINSOCKAPI_
#include <winsock2.h>
#include <Windows.h>

#include <cmath>
#include <algorithm>
#include <chrono>
#include <mutex>
#include <atomic>
#include <random>

#include "Driver.h"
#include "Mouse.h"
#include "Global.h"

void Mouse::setGHubMouse(GhubMouse* newGHub)
{
    gHub = newGHub;
}

Mouse::Mouse(int captureWidth, int captureHeight, const float sensitivity)
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    screenWidth = captureWidth;
    screenHeight = captureHeight;
    centreScreenX = captureWidth / 2;
    centreScreenY = captureHeight / 2;
    MouseSensitivity = sensitivity;

    modifier = MouseSensitivity * constant;
}

LPARAM lparam = (0xFFFF) | ((0xFFFF) << 16);
WPARAM wparam = 0;
//pistolmode
std::random_device rd3;
std::mt19937 gen3(rd3());
std::uniform_real_distribution<> dist6(88.1709, 131.1646); //���� �S�[�X�g�Ȃ�
std::uniform_real_distribution<> dist7(61.5304, 107.0358); //���� �V�F���t�Ȃ�

int ShootTime6 = dist6(gen3);
int ShootTime7 = dist7(gen3);
//sniper mode
std::random_device rd2;
std::mt19937 gen2(rd2());
std::uniform_real_distribution<> dist3(81.1087, 114.0781); // ���� �I�y���[�^�[
std::uniform_real_distribution<> dist4(76.1501, 114.9048); // ���� �}�[�V����
std::uniform_real_distribution<> dist5(68.7118, 96.6342); // ���� �A�E�g���[

double ShootTime3 = dist3(gen2);
double ShootTime4 = dist4(gen2);
double ShootTime5 = dist5(gen2);
//vandal mode
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dist1(71.9951, 90.9328); // ���� ���@���_��1��
std::uniform_real_distribution<> dist2(138.9804, 166.1767); // ���� ���@���_��2��

double ShootTime1 = dist1(gen);
double ShootTime2 = dist2(gen);

/* �}�E�X���W���v�Z */
void Mouse::calibrateCoordinates(int& x, int& y)
{
    if (abs(x) < 5)
        x = 0;
    else {
        x = x - centreScreenX;
        x = (int)((float)x / modifier);
    }
    if (abs(y) < 5)
        y = 0;
    else
    {
        y = y - centreScreenY;
        y = (int)((float)y / modifier);
    }
}

void Mouse::siderifle_pistol() //�T�C�h�A�[��(�S�[�X�g��V�F���t�Ȃ�)
{
    HWND hwnd = GetForegroundWindow();
    if (pistolmode) //�S�[�X�g�Ȃ�
    {
        PostMessage(hwnd, WM_LBUTTONDOWN, wparam, lparam);
        Sleep(dist6(gen3));
        PostMessage(hwnd, WM_LBUTTONUP, wparam, lparam);
        Sleep(10);
        PostMessage(hwnd, WM_LBUTTONDOWN, wparam, lparam);
        Sleep(dist6(gen3));
        PostMessage(hwnd, WM_LBUTTONUP, wparam, lparam);
    }
    if (sheriffmode) //�V�F���t�Ȃ�
    {
        PostMessage(hwnd, WM_LBUTTONDOWN, wparam, lparam);
        Sleep(dist7(gen3));
        PostMessage(hwnd, WM_LBUTTONUP, wparam, lparam);
    }
}

void Mouse::Sniper() //�X�i�C�p�[�n
{
    HWND hwnd = GetForegroundWindow();
    if (operatormode)//�I�y���[�^�[
    {
        gHub->mouse_down();
        Sleep(dist3(gen2));
        gHub->mouse_up();
    }
    if (marshallmode)//�}�[�V����
    {
        gHub->mouse_down();
        Sleep(dist4(gen2));
        gHub->mouse_up();
    }
    if (outlawmode)//�A�E�g���[
    {
        gHub->mouse_down();
        Sleep(dist5(gen2));
        gHub->mouse_up();
    }
}

void Mouse::rifle() // ���@���_���Ȃ� (�����IGhubDriver���g�p)
{
    if (vandalmode1) //1��
    {
        gHub->mouse_down();
        Sleep(dist1(gen));
        gHub->mouse_up();
    }
    if (vandalmode2) //2��
    {
        gHub->mouse_down();
        Sleep(dist2(gen));
        gHub->mouse_up();
    }
}

void Mouse::moveSmooth(int x, int y, int moveSpeed) //move mouse
{
    while (suspendThreads) Sleep(1);
    threads++;

    calibrateCoordinates(x, y);

    int pWidth = (int)(0.5 * screenWidth) / 100;
    int pHeight = (int)(0.5 * screenHeight) / 100;

    int movedX = 0;
    int movedY = 0;

    int steps = abs(x) + abs(y);
    int ms = 10 - moveSpeed;

    float divX = (float)x / (float)steps;
    float divY = (float)y / (float)steps;

    float X = 0.0f;
    float Y = 0.0f;
    for (int i = 0; i < steps; i++)
    {
        if (suspendThreads)
        {
            threads--;
            suspendThreads = false;
            return;
        }
        X += divX;
        Y += divY;

        if (abs(X) > 1.0f || abs(Y) > 1.0f)
        {
            gHub->mouse_xy(static_cast<int>(X), static_cast<int>(Y)); //�}�E�X�ړ�
            movedX += (int)abs(X);
            movedY += (int)abs(Y);
            X -= int(X);
            Y -= int(Y);
        }
        if (movedX >= pWidth)
        {
            movedX = 0;
            Sleep(ms);
        }
        if (movedY >= pHeight)
        {
            movedY = 0;
            Sleep(ms);
        }
    }
    suspendThreads = false;
    threads--;
}

void Mouse::terminateThreads()
{
    suspendThreads = (threads > 0) ? true : false;
}