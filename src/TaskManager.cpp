#include "TaskManager.h"

#include "Utils.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace {
// 将从文件读取的转义字符还原，例如“\\p”还原为“|”。
std::string unescapeField(const std::string& text) {
    std::string result;
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '\\' && i + 1 < text.size()) {
            char next = text[++i];
            if (next == '\\') {
                result += '\\';
            } else if (next == 'p') {
                result += '|';
            } else if (next == 'n') {
                result += '\n';
            } else {
                result += next;
            }
        } else {
            result += text[i];
        }
    }
    return result;
}

// 按“|”拆分一行文件数据，同时处理已经转义的分隔符。
std::vector<std::string> splitLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string current;
    bool escaped = false;

    for (char ch : line) {
        if (escaped) {
            current += '\\';
            current += ch;
            escaped = false;
        } else if (ch == '\\') {
            escaped = true;
        } else if (ch == '|') {
            fields.push_back(unescapeField(current));
            current.clear();
        } else {
            current += ch;
        }
    }

    if (escaped) {
        current += '\\';
    }
    fields.push_back(unescapeField(current));
    return fields;
}
}

// 将派生类专属信息拆成两个字段，例如“作业编号”和“提交平台”。
std::vector<std::string> TaskManager::splitExtraData(const std::string& data) {
    size_t pos = data.find('\t');
    if (pos == std::string::npos) {
        return {data, ""};
    }
    return {data.substr(0, pos), data.substr(pos + 1)};
}

// 遍历任务列表，根据编号寻找指定任务。
Task* TaskManager::findTaskById(int id) const {
    for (const auto& task : tasks) {
        if (task->getId() == id) {
            return task.get();
        }
    }
    return nullptr;
}

// 返回所有任务，GUI 通过该接口刷新表格。
const std::vector<std::unique_ptr<Task>>& TaskManager::getTasks() const {
    return tasks;
}

// 生成新的任务编号，并让下一个编号递增。
int TaskManager::generateTaskId() {
    return nextId++;
}

// 直接添加已经创建好的任务对象。
void TaskManager::addTask(std::unique_ptr<Task> task) {
    if (task) {
        tasks.push_back(std::move(task));
    }
}

// 根据编号替换任务对象，供 GUI 编辑任务后复用原编号。
bool TaskManager::replaceTaskById(int id, std::unique_ptr<Task> task) {
    if (!task || task->getId() != id) {
        return false;
    }

    auto it = std::find_if(tasks.begin(), tasks.end(), [id](const std::unique_ptr<Task>& existing) {
        return existing->getId() == id;
    });

    if (it == tasks.end()) {
        return false;
    }

    *it = std::move(task);
    return true;
}

// 根据编号删除任务，供 GUI 和控制台复用。
bool TaskManager::removeTaskById(int id) {
    auto it = std::find_if(tasks.begin(), tasks.end(), [id](const std::unique_ptr<Task>& task) {
        return task->getId() == id;
    });

    if (it == tasks.end()) {
        return false;
    }

    tasks.erase(it);
    return true;
}

// 根据编号设置任务完成状态。
bool TaskManager::setTaskCompleted(int id, bool completed) {
    Task* task = findTaskById(id);
    if (task == nullptr) {
        return false;
    }

    task->setCompleted(completed);
    return true;
}

// 按截止日期排序，不进行任何控制台输出。
void TaskManager::sortTasksByDueDate() {
    std::sort(tasks.begin(), tasks.end(), [](const std::unique_ptr<Task>& a, const std::unique_ptr<Task>& b) {
        if (a->getDueDate() == b->getDueDate()) {
            return a->getId() < b->getId();
        }
        return a->getDueDate() < b->getDueDate();
    });
}

