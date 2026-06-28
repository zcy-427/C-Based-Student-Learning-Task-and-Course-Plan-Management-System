#include "Task.h"

#include <iomanip>
#include <iostream>
#include <utility>
#include <vector>

namespace {
// 将字段中的特殊字符转义，避免保存到文件时与分隔符“|”冲突。
std::string escapeField(const std::string& text) {
    std::string result;
    for (char ch : text) {
        if (ch == '\\') {
            result += "\\\\";
        } else if (ch == '|') {
            result += "\\p";
        } else if (ch == '\n') {
            result += "\\n";
        } else {
            result += ch;
        }
    }
    return result;
}
}

// 初始化任务共有信息。
Task::Task(int id,
           std::string name,
           std::string course,
           std::string dueDate,
           std::string priority,
           std::string description,
           bool completed)
    : id(id),
      name(std::move(name)),
      course(std::move(course)),
      dueDate(std::move(dueDate)),
      priority(std::move(priority)),
      description(std::move(description)),
      completed(completed) {}

// 返回任务编号。
int Task::getId() const {
    return id;
}

// 返回任务名称。
const std::string& Task::getName() const {
    return name;
}

// 返回所属课程。
const std::string& Task::getCourse() const {
    return course;
}

// 返回截止日期。
const std::string& Task::getDueDate() const {
    return dueDate;
}

// 返回优先级。
const std::string& Task::getPriority() const {
    return priority;
}

// 返回任务描述。
const std::string& Task::getDescription() const {
    return description;
}

// 返回任务是否已经完成。
bool Task::isCompleted() const {
    return completed;
}

// 修改任务完成状态。
void Task::setCompleted(bool value) {
    completed = value;
}

// 按列表格式显示任务概要，供“查看所有任务”使用。
void Task::displaySummary() const {
    std::cout << std::left << std::setw(6) << id
              << std::setw(10) << getType()
              << std::setw(18) << course
              << std::setw(22) << name
              << std::setw(14) << dueDate
              << std::setw(10) << priority
              << (completed ? "已完成" : "未完成") << '\n';
}

// 显示编号、课程、日期等所有任务共有的详细信息。
void Task::displayCommonDetail() const {
    std::cout << "编号：" << id << '\n'
              << "类型：" << getType() << '\n'
              << "任务名称：" << name << '\n'
              << "所属课程：" << course << '\n'
              << "截止日期：" << dueDate << '\n'
              << "优先级：" << priority << '\n'
              << "状态：" << (completed ? "已完成" : "未完成") << '\n'
              << "任务描述：" << description << '\n';
}

// 将一个任务转换成一行文本，字段之间用“|”分隔。
std::string Task::serialize() const {
    std::vector<std::string> fields = {
        getType(),
        std::to_string(id),
        name,
        course,
        dueDate,
        priority,
        completed ? "1" : "0",
        description,
        extraDataForFile()
    };

    std::string line;
    for (size_t i = 0; i < fields.size(); ++i) {
        if (i > 0) {
            line += '|';
        }
        line += escapeField(fields[i]);
    }
    return line;
}

// 初始化作业任务的共有信息和专属信息。
HomeworkTask::HomeworkTask(int id,
                           std::string name,
                           std::string course,
                           std::string dueDate,
                           std::string priority,
                           std::string description,
                           bool completed,
                           std::string homeworkNumber,
                           std::string submitPlatform)
    : Task(id, std::move(name), std::move(course), std::move(dueDate), std::move(priority), std::move(description), completed),
      homeworkNumber(std::move(homeworkNumber)),
      submitPlatform(std::move(submitPlatform)) {}

// 返回作业任务类型名称。
std::string HomeworkTask::getType() const {
    return "作业";
}

// 显示作业任务详细信息。
void HomeworkTask::displayDetail() const {
    displayCommonDetail();
    std::cout << "作业编号：" << homeworkNumber << '\n'
              << "提交平台：" << submitPlatform << '\n';
}

