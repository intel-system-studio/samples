/*
    ==============================================================

    SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
    http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/

    Copyright (C) Intel Corporation

    THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
    NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
    PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.

    =============================================================
*/

#pragma once

#include <wrl/client.h>
#include <d3d11_1.h>
#include <d2d1_1.h>
#include <d2d1effects.h>
#include <dwrite_1.h>
#include <wincodec.h>
#include "App.xaml.h"
#include <agile.h>

#pragma warning (disable: 4449)

// Helper utilities to make DirectX APIs work with exceptions
namespace DX
{
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            // Set a breakpoint on this line to catch DirectX API errors
            throw Platform::Exception::CreateException(hr);
        }
    }
}

// Helper class that initializes DirectX APIs
ref class DirectXBase abstract
{
internal:
    DirectXBase();

public:
    virtual void Initialize(Windows::UI::Core::CoreWindow^ window, Windows::UI::Xaml::Controls::SwapChainBackgroundPanel^ panel, float dpi);
    virtual void CreateDeviceIndependentResources();
    virtual void CreateDeviceResources();
    virtual void SetDpi(float dpi);
    virtual void CreateWindowSizeDependentResources();
    virtual void UpdateForWindowSizeChange();
    virtual void Render() = 0;
    virtual void Present();
    virtual float ConvertDipsToPixels(float dips);

protected private:

    Platform::Agile<Windows::UI::Core::CoreWindow>         m_window;
    Windows::UI::Xaml::Controls::SwapChainBackgroundPanel^ m_panel;

    // Direct2D Objects
    Microsoft::WRL::ComPtr<ID2D1Factory1>                  m_d2dFactory;
    Microsoft::WRL::ComPtr<ID2D1Device>                    m_d2dDevice;
    Microsoft::WRL::ComPtr<ID2D1DeviceContext>             m_d2dContext;
    Microsoft::WRL::ComPtr<ID2D1Bitmap1>                   m_d2dTargetBitmap;

    // DirectWrite & Windows Imaging Component Objects
    Microsoft::WRL::ComPtr<IDWriteFactory1>                m_dwriteFactory;
    Microsoft::WRL::ComPtr<IWICImagingFactory2>            m_wicFactory;

    // Direct3D Objects
    Microsoft::WRL::ComPtr<ID3D11Device1>                  m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext1>           m_d3dContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain1>                m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>         m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>         m_depthStencilView;

    D3D_FEATURE_LEVEL                                      m_featureLevel;
    Windows::Foundation::Size                              m_renderTargetSize;
    Windows::Foundation::Rect                              m_windowBounds;
    float                                                  m_dpi;
};

#pragma warning (default: 4449)
