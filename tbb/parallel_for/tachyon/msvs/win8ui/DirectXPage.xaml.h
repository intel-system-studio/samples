/*
    ==============================================================

    SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
    http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/

    Copyright 2016 Intel Corporation

    THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
    NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
    PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.

    =============================================================
*/

#pragma once

#include "DirectXPage.g.h"
#include "tbbTachyonRenderer.h"

namespace tbbTachyon
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class DirectXPage sealed
    {
    public:
        DirectXPage();

    private:
        ~DirectXPage();
        void OnRendering(Object^ sender, Object^ args);

        Windows::Foundation::EventRegistrationToken m_eventToken;

        tbbTachyonRenderer^ m_renderer;
        bool m_renderNeeded;
        int m_number_of_threads;

        void ThreadsSliderValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
        void ThreadsApply_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void Exit_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
