#pragma once

#ifndef MOUSE_H
#define MOUSE_H

#include <Windows.h>
#include <iostream>
#include <mutex>

#include "Driver.h"

class Mouse
{
private:

    void calibrateCoordinates(int& x, int& y); //Š´“x‚ÆCüq‚ÉŠî‚Ã‚¢‚Ä‰æ–ÊÀ•W‚ğ•Ô‚·

    double prev_x, prev_y, prev_velocity_x, prev_velocity_y;
    std::chrono::time_point<std::chrono::steady_clock> prev_time;

    GhubMouse* gHub;

    std::chrono::steady_clock::time_point last_target_time;
    std::atomic<bool> target_detected{ false };
    std::atomic<bool> mouse_pressed{ false };

public:
    Mouse(int captureWidth, int captureHeight, const float sensitivity);

    void terminateThreads();
    void moveSmooth(int x, int y, int moveSpeed);
    void rifle();
    void Sniper();
    void siderifle_pistol();

    std::mutex input_method_mutex;
    void setGHubMouse(GhubMouse* newGHub);
};
#endif 