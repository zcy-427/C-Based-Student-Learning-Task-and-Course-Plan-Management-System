#include "WinGui.h"

#include "Utils.h"

#include <commctrl.h>

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
constexpr int ID_COURSE_FILTER = 1001;
constexpr int ID_TYPE_FILTER = 1002;
constexpr int ID_REFRESH = 1003;
constexpr int ID_TASK_LIST = 1004;
constexpr int ID_ADD = 1005;
constexpr int ID_DETAIL = 1006;
constexpr int ID_DELETE = 1007;
constexpr int ID_TOGGLE = 1008;
constexpr int ID_SORT = 1009;
constexpr int ID_STATISTICS = 1010;
constexpr int ID_SAVE = 1011;
constexpr int ID_LOAD = 1012;

constexpr int ID_DIALOG_TYPE = 2001;
constexpr int ID_DIALOG_NAME = 2002;
constexpr int ID_DIALOG_COURSE = 2003;
constexpr int ID_DIALOG_DATE = 2004;
constexpr int ID_DIALOG_PRIORITY = 2005;
constexpr int ID_DIALOG_COMPLETED = 2006;
constexpr int ID_DIALOG_DESCRIPTION = 2007;
constexpr int ID_DIALOG_EXTRA1_LABEL = 2008;
constexpr int ID_DIALOG_EXTRA1 = 2009;
constexpr int ID_DIALOG_EXTRA2_LABEL = 2010;
constexpr int ID_DIALOG_EXTRA2 = 2011;
constexpr int ID_DIALOG_OK = 2012;
constexpr int ID_DIALOG_CANCEL = 2013;

// 控件尺寸常量
constexpr int HEADER_H = 82;
constexpr int FILTER_TOP = 104;
constexpr int FILTER_HEIGHT = 30;
constexpr int BTN_W1 = 112;
constexpr int BTN_W2 = 150;
constexpr int BTN_H = 34;
constexpr int BTN_GAP = 10;
constexpr int LIST_TOP = 154;
constexpr int STATUSBAR_H = 24;

HFONT appUiFont = nullptr;

// ===================== 工具函数 =====================

std::wstring toWide(const std::string& text) {
    if (text.empty()) return L"";
    int size = MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);
    if (size <= 0) return L"";
    std::wstring result(size, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), result.data(), size);
    return result;
}

std::string toUtf8(const std::wstring& text) {
    if (text.empty()) return "";
    int size = WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);
    if (size <= 0) return "";
    std::string result(size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), result.data(), size, nullptr, nullptr);
    return result;
}

std::wstring getWindowText(HWND hwnd) {
    int length = GetWindowTextLengthW(hwnd);
    std::wstring text(length + 1, L'\0');
    GetWindowTextW(hwnd, text.data(), length + 1);
    text.resize(length);
    return text;
}

void setDefaultFont(HWND hwnd) {
    HFONT font = appUiFont ? appUiFont : reinterpret_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
    SendMessageW(hwnd, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
}

HFONT createFontObject(int pointSize, int weight) {
    HDC hdc = GetDC(nullptr);
    int height = -MulDiv(pointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    ReleaseDC(nullptr, hdc);

    return CreateFontW(height, 0, 0, 0, weight, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS,
                       L"Microsoft YaHei UI");
}

HBRUSH createSolidBrush(COLORREF color) {
    return CreateSolidBrush(color);
}

COLORREF blendColor(COLORREF color, int delta) {
    auto clamp = [](int value) { return std::max(0, std::min(255, value)); };
    return RGB(clamp(GetRValue(color) + delta), clamp(GetGValue(color) + delta), clamp(GetBValue(color) + delta));
}

void fillRoundRect(HDC hdc, RECT rect, int radius, COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    HPEN pen = CreatePen(PS_SOLID, 1, color);
    HGDIOBJ oldBrush = SelectObject(hdc, brush);
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, radius, radius);
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(pen);
    DeleteObject(brush);
}

// ===================== 控件工厂 =====================

HWND createButton(HWND parent, int id, const wchar_t* text, bool ownerDraw = false) {
    DWORD style = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON;
    style |= ownerDraw ? BS_OWNERDRAW : BS_FLAT;
    HWND hwnd = CreateWindowW(L"BUTTON", text, style,
                              0, 0, 100, 30, parent, reinterpret_cast<HMENU>(id), nullptr, nullptr);
    setDefaultFont(hwnd);
    return hwnd;
}

HWND createLabel(HWND parent, const wchar_t* text, int x, int y, int width, int height) {
    HWND hwnd = CreateWindowW(L"STATIC", text, WS_CHILD | WS_VISIBLE,
                              x, y, width, height, parent, nullptr, nullptr, nullptr);
    setDefaultFont(hwnd);
    return hwnd;
}

HWND createEdit(HWND parent, int id, int x, int y, int width, int height, DWORD extraStyle = ES_AUTOHSCROLL) {
    HWND hwnd = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                                WS_CHILD | WS_VISIBLE | extraStyle,
                                x, y, width, height, parent, reinterpret_cast<HMENU>(id), nullptr, nullptr);
    setDefaultFont(hwnd);
    return hwnd;
}

