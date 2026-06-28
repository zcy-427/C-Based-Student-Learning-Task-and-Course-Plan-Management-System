#pragma once

#include "Task.h"

#include <memory>
#include <string>
#include <vector>

/**
 * @brief 任务管理类，负责管理所有学习任务。
 *
 * 该类使用 vector<unique_ptr<Task>> 保存不同类型的任务对象，
 * 通过基类指针实现多态调用；同时负责添加、查询、删除、排序、统计、文件读写等功能。
 */
class TaskManager {
private:
    std::vector<std::unique_ptr<Task>> tasks; // 保存所有任务对象的容器
    int nextId = 1;                           // 下一个新任务使用的编号
    std::string fileName = "tasks.txt";       // 保存任务数据的文件名

    /**
     * @brief 拆分派生类专属字段。
     *
     * 文件中将两个专属字段用制表符 '\t' 连接，读取时需要重新拆开。
     */
    static std::vector<std::string> splitExtraData(const std::string& data);

    /**
     * @brief 根据任务编号查找任务。
     * @return 找到则返回任务指针，找不到返回 nullptr。
     */
    Task* findTaskById(int id) const;

public:
    /** @brief 获取全部任务对象，供 GUI 表格读取显示。 */
    const std::vector<std::unique_ptr<Task>>& getTasks() const;

    /** @brief 生成一个新的任务编号。 */
    int generateTaskId();

    /** @brief 直接添加一个任务对象，供 GUI 对话框创建任务后调用。 */
    void addTask(std::unique_ptr<Task> task);

    /** @brief 根据任务编号替换任务对象，供 GUI 编辑任务后调用。 */
    bool replaceTaskById(int id, std::unique_ptr<Task> task);

    /** @brief 根据任务编号删除任务，成功返回 true。 */
    bool removeTaskById(int id);

    /** @brief 根据任务编号设置完成状态，成功返回 true。 */
    bool setTaskCompleted(int id, bool completed);

    /** @brief 只排序不输出，供 GUI 和控制台复用。 */
    void sortTasksByDueDate();

    /** @brief 生成统计信息文本，供 GUI 弹窗和控制台输出复用。 */
    std::string buildStatisticsText() const;

    /** @brief 添加一个新的学习任务。 */
    void addTask();

    /** @brief 显示当前所有任务的概要列表。 */
    void showAllTasks() const;

    /** @brief 按课程名称查询任务。 */
    void searchByCourse() const;

    /** @brief 按任务类型筛选任务。 */
    void filterByType() const;

    /** @brief 根据任务编号修改完成状态。 */
    void changeTaskStatus();

    /** @brief 根据任务编号删除任务。 */
    void deleteTask();

    /** @brief 按截止日期从近到远排序任务。 */
    void sortByDueDate();

    /** @brief 统计任务总数、完成数、未完成数、高优先级数量和各课程任务数量。 */
    void showStatistics() const;

    /** @brief 将当前任务列表保存到文件。 */
    void saveToFile() const;

    /** @brief 从文件读取任务数据，恢复历史任务列表。 */
    void loadFromFile();
};
