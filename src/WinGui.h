#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "TaskManager.h"

#include <windows.h>
#include <commctrl.h>

#include <string>

class Task;

class WinMainWindow {
public:
    explicit WinMainWindow(HINSTANCE instance);

    bool create(int showCommand);

private:
    HINSTANCE instance;
    HWND window = nullptr;
    HWND listView = nullptr;
    HWND statusBar = nullptr;
    HWND courseFilterEdit = nullptr;
    HWND typeFilterCombo = nullptr;
    HWND refreshButton = nullptr;
    HWND addButton = nullptr;
    HWND editButton = nullptr;
    HWND deleteButton = nullptr;
    HWND toggleButton = nullptr;
    HWND sortButton = nullptr;
    HWND statisticsButton = nullptr;
    HWND saveButton = nullptr;
    HWND loadButton = nullptr;
    TaskManager manager;

    // 界面绘制相关
    HFONT headerFont = nullptr;        // 顶部标题字体
    HFONT uiFont = nullptr;            // 普通界面字体
    HFONT buttonFont = nullptr;        // 按钮字体
    HBRUSH backgroundBrush = nullptr;  // 主窗口背景
    HBRUSH headerBrush = nullptr;      // 顶部标题背景
    HBRUSH panelBrush = nullptr;       // 工具栏/卡片背景
    HBRUSH altRowBrush = nullptr;      // 交替行背景
    HBRUSH completedRowBrush = nullptr; // 已完成任务背景
    COLORREF themeColor = RGB(37, 99, 235);  // 主题蓝色

    static LRESULT CALLBACK windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT handleMessage(UINT message, WPARAM wParam, LPARAM lParam);

    void createControls();
    void setupListColumns();
    void resizeControls(int width, int height);
    void refreshList();

    // 自定义绘制 ListView
    static LRESULT CALLBACK listViewProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam,
                                         UINT_PTR subclassId, DWORD_PTR refData);
    LRESULT handleListViewMessage(UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT drawListViewItem(LPNMLVCUSTOMDRAW lvcd);
    WNDPROC oldListViewProc = nullptr;

    int selectedTaskId() const;
    const Task* findTaskById(int id) const;
    std::wstring buildTaskDetailText(const Task& task) const;

    void addTask();
    void editTask();
    void showTaskDetail();
    void deleteTask();
    void toggleTaskStatus();
    void sortTasks();
    void showStatistics();
    void saveTasks();
    void loadTasks();

    void updateStatusBar();
    void drawWindowBackground(HDC hdc, const RECT& clientRect);
    void drawOwnerButton(const DRAWITEMSTRUCT& item);
};