HWND createCombo(HWND parent, int id, int x, int y, int width, int height) {
    HWND hwnd = CreateWindowW(L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
                              x, y, width, height, parent, reinterpret_cast<HMENU>(id), nullptr, nullptr);
    setDefaultFont(hwnd);
    return hwnd;
}

void fillCombo(HWND combo, const std::vector<std::wstring>& items, int selectedIndex = 0) {
    for (const auto& item : items) {
        SendMessageW(combo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(item.c_str()));
    }
    SendMessageW(combo, CB_SETCURSEL, selectedIndex, 0);
}

void setComboText(HWND combo, const std::wstring& text) {
    int count = static_cast<int>(SendMessageW(combo, CB_GETCOUNT, 0, 0));
    for (int i = 0; i < count; ++i) {
        int length = static_cast<int>(SendMessageW(combo, CB_GETLBTEXTLEN, i, 0));
        std::wstring item(length + 1, L'\0');
        SendMessageW(combo, CB_GETLBTEXT, i, reinterpret_cast<LPARAM>(item.data()));
        item.resize(length);
        if (item == text) {
            SendMessageW(combo, CB_SETCURSEL, i, 0);
            return;
        }
    }
}

std::wstring getComboText(HWND combo) {
    int index = static_cast<int>(SendMessageW(combo, CB_GETCURSEL, 0, 0));
    if (index == CB_ERR) return L"";
    int length = static_cast<int>(SendMessageW(combo, CB_GETLBTEXTLEN, index, 0));
    std::wstring text(length + 1, L'\0');
    SendMessageW(combo, CB_GETLBTEXT, index, reinterpret_cast<LPARAM>(text.data()));
    text.resize(length);
    return text;
}

void setListText(HWND listView, int row, int column, const std::wstring& text) {
    LVITEMW item = {};
    item.iSubItem = column;
    item.pszText = const_cast<LPWSTR>(text.c_str());
    SendMessageW(listView, LVM_SETITEMTEXTW, row, reinterpret_cast<LPARAM>(&item));
}

// ===================== 添加任务对话框 =====================

struct AddTaskDialogData {
    HINSTANCE instance = nullptr;
    HWND owner = nullptr;
    HWND hwnd = nullptr;
    HWND typeCombo = nullptr;
    HWND nameEdit = nullptr;
    HWND courseEdit = nullptr;
    HWND dateEdit = nullptr;
    HWND priorityCombo = nullptr;
    HWND completedCheck = nullptr;
    HWND descriptionEdit = nullptr;
    HWND extra1Label = nullptr;
    HWND extra1Edit = nullptr;
    HWND extra2Label = nullptr;
    HWND extra2Edit = nullptr;
    int id = 0;
    bool accepted = false;
    bool editMode = false;
    const Task* initialTask = nullptr;
    std::unique_ptr<Task> task;
};

void updateDialogExtraLabels(AddTaskDialogData* data) {
    std::wstring type = getComboText(data->typeCombo);
    if (type == L"作业") {
        SetWindowTextW(data->extra1Label, L"作业编号：");
        SetWindowTextW(data->extra2Label, L"提交平台：");
    } else if (type == L"考试") {
        SetWindowTextW(data->extra1Label, L"考试地点：");
        SetWindowTextW(data->extra2Label, L"考试范围：");
    } else if (type == L"实验") {
        SetWindowTextW(data->extra1Label, L"实验地点：");
        SetWindowTextW(data->extra2Label, L"报告要求：");
    } else {
        SetWindowTextW(data->extra1Label, L"小组成员：");
        SetWindowTextW(data->extra2Label, L"项目进度：");
    }
}

void fillDialogFromTask(AddTaskDialogData* data) {
    const Task* task = data->initialTask;
    if (task == nullptr) {
        return;
    }

    setComboText(data->typeCombo, toWide(task->getType()));
    SetWindowTextW(data->nameEdit, toWide(task->getName()).c_str());
    SetWindowTextW(data->courseEdit, toWide(task->getCourse()).c_str());
    SetWindowTextW(data->dateEdit, toWide(task->getDueDate()).c_str());
    setComboText(data->priorityCombo, toWide(task->getPriority()));
    SetWindowTextW(data->descriptionEdit, toWide(task->getDescription()).c_str());
    SendMessageW(data->completedCheck, BM_SETCHECK, task->isCompleted() ? BST_CHECKED : BST_UNCHECKED, 0);

    auto fields = task->getExtraFields();
    if (fields.size() > 0) {
        SetWindowTextW(data->extra1Edit, toWide(fields[0].second).c_str());
    }
    if (fields.size() > 1) {
        SetWindowTextW(data->extra2Edit, toWide(fields[1].second).c_str());
    }
    updateDialogExtraLabels(data);
}

