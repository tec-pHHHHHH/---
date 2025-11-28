#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

using namespace std;


namespace eighteen
{

// 定义最大数组大小
const int MAX_STOCK_DATA = 1000;
const int MAX_LABELS = MAX_STOCK_DATA - 1;
const int MAX_INDICATOR_SIZE = MAX_LABELS;
const int MAX_INDICATORS = 5;

typedef struct StockDetail {
    string date;          // 日期
    double openPrice;     // 开盘价
    double closePrice;    // 收盘价
    double highest;       // 最高价
    double lowest;        // 最低价
    double volume;        // 成交量
    double turnover;      // 成交额
    string changeRate;    // 换手率
    string riseFallRate;  // 涨跌幅
    double riseFallAmount;// 涨跌额
} StockDetail;

struct IndicatorScore {
    string name;       // 指标名称
    double corrScore;  // 皮尔逊相关系数（绝对值）
};

string trim(const string &s) {
    auto start = s.begin();
    while (start != s.end() && isspace(*start)) {
        start++;
    }
    auto end = s.end();
    do {
        end--;
    } while (distance(start, end) > 0 && isspace(*end));
    return string(start, end + 1);
}

bool LoadStockFromFile(const string& filename, StockDetail detailsList[], int& count) {
// 从文件加载股票数据，返回是否成功
    ifstream ifs;

    ifs.open(filename);

    if (!ifs.is_open()) {
    cerr << "error111" << endl;
        return false;
    }

    string line;
    while (getline(ifs, line)) {
        line = trim(line);
        if (line.empty()) continue; // 跳过空行

        // 按分号分割字段（共10个字段）
        stringstream ss(line);
        vector<string> fields;
        string field;
        while (getline(ss, field, ';')) {
            fields.push_back(trim(field));
        }

        // 绑定字段到stockdetail（顺序：日期→涨跌额）
        StockDetail& d = detailsList[count];
        d.date = fields[0];                  // 日期（string）
        d.openPrice = stod(fields[1]);       // 开盘价（double）
        d.closePrice = stod(fields[2]);      // 收盘价（double）
        d.highest = stod(fields[3]);         // 最高价（double）
        d.lowest = stod(fields[4]);          // 最低价（double）
        d.volume = stod(fields[5]);          // 成交量（double）
        d.turnover = stod(fields[6]);        // 成交额（double）
        d.changeRate = fields[7];            // 换手率（string）
        d.riseFallRate = fields[8];          // 涨跌幅（string）
        d.riseFallAmount = stod(fields[9]);  // 涨跌额（double）

        count++;
    }

    ifs.close();

    return true;
}

void CreatePriceLabels(const StockDetail detailsList[], int count, int labels[], int& labelCount) {
// 创建价格变动标签（1上涨，0下跌）
    labelCount = count - 1;
    for (int i = 0; i < labelCount; ++i)
        labels[i] = (detailsList[i].closePrice > detailsList[i + 1].closePrice) ? 1 : 0;
}

double ComputeCorrelationScore(const double indicator[], const int labels[], int n,
                              double& sumX, double& sumY, double& sumXY) {
// 计算指标与标签的皮尔逊相关系数绝对值
    sumX = 0.0;
    sumY = 0.0;
    sumXY = 0.0;
    double sumX2 = 0.0;
    double sumY2 = 0.0;

    for (int i = 0; i < n; ++i) {
        double x = indicator[i];
        double y = labels[i];
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
        sumY2 += y * y;
    }

    double numerator = n * sumXY - sumX * sumY;
    double denominatorPart1 = n * sumX2 - sumX * sumX;
    double denominatorPart2 = n * sumY2 - sumY * sumY;
    double denominator = sqrt(denominatorPart1 * denominatorPart2);

    if (denominator == 0) {
        return 0.0;
    }

    return fabs(numerator / denominator);
}

void GetRiseFallRates(const StockDetail detailsList[], int count, double indicator[], int& indicatorCount) {
// 获取涨跌幅指标数据（%）
    indicatorCount = 0;
    if (count < 2) {
        return;
    }

    indicatorCount = count - 1;
    for (int i = 0; i < indicatorCount; ++i) {
        string s = detailsList[i + 1].riseFallRate;
        if (!s.empty() && s.back() == '%') s.pop_back();
        indicator[i] = stod(s);
    }
}

void GetVolumesInWanShou(const StockDetail detailsList[], int count, double indicator[], int& indicatorCount) {
// 获取成交量指标数据（转换为万手）
    indicatorCount = count - 1;
    for (int i = 0; i < indicatorCount; ++i)
        indicator[i] = detailsList[i + 1].volume / 10000.0;
}

void GetAmplitudes(const StockDetail detailsList[], int count, double indicator[], int& indicatorCount) {
// 计算振幅指标数据（%）
    indicatorCount = count - 1;
    for (int i = 0; i < indicatorCount; ++i) {
        const StockDetail& d = detailsList[i + 1];
        if (d.openPrice == 0) {
            indicator[i] = 0.0;
        } else {
            indicator[i] = (d.highest - d.lowest) / d.openPrice * 100.0;
        }
    }
}

void GetTurnoverRates(const StockDetail detailsList[], int count, double indicator[], int& indicatorCount) {
// 获取换手率指标数据（%）
    indicatorCount = count - 1;
    for (int i = 0; i < indicatorCount; ++i) {
        string s = detailsList[i + 1].changeRate;
        if (!s.empty() && s.back() == '%') s.pop_back();
        indicator[i] = stod(s);
    }
}

void GetTurnoverInQianWan(const StockDetail detailsList[], int count, double indicator[], int& indicatorCount) {
// 获取成交额指标数据（转换为千万元）
    indicatorCount = count - 1;
    for (int i = 0; i < indicatorCount; ++i)
        indicator[i] = detailsList[i + 1].turnover / 10000000.0;
}

void SortIndicatorScores(IndicatorScore scores[], int count, bool descending) {
// 对指标相关系数进行排序（降序/升序）
    for (int i = 0; i < count; ++i) {
        int maxIdx = i;
        for (int j = i + 1; j < count; ++j) {
            if (scores[j].corrScore > scores[maxIdx].corrScore) {
                maxIdx = j;
            }
        }
        swap(scores[i], scores[maxIdx]);
    }
}



void pierxun() {
    StockDetail detailsList[MAX_STOCK_DATA];
    int labels[MAX_LABELS];
    string inputFile;
    int detailCount = 0;
    int labelCount = 0;
    //cout << 666 << endl;
    cin >> inputFile;
    //cout << 666 << endl;
    if (!LoadStockFromFile(inputFile, detailsList, detailCount)) {
        cerr << "Error: 数据读取失败或有效数据量不足" << endl;
        return ;
    }
    CreatePriceLabels(detailsList, detailCount, labels, labelCount);

    if (labelCount == 0) {
        cerr << "Error: 标签生成失败" << endl;
        return ;
    }

    // 指标名称数组
    string indicatorNames[MAX_INDICATORS] = {
        "涨跌幅（%）", "成交量（万手）", "振幅（%）", "换手率（%）", "成交额（千万元）"
    };

    IndicatorScore indicatorScores[MAX_INDICATORS];
    double sumX, sumY, sumXY;
    double indicator[MAX_INDICATOR_SIZE];
    int indicatorCount = 0;

    // 计算各个指标的相关系数
    GetRiseFallRates(detailsList, detailCount, indicator, indicatorCount);
    indicatorScores[0] = {indicatorNames[0],
        ComputeCorrelationScore(indicator, labels, indicatorCount, sumX, sumY, sumXY)};

    GetVolumesInWanShou(detailsList, detailCount, indicator, indicatorCount);
    indicatorScores[1] = {indicatorNames[1],
        ComputeCorrelationScore(indicator, labels, indicatorCount, sumX, sumY, sumXY)};

    GetAmplitudes(detailsList, detailCount, indicator, indicatorCount);
    indicatorScores[2] = {indicatorNames[2],
        ComputeCorrelationScore(indicator, labels, indicatorCount, sumX, sumY, sumXY)};

    GetTurnoverRates(detailsList, detailCount, indicator, indicatorCount);
    indicatorScores[3] = {indicatorNames[3],
        ComputeCorrelationScore(indicator, labels, indicatorCount, sumX, sumY, sumXY)};

    GetTurnoverInQianWan(detailsList, detailCount, indicator, indicatorCount);
    indicatorScores[4] = {indicatorNames[4],
        ComputeCorrelationScore(indicator, labels, indicatorCount, sumX, sumY, sumXY)};

    SortIndicatorScores(indicatorScores, MAX_INDICATORS, true);

    // 输出
    cout << "=== 股票指标重要性排序（基于皮尔逊相关系数）===" << endl;
    cout << fixed << setprecision(3);
    cout << "排名 | 指标名称 | 相关系数" << endl;
    cout << "----------------------------------------" << endl;
    for (int i = 0; i < MAX_INDICATORS; ++i) {
        cout << i + 1 << " | "
             << indicatorScores[i].name << " | "
             << indicatorScores[i].corrScore << endl;
    }

    cout << "\n=== 筛选出的Top3重要指标 ===" << endl;
    cout << "排名 | 指标名称" << endl;
    cout << "------------------------" << endl;
    for (int i = 0; i < 3 && i < MAX_INDICATORS; ++i) {
        cout << setw(2) << i + 1 << "  | " << indicatorScores[i].name << endl;
    }

}
}


//      D:\file\学习\大二下\数据结构\课了个设\课设数据2025\课设数据2025\detail\cn_000007.txt