// 生成统计信息字符串，控制台和 GUI 都可以直接复用。
std::string TaskManager::buildStatisticsText() const {
    int completedCount = 0;
    int highPriorityCount = 0;
    std::map<std::string, int> courseCount;

    for (const auto& task : tasks) {
        if (task->isCompleted()) {
            ++completedCount;
        }
        if (task->getPriority() == "高") {
            ++highPriorityCount;
        }
        ++courseCount[task->getCourse()];
    }

    std::ostringstream oss;
    oss << "任务统计信息：\n"
        << "总任务数：" << tasks.size() << '\n'
        << "已完成：" << completedCount << '\n'
        << "未完成：" << tasks.size() - completedCount << '\n'
        << "高优先级任务：" << highPriorityCount << "\n\n"
        << "各课程任务数量：\n";

    if (courseCount.empty()) {
        oss << "暂无课程任务。\n";
    } else {
        for (const auto& item : courseCount) {
            oss << item.first << "：" << item.second << '\n';
        }
    }

    return oss.str();
}

// 添加新任务：先读取通用字段，再根据类型读取专属字段并创建对应派生类对象。
void TaskManager::addTask() {
    std::cout << "\n请选择任务类型：\n"
              << "1. 作业任务\n"
              << "2. 考试复习任务\n"
              << "3. 实验任务\n"
              << "4. 课程项目任务\n";
    int type = inputInt("请输入选项：", 1, 4);

    std::string name = inputLine("任务名称：");
    std::string course = inputLine("所属课程：");
    std::string dueDate = inputDate("截止日期（YYYY-MM-DD）：");
    std::string priority = inputPriority();
    std::string description = inputLine("任务描述：", true);
    bool completed = inputInt("是否完成？1. 未完成  2. 已完成：", 1, 2) == 2;

    if (type == 1) {
        std::string homeworkNumber = inputLine("作业编号：");
        std::string submitPlatform = inputLine("提交平台：");
        tasks.push_back(std::make_unique<HomeworkTask>(nextId++, name, course, dueDate, priority, description, completed, homeworkNumber, submitPlatform));
    } else if (type == 2) {
        std::string examPlace = inputLine("考试地点：");
        std::string examScope = inputLine("考试范围：");
        tasks.push_back(std::make_unique<ExamTask>(nextId++, name, course, dueDate, priority, description, completed, examPlace, examScope));
    } else if (type == 3) {
        std::string labPlace = inputLine("实验地点：");
        std::string reportRequirement = inputLine("实验报告要求：");
        tasks.push_back(std::make_unique<ExperimentTask>(nextId++, name, course, dueDate, priority, description, completed, labPlace, reportRequirement));
    } else {
        std::string members = inputLine("小组成员：");
        std::string progress = inputLine("项目进度：");
        tasks.push_back(std::make_unique<ProjectTask>(nextId++, name, course, dueDate, priority, description, completed, members, progress));
    }

    std::cout << "添加成功！\n";
}

// 以表格形式显示所有任务；如果没有任务则给出提示。
void TaskManager::showAllTasks() const {
    if (tasks.empty()) {
        std::cout << "当前没有任务。\n";
        return;
    }

    std::cout << std::left << std::setw(6) << "编号"
              << std::setw(10) << "类型"
              << std::setw(18) << "课程"
              << std::setw(22) << "任务名称"
              << std::setw(14) << "截止日期"
              << std::setw(10) << "优先级"
              << "状态" << '\n';
    printLine();
    for (const auto& task : tasks) {
        task->displaySummary();
    }
}

// 根据课程名称筛选任务，并显示匹配任务的详细信息。
void TaskManager::searchByCourse() const {
    std::string course = inputLine("请输入课程名称：");
    bool found = false;

    for (const auto& task : tasks) {
        if (task->getCourse() == course) {
            if (!found) {
                std::cout << "\n课程：" << course << "\n";
                printLine();
            }
            task->displayDetail();
            printLine();
            found = true;
        }
    }

    if (!found) {
        std::cout << "没有找到该课程下的任务。\n";
    }
}

// 根据用户选择的任务类型筛选并显示任务。
void TaskManager::filterByType() const {
    std::cout << "\n请选择要筛选的任务类型：\n"
              << "1. 作业任务\n"
              << "2. 考试任务\n"
              << "3. 实验任务\n"
              << "4. 课程项目任务\n";
    int choice = inputInt("请输入选项：", 1, 4);

    std::string type;
    if (choice == 1) {
        type = "作业";
    } else if (choice == 2) {
        type = "考试";
    } else if (choice == 3) {
        type = "实验";
    } else {
        type = "项目";
    }

    bool found = false;
    for (const auto& task : tasks) {
        if (task->getType() == type) {
            task->displayDetail();
            printLine();
            found = true;
        }
    }

    if (!found) {
        std::cout << "没有找到该类型任务。\n";
    }
}