bool acceptAddDialog(AddTaskDialogData* data) {
    std::wstring type = getComboText(data->typeCombo);
    std::wstring name = getWindowText(data->nameEdit);
    std::wstring course = getWindowText(data->courseEdit);
    std::wstring dueDate = getWindowText(data->dateEdit);
    std::wstring priority = getComboText(data->priorityCombo);
    std::wstring description = getWindowText(data->descriptionEdit);
    std::wstring extra1 = getWindowText(data->extra1Edit);
    std::wstring extra2 = getWindowText(data->extra2Edit);
    bool completed = SendMessageW(data->completedCheck, BM_GETCHECK, 0, 0) == BST_CHECKED;

    if (name.empty()) {
        MessageBoxW(data->hwnd, L"任务名称不能为空。", L"输入错误", MB_ICONWARNING);
        return false;
    }
    if (course.empty()) {
        MessageBoxW(data->hwnd, L"所属课程不能为空。", L"输入错误", MB_ICONWARNING);
        return false;
    }
    if (!isValidDate(toUtf8(dueDate))) {
        MessageBoxW(data->hwnd, L"截止日期格式不正确，请按 YYYY-MM-DD 输入。", L"输入错误", MB_ICONWARNING);
        return false;
    }

    std::string nameUtf8 = toUtf8(name);
    std::string courseUtf8 = toUtf8(course);
    std::string dueDateUtf8 = toUtf8(dueDate);
    std::string priorityUtf8 = toUtf8(priority);
    std::string descriptionUtf8 = toUtf8(description);
    std::string extra1Utf8 = toUtf8(extra1);
    std::string extra2Utf8 = toUtf8(extra2);

    if (type == L"作业") {
        data->task = std::make_unique<HomeworkTask>(data->id, nameUtf8, courseUtf8, dueDateUtf8, priorityUtf8, descriptionUtf8, completed, extra1Utf8, extra2Utf8);
    } else if (type == L"考试") {
        data->task = std::make_unique<ExamTask>(data->id, nameUtf8, courseUtf8, dueDateUtf8, priorityUtf8, descriptionUtf8, completed, extra1Utf8, extra2Utf8);
    } else if (type == L"实验") {
        data->task = std::make_unique<ExperimentTask>(data->id, nameUtf8, courseUtf8, dueDateUtf8, priorityUtf8, descriptionUtf8, completed, extra1Utf8, extra2Utf8);
    } else {
        data->task = std::make_unique<ProjectTask>(data->id, nameUtf8, courseUtf8, dueDateUtf8, priorityUtf8, descriptionUtf8, completed, extra1Utf8, extra2Utf8);
    }

    data->accepted = true;
    return true;
}

LRESULT CALLBACK addDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    auto* data = reinterpret_cast<AddTaskDialogData*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    switch (message) {
        case WM_NCCREATE: {
            auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
            data = reinterpret_cast<AddTaskDialogData*>(create->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(data));
            data->hwnd = hwnd;
            return TRUE;
        }
        case WM_CREATE: {
            // --- 基本信息分组 ---
            createLabel(hwnd, L"任务类型：", 24, 20, 80, 24);
            data->typeCombo = createCombo(hwnd, ID_DIALOG_TYPE, 120, 18, 240, 120);
            fillCombo(data->typeCombo, {L"作业", L"考试", L"实验", L"项目"});

            createLabel(hwnd, L"任务名称：", 24, 56, 80, 24);
            data->nameEdit = createEdit(hwnd, ID_DIALOG_NAME, 120, 54, 240, 24);

            createLabel(hwnd, L"所属课程：", 24, 92, 80, 24);
            data->courseEdit = createEdit(hwnd, ID_DIALOG_COURSE, 120, 90, 240, 24);

            createLabel(hwnd, L"截止日期：", 24, 128, 80, 24);
            data->dateEdit = createEdit(hwnd, ID_DIALOG_DATE, 120, 126, 240, 24);
            SetWindowTextW(data->dateEdit, L"2026-06-15");

            createLabel(hwnd, L"优先级：", 24, 164, 80, 24);
            data->priorityCombo = createCombo(hwnd, ID_DIALOG_PRIORITY, 120, 162, 240, 120);
            fillCombo(data->priorityCombo, {L"高", L"中", L"低"});

            data->completedCheck = CreateWindowW(L"BUTTON", L"已完成", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                                                 120, 196, 100, 24, hwnd, reinterpret_cast<HMENU>(ID_DIALOG_COMPLETED), nullptr, nullptr);
            setDefaultFont(data->completedCheck);

            createLabel(hwnd, L"任务描述：", 24, 230, 80, 24);
            data->descriptionEdit = createEdit(hwnd, ID_DIALOG_DESCRIPTION, 120, 228, 240, 60, ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL);

            // --- 专属信息分组 ---
            data->extra1Label = createLabel(hwnd, L"专属字段1：", 24, 302, 80, 24);
            data->extra1Edit = createEdit(hwnd, ID_DIALOG_EXTRA1, 120, 300, 240, 24);

            data->extra2Label = createLabel(hwnd, L"专属字段2：", 24, 338, 80, 24);
            data->extra2Edit = createEdit(hwnd, ID_DIALOG_EXTRA2, 120, 336, 240, 24);

            // --- 按钮 ---
            HWND okButton = createButton(hwnd, ID_DIALOG_OK, L"确定");
            MoveWindow(okButton, 120, 378, 90, 30, TRUE);
            HWND cancelButton = createButton(hwnd, ID_DIALOG_CANCEL, L"取消");
            MoveWindow(cancelButton, 230, 378, 90, 30, TRUE);

            updateDialogExtraLabels(data);
            fillDialogFromTask(data);
            return 0;
        }
        case WM_COMMAND: {
            int id = LOWORD(wParam);
            int code = HIWORD(wParam);
            if (id == ID_DIALOG_TYPE && code == CBN_SELCHANGE) {
                updateDialogExtraLabels(data);
            } else if (id == ID_DIALOG_OK) {
                if (acceptAddDialog(data)) {
                    DestroyWindow(hwnd);
                }
            } else if (id == ID_DIALOG_CANCEL) {
                DestroyWindow(hwnd);
            }
            return 0;
        }
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        default:
            return DefWindowProcW(hwnd, message, wParam, lParam);
    }
}

