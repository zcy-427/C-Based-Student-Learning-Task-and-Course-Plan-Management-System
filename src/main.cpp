#include "TaskManager.h"
#include "Utils.h"

#include <exception>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

/**
 * @brief 显示系统主菜单。
 *
 * 用户通过输入菜单编号选择对应功能，主循环会根据编号调用 TaskManager 的成员函数。
 */
void showMenu() {
    std::cout << "\n========== 学生学习任务与课程计划管理系统 ==========" << '\n'
              << "1. 添加学习任务" << '\n'
              << "2. 查看所有任务" << '\n'
              << "3. 按课程名称查询任务" << '\n'
              << "4. 按任务类型筛选任务" << '\n'
              << "5. 修改任务完成状态" << '\n'
              << "6. 删除任务" << '\n'
              << "7. 按截止日期排序" << '\n'
              << "8. 查看任务统计" << '\n'
              << "9. 保存任务到文件" << '\n'
              << "10. 重新从文件读取任务" << '\n'
              << "0. 保存并退出" << '\n'
              << "==================================================" << '\n';
}

/**
 * @brief 程序入口函数。
 *
 * 程序启动时先读取历史任务数据，然后进入菜单循环；
 * 用户选择退出时，自动把任务数据保存到 tasks.txt。
 */
int main() {
#ifdef _WIN32
    // Windows 控制台默认编码可能不是 UTF-8，这里设置后中文显示更稳定。
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    TaskManager manager;
    try {
        // 启动程序时自动读取历史数据。
        manager.loadFromFile();
        std::cout << "历史任务数据读取完成。\n";
    } catch (const std::exception& e) {
        std::cout << "读取历史任务失败：" << e.what() << '\n';
    }

    // 主菜单循环：每次处理一个功能，处理完成后暂停，方便用户查看结果。
    while (true) {
        showMenu();
        int choice = inputInt("请输入功能选项：", 0, 10);

        try {
            switch (choice) {
                case 1:
                    manager.addTask();
                    break;
                case 2:
                    manager.showAllTasks();
                    break;
                case 3:
                    manager.searchByCourse();
                    break;
                case 4:
                    manager.filterByType();
                    break;
                case 5:
                    manager.changeTaskStatus();
                    break;
                case 6:
                    manager.deleteTask();
                    break;
                case 7:
                    manager.sortByDueDate();
                    break;
                case 8:
                    manager.showStatistics();
                    break;
                case 9:
                    manager.saveToFile();
                    std::cout << "保存成功！\n";
                    break;
                case 10:
                    // 重新创建管理器对象并读取文件，等于放弃当前未保存修改并恢复文件内容。
                    manager = TaskManager();
                    manager.loadFromFile();
                    std::cout << "重新读取完成！\n";
                    break;
                case 0:
                    // 退出前自动保存，避免用户忘记手动保存。
                    manager.saveToFile();
                    std::cout << "数据已保存，程序退出。\n";
                    return 0;
                default:
                    break;
            }
        } catch (const std::exception& e) {
            std::cout << "发生错误：" << e.what() << '\n';
        }

        pauseScreen();
    }
}
