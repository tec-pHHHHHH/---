#include<bits/stdc++.h>
using namespace std;

namespace one {
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
        L.elem = new stock[MAXSIZE + 1];
        L.length = 0;
    }

    void FreeList(SqList &L) {
        // 释放内存
        for (int i = 1; i < L.length + 1; i++) {
            delete[] L.elem[i].detail; //先删里面的
        }
        delete [] L.elem; //再删整个
        L.length = 0;
    }

    void ReadDetail(stock &s, string detailDir) {
        // 从Detail文件夹中读取Detail信息
        string filename = detailDir + "/" + s.stockcode + ".txt";
        ifstream fin(filename);
        if (!fin.is_open()) {
            s.detail = nullptr;
            s.detail_count = 0;
            return;
        }

        vector<stockdetail> details;
        string line;
        while (getline(fin, line)) {
            if (line.empty()) continue;
            replace(line.begin(), line.end(), ';', ' ');
            istringstream iss(line);
            stockdetail s_temp;
            iss >> s_temp.date
                    >> s_temp.openprice
                    >> s_temp.closeprice
                    >> s_temp.highest
                    >> s_temp.lowest
                    >> s_temp.volume
                    >> s_temp.turnover
                    >> s_temp.changerate
                    >> s_temp.risefallrate
                    >> s_temp.risefallamount;
            details.push_back(s_temp);
        }
        fin.close();

        s.detail_count = details.size();

        if (s.detail_count > 0) {
            s.detail = new stockdetail[s.detail_count];
            for (int i = 0; i < s.detail_count; i++) {
                s.detail[i] = details[i];
            }
        } else {
            s.detail = nullptr;
        }
    }


    void ReadFile(SqList &L, string infoFilename, string detailDir) {
        ifstream fin(infoFilename); //创建一个流对象
        if (!fin.is_open()) {
            cerr << "无法打开股票信息文件！" << endl;
            return;
        }

        string line;
        stock current_stock; // 临时存储当前股票信息
        bool is_collecting = false; // 标记是否正在收集某支股票的字段

        while (getline(fin, line)) {
            if (line.empty()) continue; // 跳过空行

            // 遇到#说明一支股票读取完毕，立即存储当前股票
            if (line[0] == '#') {
                if (is_collecting) // 确保有数据才存储
                {
                    L.length++;
                    L.elem[L.length] = current_stock;
                    ReadDetail(L.elem[L.length], detailDir);
                    current_stock = stock(); // 重置临时变量
                }
                is_collecting = false;
                continue;
            }

            // 开始收集新股票的字段
            is_collecting = true;

            // 拆分“字段名: 值”（核心适配文档格式）
            size_t colon_pos = line.find(':');
            if (colon_pos == string::npos) continue; // 跳过无效行
            string key = line.substr(0, colon_pos);
            string value = line.substr(colon_pos + 1);
            // 去除值前后的空格（避免开头空格影响）
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            // 按字段名匹配赋值（严格对应stocks.txt的字段名）
            if (key == "CODE") current_stock.stockcode = value;
            else if (key == "股票简称") current_stock.indusname = value;
            else if (key == "英文名") current_stock.sname = value;
            else if (key == "行业编码") current_stock.briname = value;
            else if (key == "行业大类") current_stock.pricate = value;
            else if (key == "行业二级类") current_stock.seccate = value;
            else if (key == "上市交易所") current_stock.listexchange = value;
            else if (key == "机构名称") current_stock.wholecompany = value;
            else if (key == "上市日期") current_stock.launchdate = value;
            else if (key == "省份") current_stock.provin = value;
            else if (key == "城市") current_stock.city = value;
            else if (key == "法人") current_stock.legalper = value;
            else if (key == "地址") current_stock.addr = value;
            else if (key == "网址") current_stock.url = value;
            else if (key == "邮箱") current_stock.email = value;
            else if (key == "电话") current_stock.callnum = value;
            else if (key == "主营业务") current_stock.mainbus = value;
            else if (key == "经营范围") current_stock.scopeofbus = value;
        }

        // 处理最后一支股票（文件末尾无#）
        if (is_collecting) {
            L.length++;
            L.elem[L.length] = current_stock;
            ReadDetail(L.elem[L.length], detailDir);
        }
        fin.close();
    }


    void Print(SqList L, int pos) {
        // 输出股票信息，double类型变量保留3位小数，详见输出样例
        if (pos < 1 || pos > L.length) {
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

        for (int i = 0; i < s.detail_count; i++) {
            stockdetail &sd = s.detail[i];
            cout << "日期: " << sd.date
                    << " 开盘价: " << fixed << setprecision(3) << sd.openprice
                    << " 收盘价: " << fixed << setprecision(3) << sd.closeprice
                    << " 最高价: " << fixed << setprecision(3) << sd.highest
                    << " 最低价: " << fixed << setprecision(3) << sd.lowest
                    << " 成交量: " << fixed << setprecision(3) << sd.volume
                    << " 成交额: " << fixed << setprecision(3) << sd.turnover
                    << " 换手率: " << sd.changerate
                    << " 涨跌幅: " << sd.risefallrate
                    << " 涨跌额: " << fixed << setprecision(3) << sd.risefallamount
                    << endl;
        }
    }

    void ReadFile() {
        SqList L;
        InitList(L);
        string infoFilename = "D:/file/学习/大二下/数据结构/课了个设/课设数据2025/课设数据2025/stocks.txt";
        string detailDir = "D:/file/学习/大二下/数据结构/课了个设/课设数据2025/课设数据2025/detail";

        ReadFile(L, infoFilename, detailDir);
        int pos;
        cin >> pos;
        Print(L, pos);
        FreeList(L);
    }
}