std::unique_ptr<Task> showTaskDialog(HINSTANCE instance, HWND owner, int id, const Task* initialTask = nullptr) {
    const wchar_t* className = L"StudentTaskAddDialog";
    static bool registered = false;
    if (!registered) {
        WNDCLASSW wc = {};
        wc.lpfnWndProc = addDialogProc;
        wc.hInstance = instance;
        wc.lpszClassName = className;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        RegisterClassW(&wc);
        registered = true;
    }

    RECT ownerRect{};
    GetWindowRect(owner, &ownerRect);
    int x = ownerRect.left + 80;
    int y = ownerRect.top + 60;

    AddTaskDialogData data;
    data.instance = instance;
    data.owner = owner;
    data.id = id;
    data.initialTask = initialTask;
    data.editMode = initialTask != nullptr;

    EnableWindow(owner, FALSE);
    HWND dialog = CreateWindowExW(WS_EX_DLGMODALFRAME, className,
                                  data.editMode ? L"编辑学习任务" : L"添加学习任务",
                                  WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
                                  x, y, 400, 450, owner, nullptr, instance, &data);
    setDefaultFont(dialog);

    MSG msg;
    while (IsWindow(dialog) && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (!IsDialogMessageW(dialog, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    EnableWindow(owner, TRUE);
    SetActiveWindow(owner);

    if (data.accepted) {
        return std::move(data.task);
    }
    return nullptr;
}
} // anonymous namespace

// ===================== WinMainWindow 构造 =====================

WinMainWindow::WinMainWindow(HINSTANCE instance) : instance(instance) {
    headerFont = createFontObject(20, FW_BOLD);
    uiFont = createFontObject(10, FW_NORMAL);
    buttonFont = createFontObject(10, FW_SEMIBOLD);
    appUiFont = uiFont;

    backgroundBrush = CreateSolidBrush(RGB(241, 245, 249));
    headerBrush = CreateSolidBrush(RGB(30, 64, 175));
    panelBrush = CreateSolidBrush(RGB(255, 255, 255));
    altRowBrush = CreateSolidBrush(RGB(248, 250, 252));
    completedRowBrush = CreateSolidBrush(RGB(236, 253, 245));
}

// ===================== 窗口注册与创建 =====================

bool WinMainWindow::create(int showCommand) {
    const wchar_t* className = L"StudentTaskWinMainWindow";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WinMainWindow::windowProc;
    wc.hInstance = instance;
    wc.lpszClassName = className;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hbrBackground = backgroundBrush;

    if (!RegisterClassW(&wc)) {
        MessageBoxW(nullptr, L"窗口类注册失败。", L"错误", MB_ICONERROR);
        return false;
    }

    window = CreateWindowExW(0, className, L"学生学习任务与课程计划管理系统",
                             WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT, 1100, 700,
                             nullptr, nullptr, instance, this);

    if (window == nullptr) {
        MessageBoxW(nullptr, L"主窗口创建失败。", L"错误", MB_ICONERROR);
        return false;
    }

    ShowWindow(window, showCommand);
    UpdateWindow(window);
    return true;
}

// ===================== 窗口过程 =====================

LRESULT CALLBACK WinMainWindow::windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    WinMainWindow* self = nullptr;

    if (message == WM_NCCREATE) {
        auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
        self = reinterpret_cast<WinMainWindow*>(create->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        self->window = hwnd;
    } else {
        self = reinterpret_cast<WinMainWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    if (self != nullptr) {
        return self->handleMessage(message, wParam, lParam);
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

LRESULT WinMainWindow::handleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE:
            createControls();
            try {
                manager.loadFromFile();
            } catch (const std::exception& e) {
                MessageBoxW(window, toWide(std::string("读取历史任务失败：") + e.what()).c_str(), L"读取失败", MB_ICONWARNING);
            }
            refreshList();
            updateStatusBar();
            return 0;

        case WM_SIZE:
            resizeControls(LOWORD(lParam), HIWORD(lParam));
            return 0;

        case WM_NOTIFY: {
            auto* nmh = reinterpret_cast<NMHDR*>(lParam);
            if (nmh->hwndFrom == listView && nmh->code == NM_CUSTOMDRAW) {
                return drawListViewItem(reinterpret_cast<LPNMLVCUSTOMDRAW>(lParam));
            }
            if (nmh->hwndFrom == listView && nmh->code == NM_DBLCLK) {
                showTaskDetail();
                return 0;
            }
            if (nmh->hwndFrom == listView && nmh->code == LVN_ITEMCHANGED) {
                updateStatusBar();
            }
            return 0;
        }

        case WM_ERASEBKGND: {
            RECT rect{};
            GetClientRect(window, &rect);
            drawWindowBackground(reinterpret_cast<HDC>(wParam), rect);
            return 1;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps{};
            HDC hdc = BeginPaint(window, &ps);
            RECT rect{};
            GetClientRect(window, &rect);
            drawWindowBackground(hdc, rect);
            EndPaint(window, &ps);
            return 0;
        }

        case WM_DRAWITEM:
            drawOwnerButton(*reinterpret_cast<DRAWITEMSTRUCT*>(lParam));
            return TRUE;

        case WM_COMMAND: {
            int id = LOWORD(wParam);
            int code = HIWORD(wParam);
            if (id == ID_COURSE_FILTER && code == EN_CHANGE) {
                refreshList();
                updateStatusBar();
            } else if (id == ID_TYPE_FILTER && code == CBN_SELCHANGE) {
                refreshList();
                updateStatusBar();
            } else if (id == ID_REFRESH) {
                refreshList();
                updateStatusBar();
            } else if (id == ID_ADD) {
                addTask();
                updateStatusBar();
            } else if (id == ID_DETAIL) {
                editTask();
            } else if (id == ID_DELETE) {
                deleteTask();
                updateStatusBar();
            } else if (id == ID_TOGGLE) {
                toggleTaskStatus();
                updateStatusBar();
            } else if (id == ID_SORT) {
                sortTasks();
            } else if (id == ID_STATISTICS) {
                showStatistics();
            } else if (id == ID_SAVE) {
                saveTasks();
            } else if (id == ID_LOAD) {
                loadTasks();
                updateStatusBar();
            }
            return 0;
        }

        case WM_CTLCOLORSTATIC: {
            HDC hdc = reinterpret_cast<HDC>(wParam);
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(51, 65, 85));
            return reinterpret_cast<LRESULT>(panelBrush);
        }

        case WM_DESTROY:
            try {
                manager.saveToFile();
            } catch (...) {}
            if (backgroundBrush) DeleteObject(backgroundBrush);
            if (headerBrush) DeleteObject(headerBrush);
            if (panelBrush) DeleteObject(panelBrush);
            if (altRowBrush) DeleteObject(altRowBrush);
            if (completedRowBrush) DeleteObject(completedRowBrush);
            if (headerFont) DeleteObject(headerFont);
            if (uiFont) DeleteObject(uiFont);
            if (buttonFont) DeleteObject(buttonFont);
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProcW(window, message, wParam, lParam);
    }
}

// ===================== 控件创建 =====================

void WinMainWindow::createControls() {
    // --- 顶部筛选栏 ---
    createLabel(window, L"课程筛选：", 30, FILTER_TOP + 5, 80, 24);
    courseFilterEdit = createEdit(window, ID_COURSE_FILTER, 112, FILTER_TOP, 230, FILTER_HEIGHT);

    createLabel(window, L"任务类型：", 370, FILTER_TOP + 5, 80, 24);
    typeFilterCombo = createCombo(window, ID_TYPE_FILTER, 452, FILTER_TOP, 150, 200);
    fillCombo(typeFilterCombo, {L"全部", L"作业", L"考试", L"实验", L"项目"});

    refreshButton = createButton(window, ID_REFRESH, L"刷新", true);

    // --- 任务列表 ---
    listView = CreateWindowExW(WS_EX_STATICEDGE, WC_LISTVIEWW, L"",
                               WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
                               0, 0, 100, 100, window, reinterpret_cast<HMENU>(ID_TASK_LIST), instance, nullptr);
    setDefaultFont(listView);
    SendMessageW(listView, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_INFOTIP);
    SendMessageW(listView, LVM_SETBKCOLOR, 0, RGB(255, 255, 255));
    SendMessageW(listView, LVM_SETTEXTBKCOLOR, 0, CLR_NONE);
    SendMessageW(listView, LVM_SETTEXTCOLOR, 0, RGB(30, 41, 59));
    setupListColumns();

    // --- 底部操作按钮 ---
    addButton = createButton(window, ID_ADD, L"＋ 添加任务", true);
    editButton = createButton(window, ID_DETAIL, L"编辑任务", true);
    deleteButton = createButton(window, ID_DELETE, L"删除任务", true);
    toggleButton = createButton(window, ID_TOGGLE, L"切换完成", true);
    sortButton = createButton(window, ID_SORT, L"按日期排序", true);
    statisticsButton = createButton(window, ID_STATISTICS, L"统计", true);
    saveButton = createButton(window, ID_SAVE, L"保存", true);
    loadButton = createButton(window, ID_LOAD, L"重新读取", true);

    // --- 状态栏 ---
    statusBar = CreateWindowExW(0, STATUSCLASSNAMEW, L"",
                                 WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
                                 0, 0, 0, 0, window, nullptr, instance, nullptr);
    setDefaultFont(statusBar);
    int statusParts[] = { 240, 430, -1 };
    SendMessageW(statusBar, SB_SETPARTS, 3, reinterpret_cast<LPARAM>(statusParts));
}

void WinMainWindow::setupListColumns() {
    const std::vector<std::pair<std::wstring, int>> columns = {
        {L"编号", 65},
        {L"类型", 75},
        {L"课程", 140},
        {L"任务名称", 240},
        {L"截止日期", 120},
        {L"优先级", 65},
        {L"状态", 80}
    };

    for (size_t i = 0; i < columns.size(); ++i) {
        LVCOLUMNW column = {};
        column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM | LVCF_FMT;
        column.fmt = (i == 0) ? LVCFMT_CENTER : LVCFMT_LEFT;
        column.pszText = const_cast<LPWSTR>(columns[i].first.c_str());
        column.cx = columns[i].second;
        column.iSubItem = static_cast<int>(i);
        SendMessageW(listView, LVM_INSERTCOLUMNW, static_cast<WPARAM>(i), reinterpret_cast<LPARAM>(&column));
    }
}

// ===================== 布局 =====================

void WinMainWindow::resizeControls(int width, int height) {
    if (listView == nullptr) return;

    SendMessageW(statusBar, WM_SIZE, 0, 0);

    MoveWindow(refreshButton, width - 112, FILTER_TOP - 1, 82, 32, TRUE);

    int btnAreaH = BTN_H * 2 + BTN_GAP * 3 + 18;
    int listHeight = std::max(150, height - LIST_TOP - btnAreaH - STATUSBAR_H - 8);
    MoveWindow(listView, 22, LIST_TOP, width - 44, listHeight, TRUE);

    int btnTop1 = LIST_TOP + listHeight + BTN_GAP + 8;
    int btnTop2 = btnTop1 + BTN_H + BTN_GAP;

    int x1 = 22;
    MoveWindow(addButton, x1, btnTop1, BTN_W2, BTN_H, TRUE); x1 += BTN_W2 + BTN_GAP;
    MoveWindow(editButton, x1, btnTop1, BTN_W1, BTN_H, TRUE); x1 += BTN_W1 + BTN_GAP;
    MoveWindow(toggleButton, x1, btnTop1, BTN_W1, BTN_H, TRUE); x1 += BTN_W1 + BTN_GAP;
    MoveWindow(deleteButton, x1, btnTop1, BTN_W1, BTN_H, TRUE);

    int x2 = 22;
    MoveWindow(sortButton, x2, btnTop2, BTN_W1, BTN_H, TRUE); x2 += BTN_W1 + BTN_GAP;
    MoveWindow(statisticsButton, x2, btnTop2, BTN_W1, BTN_H, TRUE); x2 += BTN_W1 + BTN_GAP;
    MoveWindow(saveButton, x2, btnTop2, BTN_W1, BTN_H, TRUE); x2 += BTN_W1 + BTN_GAP;
    MoveWindow(loadButton, x2, btnTop2, BTN_W1, BTN_H, TRUE);

    InvalidateRect(window, nullptr, TRUE);
}

// ===================== 状态栏 =====================

void WinMainWindow::updateStatusBar() {
    const auto& tasks = manager.getTasks();
    int total = static_cast<int>(tasks.size());
    int completed = 0;
    for (const auto& t : tasks) {
        if (t->isCompleted()) ++completed;
    }

    wchar_t buf1[128], buf2[128], buf3[128];
    swprintf(buf1, 128, L"  总计 %d", total);
    swprintf(buf2, 128, L"已完成 %d", completed);
    swprintf(buf3, 128, L"待完成 %d    ", total - completed);

    SendMessageW(statusBar, SB_SETTEXTW, 0, reinterpret_cast<LPARAM>(buf1));
    SendMessageW(statusBar, SB_SETTEXTW, 1, reinterpret_cast<LPARAM>(buf2));
    SendMessageW(statusBar, SB_SETTEXTW, 2, reinterpret_cast<LPARAM>(buf3));
}

// ===================== 自定义绘制 ListView =====================

LRESULT CALLBACK WinMainWindow::listViewProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam,
                                             UINT_PTR /*subclassId*/, DWORD_PTR refData) {
    auto* self = reinterpret_cast<WinMainWindow*>(refData);
    if (message == WM_NOTIFY) {
        auto* nmh = reinterpret_cast<NMHDR*>(lParam);
        if (nmh->code == NM_CUSTOMDRAW) {
            auto* lvcd = reinterpret_cast<LPNMLVCUSTOMDRAW>(lParam);
            return self->drawListViewItem(lvcd);
        }
    }
    return DefSubclassProc(hwnd, message, wParam, lParam);
}

LRESULT WinMainWindow::drawListViewItem(LPNMLVCUSTOMDRAW lvcd) {
    switch (lvcd->nmcd.dwDrawStage) {
        case CDDS_PREPAINT:
            return CDRF_NOTIFYITEMDRAW;

        case CDDS_ITEMPREPAINT: {
            int row = static_cast<int>(lvcd->nmcd.dwItemSpec);
            const auto& tasks = manager.getTasks();
            if (row >= 0) {
                LVITEMW item = {};
                item.mask = LVIF_PARAM;
                item.iItem = row;
                SendMessageW(listView, LVM_GETITEMW, 0, reinterpret_cast<LPARAM>(&item));
                const Task* task = findTaskById(static_cast<int>(item.lParam));
                if (task == nullptr) return CDRF_DODEFAULT;

                if (task->isCompleted()) {
                    lvcd->clrTextBk = RGB(236, 253, 245);
                    lvcd->clrText = RGB(100, 116, 139);
                } else if (row % 2 == 0) {
                    lvcd->clrTextBk = RGB(248, 250, 252);
                    lvcd->clrText = RGB(30, 41, 59);
                } else {
                    lvcd->clrTextBk = RGB(255, 255, 255);
                    lvcd->clrText = RGB(30, 41, 59);
                }

                if (!task->isCompleted() && task->getPriority() == "高") {
                    lvcd->clrText = RGB(220, 38, 38);
                } else if (!task->isCompleted() && task->getPriority() == "中") {
                    lvcd->clrText = RGB(217, 119, 6);
                }
            }
            return CDRF_NEWFONT;
        }
        default:
            return CDRF_DODEFAULT;
    }
}

// ===================== 窗口与按钮绘制 =====================

void WinMainWindow::drawWindowBackground(HDC hdc, const RECT& clientRect) {
    FillRect(hdc, &clientRect, backgroundBrush);

    RECT headerRect{0, 0, clientRect.right, HEADER_H};
    FillRect(hdc, &headerRect, headerBrush);

    RECT panelRect{20, 92, clientRect.right - 20, 144};
    fillRoundRect(hdc, panelRect, 18, RGB(255, 255, 255));

    SetBkMode(hdc, TRANSPARENT);
    HGDIOBJ oldFont = SelectObject(hdc, headerFont);
    SetTextColor(hdc, RGB(255, 255, 255));
    RECT titleRect{28, 16, clientRect.right - 28, 48};
    DrawTextW(hdc, L"学生学习任务管理", -1, &titleRect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

    SelectObject(hdc, uiFont);
    SetTextColor(hdc, RGB(219, 234, 254));
    RECT subtitleRect{30, 50, clientRect.right - 28, 74};
    DrawTextW(hdc, L"课程作业 / 考试复习 / 实验项目统一管理", -1, &subtitleRect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
    SelectObject(hdc, oldFont);
}

void WinMainWindow::drawOwnerButton(const DRAWITEMSTRUCT& item) {
    if (item.CtlType != ODT_BUTTON) return;

    bool pressed = (item.itemState & ODS_SELECTED) != 0;
    bool disabled = (item.itemState & ODS_DISABLED) != 0;
    int id = static_cast<int>(item.CtlID);

    COLORREF bg = RGB(248, 250, 252);
    COLORREF text = RGB(30, 41, 59);
    if (id == ID_ADD) {
        bg = themeColor;
        text = RGB(255, 255, 255);
    } else if (id == ID_DELETE) {
        bg = RGB(220, 38, 38);
        text = RGB(255, 255, 255);
    } else if (id == ID_REFRESH) {
        bg = RGB(14, 165, 233);
        text = RGB(255, 255, 255);
    }

    if (pressed) {
        bg = blendColor(bg, -24);
    }
    if (disabled) {
        bg = RGB(226, 232, 240);
        text = RGB(148, 163, 184);
    }

    RECT rect = item.rcItem;
    InflateRect(&rect, -1, -1);
    fillRoundRect(item.hDC, rect, 16, bg);

    wchar_t label[128] = {};
    GetWindowTextW(item.hwndItem, label, 128);

    SetBkMode(item.hDC, TRANSPARENT);
    SetTextColor(item.hDC, text);
    HGDIOBJ oldFont = SelectObject(item.hDC, buttonFont ? buttonFont : uiFont);
    DrawTextW(item.hDC, label, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    SelectObject(item.hDC, oldFont);
}

// ===================== 数据操作 =====================

void WinMainWindow::refreshList() {
    std::wstring courseFilter = getWindowText(courseFilterEdit);
    std::wstring typeFilter = getComboText(typeFilterCombo);

    SendMessageW(listView, LVM_DELETEALLITEMS, 0, 0);

    // 先排序再显示
    manager.sortTasksByDueDate();

    int row = 0;
    for (const auto& taskPtr : manager.getTasks()) {
        const Task& task = *taskPtr;
        std::wstring course = toWide(task.getCourse());
        std::wstring type = toWide(task.getType());

        if (!courseFilter.empty() && course.find(courseFilter) == std::wstring::npos) {
            continue;
        }
        if (typeFilter != L"全部" && type != typeFilter) {
            continue;
        }

        std::wstring idText = std::to_wstring(task.getId());
        LVITEMW item = {};
        item.mask = LVIF_TEXT | LVIF_PARAM;
        item.iItem = row;
        item.iSubItem = 0;
        item.pszText = const_cast<LPWSTR>(idText.c_str());
        item.lParam = task.getId();
        SendMessageW(listView, LVM_INSERTITEMW, 0, reinterpret_cast<LPARAM>(&item));

        setListText(listView, row, 1, type);
        setListText(listView, row, 2, course);
        setListText(listView, row, 3, toWide(task.getName()));
        setListText(listView, row, 4, toWide(task.getDueDate()));
        setListText(listView, row, 5, toWide(task.getPriority()));
        setListText(listView, row, 6, task.isCompleted() ? L"已完成" : L"未完成");
        ++row;
    }
}

int WinMainWindow::selectedTaskId() const {
    int row = static_cast<int>(SendMessageW(listView, LVM_GETNEXTITEM, static_cast<WPARAM>(-1), LVNI_SELECTED));
    if (row == -1) return -1;

    wchar_t buffer[64] = {};
    LVITEMW item = {};
    item.iSubItem = 0;
    item.pszText = buffer;
    item.cchTextMax = 64;
    SendMessageW(listView, LVM_GETITEMTEXTW, row, reinterpret_cast<LPARAM>(&item));
    return _wtoi(buffer);
}

const Task* WinMainWindow::findTaskById(int id) const {
    for (const auto& task : manager.getTasks()) {
        if (task->getId() == id) return task.get();
    }
    return nullptr;
}

std::wstring WinMainWindow::buildTaskDetailText(const Task& task) const {
    std::wstring text;
    text += L"编号：" + std::to_wstring(task.getId()) + L"\n";
    text += L"类型：" + toWide(task.getType()) + L"\n";
    text += L"任务名称：" + toWide(task.getName()) + L"\n";
    text += L"所属课程：" + toWide(task.getCourse()) + L"\n";
    text += L"截止日期：" + toWide(task.getDueDate()) + L"\n";
    text += L"优先级：" + toWide(task.getPriority()) + L"\n";
    text += std::wstring(L"状态：") + (task.isCompleted() ? L"已完成" : L"未完成") + L"\n";
    text += L"任务描述：" + toWide(task.getDescription()) + L"\n";

    for (const auto& field : task.getExtraFields()) {
        text += toWide(field.first) + L"：" + toWide(field.second) + L"\n";
    }
    return text;
}

// ===================== 功能操作 =====================

void WinMainWindow::addTask() {
    auto task = showTaskDialog(instance, window, manager.generateTaskId());
    if (task) {
        manager.addTask(std::move(task));
        refreshList();
    }
}

void WinMainWindow::editTask() {
    int id = selectedTaskId();
    const Task* existing = findTaskById(id);
    if (existing == nullptr) {
        MessageBoxW(window, L"请先在列表中选择一个任务。", L"提示", MB_ICONINFORMATION);
        return;
    }

    auto editedTask = showTaskDialog(instance, window, id, existing);
    if (editedTask) {
        if (manager.replaceTaskById(id, std::move(editedTask))) {
            refreshList();
            updateStatusBar();
        } else {
            MessageBoxW(window, L"编辑任务失败。", L"错误", MB_ICONWARNING);
        }
    }
}

void WinMainWindow::showTaskDetail() {
    int id = selectedTaskId();
    const Task* task = findTaskById(id);
    if (task == nullptr) {
        MessageBoxW(window, L"请先在列表中选择一个任务。", L"提示", MB_ICONINFORMATION);
        return;
    }
    MessageBoxW(window, buildTaskDetailText(*task).c_str(), L"任务详情", MB_ICONINFORMATION);
}

void WinMainWindow::deleteTask() {
    int id = selectedTaskId();
    if (id == -1) {
        MessageBoxW(window, L"请先在列表中选择一个任务。", L"提示", MB_ICONINFORMATION);
        return;
    }
    if (MessageBoxW(window, L"确认删除选中的任务吗？", L"确认删除", MB_YESNO | MB_ICONQUESTION) == IDYES) {
        manager.removeTaskById(id);
        refreshList();
    }
}

void WinMainWindow::toggleTaskStatus() {
    int id = selectedTaskId();
    const Task* task = findTaskById(id);
    if (task == nullptr) {
        MessageBoxW(window, L"请先在列表中选择一个任务。", L"提示", MB_ICONINFORMATION);
        return;
    }
    manager.setTaskCompleted(id, !task->isCompleted());
    refreshList();
}

void WinMainWindow::sortTasks() {
    manager.sortTasksByDueDate();
    refreshList();
}

void WinMainWindow::showStatistics() {
    MessageBoxW(window, toWide(manager.buildStatisticsText()).c_str(), L"任务统计", MB_ICONINFORMATION);
}

void WinMainWindow::saveTasks() {
    try {
        manager.saveToFile();
        MessageBoxW(window, L"任务已保存到 tasks.txt。", L"保存成功", MB_ICONINFORMATION);
    } catch (const std::exception& e) {
        MessageBoxW(window, toWide(std::string("保存失败：") + e.what()).c_str(), L"保存失败", MB_ICONWARNING);
    }
}

void WinMainWindow::loadTasks() {
    try {
        manager.loadFromFile();
        refreshList();
        MessageBoxW(window, L"已重新读取 tasks.txt。", L"读取成功", MB_ICONINFORMATION);
    } catch (const std::exception& e) {
        MessageBoxW(window, toWide(std::string("读取失败：") + e.what()).c_str(), L"读取失败", MB_ICONWARNING);
    }
}
