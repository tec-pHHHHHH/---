#include<bits/stdc++.h>
#include<fstream>
#include<sstream>
#include<iomanip>

using namespace std;

namespace  six {

const int MAXSIZE = 5000;
typedef struct stockdetail{
    string date;//日期
    double openprice;//开盘价
    double closeprice;//收盘价
    double highest;//最高价
    double lowest;//最低价
    double volume;//成交量
    double turnover;//成交额
    string changerate;//换手率
    string risefallrate;//涨跌幅
    double risefallamount;//涨跌额
}stockdetail;//股票的价格信息

typedef struct stock{
    string stockcode;//股票代码
    string indusname;//股票简称
    string sname;//英文名
    string briname;//行业编码
    string pricate;//一级门类（即行业大类）
    string seccate;//二级门类（即行业二级类）
    string listexchange;//上市交易所
    string wholecompany;//公司全称（即机构名称）
    string launchdate;//上市日期
    string provin;//省份
    string city;//城市
    string legalper;//法人
    string addr;//地址
    string url;//网址
    string email;//邮箱
    string callnum;//电话
    string mainbus;//主营业务
    string scopeofbus;//经营范围
    stockdetail *detail;//定义一个stockdetail类型的数组用来存放该股票每一日的相关价格信息
    int detail_count;//detail数组的长度
}stock;

typedef struct {
    stock* elem;                 // 指向数组的指针
    int length;                 // 数组的长度
} SqList;

typedef struct TNode {			//字典树结点结构
    stock* stockPtr;              // 股票指针（结尾结点指向对应股票，否则为NULL）
    struct TNode* child[53];    // 子结点指针数组：0-25小写字母，26-51大写字母，52空格
} TNode, *TrieTree;

// -------------------------- 1. 顺序表操作 --------------------------
// 初始化顺序表（数组下标从1开始存储）
void InitList(SqList& L) {
    L.elem = new stock[MAXSIZE + 1];  // 下标0闲置，1~MAXSIZE存储股票
    if (!L.elem) {
        cerr << "顺序表内存分配失败！" << endl;
        exit(1);
    }
    L.length = 0;
}

// 释放顺序表内存（含股票的行情数据）
void FreeList(SqList& L) {
    for (int i = 1; i <= L.length; i++) {
        if (L.elem[i].detail != nullptr) {
            delete[] L.elem[i].detail;
            L.elem[i].detail = nullptr;
        }
    }
    delete[] L.elem;
    L.elem = nullptr;
    L.length = 0;
}

TNode* InitTNode() {
    TNode* node = new TNode;
    node->stockPtr = nullptr;
    for (int i = 0; i < 53; i++) {
        node->child[i] = nullptr;
    }
    return node;
}

TNode* BuildTree(SqList& L) {
    TNode* root = InitTNode();
    for (int i = 1; i <= L.length; i++) {
        TNode* p = root;
        string& sname = L.elem[i].sname;
        for (char ch : sname) {
            int idx;
            if (ch == ' ') {
                idx = 52;
            } else if (ch >= 'a' && ch <= 'z') {
                idx = ch - 'a';
            } else if (ch >= 'A' && ch <= 'Z') {
                idx = ch - 'A' + 26;
            } else {
                break;
            }
            if (p->child[idx] == nullptr) {
                p->child[idx] = InitTNode();
            }
            p = p->child[idx];
        }
        p->stockPtr = &L.elem[i];
    }
    return root;
}

TNode* SearchPrefixNode(TNode* root, string prefix) {
    TNode* p = root;
    for (char ch : prefix) {
        int idx;
        if (ch == ' ') {
            idx = 52;
        } else if (ch >= 'a' && ch <= 'z') {
            idx = ch - 'a';
        } else if (ch >= 'A' && ch <= 'Z') {
            idx = ch - 'A' + 26;
        } else {
            return nullptr;
        }
        if (p->child[idx] == nullptr) {
            return nullptr;
        }
        p = p->child[idx];
    }
    return p;
}

void ReadDetail(stock& s, string detailDir) {
    string detailPath = detailDir + "/" + s.stockcode + ".txt";
    ifstream detailFile(detailPath);
    if (!detailFile.is_open()) {
        s.detail = nullptr;
        s.detail_count = 0;
        return;
    }

    vector<stockdetail> tempDetails;
    string line;
    while (getline(detailFile, line)) {
        if (line.empty()) continue;
        replace(line.begin(), line.end(), ';', ' ');  // 分号转空格
        stringstream ss(line);
        stockdetail sd;
        ss >> sd.date >> sd.openprice >> sd.closeprice >> sd.highest
           >> sd.lowest >> sd.volume >> sd.turnover >> sd.changerate
           >> sd.risefallrate >> sd.risefallamount;
        tempDetails.push_back(sd);
    }
    detailFile.close();

    s.detail_count = tempDetails.size();
    if (s.detail_count > 0) {
        s.detail = new stockdetail[s.detail_count];
        for (int i = 0; i < s.detail_count; i++) {
            s.detail[i] = tempDetails[i];
        }
    } else {
        s.detail = nullptr;
    }
}

// 从stocks.txt读取股票基础信息（下标从1开始存储）
void ReadFile(SqList& L, string infoFilename, string detailDir) {
    ifstream infoFile(infoFilename);
    if (!infoFile.is_open()) {
        cerr << "股票基础信息文件打开失败！" << endl;
        return;
    }

    string line, stockBlock;
    while (getline(infoFile, line) && L.length < MAXSIZE) {
        if (line.empty()) continue;
        // 按"#"分割单只股票信息块
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

                // 匹配stocks.txt实际字段名
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
            // 读取行情数据并加入顺序表（下标从1开始）
            ReadDetail(s, detailDir);
            if (!s.stockcode.empty() && !s.sname.empty()) {
                L.elem[++L.length] = s;
            }
            stockBlock = line;
        } else {
            if (!stockBlock.empty()) stockBlock += "\n";
            stockBlock += line;
        }
    }

