#ifndef CAPTURE_H
#define CAPTURE_H

#include <windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <winrt/Windows.Foundation.h>
#include <string>

class WinRTScreenCapture
{
public:
    WinRTScreenCapture()
        : regionX(0), regionY(0), regionWidth(0), regionHeight(0), screenWidth(0), screenHeight(0)
    {
        // 必要に応じてデフォルト初期化処理を追加
    }

    WinRTScreenCapture(RECT captureRegion);

    ~WinRTScreenCapture();
    winrt::com_ptr<ID3D11Texture2D> GetNextFrame();
    int getWidth();
    int getHeight();

    winrt::com_ptr<ID3D11Device> d3dDevice;
    winrt::com_ptr<ID3D11DeviceContext> d3dContext;

private:
    char* WindowName;
    int regionX, regionY, regionWidth, regionHeight;
    winrt::Windows::Graphics::Capture::GraphicsCaptureItem CreateCaptureItemForMonitor(HMONITOR hMonitor);
    winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice CreateDirect3DDevice(IDXGIDevice* dxgiDevice);

    template<typename T>
    winrt::com_ptr<T> GetDXGIInterfaceFromObject(winrt::Windows::Foundation::IInspectable const& object);

    winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice device{ nullptr };
    winrt::Windows::Graphics::Capture::GraphicsCaptureItem captureItem{ nullptr };
    winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool framePool{ nullptr };
    winrt::Windows::Graphics::Capture::GraphicsCaptureSession session{ nullptr };

    int screenWidth, screenHeight;
};
HMONITOR GetMonitorHandleByIndex(int monitorIndex);
#endif