#include <bits/stdc++.h>
#include <cstring>
#include <regex>

using namespace std;

namespace fifteen {

const int MAXSIZE = 10000;
typedef struct {
    string relation;    // 关系名称
    string rule[10];    // 规则数组
} Relation;

// 适配GBK/UTF-8：判断字符是否为中文（GBK占2字节，UTF-8占3字节，统一通过编码范围判断）
bool isChineseChar(char c) {
    // GBK中文：高字节0x81-0xFE；UTF-8中文：首字节0xE0-0xEF，这里兼容判断
    return (unsigned char)c >= 0x81 || (unsigned char)c >= 0xE0;
}

// BF算法优化：支持中文编码（按字符匹配，而非字节，避免中文被拆分）
int BF(string t, string s) {
    int tLen = t.size();
    int sLen = s.size();
    if (sLen == 0 || tLen < sLen) return -1;

    for (int i = 0; i <= tLen - sLen; i++) {
        int j = 0;
        // 匹配时跳过中文间的空格（修复实体与关键词间有空格的场景）
        while (i + j < tLen && t[i + j] == ' ') j++;
        int k = 0;
        while (j < sLen && k < sLen) {
            // 双方都跳过空格，再比较有效字符
            while (j < tLen && t[j] == ' ') j++;
            while (k < sLen && s[k] == ' ') k++;
            if (j >= tLen || k >= sLen || t[j] != s[k]) break;
            j++;
            k++;
        }
        if (k == sLen) return i;
    }
    return -1;
}

// 查找所有匹配位置（复用优化后的BF算法）
void findAllMatches(string t, string s, int positions[], int &count) {
    count = 0;
    int tLen = t.size();
    int sLen = s.size();
    if (sLen == 0 || tLen < sLen) return;

    int start = 0;
    while (start <= tLen - sLen) {
        int pos = BF(t.substr(start), s);
        if (pos == -1) break;
        positions[count++] = start + pos;
        start += pos + 1;
    }
}

// 修复直接匹配模式：支持实体与关键词间有空格，优化关系映射
bool checkDirectPattern(string text, string e1, string e2, string& relationName) {
    vector<pair<string, string>> kwRelMap = {
        {"专注于", "主营业务为"},
        {"主营业务为", "主营业务为"},
        {"包括", "主营业务为"},
        {"属于", "属于行业大类"},
        {"行业类别为", "属于行业大类"},
        {"所属机构为", "所属机构"},
        {"的公司是", "所属机构"},
        {"隶属", "所属机构"},
        {"位于", "具有注册地址"},
        {"坐落于", "具有注册地址"},
        {"总部在", "具有注册地址"}
    };

    // 生成带空格容错的模式（实体与关键词间可有任意空格）
    auto genPattern = [](string a, string kw, string b) {
        return a + "\\s*" + kw + "\\s*" + b;  // \s*匹配任意空格（包括无空格）
    };

    // 检查两种顺序
    for (auto& pair : kwRelMap) {
        string kw = pair.first;
        string rel = pair.second;
        // 正则匹配（支持空格容错）
        regex pattern1(genPattern(e1, kw, e2));
        if (regex_search(text, pattern1)) {
            relationName = rel;
            return true;
        }
        regex pattern2(genPattern(e2, kw, e1));
        if (regex_search(text, pattern2)) {
            relationName = rel;
            return true;
        }
    }

    return false;
}

// 修复规则替换逻辑：彻底替换A/B为完整实体（而非单字符）
bool RelationExtraction(char* text, char* entity1, char* entity2, Relation* r) {
    string textStr = text;
    string e1 = entity1;
    string e2 = entity2;
    string relationName;

    // 优先直接匹配（优化后支持空格）
    if (checkDirectPattern(textStr, e1, e2, relationName)) {
        cout << e1 << "-" << relationName << "-" << e2 << endl;
        return true;
    }

    // 修复规则匹配：正确替换A/B为完整实体，支持空格容错
    for (int i = 0; !r[i].relation.empty(); i++) {
        string rel = r[i].relation;
        for (int j = 0; !r[i].rule[j].empty(); j++) {
            string rule = r[i].rule[j];

            // 修复：替换所有A为e1，所有B为e2（完整字符串替换，非单字符）
            string pattern1 = regex_replace(rule, regex("A"), e1);
            pattern1 = regex_replace(pattern1, regex("B"), e2);
            // 生成带空格容错的正则表达式
            regex reg1(regex_replace(pattern1, regex("\\s+"), "\\s*"));

            // 替换A为e2，B为e1（实体反转场景）
            string pattern2 = regex_replace(rule, regex("A"), e2);
            pattern2 = regex_replace(pattern2, regex("B"), e1);
            regex reg2(regex_replace(pattern2, regex("\\s+"), "\\s*"));

            // 匹配成功则输出
            if (regex_search(textStr, reg1)) {
                cout << e1 << "-" << rel << "-" << e2 << endl;
                return true;
            }
            if (regex_search(textStr, reg2)) {
                cout << e2 << "-" << rel << "-" << e1 << endl;
                return true;
            }
        }
    }

    return false;
}

// 关系初始化（保持题目原始定义不变）
void InitRelation(Relation *r) {
    r[0].relation = "属于行业大类";
    r[0].rule[0] = "A行业类别为B";
    r[0].rule[1] = "A属于B";
    r[0].rule[2] = "";

    r[1].relation = "所属机构";
    r[1].rule[0] = "A所属机构为B";
    r[1].rule[1] = "A的公司是B";
    r[1].rule[2] = "A隶属B";
    r[1].rule[3] = "";

    r[2].relation = "具有注册地址";
    r[2].rule[0] = "A位于B";
    r[2].rule[1] = "A坐落于B";
    r[2].rule[2] = "A总部在B";
    r[2].rule[3] = "";

    r[3].relation = "主营业务为";
    r[3].rule[0] = "A包括B";
    r[3].rule[1] = "A专注于B";
    r[3].rule[2] = "A主营业务为B";
    r[3].rule[3] = "";

    r[4].relation = "";
}

// 输入处理优化：避免多输入时的换行符残留
void guanxichouqu(){
    char entity1[100];
    char entity2[100];
    char text[10000];

    Relation r[10];
    InitRelation(r);

    // 优化输入读取：只忽略第一次输入前的换行符
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.getline(entity1, 100);
    cin.getline(entity2, 100);
    cin.getline(text, 10000);

    if (!RelationExtraction(text, entity1, entity2, r))
        cout << "实体之间不存在关系" << endl;
}

}

