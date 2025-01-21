#include <list>

#include "Capture.h"
#include "Mouse.h"
#include "ColorFilter.h"
#include "Global.h"
#include <thread>

#define PROCESS_NAME L"VALORANT  " 
HWND game_window;
uint32_t width;
uint32_t height;

int cwidth = GetSystemMetrics(SM_CXSCREEN);
int cheight = GetSystemMetrics(SM_CYSCREEN);

Mouse mouse(cwidth, cheight, MouseSensitivity);
void AimBot(int height, int width);

using namespace std;
#define M_PI 3.14159265358979323846  
#define DEGTORAD M_PI / 180 
#define RADTODEG 180 / M_PI

int full360 = 0;
// float moveSpeed = 0.2;

std::atomic<bool> runAimBot(true);

void Call_AimBOT();

struct Vector2 {
    int x;
    int y;
    Vector2(int X, int Y) {
        x = X;
        y = Y;
    }
    float Len() {
        return sqrt(pow(x, 2) + pow(y, 2));
    }
    Vector2 operator+(const Vector2& a) const
    {
        return Vector2(a.x + x, a.y + y);
    }
};
void MoveMouse(Vector2 front, int height, int width);

int Full360() {
    return full360;
}

int GetCoordsX(int delta, int total) {
    double lookAt = delta * 2.0 / total;
    double degrees = atan(lookAt * tan(52.0 * DEGTORAD)) * RADTODEG;
    return (Full360() * degrees * CrossRadius) / (360 * screenWidth);
}

int GetCoordsY(int delta, int total) {
    double lookAt = delta * 2.0 / total;
    double degrees = atan(lookAt * tan(36.0 * DEGTORAD)) * RADTODEG;
    return (Full360() * degrees * CrossRadius) / (360 * screenHeight);
}

void Call_AimBOT()
{
    RECT rect;
    game_window = FindWindowW(NULL, PROCESS_NAME);
    GetClientRect(game_window, &rect);

    HDC monitor = GetDC(game_window); 

    int current = GetDeviceCaps(monitor, VERTRES);
    int total = GetDeviceCaps(monitor, DESKTOPVERTRES);

    width = (rect.right - rect.left) * total / current;
    height = (rect.bottom - rect.top) * total / current;
    AimBot(height, width);
}

void AimBot(int height, int width)
{
    cv::Scalar base_color;
    int CrossRadius = 60;
    const int maxCount = 5;
    const int forSize = 100;

    list<Vector2> vects;
    int hWidth = width / 2;
    int hHeight = height / 2;

    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);

    int screenWidth = desktop.right;
    int screenHeight = desktop.bottom;

    RECT sourceRectangle;
    sourceRectangle.left = screenWidth / 2 - CrossRadius;
    sourceRectangle.top = screenHeight / 2 - CrossRadius;
    sourceRectangle.right = sourceRectangle.left + CrossRadius * 2;
    sourceRectangle.bottom = sourceRectangle.top + CrossRadius * 2;

    WinRTScreenCapture screenCapture(sourceRectangle);

    D3D11_TEXTURE2D_DESC stagingDesc = {};
    winrt::com_ptr<ID3D11Texture2D> stagingTexture;
    UINT frwidth = 0, frheight = 0;

    while (true)
    {
        if (AimAssist)
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
                            if (ColorFilter(pixel, width, height, base_color, shadeVariation, erode_iterations, dilate_iterations)) {
                                if (body)
                                {
                                    MoveMouse(Vector2(x - hWidth, y - hHeight), height, width); //Mouse body
                                }
                                if (head)
                                {
                                    vects.push_back(Vector2(x - hWidth, y - hHeight));
                                    if (vects.size() > 0) {
                                        vects.sort([](const Vector2& lhs, const Vector2& rhs)
                                            {
                                                return  lhs.y < rhs.y;
                                            });
                                        list<Vector2> forbidden;
                                        for (auto& current : vects)
                                        {
                                            bool canUpdate = true;
                                            if (abs(current.x) > screenWidth || abs(current.y) > screenHeight) {
                                                continue;
                                            }
                                            for (auto& forb : forbidden)
                                            {
                                                if ((current + forb).Len() < forSize) {
                                                    canUpdate = false;
                                                    break;
                                                }
                                                if (abs(current.x + forb.x) < forSize) {
                                                    canUpdate = false;
                                                    break;
                                                }
                                            }
                                            if (canUpdate) {
                                                forbidden.push_front(current);
                                                if (forbidden.size() > maxCount) {
                                                    break;
                                                }
                                            }
                                        }
                                        if (forbidden.size() > 0) {
                                            forbidden.sort([](const Vector2& lhs, const Vector2& rhs)
                                                {
                                                    return sqrt(pow(lhs.x, 2) + pow(lhs.y * 10, 2)) < sqrt(pow(rhs.x, 2) + pow(rhs.y * 10, 2));
                                                });
                                            Vector2 front = forbidden.front();

                                            MoveMouse(front, height, width); //Mouse head
                                        }
                                    }
                                }
                            }
                            screenCapture.d3dContext->Unmap(stagingTexture.get(), 0);
                        }
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60)); //FPSÇí≤êÆ
            }
        }
    }
}

void MoveMouse(Vector2 front, int height, int width) 
{
    int x = GetCoordsX(front.x, width);
    int y = GetCoordsY(front.y, height);
    mouse.moveSmooth(static_cast<int>(x * AimSpeed), static_cast<int>(y * AimSpeed), AimSpeed); //moveSpeed
}