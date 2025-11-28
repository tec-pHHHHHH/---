#include<bits/stdc++.h>
#include<fstream>
#include<sstream>
using namespace std;

namespace thirteen {
const int MAXSIZE = 5000;
const int m  = 300;
const int VOLUME_THRESHOLD = 10000000.0;  // 1000万股（转换为数值阈值）

    // 股票每日交易详情结构体
    typedef struct stockdetail {
        string date; //日期
        double openprice; //开盘价
        double closeprice; //收盘价
        double highest; //最高价
        double lowest; //最低价
        double volume; //成交量（单位：股）
        double turnover; //成交额
        string changerate; //换手率
        string risefallrate; //涨跌幅（字符串格式，可能带%）
        double risefallamount; //涨跌额
    } stockdetail; //股票的价格信息

    // 股票基本信息结构体
    typedef struct stock {
        string stockcode; //股票代码
        string indusname; //股票简称
        string sname; //英文名
        string briname; //行业编码
        string pricate; //一级门类（即行业大类）
        string seccate; //二级门类（即行业二级类）
        string listexchange; //上市交易所
        string wholecompany; //公司全称（即机构名称）
        string launchdate; //上市日期
        string provin; //省份
        string city; //城市
        string legalper; //法人
        string addr; //地址
        string url; //网址
        string email; //邮箱
        string callnum; //电话
        string mainbus; //主营业务
        string scopeofbus; //经营范围
        stockdetail *detail; //存放该股票每一日的价格信息
        int detail_count; //detail数组的长度
    } stock;

    // 二叉树节点（决策树节点）
    typedef struct BiTNode {
        vector<stock> stocks; // 该节点包含的股票集合
        struct BiTNode *lchild, *rchild; // 左孩子（满足决策条件）、右孩子（不满足）
        string decision; // 该节点的决策条件描述
    } BiTNode, *BiTree;

    // 股票顺序表结构
    typedef struct {
        stock *elem; // 指向数组的指针
        int length; // 数组的长度（下标从1开始）
    } SqList;

    // -------------------------- 1. 顺序表初始化（下标从1开始） --------------------------
    void InitList(SqList &L) {
        L.elem = new stock[MAXSIZE + 1];
        L.length = 0;
    }