// 根据任务编号查找任务，然后把状态修改为未完成或已完成。
void TaskManager::changeTaskStatus() {
    int id = inputInt("请输入要修改的任务编号：", 1, std::numeric_limits<int>::max());
    Task* task = findTaskById(id);
    if (task == nullptr) {
        std::cout << "未找到该任务。\n";
        return;
    }

    int choice = inputInt("请选择状态：1. 未完成  2. 已完成：", 1, 2);
    task->setCompleted(choice == 2);
    std::cout << "修改成功！\n";
}

// 根据任务编号删除任务，删除前先显示任务详情并要求用户确认。
void TaskManager::deleteTask() {
    int id = inputInt("请输入要删除的任务编号：", 1, std::numeric_limits<int>::max());
    Task* task = findTaskById(id);

    if (task == nullptr) {
        std::cout << "未找到该任务。\n";
        return;
    }

    task->displayDetail();
    std::string confirm = inputLine("确认删除该任务吗？Y/N：");
    if (confirm == "Y" || confirm == "y") {
        removeTaskById(id);
        std::cout << "删除成功！\n";
    } else {
        std::cout << "已取消删除。\n";
    }
}

// 利用日期字符串 YYYY-MM-DD 可以直接按字典序比较的特点，对任务进行升序排序。
void TaskManager::sortByDueDate() {
    sortTasksByDueDate();

    std::cout << "已按截止日期从近到远排序。\n";
    showAllTasks();
}

// 统计总任务、完成任务、未完成任务、高优先级任务和每门课程的任务数量。
void TaskManager::showStatistics() const {
    std::cout << '\n' << buildStatisticsText();
}

// 将所有任务逐行写入 tasks.txt，每一行表示一个任务对象。
void TaskManager::saveToFile() const {
    std::ofstream ofs(fileName);
    if (!ofs) {
        throw std::runtime_error("无法打开文件进行保存：" + fileName);
    }

    for (const auto& task : tasks) {
        ofs << task->serialize() << '\n';
    }
}

// 从 tasks.txt 读取任务数据，根据任务类型创建不同的派生类对象。
void TaskManager::loadFromFile() {
    std::ifstream ifs(fileName);
    if (!ifs) {
        return;
    }

    tasks.clear();

    std::string line;
    int maxId = 0;
    while (getline(ifs, line)) {
        if (trim(line).empty()) {
            continue;
        }

        try {
            std::vector<std::string> fields = splitLine(line);
            if (fields.size() != 9) {
                throw std::runtime_error("字段数量不正确");
            }

            std::string type = fields[0];
            int id = std::stoi(fields[1]);
            std::string name = fields[2];
            std::string course = fields[3];
            std::string dueDate = fields[4];
            std::string priority = fields[5];
            bool completed = fields[6] == "1";
            std::string description = fields[7];
            std::vector<std::string> extra = splitExtraData(fields[8]);

            if (type == "作业") {
                tasks.push_back(std::make_unique<HomeworkTask>(id, name, course, dueDate, priority, description, completed, extra[0], extra[1]));
            } else if (type == "考试") {
                tasks.push_back(std::make_unique<ExamTask>(id, name, course, dueDate, priority, description, completed, extra[0], extra[1]));
            } else if (type == "实验") {
                tasks.push_back(std::make_unique<ExperimentTask>(id, name, course, dueDate, priority, description, completed, extra[0], extra[1]));
            } else if (type == "项目") {
                tasks.push_back(std::make_unique<ProjectTask>(id, name, course, dueDate, priority, description, completed, extra[0], extra[1]));
            } else {
                throw std::runtime_error("未知任务类型");
            }

            maxId = std::max(maxId, id);
        } catch (const std::exception& e) {
            std::cout << "读取一条任务数据失败，已跳过。原因：" << e.what() << '\n';
        }
    }

    nextId = maxId + 1;
}
