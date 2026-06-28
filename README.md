# 学生学习任务与课程计划管理系统

这是一个基于 C++ 的学生学习任务管理系统，用于管理课程作业、考试复习、实验任务和课程项目任务。

当前项目只保留 **Win32 GUI 图形界面版本**：

- 使用 Windows 原生 Win32 API 编写界面
- 不依赖 Qt
- 不依赖 EasyX
- 不需要额外 GUI 框架

---

## 1. 功能简介

系统支持以下功能：

1. 添加学习任务
2. 查看所有任务
3. 按课程名称筛选任务
4. 按任务类型筛选任务
5. 查看任务详情
6. 编辑任务
7. 修改任务完成状态
8. 删除任务
9. 按截止日期排序
10. 查看任务统计信息
11. 保存任务到文件
12. 从文件重新读取历史任务数据

任务类型包括：

- 作业任务
- 考试任务
- 实验任务
- 课程项目任务

---

## 2. 项目文件说明

| 文件 | 作用 |
|---|---|
| `Task.h` / `Task.cpp` | 定义任务基类 `Task`，以及作业、考试、实验、项目等派生类 |
| `TaskManager.h` / `TaskManager.cpp` | 任务管理类，负责添加、编辑、删除、排序、统计、文件保存和读取 |
| `Utils.h` / `Utils.cpp` | 工具函数，例如日期校验等 |
| `win_gui_main.cpp` | Win32 GUI 程序入口，包含 `wWinMain` |
| `WinGui.h` / `WinGui.cpp` | Win32 图形界面窗口、控件和按钮事件逻辑 |
| `app.manifest` | Windows GUI 程序清单文件，用于适配系统界面显示 |
| `CMakeLists.txt` | CMake 构建配置文件 |
| `README.md` | 项目说明和运行方法 |
| `tasks.txt` | 程序运行时保存任务数据的文件，自动生成 |

说明：

- `main.cpp` 是之前控制台版本入口，现在不再作为项目运行入口。
- 当前推荐运行的是 `task_manager_gui.exe`。

---

## 3. 运行环境

推荐环境：

- Windows 10 / Windows 11
- C++17 编译器
- MinGW-w64 或 Visual Studio 2022
- CMake 3.16 及以上

GUI 使用 **Win32 API**，因此不需要安装 Qt。

---

## 4. 使用 CMake 编译运行

### 4.1 使用 MinGW 编译

在项目目录下执行：

```bash
cmake -S . -B build_mingw -G "MinGW Makefiles"
cmake --build build_mingw
```

编译完成后，可执行文件通常位于：

```text
build_mingw/task_manager_gui.exe
```

运行：

```bash
./build_mingw/task_manager_gui.exe
```

也可以直接双击：

```text
build_mingw/task_manager_gui.exe
```

---

### 4.2 使用 Visual Studio 2022 编译

在项目目录下执行：

```bash
cmake -B build_utf8_dash -G "Visual Studio 17 2022" -DCMAKE_CXX_FLAGS=-utf-8
cmake --build build_utf8_dash
```

注意：

- 源文件中包含中文字符，使用 MSVC 编译时建议添加 `-utf-8`。
- 如果在 Git Bash 中执行，不要写成 `/utf-8`，应写成 `-utf-8`。

编译完成后，可执行文件通常位于：

```text
build_utf8_dash/Debug/task_manager_gui.exe
```

运行：

```bash
./build_utf8_dash/Debug/task_manager_gui.exe
```

也可以在资源管理器中双击 `task_manager_gui.exe`。

---

## 5. 不使用 CMake 的手动编译方式

如果只想用 `g++` 直接编译 GUI 版本，可以使用：

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -static -mwindows -municode win_gui_main.cpp WinGui.cpp Task.cpp TaskManager.cpp Utils.cpp -o task_manager_gui -lcomctl32
```

运行：

```bash
./task_manager_gui
```

或直接双击：

```text
task_manager_gui.exe
```

参数说明：

| 参数 | 作用 |
|---|---|
| `-std=c++17` | 使用 C++17 标准 |
| `-static` | 尽量静态链接运行库，减少缺少 DLL 的问题 |
| `-mwindows` | 生成 Windows GUI 程序，不显示控制台黑窗口 |
| `-municode` | 使用 Unicode 版本入口 `wWinMain`，避免 MinGW 找不到 `WinMain` |
| `-lcomctl32` | 链接 Windows 通用控件库，GUI 中的 ListView、状态栏等控件需要它 |

---

## 6. GUI 程序使用方法

运行 `task_manager_gui.exe` 后，会打开图形界面。

界面中主要包括：

- 课程筛选输入框
- 任务类型筛选下拉框
- 任务列表表格
- 添加任务按钮
- 查看详情按钮
- 编辑任务按钮
- 删除任务按钮
- 切换完成状态按钮
- 按日期排序按钮
- 统计按钮
- 保存按钮
- 重新读取按钮

常见操作：

1. 点击 **添加任务**，填写任务信息后确认。
2. 在表格中选中某一行，点击 **查看详情** 查看完整信息。
3. 选中任务后点击 **编辑任务** 修改任务内容。
4. 选中任务后点击 **删除任务** 删除该任务。
5. 选中任务后点击 **切换完成状态**，可在“未完成”和“已完成”之间切换。
6. 点击 **按日期排序**，任务会按截止日期从近到远排列。
7. 点击 **统计**，查看总任务数、已完成数量、未完成数量、高优先级数量和各课程任务数量。
8. 点击 **保存**，将当前任务保存到 `tasks.txt`。
9. 点击 **重新读取**，从 `tasks.txt` 重新加载任务。

---

## 7. 数据保存说明

任务数据保存在当前运行目录下的：

```text
tasks.txt
```

程序启动时会尝试读取该文件，退出或点击保存时会写入该文件。

保存格式为管道符分隔文本，例如：

```text
类型|编号|任务名称|课程|截止日期|优先级|完成状态|任务描述|专属字段1\t专属字段2
```

普通用户不需要手动修改 `tasks.txt`，通过程序操作即可。

---

## 8. 注意事项

1. 建议在项目根目录运行程序，这样 `tasks.txt` 会保存在项目目录中。
2. 日期格式必须为：

   ```text
   YYYY-MM-DD
   ```

   例如：

   ```text
   2026-06-15
   ```

3. GUI 版本仅适用于 Windows，因为它使用了 Win32 API。
4. 如果使用 Visual Studio / MSVC 编译，建议添加 `-utf-8`，避免中文乱码或编码警告。
5. 如果使用 Git Bash，不要把 UTF-8 编译参数写成 `/utf-8`，应写成 `-utf-8`。
6. 如果使用 MinGW 编译 GUI，必须保留 `-municode`，否则可能出现 `undefined reference to WinMain` 错误。

---

## 9. 推荐运行方式

推荐使用 CMake + MinGW：

```bash
cmake -S . -B build_mingw -G "MinGW Makefiles"
cmake --build build_mingw
./build_mingw/task_manager_gui.exe
```

如果不用 CMake，推荐直接使用：

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -static -mwindows -municode win_gui_main.cpp WinGui.cpp Task.cpp TaskManager.cpp Utils.cpp -o task_manager_gui -lcomctl32
./task_manager_gui
```
