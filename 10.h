#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>  
#include <cmath>   
#include <algorithm>  
#include <iomanip>    
using namespace std;     

namespace ten {

    const int MAXSIZE = 5000;

typedef struct stockdetail {
    string date;
    double openprice;
    double closeprice;
    double highest;
    double lowest;
    double volume;
    double turnover;
    string changerate;
    string risefallrate;
    double risefallamount;
} stockdetail;

typedef struct stock {
    string stockcode;
    string indusname;
    string sname;
    string briname;
    string pricate;
    string seccate;
    string listexchange;
    string wholecompany;
    string launchdate;
    string provin;
    string city;
    string legalper;
    string addr;
    string url;
    string email;
    string callnum;
    string mainbus;
    string scopeofbus;
    stockdetail *detail;
    int detail_count;
} stock;

typedef struct {
    stock *elem;
    int length;
} SqList;

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

void ReadDetail(stock &s, string detailDir) {
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


    //使用并查集来动态维护和管理属于同一行业的所有股票，然后通过这个集合来快速、准确地计算指定行业在特定日期的总换手率（或总变化率）。


void Init_UFSet(int UFSet[], int size)
{
    for (int i = 1; i <= size; i++)
    {
        UFSet[i] = i;//每个元素更节电是自己
    }
}

void Union_UFSet(int UFSet[], int Root1, int Root2)//合并
{
    UFSet[Root1] = Root2;
}

int CompressionFind(int UFSet[], int x)//查找所属
{
    if (UFSet[x] != x) {
        UFSet[x] = CompressionFind(UFSet, UFSet[x]);
    }
    return UFSet[x];
}


void ClassifyByIndustry(SqList& L, int parent[], string industries[], int firstOccurrence[], int& industryCount) {
    industryCount = 0;
    if (L.length == 0) return;

    Init_UFSet(parent, L.length);

    for (int i = 1; i <= L.length; i++)
    {
        string currIndustry = L.elem[i].briname;
        if (currIndustry.empty()) continue;

        int existIdx = -1;//行业是否被记录过
        for (int j = 0; j < industryCount; j++) {
            if (industries[j] == currIndustry) {
                existIdx = j;
                break;
            }
        }

        //两种情况
        if (existIdx == -1)
        {
            industries[industryCount] = currIndustry;
            firstOccurrence[industryCount] = i;
            industryCount++;
        } else {
            int rootCurr = CompressionFind(parent, i);
            int rootFirst = CompressionFind(parent, firstOccurrence[existIdx]);
            if (rootCurr != rootFirst) {
                Union_UFSet(parent, rootCurr, rootFirst);
            }
        }
    }
}

    //指定行业每日涨跌
double CalculateTotalChangeRate(SqList& L, int parent[], string targetIndustry, string targetDate) {
    double total = 0.0;

    int targetFirstIdx = -1;
    for (int i = 1; i <= L.length; i++)
    {//遍历，行业
        if (L.elem[i].briname == targetIndustry)
        {
            targetFirstIdx = i;
            break;
        }
    }
    if (targetFirstIdx == -1) return total;//没找到

    int targetRoot = CompressionFind(parent, targetFirstIdx);//找到族长

    for (int i = 1; i <= L.length; i++)
    {
        if (CompressionFind(parent, i) != targetRoot) continue;//没找到

        stock& currStock = L.elem[i];//找到，遍历
        for (int j = 0; j < currStock.detail_count; j++)
        {
            if (currStock.detail[j].date == targetDate)
            {
                string rateStr = currStock.detail[j].changerate;
                rateStr.erase(remove(rateStr.begin(), rateStr.end(), '%'), rateStr.end());
                total += stod(rateStr);
                break;
            }
        }
    }

    //累加
    total = round(total * 100) / 100;

    return total;
}

void bingchaji() {
    SqList L;
    InitList(L);
    string infoFilename = "D:/file/学习/大二下/数据结构/课了个设/课设数据2025/课设数据2025/stocks.txt";
    string detailDir = "D:/file/学习/大二下/数据结构/课了个设/课设数据2025/课设数据2025/detail";
    ReadFile(L, infoFilename, detailDir);
    int parent[MAXSIZE];
    string industries[MAXSIZE];
    int firstOccurrence[MAXSIZE];
    int industryCount = 0;
    ClassifyByIndustry(L, parent, industries, firstOccurrence, industryCount);
    string targetIndustry, targetDate;
    cin >> targetIndustry >> targetDate;
    double totalChangeRate = CalculateTotalChangeRate(L, parent, targetIndustry, targetDate);
    cout << totalChangeRate << endl;
    FreeList(L);
}

}
