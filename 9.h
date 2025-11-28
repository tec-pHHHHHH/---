#include<bits/stdc++.h>
#include<fstream>
#include<sstream>

namespace nine
{
const int MAXSIZE = 5000;
using namespace std;

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

// 链表节点结构（用于存储筛选后的股票信息）
struct StockNode {
    string stockcode;       // 股票代码
    string indusname;       // 股票简称
    string induscode;       // 行业编码
    string date;            // 交易日期
    double risefallamount;  // 涨跌额
    StockNode* next;
};

void InitList(SqList& L) {
    L.elem = new stock[MAXSIZE + 1];
    L.length = 0;
}

void FreeList(SqList& L) {
    if (L.elem != nullptr) {
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
        replace(line.begin(), line.end(), ';', ' ');
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

void ReadFile(SqList& L, string infoFilename, string detailDir) {
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
        if (!s.stockcode.empty()) {
            L.length++;
            L.elem[L.length] = s;
        }
    }

    infoFile.close();
}

StockNode* SplitStockListByRiseFall(SqList& L, const string& induscode, const string& targetdate) {
    StockNode* negHead = new StockNode();
    StockNode* posHead = new StockNode();
    StockNode* negTail = negHead;
    StockNode* posTail = posHead;


    for (int i = 1; i <= L.length; i++) {
        stock& currStock = L.elem[i];
        if (currStock.briname != induscode) continue;

        double riseFall = 0.0;
        bool hasTargetDate = false;
        for (int j = 0; j < currStock.detail_count; j++) {
            if (currStock.detail[j].date == targetdate) {
                riseFall = currStock.detail[j].risefallamount;
                hasTargetDate = true;
                break;
            }
        }
        if (!hasTargetDate) continue;

        StockNode* newNode = new StockNode();
        newNode->stockcode = currStock.stockcode;
        newNode->indusname = currStock.indusname;
        newNode->induscode = currStock.briname;
        newNode->date = targetdate;
        newNode->risefallamount = riseFall;
        newNode->next = nullptr;

        if (riseFall < 0) {
            negTail->next = newNode;
            negTail = newNode;
        } else {
            posTail->next = newNode;
            posTail = newNode;
        }
    }

    negTail->next = posHead->next;
    delete posHead;
    StockNode* resultHead = negHead->next;
    delete negHead;

    return resultHead;
}

void lianbiao(){
    SqList L;
    InitList(L);
    string infoFilename = "D:/file/学习/大二下/数据结构/课了个设/课设数据2025/课设数据2025/stocks.txt";
    string detailDir = "D:/file/学习/大二下/数据结构/课了个设/课设数据2025/课设数据2025/detail";
    ReadFile(L, infoFilename, detailDir);
    string induscode;
    string targetdate;
    cin>>induscode>>targetdate;
    StockNode* head = SplitStockListByRiseFall(L, induscode, targetdate);
    StockNode* p = head;
    while (p) {
        cout << "股票代码: " << p->stockcode << " ";
        cout << "股票简称: " << p->indusname << " ";
        cout << "行业编码：" << p->induscode <<" ";
        cout << "日期: " << p->date << " ";
        cout << "涨跌额: " << p->risefallamount <<endl;
        p = p->next;
    }
    while (head) {
        StockNode* tmp = head;
        head = head->next;
        delete tmp;
    }
    FreeList(L);
}

}

