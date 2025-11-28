#include<bits/stdc++.h>
#include<fstream>
#include<sstream>
#include<iomanip>

namespace eight
{


const int m = 400;  // 散列表大小（题目预定义）
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

typedef struct LNode {
    stock data;
    struct LNode* next;
} LNode, *LinkList;

void InitList(LinkList* H) {
    for (int i = 0; i < m; i++) {
        H[i] = new LNode;
        H[i]->next = nullptr;
    }
}

int Hash(string sname) {
    int sum = 0;
    for (int i = 0; i < sname.length(); i++) {
        sum += (i * i) * static_cast<int>(sname[i]);
    }
    return sum % 397;
}

void ListInsert(LinkList* H, stock s, int& sumCmp) {
    int addr = Hash(s.sname);
    LinkList p = H[addr];
    sumCmp++;

    while (p->next != nullptr) {
        p = p->next;
        sumCmp++;
    }

    LinkList newNode = new LNode;
    newNode->data = s;
    newNode->next = nullptr;
    p->next = newNode;
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

int ReadFile(LinkList* H, int& sumCmp, string infoFilename, string detailDir) {
    ifstream infoFile(infoFilename);
    if (!infoFile.is_open()) {
        cerr << "股票基础信息文件打开失败！" << endl;
        return 0;
    }

    int count = 0;
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
                ListInsert(H, s, sumCmp);
                count++;
            }
            stockBlock = line;
        } else {
            if (!stockBlock.empty()) stockBlock += "\n";
            stockBlock += line;
        }
    }

    if (!stockBlock.empty()) {
        stock s;
        stringstream ss(stockBlock);
        string segment;
        while (getline(ss, segment, '\n')) {
            if (segment.empty()) continue;
            size_t colonPos = segment.find(':');
            if (colonPos == string::npos) continue;
            string key = segment.substr(0, colonPos);
            string value = segment.substr(colonPos + 1);

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
            ListInsert(H, s, sumCmp);
            count++;
        }
    }

    infoFile.close();
    return count;
}

int SearchHL(LinkList* H, string key) {
    int addr = Hash(key);
    LinkList p = H[addr]->next;
    while (p != nullptr) {
        if (p->data.sname == key) {
            return addr;
        }
        p = p->next;
    }
    return -1;
}

double GetASL(int sumCmp, int count) {
    if (count == 0) return 0.0;
    return static_cast<double>(sumCmp) / count;
}

void Print(LinkList T, string sname) {
    LinkList p = T;
    while (p != nullptr) {
        if (p->data.sname == sname) {
            stock& s = p->data;
            cout << "股票代码: " << s.stockcode << endl;
            cout << "股票简称: " << s.indusname << endl;
            cout << "英文名: " << s.sname << endl;
            cout << "行业编码: " << s.briname << endl;
            cout << "一级门类: " << s.pricate << endl;
            cout << "二级门类: " << s.seccate << endl;
            cout << "上市交易所: " << s.listexchange << endl;
            cout << "公司全称: " << s.wholecompany << endl;
            cout << "上市日期: " << s.launchdate << endl;
            cout << "省份: " << s.provin << endl;
            cout << "城市: " << s.city << endl;
            cout << "法人: " << s.legalper << endl;
            cout << "地址: " << s.addr << endl;
            cout << "网址: " << s.url << endl;
            cout << "邮箱: " << s.email << endl;
            cout << "电话: " << s.callnum << endl;
            cout << "主营业务: " << s.mainbus << endl;
            cout << "经营范围: " << s.scopeofbus << endl;

            for (int i = 0; i < s.detail_count; i++) {
                stockdetail& sd = s.detail[i];
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
            break;
        }
        p = p->next;
    }
}

int liandizhi(){
    LinkList H[m];
    InitList(H);
    string infoFilename = "D:/file/学习/大二下/数据结构/课了个设/课设数据2025/课设数据2025/stocks.txt";
    string detailDir = "D:/file/学习/大二下/数据结构/课了个设/课设数据2025/课设数据2025/detail";
    int count = 0;	// 记录元素的总数
    int sumCmp = 0; // 在插入的过程中统计比较次数，便于计算ASL
    count = ReadFile(H, sumCmp, infoFilename, detailDir);
    string sname; // 输入要查找的股票英文名称
    cin.ignore();
    getline(cin, sname);
    int pos = SearchHL(H, sname);
    if (pos != -1){
        LinkList p = H[pos]->next; // 查找成功，将p指向单链表头结点
        Print(p, sname);
        double ASL = GetASL(sumCmp, count);
        cout << "ASL为：" << fixed << setprecision(2) << ASL << endl;
    }
    else{
        cout << "查找失败" << endl;
    }
}

}
