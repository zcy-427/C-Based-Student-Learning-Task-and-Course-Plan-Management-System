#pragma once

#include <string>

/** @brief 去除字符串首尾的空格、制表符和换行符。 */
std::string trim(const std::string& text);

/** @brief 判断日期字符串是否符合 YYYY-MM-DD 格式，并检查日期是否合法。 */
bool isValidDate(const std::string& date);

/**
 * @brief 读取一行用户输入。
 * @param prompt 输入提示语。
 * @param allowEmpty 是否允许输入为空。
 * @return 用户输入并去除首尾空白后的字符串。
 */
std::string inputLine(const std::string& prompt, bool allowEmpty = false);

/**
 * @brief 读取指定范围内的整数。
 * @param prompt 输入提示语。
 * @param minValue 最小允许值。
 * @param maxValue 最大允许值。
 * @return 合法整数输入。
 */
int inputInt(const std::string& prompt, int minValue, int maxValue);

/** @brief 循环读取日期，直到用户输入合法日期。 */
std::string inputDate(const std::string& prompt);

/** @brief 读取任务优先级，并返回“高”“中”“低”。 */
std::string inputPriority();

/** @brief 暂停程序，等待用户按回车继续。 */
void pauseScreen();

/** @brief 输出一条分隔线，用于美化控制台显示。 */
void printLine();
