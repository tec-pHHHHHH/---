#include <bits/stdc++.h>

using namespace std;

namespace eleven {
    const int MAXSIZE = 5000;

    typedef struct stockdetail {
        string date; // 交易日期
        double openprice; // 开盘价
        double closeprice; // 收盘价
        double highest; // 最高价
        double lowest; // 最低价
        double volume; // 成交量
        double turnover; // 成交额
        string changerate; // 换手率
        string risefallrate; // 涨跌幅
        double risefallamount; // 涨跌额
    } stockdetail;

    // 股票基本信息结构体
    typedef struct stock {
        string stockcode; // 股票代码
        string indusname; // 股票简称
        string sname; // 英文名
        string briname; // 行业编码（用于分类的关键字段）
        string pricate; // 一级门类（即行业大类）
        string seccate; // 二级门类（即行业二级类）
        string listexchange; // 上市交易所
        string wholecompany; // 公司全称（即机构名称）
        string launchdate; // 上市日期
        string provin; // 省份
        string city; // 城市
        string legalper; // 法人
        string addr; // 地址
        string url; // 网址
        string email; // 邮箱
        string callnum; // 电话
        string mainbus; // 主营业务
        string scopeofbus; // 经营范围
        stockdetail *detail; // 指向每日交易详情数组的指针
        int detail_count; // 交易详情记录数量
    } stock;

    typedef struct {
        stock *elem;
        int length;
    } SqList;

    void InitList(SqList &L) {
        L.elem = new stock[MAXSIZE + 1];
        L.length = 0;
    }

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

    void HeapAdjust(SqList &L, int s, int m, int &kcn, int &rmn) {//s代表根，m代表最后一个参数
        stock temp = L.elem[s];
        rmn++;

        for (int j = 2 * s; j <= m; j *= 2) {
            if (j < m) {
                kcn++;
                if (L.elem[j].detail[0].volume < L.elem[j + 1].detail[0].volume) {//左右
                    j++;
                }
            }

            kcn++;
            if (temp.detail[0].volume >= L.elem[j].detail[0].volume) {//需不需要调整
                break;
            }

            L.elem[s] = L.elem[j];
            rmn++;
            s = j;
        }

        L.elem[s] = temp;
        rmn++;
    }

    void CreatHeap(SqList &L, int &kcn, int &rmn) {
        int n = L.length;
        for (int i = n / 2; i >= 1; i--) {//最后一个父亲节点开始操作
            HeapAdjust(L, i, n, kcn, rmn);
        }
    }


    void dui() {
        SqList L;
        InitList(L); // 初始化顺序表
        string infoFile = "D:/file/学习/大二下/数据结构/课了个设/课设数据2025/课设数据2025/stocks.txt", detailDir =
                "D:/file/学习/大二下/数据结构/课了个设/课设数据2025/课设数据2025/detail";
        ReadFile(L, infoFile, detailDir); // 加载数据
        string targetInd, targetDate;
        cin >> targetInd >> targetDate; // 输入目标行业和日期
        int k = 0;
        for (int i = 1; i <= L.length; ++i) {
            // 检查行业匹配
            if (L.elem[i].briname != targetInd) continue;
            bool ok = false;
            // 在交易记录中查找目标日期
            for (int j = 0; j < L.elem[i].detail_count; ++j) {
                if (L.elem[i].detail[j].date == targetDate) {
                    // 将目标日期记录交换到首位（便于后续堆排序）
                    swap(L.elem[i].detail[0], L.elem[i].detail[j]);
                    ok = true;
                    break;
                }
            }
            // 若找到则加入筛选列表
            if (ok) L.elem[++k] = L.elem[i];
        }
        L.length = k; // 更新有效长度
        int num = 0;
        int kcn = 0, rmn = 0;
        CreatHeap(L, kcn, rmn);
        while (L.length > 0) {
            // 输出堆顶元素（当前成交量最大）
            if (num == 3)
                break;
            stock s = L.elem[1];
            cout << "股票代码: " << s.stockcode << '\n'
                    << "股票简称: " << s.indusname << '\n'
                    << "行业编码: " << s.briname << '\n'
                    << "一级门类: " << s.pricate << '\n'
                    << "二级门类: " << s.seccate << '\n'
                    << "公司全称: " << s.wholecompany << '\n'
                    << "日期: " << s.detail[0].date
                    << " 成交量: " << (long long) s.detail[0].volume << '\n'
                    << "----------------------------\n";

            // 堆顶与末尾元素交换
            L.elem[1] = L.elem[L.length--];
            // 调整新堆顶
            HeapAdjust(L, 1, L.length, kcn, rmn);
            num++;
        }
        cout << "总的关键字比较次数KCN为：" << kcn << endl;
        cout << "总的记录移动次数RMN为：" << rmn << endl;
        FreeList(L); // 释放内存
    }
}
