#include <iostream>
#include <Windows.h>
#include <chrono>

#include "Capture.h"
#include "Mouse.h"
#include "ColorFilter.h"
#include "Global.h"
#include <thread>

using namespace std;

std::atomic<bool> runTriggerBot(true);

void SideriflePistol(Mouse* m);
void Rifle(Mouse* m);
void sniper(Mouse* m);

void TriggerBot() {
    cv::Scalar base_color;
    int CrossRadius = 20; //20

    bool OPRButnDown = false;
    bool marshalRButnDown = false;
    bool outlawRButnDown = false;

    auto OPTime = std::chrono::steady_clock::now();
    auto marshalTime = std::chrono::steady_clock::now();
    auto outlawTime = std::chrono::steady_clock::now();

    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);

    int mwidth = capture.getWidth();
    int mheight = capture.getHeight();

    Mouse mousey(mwidth, mheight, MouseSensitivity);
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    RECT sourceRectangle;
    sourceRectangle.left = screenWidth / 2 - CrossRadius;
    sourceRectangle.top = screenHeight / 2 - CrossRadius;
    sourceRectangle.right = sourceRectangle.left + CrossRadius * 2;
    sourceRectangle.bottom = sourceRectangle.top + CrossRadius * 2;

    WinRTScreenCapture screenCapture(sourceRectangle);

    D3D11_TEXTURE2D_DESC stagingDesc = {};
    winrt::com_ptr<ID3D11Texture2D> stagingTexture;
    UINT width = 0, height = 0;

    while (true)
    {
        if (Triggerbot)
        {
            auto frameTexture = screenCapture.GetNextFrame();
            if (frameTexture)
            {
                D3D11_TEXTURE2D_DESC desc;
                frameTexture->GetDesc(&desc);

                if (width != desc.Width || height != desc.Height)
                {
                    width = desc.Width;
                    height = desc.Height;

                    stagingDesc = desc;
                    stagingDesc.Usage = D3D11_USAGE_STAGING;
                    stagingDesc.BindFlags = 0;
                    stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
                    stagingDesc.MiscFlags = 0;
                    screenCapture.d3dDevice->CreateTexture2D(&stagingDesc, nullptr, stagingTexture.put());
                }
                screenCapture.d3dContext->CopyResource(stagingTexture.get(), frameTexture.get());
                D3D11_MAPPED_SUBRESOURCE mappedResource;
                HRESULT hr = screenCapture.d3dContext->Map(stagingTexture.get(), 0, D3D11_MAP_READ, 0, &mappedResource);
                if (SUCCEEDED(hr))
                {
                    auto* pixels = static_cast<unsigned char*>(mappedResource.pData);
                    UINT rowPitch = mappedResource.RowPitch;

                    for (UINT y = 0; y < height; ++y)
                    {
                        for (UINT x = 0; x < width; ++x)
                        {
                            const unsigned char* pixel = &pixels[y * rowPitch + x * 4];
                            //if (ColorFilter(pixel, width, height, base_color, shadeVariation, erode_iterations, dilate_iterations))
                             if (IsColor(pixel)){
                                
                                //Vandal mode«
                                if (vandalmode1)
                                {
                                    thread t(Rifle, &mousey);
                                    t.detach();
                                }
                                if (vandalmode2)
                                {
                                    thread t(Rifle, &mousey);
                                    t.detach();
                                }
                                //sniper mode«
                                if (operatormode) //operator 447
                                {
                                    if ((GetAsyncKeyState(VK_RBUTTON) & 0x8000))
                                    {
                                        if (!OPRButnDown)
                                        {
                                            OPRButnDown = true;
                                            OPTime = std::chrono::steady_clock::now();
                                        }
                                        else {
                                            auto elapsedTime = std::chrono::steady_clock::now() - OPTime;
                                            if (std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count() >= 447)
                                            {
                                                thread t(sniper, &mousey);
                                                t.detach();
                                            }
                                        }
                                    }
                                    else
                                    {
                                        OPRButnDown = false;
                                    }
                                }
                            }
                            if (marshallmode) //marshal 280
                            {
                                if ((GetAsyncKeyState(VK_RBUTTON) & 0x8000))
                                {
                                    if (!marshalRButnDown)
                                    {
                                        marshalRButnDown = true;
                                        marshalTime = std::chrono::steady_clock::now();
                                    }
                                    else {
                                        auto elapsedTime = std::chrono::steady_clock::now() - marshalTime;
                                        if (std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count() >= 280)
                                        {
                                            thread t(sniper, &mousey);
                                            t.detach();
                                        }
                                    }
                                }
                                else
                                {
                                    marshalRButnDown = false;
                                }
                            }
                        }
                        if (outlawmode) //outlaw 256
                        {
                            if ((GetAsyncKeyState(VK_RBUTTON) & 0x8000))
                            {
                                if (!outlawRButnDown)
                                {
                                    outlawRButnDown = true;
                                    outlawTime = std::chrono::steady_clock::now();
                                }
                                else {
                                    auto elapsedTime = std::chrono::steady_clock::now() - outlawTime;
                                    if (std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count() >= 256)
                                    {
                                        thread t(sniper, &mousey);
                                        t.detach();
                                    }
                                }
                            }
                            else
                            {
                                outlawRButnDown = false;
                            }
                        }
                    }
                }
                screenCapture.d3dContext->Unmap(stagingTexture.get(), 0);
            }
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60)); //FPS‚ð’²®
}

void SideriflePistol(Mouse* m)
{
    m->siderifle_pistol();
}

void Rifle(Mouse* m)
{
    m->rifle();
}

void sniper(Mouse* m)
{
    m->Sniper();
}