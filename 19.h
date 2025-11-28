#include <iostream>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>

using namespace std;


namespace nineteen
{
    // 定义最大数组大小
const int MAX_SAMPLES = 1000;
const int MAX_FEATURES = 3;
const int MAX_NODE_COUNT = 5;

struct Sample {
    string currentDate;
    double features[MAX_FEATURES];
    int label;
    int featureCount;
};

// 节点类型：特征节点 / 标签节点
enum NodeType { FEATURE, LABEL };

// 边的参数结构体（存储高斯分布的均值和方差）
struct EdgeParams {
    double mean = 0.0;       // 均值（初始化为0）
    double variance = 1e-6;  // 方差（默认最小阈值，避免除以0）
    bool exists = false;     // 标记边是否存在
};

// 贝叶斯网络图结构体
struct BayesianGraph {
    string nodeNames[MAX_NODE_COUNT] = {"涨跌幅", "振幅", "成交额", "跌", "涨"};
    NodeType nodeTypes[MAX_NODE_COUNT] = {FEATURE, FEATURE, FEATURE, LABEL, LABEL};
    int nodeCount = MAX_NODE_COUNT;
    EdgeParams adj[MAX_NODE_COUNT][MAX_NODE_COUNT];
    double prior[2] = {0.0, 0.0};

    // 构造函数：初始化邻接矩阵
    BayesianGraph() {
        for (int i = 0; i < 3; ++i) {
            for (int j = 3; j < 5; ++j) {
                adj[i][j].exists = true;
            }
        }
    }

    int FindNodeIndex(NodeType type, int index = -1) const {
        if (type == FEATURE) {
            if (index >= 0 && index < 3) {
                return index;
            }
        } else if (type == LABEL) {
            if (index >= 0 && index < 2) {
                return 3 + index; // 3对应标签0（跌），4对应标签1（涨）
            }
        }
        return -1;
    }
    // 计算涨跌频率
    void ComputePriorProbabilities(const Sample samples[], int sampleCount) {
        int count0 = 0, count1 = 0;
        for (int i = 0; i < sampleCount; ++i) {
            if (samples[i].label == 0) count0++;
            else if (samples[i].label == 1) count1++;
        }
        int total = count0 + count1;
        prior[0] = 1.0 * count0 / total;
        prior[1] = 1.0 * count1 / total;
    }

    void ComputeEdgeMeans(const Sample samples[], int sampleCount) {
        for (int c = 0; c < 2; ++c) { // 遍历标签0（跌）和1（涨）
            int labelNode = FindNodeIndex(LABEL, c);
            if (labelNode == -1) continue;
            for (int f = 0; f < 3; ++f) { // 遍历三个特征
                int featureNode = FindNodeIndex(FEATURE, f);
                if (featureNode == -1) continue;
                double sum = 0.0;
                int count = 0;
                for (int i = 0; i < sampleCount; ++i) {
                    if (samples[i].label == c) {
                        sum += samples[i].features[f];
                        count++;
                    }
                }
                if (count > 0) {
                    adj[featureNode][labelNode].mean = sum / count;
                    adj[featureNode][labelNode].exists = true;
                }
            }
        }
    }

    void ComputeEdgeVariances(const Sample samples[], int sampleCount) {
        for (int c = 0; c < 2; ++c) {
            int labelNode = FindNodeIndex(LABEL, c);
            if (labelNode == -1) continue;
            for (int f = 0; f < 3; ++f) {
                int featureNode = FindNodeIndex(FEATURE, f);
                if (featureNode == -1) continue;
                double mean = adj[featureNode][labelNode].mean;
                double sumVar = 0.0;
                int count = 0;
                for (int i = 0; i < sampleCount; ++i) {
                    if (samples[i].label == c) {
                        double diff = samples[i].features[f] - mean;
                        sumVar += diff * diff;
                        count++;
                    }
                }
                if (count > 1) {
                    adj[featureNode][labelNode].variance = sumVar / (count - 1) + 1e-6;
                } else {
                    adj[featureNode][labelNode].variance = 1e-6;
                }
                adj[featureNode][labelNode].exists = true;
            }
        }
    }
    // 训练数据
    void Train(const Sample samples[], int sampleCount) {
        ComputePriorProbabilities(samples, sampleCount);
        ComputeEdgeMeans(samples, sampleCount);
        ComputeEdgeVariances(samples, sampleCount);
    }

    double CalculateLogProbability(const double features[], int label) const {
        double logProb = log(prior[label]);
        for (int f = 0; f < 3; ++f) {
            int featureNode = FindNodeIndex(FEATURE, f);
            int labelNode = FindNodeIndex(LABEL, label);
            if (featureNode == -1 || labelNode == -1 || !adj[featureNode][labelNode].exists) {
                continue;
            }
            double mu = adj[featureNode][labelNode].mean;
            double sigma2 = adj[featureNode][labelNode].variance;
            double x = features[f];
            double term = -0.5 * log(2 * M_PI * sigma2) - (x - mu) * (x - mu) / (2 * sigma2);
            logProb += term;
        }
        return logProb;
    }

    int Predict(const double features[]) const {
        double prob0 = CalculateLogProbability(features, 0);
        double prob1 = CalculateLogProbability(features, 1);
        return prob1 > prob0 ? 1 : 0;
    }
};

Sample ParseSampleLine(const string& line) {
    Sample sample;
    sample.featureCount = MAX_FEATURES;
    vector<string> fields;
    stringstream ss(line);
    string field;
    while (getline(ss, field, ';')) {
        fields.push_back(field);
    }
    sample.currentDate = fields[0];
    sample.features[0] = stod(fields[2]); // 涨跌幅
    sample.features[1] = stod(fields[3]); // 振幅
    sample.features[2] = stod(fields[4]); // 成交额
    sample.label = stoi(fields[5]);       // 次日涨跌标签
    return sample;
}

int LoadTrainData(const string& filename, Sample samples[]) {
    ifstream ifs(filename);
    if (!ifs.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        return 0;
    }
    string header;
    getline(ifs, header);
    int count = 0;
    string line;
    while (getline(ifs, line)) {
        samples[count] = ParseSampleLine(line);
        count++;
    }
    ifs.close();
    return count;
}



void beiyesi()
{
    string inputFile;
    cin.ignore();
    getline(cin, inputFile);
    Sample allSamples[MAX_SAMPLES];
    int sampleCount = LoadTrainData(inputFile, allSamples);
    if (sampleCount < 2) {
        cerr << "数据量不足，至少需要2条数据！" << endl;
        return ;
    }

    Sample latest = allSamples[0];
    Sample trainSamples[MAX_SAMPLES];
    int trainCount = 0;
    for (int i = 1; i < sampleCount && trainCount < MAX_SAMPLES; ++i) {
        trainSamples[trainCount++] = allSamples[i];
    }

    BayesianGraph graph;
    graph.Train(trainSamples, trainCount);

    int prediction = graph.Predict(latest.features);

    cout << "=== 预测结果 ===" << endl;
    cout << "日期：" << latest.currentDate << endl;
    cout << "输入指标：" << endl;
    cout << "涨跌幅：" << latest.features[0] << "% | 振幅：" << latest.features[1] << "% | 成交额：" << latest.features[2] << "千万元" << endl;
    cout << "预测结果：" << (prediction == 1 ? "次日上涨" : "次日下跌") << endl;
}
}


//    D:\file\学习\大二下\数据结构\课了个设\课设数据2025\课设数据2025\detail\cn_000007.txt