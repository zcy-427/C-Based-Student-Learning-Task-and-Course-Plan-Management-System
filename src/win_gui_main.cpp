#include "WinGui.h"

#include <commctrl.h>
#include <windows.h>

/**
 * @brief Win32 GUI 版本程序入口。
 *
 * 该入口不依赖 Qt，直接使用 Windows API 创建桌面窗口。
 */
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int showCommand) {
    INITCOMMONCONTROLSEX controls{};
    controls.dwSize = sizeof(controls);
    controls.dwICC = ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES;
    InitCommonControlsEx(&controls);

    WinMainWindow mainWindow(instance);
    if (!mainWindow.create(showCommand)) {
        return 1;
    }

    MSG message;
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    return static_cast<int>(message.wParam);
}