// 将作业任务的专属字段组合成字符串保存。
std::string HomeworkTask::extraDataForFile() const {
    return homeworkNumber + "\t" + submitPlatform;
}

// 返回作业任务专属字段，供 GUI 展示详情。
std::vector<std::pair<std::string, std::string>> HomeworkTask::getExtraFields() const {
    return {{"作业编号", homeworkNumber}, {"提交平台", submitPlatform}};
}

// 初始化考试任务的共有信息和专属信息。
ExamTask::ExamTask(int id,
                   std::string name,
                   std::string course,
                   std::string dueDate,
                   std::string priority,
                   std::string description,
                   bool completed,
                   std::string examPlace,
                   std::string examScope)
    : Task(id, std::move(name), std::move(course), std::move(dueDate), std::move(priority), std::move(description), completed),
      examPlace(std::move(examPlace)),
      examScope(std::move(examScope)) {}

// 返回考试任务类型名称。
std::string ExamTask::getType() const {
    return "考试";
}

// 显示考试任务详细信息。
void ExamTask::displayDetail() const {
    displayCommonDetail();
    std::cout << "考试地点：" << examPlace << '\n'
              << "考试范围：" << examScope << '\n';
}

// 将考试任务的专属字段组合成字符串保存。
std::string ExamTask::extraDataForFile() const {
    return examPlace + "\t" + examScope;
}

// 返回考试任务专属字段，供 GUI 展示详情。
std::vector<std::pair<std::string, std::string>> ExamTask::getExtraFields() const {
    return {{"考试地点", examPlace}, {"考试范围", examScope}};
}

// 初始化实验任务的共有信息和专属信息。
ExperimentTask::ExperimentTask(int id,
                               std::string name,
                               std::string course,
                               std::string dueDate,
                               std::string priority,
                               std::string description,
                               bool completed,
                               std::string labPlace,
                               std::string reportRequirement)
    : Task(id, std::move(name), std::move(course), std::move(dueDate), std::move(priority), std::move(description), completed),
      labPlace(std::move(labPlace)),
      reportRequirement(std::move(reportRequirement)) {}

// 返回实验任务类型名称。
std::string ExperimentTask::getType() const {
    return "实验";
}

// 显示实验任务详细信息。
void ExperimentTask::displayDetail() const {
    displayCommonDetail();
    std::cout << "实验地点：" << labPlace << '\n'
              << "实验报告要求：" << reportRequirement << '\n';
}

// 将实验任务的专属字段组合成字符串保存。
std::string ExperimentTask::extraDataForFile() const {
    return labPlace + "\t" + reportRequirement;
}

// 返回实验任务专属字段，供 GUI 展示详情。
std::vector<std::pair<std::string, std::string>> ExperimentTask::getExtraFields() const {
    return {{"实验地点", labPlace}, {"实验报告要求", reportRequirement}};
}

// 初始化项目任务的共有信息和专属信息。
ProjectTask::ProjectTask(int id,
                         std::string name,
                         std::string course,
                         std::string dueDate,
                         std::string priority,
                         std::string description,
                         bool completed,
                         std::string members,
                         std::string progress)
    : Task(id, std::move(name), std::move(course), std::move(dueDate), std::move(priority), std::move(description), completed),
      members(std::move(members)),
      progress(std::move(progress)) {}

// 返回项目任务类型名称。
std::string ProjectTask::getType() const {
    return "项目";
}

// 显示项目任务详细信息。
void ProjectTask::displayDetail() const {
    displayCommonDetail();
    std::cout << "小组成员：" << members << '\n'
              << "项目进度：" << progress << '\n';
}

// 将项目任务的专属字段组合成字符串保存。
std::string ProjectTask::extraDataForFile() const {
    return members + "\t" + progress;
}

// 返回项目任务专属字段，供 GUI 展示详情。
std::vector<std::pair<std::string, std::string>> ProjectTask::getExtraFields() const {
    return {{"小组成员", members}, {"项目进度", progress}};
}