    // -------------------------- 2. 顺序表内存释放 --------------------------
    void FreeList(SqList &L) {
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

        sort(tempDetails.begin(), tempDetails.end(), [](const stockdetail &a, const stockdetail &b) {
            return a.date > b.date;
        });

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

    void ReadFile(SqList &L, string infoFilename, string detailDir) {
        ifstream infoFile(infoFilename);
        if (!infoFile.is_open()) {
            cerr << "股票基础信息文件打开失败！" << endl;
            return;
        }

        string line, stockBlock;
        while (getline(infoFile, line)) {
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
                    // 去除字段值前后空格
                    while (!value.empty() && isspace(value[0])) value.erase(0, 1);
                    while (!value.empty() && isspace(value.back())) value.pop_back();

                    if (key == "CODE") s.stockcode = value;
                    else if (key == "股票简称") s.indusname = value;
                    else if (key == "英文名") s.sname = value;
                    else if (key == "行业编码") s.briname = value;
                    else if (key == "行业大类") s.pricate = value;
                    else if (key == "行业二级类") s.seccate = value; // 决策条件1：二级门类
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

    void InitTree(BiTree &BT) {
        BT = new BiTNode;
        BT->lchild = NULL;
        BT->rchild = NULL;
        BT->decision = "根节点";

        BiTree b1 = new BiTNode;
        b1->decision = "二级门类为计算机、通信和其他电子设备制造业";
        b1->lchild = NULL;
        b1->rchild = NULL;
        BiTree b2 = new BiTNode;
        b2->decision = "二级门类不为计算机、通信和其他电子设备制造业";
        b2->lchild = NULL;
        b2->rchild = NULL;

        BiTree b3 = new BiTNode;
        b3->decision = "最近一天跌涨幅>0";
        b3->lchild = NULL;
        b3->rchild = NULL;
        BiTree b4 = new BiTNode;
        b4->decision = "最近一天跌涨幅<=0";
        b4->lchild = NULL;
        b4->rchild = NULL;

        BiTree b5 = new BiTNode;
        b5->decision = "最近一天成交量>1000万股";
        b5->lchild = NULL;
        b5->rchild = NULL;
        BiTree b6 = new BiTNode;
        b6->decision = "最近一天成交量<=1000万股";
        b6->lchild = NULL;
        b6->rchild = NULL;

        BT->lchild = b1;
        BT->rchild = b2;
        b1->lchild = b3;
        b1->rchild = b4;
        b3->lchild = b5;
        b3->rchild = b6;
    }

    void insertNodeToTree(BiTree &root, BiTree &node) {
    }

    void createDecisionTree(BiTree &BT, SqList L) {
        if (BT == nullptr) return;

        string targetSeccate = "计算机、通信和其他电子设备制造业";

        for (int i = 1; i <= L.length; i++) {
            stock currStock = L.elem[i];
            BiTree currNode = BT;

            if (currStock.detail_count == 0) continue;

            if (currStock.seccate == targetSeccate) {
                currNode = currNode->lchild;
            } else {
                currNode = currNode->rchild;
                currNode->stocks.push_back(currStock);
                continue;
            }

            stockdetail latestDetail = currStock.detail[0];
            string riseRateStr = latestDetail.risefallrate;
            riseRateStr.erase(remove(riseRateStr.begin(), riseRateStr.end(), '%'), riseRateStr.end());
            double riseRate = stod(riseRateStr);

            if (riseRate > 0) {
                currNode = currNode->lchild;
            } else {
                currNode = currNode->rchild;
                currNode->stocks.push_back(currStock);
                continue;
            }

            double volume = latestDetail.volume;
            if (volume > VOLUME_THRESHOLD) {
                currNode = currNode->lchild;
            } else {
                currNode = currNode->rchild;
            }
            currNode->stocks.push_back(currStock);
        }
    }

    BiTree FindNodeByName(BiTree BT, string name) {
        if (BT == nullptr) return nullptr;


        for (auto &s: BT->stocks) {
            if (s.indusname == name) {
                return BT;
            }
        }

        BiTree leftRes = FindNodeByName(BT->lchild, name);
        if (leftRes != nullptr) return leftRes;
        BiTree rightRes = FindNodeByName(BT->rchild, name);
        if (rightRes != nullptr) return rightRes;

        return nullptr;
    }

    BiTree FindOriginalFather(BiTree BT, BiTree t) {
        if (BT == nullptr || t == nullptr) return nullptr;

        if (BT->lchild == t || BT->rchild == t) {
            return BT;
        }

        BiTree leftFather = FindOriginalFather(BT->lchild, t);
        if (leftFather != nullptr) return leftFather;
        return FindOriginalFather(BT->rchild, t);
    }

    void FindClass(BiTree &BT, string name) {
        BiTree targetNode = FindNodeByName(BT, name);
        if (targetNode == nullptr) return;

        vector<string> decisions;
        BiTree currNode = targetNode;
        BiTree fatherNode = nullptr;

        while (true) {
            fatherNode = FindOriginalFather(BT, currNode);
            if (fatherNode == nullptr) break;

            if (fatherNode->lchild == currNode) {
                decisions.push_back(fatherNode->lchild->decision);
            } else {
                decisions.push_back(fatherNode->rchild->decision);
            }

            currNode = fatherNode;
        }

        for (auto &dec: decisions) {
            cout << dec << endl;
        }
    }

    void jueceshu() {
        SqList L;
        InitList(L);
        string infoFilename = "D:/file/学习/大二下/数据结构/课了个设/课设数据2025/课设数据2025/stocks.txt";
        string detailDir = "D:/file/学习/大二下/数据结构/课了个设/课设数据2025/课设数据2025/detail";
        ReadFile(L, infoFilename, detailDir);

        BiTree BT;
        InitTree(BT);

        createDecisionTree(BT, L);

        string stockName;
        cin.ignore();
        getline(cin, stockName);

        FindClass(BT, stockName);


        FreeList(L);
    }
}