    // 解析最后一只股票
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

            // 同上述字段匹配逻辑
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
        if (!s.stockcode.empty() && !s.sname.empty()) {
            L.elem[++L.length] = s;
        }
    }

    infoFile.close();
}

void CollectStocks(TNode* node, stock** s, int& count) {
    if (node == nullptr) return;
    // 若当前结点是股票英文名结尾，加入收集列表
    if (node->stockPtr != nullptr) {
        s[count++] = node->stockPtr;
    }
    // 递归遍历所有子结点
    for (int i = 0; i < 53; i++) {
        CollectStocks(node->child[i], s, count);
    }
}

double GetASL(SqList& L) {
    if (L.length == 0) return 0.0;
    int totalLength = 0;
    for (int i = 1; i <= L.length; i++) {
        totalLength += L.elem[i].sname.length();  // 累加英文名长度
    }
    return (double)totalLength / L.length;  // 返回平均查找长度
}

void Print(stock *s) {
    if (s == nullptr) return;

    // 输出基础信息
    cout << "股票代码: " << s->stockcode << endl;
    cout << "股票简称: " << s->indusname << endl;
    cout << "英文名: " << s->sname << endl;
    cout << "行业编码: " << s->briname << endl;
    cout << "一级门类: " << s->pricate << endl;
    cout << "二级门类: " << s->seccate << endl;
    cout << "上市交易所: " << s->listexchange << endl;
    cout << "公司全称: " << s->wholecompany << endl;
    cout << "上市日期: " << s->launchdate << endl;
    cout << "省份: " << s->provin << endl;
    cout << "城市: " << s->city << endl;
    cout << "法人: " << s->legalper << endl;
    cout << "地址: " << s->addr << endl;
    cout << "网址: " << s->url << endl;
    cout << "邮箱: " << s->email << endl;
    cout << "电话: " << s->callnum << endl;
    cout << "主营业务: " << s->mainbus << endl;
    cout << "经营范围: " << s->scopeofbus << endl;

    // 输出行情数据
    for (int i = 0; i < s->detail_count; i++) {
        stockdetail& sd = s->detail[i];
        cout << "日期: " << sd.date
             << " 开盘价: " << fixed << setprecision(3) << sd.openprice
             << " 收盘价: " << fixed << setprecision(3) << sd.closeprice
             << " 最高价: " << fixed << setprecision(3) << sd.highest
             << " 最低价: " << fixed << setprecision(3) << sd.lowest
             << " 成交量: " << fixed << setprecision(3) << sd.volume
             << " 成交额: " << fixed << setprecision(3) << sd.turnover
             << " 换手率: " << sd.changerate
             << " 涨跌幅: " << sd.risefallrate
             << " 涨跌额: " << fixed << setprecision(3) << sd.risefallamount << endl;
    }
}


int zidian(){
    SqList L;
    InitList(L);
    string infoFilename = "D:/file/学习/大二下/数据结构/课了个设/课设数据2025/课设数据2025/stocks.txt";
    string detailDir = "D:/file/学习/大二下/数据结构/课了个设/课设数据2025/课设数据2025/detail";
    ReadFile(L, infoFilename, detailDir);
    TNode *root = BuildTree(L);
    string prefix;
    cin.ignore();
    getline(cin, prefix);
    // 查找前缀节点
    TNode* prefixNode = SearchPrefixNode(root, prefix);

    // 使用动态数组存储匹配的股票
    stock** matchedStocks = new stock*[MAXSIZE];
    int count = 0;

    if (prefixNode != NULL) {
        // 收集所有匹配的股票
        CollectStocks(prefixNode, matchedStocks, count);
    }

    if (count > 0) {
        for (int i = 0; i < count; i++) {
            Print(matchedStocks[i]);
        }
        double ASL = GetASL(L);
        cout << "ASL为：" << fixed << setprecision(2) << ASL << endl;
    } else {
        cout<<"查找失败"<<endl;
    }

    // 释放动态分配的内存
    delete[] matchedStocks;
    FreeList(L);
}

}
