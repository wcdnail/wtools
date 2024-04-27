#pragma once

namespace Ui { class StatusBar; }

namespace Twins
{
    Ui::StatusBar& UiStatus();

    void SetMainframeStatus(HRESULT hr, HICON icon, PCWSTR format, ...);
    void SetMainframeStatus(HRESULT hr, HICON icon, PCSTR format, ...);
}
