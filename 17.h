#include <bits/stdc++.h>

using namespace std;

namespace seventeen {
    const int MVNum = 30000;
    // 定义关系数组，用于表示边的类型
    string Relationship[] = {"所属机构", "上市于", "具有行业编码", "属于行业大类", "属于行业二级类", "位于省份", "位于城市", "具有注册地址", "主营业务为"};

    // 定义顶点数组，用于表示实体类型和内容
    string Entity[] = {"股票简称", "机构", "上市交易所", "行业编码", "行业大类", "行业二级类", "省份", "城市", "地址", "主营业务"};

    // 定义边结点结构体
    typedef struct ArcNode {
        int adjvex; // 该边所指向顶点的位置
        int relationship; // 表示边的类型，即关系的类型，对应为数组下标
        struct ArcNode *nextarc; // 下一条边
    } ArcNode;

    // 定义顶点结点结构体
    typedef struct VNode {
        int entity; // 表示顶点的类型，即实体的类型，对应为数组下标
        string info; // 表示顶点的内容，即实体的内容
        ArcNode *firstarc; // 指向第一条依附该顶点的边的指针
    } VNode, AdjList[MVNum];

    // 定义图的结构体
    typedef struct {
        AdjList vertices; // 邻接表
        int vexnum, arcnum; // 图的当前顶点数和边数
    } ALGraph;


    // 辅助函数：去除字符串首尾空白
    string trim(const string &s) {
        auto start = s.begin();
        while (start != s.end() && isspace(*start)) {
            start++;
        }
        auto end = s.end();
        do {
            end--;
        } while (distance(start, end) > 0 && isspace(*end));
        return string(start, end + 1);
    }


    int LocateEntity(string str) {
        // 返回str在Entity数组中的位置
        for (int i = 0; i < 10; i++)
            if (Entity[i] == str) return i;
        return -1;
    }

    int LocateRelationship(string str) {
        // 返回str在Relationship数组中的位置
        for (int i = 0; i < 9; i++) {
            if (Relationship[i] == str) return i;
        }
        return -1;
    }


    void InitALGraph(ALGraph &G) {
        // InitALGraph函数，初始化邻接表 初始化顶点数和边数为0
        G.vexnum = G.arcnum = 0;
        for (int i = 0; i < MVNum; i++) {
            G.vertices[i].entity = -1;
            G.vertices[i].info = "";
            G.vertices[i].firstarc = NULL;
        }
    }

    void CreateAdjList(ALGraph &G, string filename) {
        ifstream ifs(filename);
        string line;
        int p = 0;
        while (getline(ifs, line)) {
            // 第一步：彻底去除字符串前后所有空格、换行符、制表符
            // 去前导空白（空格、\n、\r、\t）
            size_t start = line.find_first_not_of("\n\r\t");
            if (start == string::npos) continue; // 空行直接跳过
            // 去尾随空白
            size_t end = line.find_last_not_of("\n\r\t");
            line = line.substr(start, end - start + 1); // 截取有效部分

            line = trim(line);

            // 第二步：找最后一个空格，分割为「具体信息」和「实体类型」
            size_t last_space = line.rfind(' ');
            if (last_space == string::npos) continue; // 无空格，格式错误跳过

            size_t first_space = line.find(' ');
            if (first_space == string::npos) continue;

            string s1 = line.substr(0, last_space); // 最后一个空格前：具体信息

            // string s1 = line.substr(0, first_space); // 最后一个空格前：具体信息
            string s2 = line.substr(last_space + 1); // 最后一个空格后：实体类型

            s1 = trim(s1);
            s2 = trim(s2);

            // 存入邻接表（s1、s2已确保前后无空白）
            G.vertices[p].entity = LocateEntity(s2);
            G.vertices[p].info = s1;
            p++;
        }
        G.vexnum = p;
    }

    int LocateVex(ALGraph &G, string str) {
        // 返回str在AdjList中的位置
        for (int i = 0; !G.vertices[i].info.empty(); i++) {
            if (G.vertices[i].info == str) {
                return i;
            }
        }
        return -1;
    }

