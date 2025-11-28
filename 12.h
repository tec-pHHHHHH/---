#include <bits/stdc++.h>

using namespace std;

namespace tewve {

const int MAXSIZE = 5000;
// 股票每日交易详情结构体
typedef struct stockdetail {
    string date;            // 交易日期
    double openprice;       // 开盘价
    double closeprice;      // 收盘价
    double highest;         // 最高价
    double lowest;          // 最低价
    double volume;          // 成交量
    double turnover;        // 成交额
    string changerate;      // 换手率
    string risefallrate;    // 涨跌幅
    double risefallamount;  // 涨跌额
} stockdetail;

// 股票基本信息结构体
typedef struct stock {
    string stockcode;       // 股票代码
    string indusname;       // 股票简称
    string sname;           // 英文名
    string briname;         // 行业编码
    string pricate;         // 一级门类（即行业大类）
    string seccate;         // 二级门类（即行业二级类）
    string listexchange;    // 上市交易所
    string wholecompany;    // 公司全称（即机构名称）
    string launchdate;      // 上市日期
    string provin;          // 省份
    string city;            // 城市
    string legalper;        // 法人
    string addr;            // 地址
    string url;             // 网址
    string email;           // 邮箱
    string callnum;         // 电话
    string mainbus;         // 主营业务
    string scopeofbus;      // 经营范围
    stockdetail *detail;    // 每日交易详情数组指针
    int detail_count;       // 交易详情数量
} stock;

// 日期-成交量结构体
typedef struct DailyVolume {
    string date;            // 交易日期
    double volume;          // 成交量
} DailyVolume;

// 顺序表结构（股票）
typedef struct {
    stock* elem;
    int length;
} SqList;

typedef struct {
    DailyVolume* elem;
    int length;
} SqListDailyVolume;

// 初始化股票顺序表
void InitList(SqList &L) {
    L.elem = new stock[MAXSIZE + 1];
    L.length = 0;
}

// 释放股票顺序表内存
void FreeList(SqList &L) {
    if (L.elem != nullptr) {
        for (int i = 1; i <= L.length; ++i) {
            if (L.elem[i].detail != nullptr) {
                delete[] L.elem[i].detail;
                L.elem[i].detail = nullptr;
            }
        }
        delete[] L.elem;
        L.elem = nullptr;
    }
    L.length = 0;
}

// 初始化日期-成交量顺序表
void InitDailyVolumeList(SqListDailyVolume &DL) {
    DL.elem = new DailyVolume[MAXSIZE * 200];  // 足够大的空间
    DL.length = 0;
}

// 释放日期-成交量顺序表内存
void FreeDailyVolume(SqListDailyVolume &DL) {
    if (DL.elem != nullptr) {
        delete[] DL.elem;
        DL.elem = nullptr;
    }
    DL.length = 0;
}

// 从detail文件夹读取股票每日交易详情
void ReadDetail(stock& s, string detailDir) {
    s.detail_count = 0;
    s.detail = nullptr;
    if (s.stockcode.empty()) return;
    string filename = detailDir + "/" + s.stockcode + ".txt";
    ifstream file(filename);
    if (!file.is_open()) return;
    int count = 0;
    string line;
    // 统计有效行数
    while (getline(file, line)) {
        if (!line.empty()) count++;
    }
    if (count == 0) {
        file.close();
        return;
    }
    s.detail = new stockdetail[count];
    s.detail_count = count;
    file.clear();
    file.seekg(0, ios::beg);
    int index = 0;
    // 解析每行数据
    while (getline(file, line) && index < count) {
        if (line.empty()) continue;
        for (char& c : line) {
            if (c == ';') c = ' '; // 替换分隔符为空格，便于流提取
        }
        stringstream ss(line);
        ss >> s.detail[index].date
           >> s.detail[index].openprice
           >> s.detail[index].closeprice
           >> s.detail[index].highest
           >> s.detail[index].lowest
           >> s.detail[index].volume
           >> s.detail[index].turnover
           >> s.detail[index].changerate
           >> s.detail[index].risefallrate
           >> s.detail[index].risefallamount;
        index++;
    }
    file.close();
}

// 从文件读取股票基本信息
    void ReadFile(SqList &L, string infoFilename, string detailDir) {
        ifstream infoFile(infoFilename);
        if (!infoFile.is_open()) {
            cerr << "股票基础信息文件打开失败！" << endl;
            return;
        }

        string line, stockBlock;
        while (getline(infoFile, line)) {
            if (line.empty()) continue;
            if (line.find("CODE:") != string::npos && !stockBlock.empty()) {
                stock s;
                stringstream ss(stockBlock);
                string segment;
                while (getline(ss, segment, '\n')) {
                    if (segment.empty()) continue;
                    size_t colonPos = segment.find(':');
                    if (colonPos == string::npos) continue;
                    string key = segment.substr(0, colonPos);
                    string value = segment.substr(colonPos + 1);
                    while (!value.empty() && isspace(value[0])) value.erase(0, 1);
                    while (!value.empty() && isspace(value.back())) value.pop_back();

                    if (key == "CODE") s.stockcode = value;
                    else if (key == "股票简称") s.indusname = value;
                    else if (key == "英文名") s.sname = value;
                    else if (key == "行业编码") s.briname = value;
                    else if (key == "行业大类") s.pricate = value;
                    else if (key == "行业二级类") s.seccate = value;
                    else if (key == "上市交易所") s.listexchange = value;
                    else if (key == "机构名称") s.wholecompany = value;
                    else if (key == "上市日期") s.launchdate = value;
                    else if (key == "省份") s.provin = value;
                    else if (key == "城市") s.city = value;
                    else if (key == "法人") s.legalper = value;
                    else if (key == "地址") s.addr = value;
                    else if (key == "网址") s.url = value;
                    else if (key == "邮箱") s.email = value;
                    else if (key == "电话") s.callnum = value;
                    else if (key == "主营业务") s.mainbus = value;
                    else if (key == "经营范围") s.scopeofbus = value;
                }
                ReadDetail(s, detailDir);
                if (!s.stockcode.empty() && L.length < MAXSIZE) {
                    L.length++;
                    L.elem[L.length] = s;
                }
                stockBlock = line;
            } else {
                if (!stockBlock.empty()) stockBlock += "\n";
                stockBlock += line;
            }
        }

        if (!stockBlock.empty() && L.length < MAXSIZE) {
            stock s;
            stringstream ss(stockBlock);
            string segment;
            while (getline(ss, segment, '\n')) {
                if (segment.empty()) continue;
                size_t colonPos = segment.find(':');
                if (colonPos == string::npos) continue;
                string key = segment.substr(0, colonPos);
                string value = segment.substr(colonPos + 1);
                while (!value.empty() && isspace(value[0])) value.erase(0, 1);
                while (!value.empty() && isspace(value.back())) value.pop_back();

                if (key == "CODE") s.stockcode = value;
                else if (key == "股票简称") s.indusname = value;
                else if (key == "英文名") s.sname = value;
                else if (key == "行业编码") s.briname = value;
                else if (key == "行业大类") s.pricate = value;
                else if (key == "行业二级类") s.seccate = value;
                else if (key == "机构名称") s.wholecompany = value;
                else if (key == "上市交易所") s.listexchange = value;
                else if (key == "上市日期") s.launchdate = value;
                else if (key == "省份") s.provin = value;
                else if (key == "城市") s.city = value;
                else if (key == "法人") s.legalper = value;
                else if (key == "地址") s.addr = value;
                else if (key == "网址") s.url = value;
                else if (key == "邮箱") s.email = value;
                else if (key == "电话") s.callnum = value;
                else if (key == "主营业务") s.mainbus = value;
                else if (key == "经营范围") s.scopeofbus = value;
            }
            ReadDetail(s, detailDir);
            if (!s.stockcode.empty()) {
                L.length++;
                L.elem[L.length] = s;
            }
        }

        infoFile.close();
    }


void Merge(DailyVolume* elem, int low, int mid, int high, int& kcn, int& rmn) {
    int n = high - low + 1;
    DailyVolume* temp = new DailyVolume[n];//申请一个临时空间，长度是右减左
    int i = low, j = mid + 1, k = 0;//开始
    while (i <= mid && j <= high) {
        kcn++;
        if (elem[i].volume <= elem[j].volume) {//=稳定性
            temp[k++] = elem[i++];
        } else {
            temp[k++] = elem[j++];
        }
    }
    while (i <= mid) {
        temp[k++] = elem[i++];
    }
    while (j <= high) {
        temp[k++] = elem[j++];
    }
    for (k = 0, i = low; i <= high; k++, i++) {//覆盖回原数组
        elem[i] = temp[k];
        rmn++;
    }

    delete[] temp;
}

void MergeSortRecursive(DailyVolume* elem, int low, int high, int& kcn, int& rmn) {
    if (low < high) {
        int mid = (low + high) / 2;
        MergeSortRecursive(elem, low, mid, kcn, rmn);
        MergeSortRecursive(elem, mid + 1, high, kcn, rmn);
        Merge(elem, low, mid, high, kcn, rmn);
    }
}

void MergeSort(SqListDailyVolume& DL, int& kcn, int& rmn) {
    if (DL.length <= 1) return;
    MergeSortRecursive(DL.elem, 1, DL.length, kcn, rmn);
}

void industryDailyVolume(SqList &L, SqListDailyVolume& DL, string briname, int &kcn, int &rmn) {
    InitDailyVolumeList(DL);
    for (int i = 1; i <= L.length; ++i) {
        stock& s = L.elem[i];
        if (s.briname != briname) continue;
        for (int j = 0; j < s.detail_count; ++j) {
            stockdetail& d = s.detail[j];
            string date = d.date;
            double vol = d.volume;
            bool found = false;
            for (int k = 1; k <= DL.length; ++k) {
                if (DL.elem[k].date == date) {
                    DL.elem[k].volume += vol;
                    found = true;
                    break;
                }
            }
            if (!found) {
                DL.length++;
                DL.elem[DL.length].date = date;
                DL.elem[DL.length].volume = vol;
            }
        }
    }
    MergeSort(DL, kcn, rmn);
}

void guibing() {
    SqList L;
    InitList(L);
    SqListDailyVolume DL;
    InitDailyVolumeList(DL);
    string infoFile = "D:/file/学习/大二下/数据结构/课了个设/课设数据2025/课设数据2025/stocks.txt";
    string detailDir = "D:/file/学习/大二下/数据结构/课了个设/课设数据2025/课设数据2025/detail";
    ReadFile(L, infoFile, detailDir);

    string briname;
    cin >> briname;

    int kcn = 0;
    int rmn = 0;
    industryDailyVolume(L, DL, briname, kcn, rmn);

    cout << "总的关键字比较次数KCN为：" << kcn << endl;
    cout << "总的记录移动次数RMN为：" << rmn << endl;
    for (int i = 1; i <= DL.length; i++)
        cout<<DL.elem[i].date <<" "<< fixed << std::setprecision(3) << DL.elem[i].volume<<endl;

    FreeList(L);
}

}
