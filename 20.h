#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <iomanip>
#include <cstring>
#include <errno.h>
#include <vector>
#include <limits>


using namespace std;

namespace twenty
{
    const int K_NEIGHBORS = 3;  // K近邻数量
const int MAX_DAYS = 1000;  // 最大交易日数量

struct TradingDay {
    string date;               // 当前日期
    double change;             // 当日涨跌幅（%）
    double amplitude;          // 振幅（%）
    double amountMil;          // 成交额（千万元）
    double nextChange;         // 次日涨跌幅（%）
};

struct NeighborNode {
    double distance;           // 与目标样本的距离
    double nextChange;         // 该邻居的次日涨跌幅（%）
    NeighborNode* next;        // 下一个邻居节点

    // 构造函数
    NeighborNode(double d = 0, double nc = 0) : distance(d), nextChange(nc), next(nullptr) {}

    static NeighborNode* createSentinelList() {
        NeighborNode* dummy = new NeighborNode();
        NeighborNode* current = dummy;
        for (int i = 0; i < K_NEIGHBORS; ++i) {
            current->next = new NeighborNode(numeric_limits<double>::infinity(), 0.0);
            current = current->next;
        }
        return dummy;
    }

    static void insertOrdered(NeighborNode* dummy, double dist, double nc) {
        NeighborNode* newNode = new NeighborNode(dist, nc);
        NeighborNode* prev = dummy;
        NeighborNode* curr = dummy->next;

        while (curr != nullptr && curr->distance < dist) {
            prev = curr;
            curr = curr->next;
        }

        prev->next = newNode;
        newNode->next = curr;
    }

    static double calculateAverage(NeighborNode* dummy) {
        double sum = 0.0;
        int validCount = 0;
        NeighborNode* current = dummy->next;

        while (current != nullptr && validCount < K_NEIGHBORS) {
            sum += current->nextChange;
            validCount++;
            current = current->next;
        }

        return sum / validCount;
    }

    static void freeList(NeighborNode* dummy) {
        NeighborNode* current = dummy;
        while (current != nullptr) {
            NeighborNode* temp = current;
            current = current->next;
            delete temp;
        }
    }
};

void FindFeatureMinMax(const double features[][3], int featureCount, double minVal[], double maxVal[]) {
    // 初始化：先遍历所有有效特征找到真实min/max
    for (int i = 0; i < 3; ++i)
    {
        minVal[i] = numeric_limits<double>::max();
        maxVal[i] = numeric_limits<double>::lowest();
    }

    // 只计算有效特征
    for (int j = 0; j < featureCount; ++j) {
        for (int i = 0; i < 3; ++i) {
            minVal[i] = min(minVal[i], features[j][i]);
            maxVal[i] = max(maxVal[i], features[j][i]);
        }
    }
}

double EuclideanDistance(const double a[], const double b[]) {
    double sum = 0.0;
    // 移除权重，直接计算三个特征的欧几里得距离
    for (int i = 0; i < 3; ++i) {
        sum += pow(a[i] - b[i], 2);
    }
    return sqrt(sum);
}

bool ParseTradingLine(const string& line, TradingDay& day, int& label) {
    stringstream ss(line);
    string field;
    vector<string> fields;

    // 按分号分割，去除字段前后空白
    while (getline(ss, field, ';')) {
        fields.push_back(field);
    }

    day.date = fields[0];
    // 解析核心指标（严格按字段索引，避免错位）
    day.change = stod(fields[2]);
    day.amplitude = stod(fields[3]);
    day.amountMil = stod(fields[4]);
    label = stoi(fields[5]);
    return true;
}

void SetNextChanges(TradingDay days[], const int labels[], int dayCount) {
    // 关键修复：次日涨跌幅对应逻辑（days[i]的次日是days[i-1]，仅需判断次日涨跌幅有效）
    for (int i = 1; i < dayCount; ++i)
        days[i].nextChange = days[i-1].change;
}

bool LoadDataFromFile(const string& filename, TradingDay days[], int& dayCount, int labels[]) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "error111" << endl;
        return false;
    }


    string line;
    dayCount = 0;

    // 跳过表头（第一行）
    getline(file, line);

    // 只读取非空且格式正确的数据行
    while (getline(file, line) && dayCount < MAX_DAYS) {
        TradingDay day;
        int label;
        ParseTradingLine(line, day, label);
        days[dayCount] = day;
        labels[dayCount] = label;
        dayCount++;
    }

    file.close();

    SetNextChanges(days, labels, dayCount);

    return true;
}


void ExtractTrainData(const TradingDay days[], int dayCount, double features[][3], double nextChanges[], int& featureCount) {
    featureCount = 0;
    for (int i = 1; i < dayCount; ++i) {
        features[featureCount][0] = days[i].change;
        features[featureCount][1] = days[i].amplitude;
        features[featureCount][2] = days[i].amountMil;
        nextChanges[featureCount] = days[i].nextChange;
        featureCount++;
    }
}

void NormalizeTargetFeature(const TradingDay days[], double targetFeatures[], const double minVal[], const double maxVal[]) {
    // 使用传入的训练数据min/max进行归一化
    targetFeatures[0] = (days[0].change - minVal[0]) / (maxVal[0] - minVal[0]);
    targetFeatures[1] = (days[0].amplitude - minVal[1]) / (maxVal[1] - minVal[1]);
    targetFeatures[2] = (days[0].amountMil - minVal[2]) / (maxVal[2] - minVal[2]);
}

double PredictNextChange(const TradingDay days[], int dayCount) {
    // 提取有效训练数据
    double trainFeatures[MAX_DAYS][3];
    double nextChanges[MAX_DAYS];
    int featureCount;
    ExtractTrainData(days, dayCount, trainFeatures, nextChanges, featureCount);

    //计算训练数据的min/max
    double minVal[3], maxVal[3];
    FindFeatureMinMax(trainFeatures, featureCount, minVal, maxVal);

    //归一化训练特征
    for (int i = 0; i < featureCount; ++i) {
        for (int j = 0; j < 3; ++j) {
            double val = (trainFeatures[i][j] - minVal[j]) / (maxVal[j] - minVal[j]);
            trainFeatures[i][j] = val;
        }
    }

    //归一化目标特征
    double targetFeatures[3];
    NormalizeTargetFeature(days, targetFeatures, minVal, maxVal);

    //查找最近邻
    NeighborNode* dummy = NeighborNode::createSentinelList();
    for (int i = 0; i < featureCount; ++i) {
        double dist = EuclideanDistance(targetFeatures, trainFeatures[i]);
        dummy->insertOrdered(dummy, dist, nextChanges[i]);
    }

    // 6. 计算平均涨跌幅
    double predicted = dummy->calculateAverage(dummy);
    dummy->freeList(dummy);

    return predicted;
}


void k_means() {
    TradingDay days[MAX_DAYS];
    int labels[MAX_DAYS];
    int dayCount = 0;

    string inputFile;
    cin.ignore();
    getline(cin, inputFile);

    if (!LoadDataFromFile(inputFile, days, dayCount, labels)) {
        cerr << "错误：数据加载失败，程序退出" << endl;
        return ;
    }

    string targetDate = days[0].date;
    double predictedChange = PredictNextChange(days, dayCount);

    cout << "=== KNN预测结果 ===" << endl;
    cout << "目标日期：" << targetDate << endl;
    cout << "使用指标：当日涨跌幅(%)、振幅(%)、成交额(千万元)" << endl;
    cout << fixed << setprecision(3);
    cout << "预测次日涨跌幅：" << predictedChange << "%" << endl;

}
}
