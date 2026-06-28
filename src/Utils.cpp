#include "Utils.h"

#include <iostream>
#include <regex>
#include <sstream>

// 去除字符串首尾空白字符，避免用户输入多余空格影响查询和判断。
std::string trim(const std::string& text) {
    const std::string blanks = " \t\r\n";
    size_t start = text.find_first_not_of(blanks);
    if (start == std::string::npos) {
        return "";
    }
    size_t end = text.find_last_not_of(blanks);
    return text.substr(start, end - start + 1);
}

// 检查日期格式是否为 YYYY-MM-DD，并进一步判断月份和天数是否合法。
bool isValidDate(const std::string& date) {
    static const std::regex pattern(R"(\d{4}-\d{2}-\d{2})");
    if (!std::regex_match(date, pattern)) {
        return false;
    }

    int year = std::stoi(date.substr(0, 4));
    int month = std::stoi(date.substr(5, 2));
    int day = std::stoi(date.substr(8, 2));

    if (month < 1 || month > 12 || day < 1) {
        return false;
    }

    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
    if (leap) {
        daysInMonth[1] = 29;
    }

    return day <= daysInMonth[month - 1];
}

// 读取一行文本；如果不允许为空，则会反复提示直到输入非空内容。
std::string inputLine(const std::string& prompt, bool allowEmpty) {
    while (true) {
        std::cout << prompt;
        std::string value;
        getline(std::cin, value);
        value = trim(value);
        if (allowEmpty || !value.empty()) {
            return value;
        }
        std::cout << "输入不能为空，请重新输入。\n";
    }
}

// 读取整数并校验范围，用于菜单选择、编号选择等输入场景。
int inputInt(const std::string& prompt, int minValue, int maxValue) {
    while (true) {
        std::cout << prompt;
        std::string line;
        getline(std::cin, line);
        std::stringstream ss(line);
        int value;
        char extra;
        if (ss >> value && !(ss >> extra) && value >= minValue && value <= maxValue) {
            return value;
        }
        std::cout << "输入无效，请输入 " << minValue << " 到 " << maxValue << " 之间的整数。\n";
    }
}

// 读取日期并调用 isValidDate 校验，保证保存的截止日期格式统一。
std::string inputDate(const std::string& prompt) {
    while (true) {
        std::string date = inputLine(prompt);
        if (isValidDate(date)) {
            return date;
        }
        std::cout << "日期格式或日期值不正确，请按 YYYY-MM-DD 格式输入，例如 2026-06-15。\n";
    }
}

// 通过菜单让用户选择优先级，避免用户输入不统一的文字。
std::string inputPriority() {
    std::cout << "请选择优先级：\n"
              << "1. 高\n"
              << "2. 中\n"
              << "3. 低\n";
    int choice = inputInt("请输入选项：", 1, 3);
    if (choice == 1) {
        return "高";
    }
    if (choice == 2) {
        return "中";
    }
    return "低";
}

// 暂停控制台界面，方便用户看清本次操作结果。
void pauseScreen() {
    std::cout << "\n按回车键继续...";
    std::string ignored;
    getline(std::cin, ignored);
}

// 输出统一分隔线，让任务列表和详细信息更易读。
void printLine() {
    std::cout << "------------------------------------------------------------\n";
}
