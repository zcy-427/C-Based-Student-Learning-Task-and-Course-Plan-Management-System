#pragma once

#include <string>
#include <utility>
#include <vector>

/**
 * @brief 学习任务基类，保存所有任务共有的信息。
 *
 * 该类是抽象类，不能直接创建对象；通过虚函数让作业、考试、实验、项目等
 * 派生类拥有各自的显示方式和文件保存方式，体现继承与多态。
 */
class Task {
protected:
    int id;                    // 任务编号，用于查询、修改和删除任务
    std::string name;          // 任务名称
    std::string course;        // 所属课程
    std::string dueDate;       // 截止日期，格式为 YYYY-MM-DD
    std::string priority;      // 优先级：高 / 中 / 低
    std::string description;   // 任务描述
    bool completed;            // 完成状态：true 表示已完成，false 表示未完成

public:
    /**
     * @brief 构造一个通用学习任务对象。
     */
    Task(int id,
         std::string name,
         std::string course,
         std::string dueDate,
         std::string priority,
         std::string description,
         bool completed);

    /**
     * @brief 虚析构函数，保证通过基类指针释放派生类对象时安全。
     */
    virtual ~Task() = default;

    /** @brief 获取任务编号。 */
    int getId() const;

    /** @brief 获取任务名称。 */
    const std::string& getName() const;

    /** @brief 获取所属课程。 */
    const std::string& getCourse() const;

    /** @brief 获取截止日期。 */
    const std::string& getDueDate() const;

    /** @brief 获取优先级。 */
    const std::string& getPriority() const;

    /** @brief 获取任务描述。 */
    const std::string& getDescription() const;

    /** @brief 判断任务是否已完成。 */
    bool isCompleted() const;

    /** @brief 设置任务完成状态。 */
    void setCompleted(bool value);

    /** @brief 获取任务类型，例如“作业”“考试”“实验”“项目”。 */
    virtual std::string getType() const = 0;

    /** @brief 显示某一类任务的详细信息。 */
    virtual void displayDetail() const = 0;

    /** @brief 获取派生类专属数据，用于保存到文件。 */
    virtual std::string extraDataForFile() const = 0;

    /** @brief 获取派生类专属字段，供 GUI 详情展示使用。 */
    virtual std::vector<std::pair<std::string, std::string>> getExtraFields() const = 0;

    /** @brief 以表格行形式显示任务概要信息。 */
    void displaySummary() const;

    /** @brief 显示所有任务共有的详细信息。 */
    void displayCommonDetail() const;

    /** @brief 将任务对象转换成一行文本，便于写入 tasks.txt。 */
    std::string serialize() const;
};

/**
 * @brief 作业任务类。
 *
 * 在通用任务信息的基础上，增加作业编号和提交平台两个属性。
 */
class HomeworkTask : public Task {
private:
    std::string homeworkNumber; // 作业编号
    std::string submitPlatform; // 提交平台

public:
    /** @brief 构造一个作业任务对象。 */
    HomeworkTask(int id,
                 std::string name,
                 std::string course,
                 std::string dueDate,
                 std::string priority,
                 std::string description,
                 bool completed,
                 std::string homeworkNumber,
                 std::string submitPlatform);

    /** @brief 返回任务类型“作业”。 */
    std::string getType() const override;

    /** @brief 显示作业任务的详细信息。 */
    void displayDetail() const override;

    /** @brief 返回作业编号和提交平台，供文件保存使用。 */
    std::string extraDataForFile() const override;

    /** @brief 返回作业编号和提交平台，供 GUI 详情展示使用。 */
    std::vector<std::pair<std::string, std::string>> getExtraFields() const override;
};

/**
 * @brief 考试任务类。
 *
 * 在通用任务信息的基础上，增加考试地点和考试范围两个属性。
 */
class ExamTask : public Task {
private:
    std::string examPlace; // 考试地点
    std::string examScope; // 考试范围

public:
    /** @brief 构造一个考试任务对象。 */
    ExamTask(int id,
             std::string name,
             std::string course,
             std::string dueDate,
             std::string priority,
             std::string description,
             bool completed,
             std::string examPlace,
             std::string examScope);

    /** @brief 返回任务类型“考试”。 */
    std::string getType() const override;

    /** @brief 显示考试任务的详细信息。 */
    void displayDetail() const override;

    /** @brief 返回考试地点和考试范围，供文件保存使用。 */
    std::string extraDataForFile() const override;

    /** @brief 返回考试地点和考试范围，供 GUI 详情展示使用。 */
    std::vector<std::pair<std::string, std::string>> getExtraFields() const override;
};

/**
 * @brief 实验任务类。
 *
 * 在通用任务信息的基础上，增加实验地点和实验报告要求两个属性。
 */
class ExperimentTask : public Task {
private:
    std::string labPlace;          // 实验地点
    std::string reportRequirement; // 实验报告要求

public:
    /** @brief 构造一个实验任务对象。 */
    ExperimentTask(int id,
                   std::string name,
                   std::string course,
                   std::string dueDate,
                   std::string priority,
                   std::string description,
                   bool completed,
                   std::string labPlace,
                   std::string reportRequirement);

    /** @brief 返回任务类型“实验”。 */
    std::string getType() const override;

    /** @brief 显示实验任务的详细信息。 */
    void displayDetail() const override;

    /** @brief 返回实验地点和报告要求，供文件保存使用。 */
    std::string extraDataForFile() const override;

    /** @brief 返回实验地点和报告要求，供 GUI 详情展示使用。 */
    std::vector<std::pair<std::string, std::string>> getExtraFields() const override;
};

/**
 * @brief 课程项目任务类。
 *
 * 在通用任务信息的基础上，增加小组成员和项目进度两个属性。
 */
class ProjectTask : public Task {
private:
    std::string members;  // 小组成员
    std::string progress; // 项目进度

public:
    /** @brief 构造一个课程项目任务对象。 */
    ProjectTask(int id,
                std::string name,
                std::string course,
                std::string dueDate,
                std::string priority,
                std::string description,
                bool completed,
                std::string members,
                std::string progress);

    /** @brief 返回任务类型“项目”。 */
    std::string getType() const override;

    /** @brief 显示课程项目任务的详细信息。 */
    void displayDetail() const override;

    /** @brief 返回小组成员和项目进度，供文件保存使用。 */
    std::string extraDataForFile() const override;

    /** @brief 返回小组成员和项目进度，供 GUI 详情展示使用。 */
    std::vector<std::pair<std::string, std::string>> getExtraFields() const override;
};
