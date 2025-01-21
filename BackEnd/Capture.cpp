#include "windows.graphics.directx.direct3d11.interop.h"
#include "Windows.Graphics.Capture.Interop.h"
#include "d3d11.h"
#include "Capture.h"
#include <iostream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

int capture_fps = 60;
int monitor_idx = 0;
bool capture_borders = true;
bool capture_cursor = true;

WinRTScreenCapture::WinRTScreenCapture(RECT captureRegion)
{
    regionX = captureRegion.left;
    regionY = captureRegion.top;
    regionWidth = captureRegion.right - captureRegion.left;
    regionHeight = captureRegion.bottom - captureRegion.top;

    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    winrt::check_hresult(D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        0,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        featureLevels,
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        d3dDevice.put(),
        nullptr,
        d3dContext.put()
    ));

    winrt::com_ptr<IDXGIDevice> dxgiDevice;
    winrt::check_hresult(d3dDevice->QueryInterface(IID_PPV_ARGS(dxgiDevice.put())));
    device = CreateDirect3DDevice(dxgiDevice.get());

    if (!device)
    {
        std::cerr << "[Capture] Can't create Direct3DDevice!" << std::endl;
        return;
    }

    HMONITOR hMonitor = GetMonitorHandleByIndex(monitor_idx); //???

    if (!hMonitor)
    {
        std::cerr << "[Capture] Failed to get monitor handle for index: " << monitor_idx << std::endl;
        return;
    }

    captureItem = CreateCaptureItemForMonitor(hMonitor);

    if (!captureItem)
    {
        std::cerr << "[Capture] GraphicsCaptureItem not created!" << std::endl;
        return;
    }

    screenWidth = captureItem.Size().Width;
    screenHeight = captureItem.Size().Height;

    regionX = (screenWidth - regionWidth) / 2;
    regionY = (screenHeight - regionHeight) / 2;

    framePool = winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::CreateFreeThreaded(
        device,
        winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
        1,
        captureItem.Size()
    );

    session = framePool.CreateCaptureSession(captureItem);

    if (!capture_borders)
    {
        session.IsBorderRequired(false);
    }

    if (!capture_cursor)
    {
        session.IsCursorCaptureEnabled(false);
    }

    session.StartCapture();
}

WinRTScreenCapture::~WinRTScreenCapture()
{
    if (session)
    {
        session.Close();
        session = nullptr;
    }

    if (framePool)
    {
        framePool.Close();
        framePool = nullptr;
    }

    d3dContext = nullptr;
    d3dDevice = nullptr;
}

winrt::com_ptr<ID3D11Texture2D> WinRTScreenCapture::GetNextFrame() //キャプチャ
{
    try
    {
        winrt::Windows::Graphics::Capture::Direct3D11CaptureFrame frame = nullptr;

        // フレーム取得（最新のフレームを取得する）
        while (true)
        {
            auto tempFrame = framePool.TryGetNextFrame();
            if (!tempFrame)
                break;
            frame = tempFrame;
        }

        // フレームが取得できなかった場合
        if (!frame)
        {
            std::cerr << "[Capture] No frame available." << std::endl;
            return nullptr;
        }

        // フレームからテクスチャを取得
        auto surface = frame.Surface();
        winrt::com_ptr<ID3D11Texture2D> frameTexture = GetDXGIInterfaceFromObject<ID3D11Texture2D>(surface);

        if (!frameTexture)
        {
            throw std::runtime_error("[Capture] Failed to get ID3D11Texture2D from frame surface.");
        }

        // テクスチャを返す
        return frameTexture;
    }
    catch (const std::exception& e)
    {
        std::cerr << "[Capture] Error in GetNextFrame: " << e.what() << std::endl;
        return nullptr;
    }
}

winrt::Windows::Graphics::Capture::GraphicsCaptureItem WinRTScreenCapture::CreateCaptureItemForMonitor(HMONITOR hMonitor)
{
    try
    {
        auto interopFactory = winrt::get_activation_factory<winrt::Windows::Graphics::Capture::GraphicsCaptureItem, IGraphicsCaptureItemInterop>();
        winrt::Windows::Graphics::Capture::GraphicsCaptureItem item{ nullptr };
        HRESULT hr = interopFactory->CreateForMonitor(hMonitor, winrt::guid_of<winrt::Windows::Graphics::Capture::GraphicsCaptureItem>(), winrt::put_abi(item));
        if (FAILED(hr))
        {
            throw std::runtime_error("[Capture] Can't create GraphicsCaptureItem for monitor. HRESULT: " + std::to_string(hr));
        }
        return item;
    }
    catch (const std::exception& e)
    {
        std::cerr << "[Capture] Error in CreateCaptureItemForMonitor: " << e.what() << std::endl;
        return nullptr;
    }
}

winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice WinRTScreenCapture::CreateDirect3DDevice(IDXGIDevice* dxgiDevice)
{
    winrt::com_ptr<::IInspectable> inspectable;
    winrt::check_hresult(CreateDirect3D11DeviceFromDXGIDevice(dxgiDevice, inspectable.put()));
    return inspectable.as<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice>();
}

template<typename T>
winrt::com_ptr<T> WinRTScreenCapture::GetDXGIInterfaceFromObject(winrt::Windows::Foundation::IInspectable const& object)
{
    auto access = object.as<Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>();
    winrt::com_ptr<T> result;
    winrt::check_hresult(access->GetInterface(winrt::guid_of<T>(), result.put_void()));
    return result;
}

int WinRTScreenCapture::getWidth() {
    HWND hwnd = FindWindowA(0, WindowName);
    if (hwnd == NULL) return 0;

    RECT rekt;
    GetWindowRect(hwnd, &rekt);
    return rekt.right - rekt.left;
}

int WinRTScreenCapture::getHeight() {
    HWND hwnd = FindWindowA(0, WindowName);
    if (hwnd == NULL) return 0;

    RECT rekt;
    GetWindowRect(hwnd, &rekt);
    return rekt.bottom - rekt.top;
}

HMONITOR GetMonitorHandleByIndex(int monitorIndex)
{
    struct MonitorSearch
    {
        int targetIndex;
        int currentIndex;
        HMONITOR targetMonitor;
    };

    MonitorSearch search = { monitorIndex, 0, nullptr };

    EnumDisplayMonitors(nullptr, nullptr,
        [](HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam) -> BOOL
        {
            MonitorSearch* search = reinterpret_cast<MonitorSearch*>(lParam);
            if (search->currentIndex == search->targetIndex)
            {
                search->targetMonitor = hMonitor;
                return FALSE;
            }
            search->currentIndex++;
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&search));

    return search.targetMonitor;
}