    void CreateUDG(ALGraph &G, string filename) {
        ifstream ifs(filename);
        string line;
        while (getline(ifs, line)) {
            // 第一步：彻底去除字符串前后所有空格、换行符、制表符
            size_t start = line.find_first_not_of("\n\r\t");
            if (start == string::npos) continue; // 空行直接跳过
            size_t end = line.find_last_not_of("\n\r\t");
            line = line.substr(start, end - start + 1); // 截取有效部分

            line = trim(line);

            // 第二步：找第一个空格（分割头实体s1）
            size_t pos1 = line.find(' ');
            if (pos1 == string::npos) continue; // 无第一个空格，格式错误跳过
            string s1 = line.substr(0, pos1); // 第一个空格前：头实体

            // 第三步：从第一个空格后找第二个空格（分割关系s2）
            size_t pos2 = line.find(' ', pos1 + 1);
            if (pos2 == string::npos) continue; // 无第二个空格，格式错误跳过
            string s2 = line.substr(pos1 + 1, pos2 - (pos1 + 1)); // 两个空格之间：关系

            // 第四步：第二个空格后：尾实体s3（允许含空格，且前后无空白）
            // size_t pos3 = line.find(' ', pos2 + 1);
            // if (pos3 == std::string::npos) pos3 = 11451419;
            // string s3 = line.substr(pos2 + 1, pos3 - (pos2 + 1));
            string s3 = line.substr(pos2 + 1);

            s1 = trim(s1);
            s2 = trim(s2);
            s3 = trim(s3);

            // 查找实体位置（确保实体存在）
            int p1 = LocateVex(G, s1);
            int p2 = LocateVex(G, s3);
            if (p1 == -1 || p2 == -1) continue;

            // 查找关系类型（确保关系存在）
            int rel = LocateRelationship(s2);
            if (rel == -1) continue;

            // 头插法创建正向边（s1 → s2）
            ArcNode *new_node1 = new ArcNode;
            new_node1->adjvex = p2;
            new_node1->relationship = rel;
            new_node1->nextarc = G.vertices[p1].firstarc;
            G.vertices[p1].firstarc = new_node1;
            G.arcnum++;

            // 头插法创建反向边（s2 → s1）
            ArcNode *new_node2 = new ArcNode;
            new_node2->adjvex = p1;
            new_node2->relationship = rel;
            new_node2->nextarc = G.vertices[p2].firstarc;
            G.vertices[p2].firstarc = new_node2;
            G.arcnum++;
        }
    }

    void SearchByEntity(ALGraph &G, const string &entityName) {
        //使用知识图谱进行股票推荐
        // 1. 定位输入实体在邻接表中的顶点位置
        int pos = LocateVex(G, entityName);

        ArcNode *cur = G.vertices[pos].firstarc;

        string attr_names[] = {
            "所属机构",
            "上市于",
            "具有行业编码",
            "属于行业大类",
            "属于行业二级类",
            "位于省份",
            "位于城市",
            "具有注册地址",
            "主营业务为"
        };

        int *list = new int[2000];
        int p = 0;
        while (cur) {
            list[p++] = cur->adjvex;
            cur = cur->nextarc;
        }

        int l = 0, r = p - 1;
        while (l < r) {
            swap(list[l], list[r]);
            l++;
            r--;
        }

        for (int i = 0; i < p; i++) {
            int node = list[i];
            ArcNode *now = G.vertices[node].firstarc;
            while (now) {
                cout << G.vertices[node].info << "->" << attr_names[now->relationship] << "->" << G.vertices[now->
                    adjvex].info << '\n';
                now = now->nextarc;
            }
        }
    }


    void gupiaotuijian() {
        ALGraph G;
        InitALGraph(G);
        CreateAdjList(G, "D:/file/学习/大二下/数据结构/课了个设/课设数据2025/课设数据2025/知识图谱/entity.txt");
        CreateUDG(G, "D:/file/学习/大二下/数据结构/课了个设/课设数据2025/课设数据2025/知识图谱/relation.txt");
        string entityToSearch;
        cin >> entityToSearch;
        SearchByEntity(G, entityToSearch);
    }
}
