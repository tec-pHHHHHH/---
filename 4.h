#include<bits/stdc++.h>

using namespace std;

namespace four {
    const int MAXSIZE = 5000;


    typedef struct stockdetail {
        string date; //日期
        double openprice; //开盘价
        double closeprice; //收盘价
        double highest; //最高价
        double lowest; //最低价
        double volume; //成交量
        double turnover; //成交额
        string changerate; //换手率
        string risefallrate; //涨跌幅
        double risefallamount; //涨跌额
    } stockdetail; //股票的价格信息


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
        stockdetail *detail; //定义一个stockdetail类型的数组用来存放该股票每一日的相关价格信息
        int detail_count; //detail数组的长度
    } stock;

    typedef struct {
        stock *elem; // 指向数组的指针
        int length; // 数组的长度
    } SqList;

    void InitList(SqList &L) {
        // 使用动态内存分配new进行初始化
        L.elem = new stock[MAXSIZE];
        L.length = 0;
    }

    void FreeList(SqList &L) {
        // 释放内存
        delete[] L.elem;
        L.length = 0;
    }

    void ReadDetail(stock &s, string detailDir) {
        // 构建文件路径
        string filename = detailDir + "/" + s.stockcode + ".txt";
        ifstream file(filename);
        // 先统计行数
        int count = 0;
        string line;
        while (getline(file, line)) {
            if (!line.empty()) count++;
        }

        s.detail_count = count; //读完一便文件之后，读到的信息是什么----文件里面有多少条
        s.detail = new stockdetail[count]; // -> 用来初始化

        file.clear();
        file.seekg(0); //将窗口至于最前面，准备重新读文件
        int index = 0;
        while (getline(file, line)) {
            if (line.empty()) continue;
            // 替换分号为空格
            for (char &c: line) {
                if (c == ';') c = ' ';
            }
            stockdetail detail;
            stringstream segmentStream(line);
            // 解析每个字段
            segmentStream >> detail.date;
            segmentStream >> detail.openprice;
            segmentStream >> detail.closeprice;
            segmentStream >> detail.highest;
            segmentStream >> detail.lowest;
            segmentStream >> detail.volume;
            segmentStream >> detail.turnover;
            segmentStream >> detail.changerate;
            segmentStream >> detail.risefallrate;
            segmentStream >> detail.risefallamount;
            s.detail[index++] = detail;
        }
        file.close();
    }

    void ReadFile(SqList &L, string infoFilename, string detailDir) {
        // 从文件中读取股票信息，将其按顺序存入L.elem指向的数组中，数组下标从1开始存储
        ifstream file(infoFilename);
        if (!file.is_open()) {
            return;
        }

        string line;
        int currentStock = -1;

        while (getline(file, line)) {
            if (line.empty()) continue;

            if (line == "#") {
                // 遇到#表示一个股票信息结束，读取其detail信息
                if (currentStock >= 0) {
                    ReadDetail(L.elem[currentStock], detailDir);
                }
                currentStock++;
                continue;
            }

            size_t pos = line.find(':');
            if (pos == string::npos) continue;

            string key = line.substr(0, pos);
            string value = line.substr(pos + 1);

            // 去除value前后的空格
            value.erase(0, value.find_first_not_of(" "));
            value.erase(value.find_last_not_of(" ") + 1);

            if (currentStock < 0) currentStock = 0;
            stock &s = L.elem[currentStock];

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

        // 处理最后一个股票
        if (currentStock >= 0) {
            ReadDetail(L.elem[currentStock], detailDir);
            L.length = currentStock + 1;
        }

        file.close();
    }


    void BubbleSort(SqList &L, int &kcn, int &rmn) {
        kcn = 0;
        rmn = 0;
        bool swapped;
        for (int i = 1; i < L.length; ++i) {
            swapped = false;
            for (int j = 1; j <= L.length - i; ++j) //知识点：每一次冒泡排序后都要少一个元素
            {
                kcn++;
                if (L.elem[j].sname > L.elem[j + 1].sname) {
                    stock temp = L.elem[j];
                    rmn++;
                    L.elem[j] = L.elem[j + 1];
                    rmn++;
                    L.elem[j + 1] = temp;
                    rmn++;
                    swapped = true;
                }
            }
            if (swapped == false) break; //如果内层循环之后还等于false，说明：当前序列已经排好序了，OK！
        }
    }

    void Print(SqList L, int pos) {
        if (pos < 1 || pos > L.length) {
            cout << "无效的位置!" << endl;
            return;
        }
        stock &s = L.elem[pos];
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
        for (int i = 0; i < s.detail_count; ++i) {
            stockdetail &d = s.detail[i];
            cout << "日期: " << d.date;
            cout << " 开盘价: " << fixed << setprecision(3) << fixed << setprecision(3) << d.openprice;
            cout << " 收盘价: " << fixed << setprecision(3) << d.closeprice;
            cout << " 最高价: " << fixed << setprecision(3) << d.highest;
            cout << " 最低价: " << fixed << setprecision(3) << d.lowest;
            cout << " 成交量: " << fixed << setprecision(3) << d.volume;
            cout << " 成交额: " << fixed << setprecision(3) << d.turnover;
            cout << " 换手率: " << fixed << setprecision(3) << d.changerate;
            cout << " 涨跌幅: " << fixed << setprecision(3) << d.risefallrate;
            cout << " 涨跌额: " << fixed << setprecision(3) << d.risefallamount << endl;
        }
    }


    int BinarySearch(SqList &L, string sname) {
        int low = 0; //第一个元素哦
        int high = L.length - 1; //最后一元素捏
        while (low <= high) //硬性条件，不满足就是查找到了
        {
            int mid = (low + high) / 2;
            if (L.elem[mid].sname == sname) {
                return mid; // 找到返回下标
            } else if (L.elem[mid].sname > sname) {
                high = mid - 1;
            } else {
                low = mid + 1;
            }
        }
        return -1;
    }

    double GetASL(SqList &L) //欢迎来到折半查找ASL计算函数
    {
        if (L.length == 0) return 0.0;
        double total = 0.0;
        int n = L.length;
        int level = 1;
        int count = 0;
        while (count < n) {
            int nodesInLevel = 2 ^ (level - 1); //理论上这一层有多少个节点
            int actual = min(nodesInLevel, n - count); //实际含有的值
            total += level * actual; //查找长度
            count += actual; //更新元素个数
            level++; //出发向下一层
        }
        return total / n;
    }


    void BinaryReserch() {
        SqList L;
        InitList(L);
        string infoFilename = "D:/file/学习/大二下/数据结构/课了个设/课设数据2025/课设数据2025/stocks.txt";
        string detailDir = "D:/file/学习/大二下/数据结构/课了个设/课设数据2025/课设数据2025/detail";
        ReadFile(L, infoFilename, detailDir);
        int kcn = 0;
        int rmn = 0;
        BubbleSort(L, kcn, rmn);
        string sname;
        cin.ignore();
        getline(cin, sname);
        cout << "总的关键字比较次数KCN为：" << kcn << endl;
        cout << "总的记录移动次数RMN为：" << rmn << endl;
        int pos = BinarySearch(L, sname);
        if (pos != -1) {
            Print(L, pos);
            double ASL = GetASL(L);
            cout << "ASL为：" << fixed << setprecision(2) << ASL << endl;
        } else {
            cout << "查找失败" << endl;
        }
        FreeList(L);

    }
}
