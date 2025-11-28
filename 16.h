#ifndef SIXTTEEN_H
#define SIXTEEEN_H

#include <bits/stdc++.h>
#include <fstream>
#include <sstream>

using namespace std;

namespace sixtheen {
    const int MVNum = 30000;
    string Relationship[] = {"所属机构", "上市于", "具有行业编码", "属于行业大类", "属于行业二级类", "位于省份", "位于城市", "具有注册地址", "主营业务为"};


    string Entity[] = {"股票简称", "机构", "上市交易所", "行业编码", "行业大类", "行业二级类", "省份", "城市", "地址", "主营业务"};


    typedef struct ArcNode {
        int adjvex;
        int relationship;
        struct ArcNode *nextarc;
    } ArcNode;

    typedef struct VNode {
        int entity;
        string info;
        ArcNode *firstarc;
    } VNode, AdjList[MVNum];


    typedef struct {
        AdjList vertices;
        int vexnum, arcnum;
    } ALGraph;

    int LocateEntity(string str) {
        int len = sizeof(Entity) / sizeof(Entity[0]);
        for (int i = 0; i < len; i++) {
            if (Entity[i] == str) {
                return i;
            }
        }
        return -1;
    }

    int LocateRelationship(string str) {
        int len = sizeof(Relationship) / sizeof(Relationship[0]);
        for (int i = 0; i < len; i++) {
            if (Relationship[i] == str) {
                return i;
            }
        }
        return -1;
    }

    void InitALGraph(ALGraph &G) {
        G.vexnum = 0;
        G.arcnum = 0;
        for (int i = 0; i < MVNum; i++) {
            G.vertices[i].entity = -1;
            G.vertices[i].info = "";
            G.vertices[i].firstarc = NULL;
        }
    }

    void CreateAdjList(ALGraph &G, string filename) {
        ifstream infile(filename);
        if (!infile.is_open()) {
            cerr << "实体文件打开失败！" << endl;
            return;
        }

        string line;
        while (getline(infile, line)) {
            if (line.empty()) continue;
            size_t lastSpace = line.find_last_of(' ');
            if (lastSpace == string::npos) continue;
            string entityInfo = line.substr(0, lastSpace);
            string entityType = line.substr(lastSpace + 1);

            int entityIdx = LocateEntity(entityType);
            if (entityIdx == -1) continue;

            int vexIdx = G.vexnum;
            G.vertices[vexIdx].entity = entityIdx;
            G.vertices[vexIdx].info = entityInfo;
            G.vertices[vexIdx].firstarc = NULL;
            G.vexnum++;
        }

        infile.close();
    }

    int LocateVex(ALGraph &G, string str) {
        for (int i = 0; i < G.vexnum; i++) {
            if (G.vertices[i].info == str) {
                return i;
            }
        }
        return -1;
    }

    void InsertArc(ALGraph &G, int startVex, int endVex, int relIdx) {
        ArcNode *newArc = new ArcNode;
        newArc->adjvex = endVex;
        newArc->relationship = relIdx;
        newArc->nextarc = G.vertices[startVex].firstarc;
        G.vertices[startVex].firstarc = newArc;
        G.arcnum++;
    }

    void CreateUDG(ALGraph &G, string filename) {
        ifstream infile(filename);
        if (!infile.is_open()) {
            cerr << "关系文件打开失败！" << endl;
            return;
        }

        string line;
        while (getline(infile, line)) {
            if (line.empty()) continue;
            vector<string> parts;
            stringstream ss(line);
            string part;
            while (ss >> part) {
                parts.push_back(part);
            }
            if (parts.size() < 3) continue;

            string headEntity = "", relation = "", tailEntity = "";
            int relPos = -1;
            int relCount = sizeof(Relationship) / sizeof(Relationship[0]);
            for (int i = 0; i < parts.size(); i++) {
                for (int j = 0; j < relCount; j++) {
                    if (parts[i] == Relationship[j]) {
                        relation = parts[i];
                        relPos = i;
                        break;
                    }
                }
                if (relPos != -1) break;
            }
            if (relPos == -1) continue;

            for (int i = 0; i < relPos; i++) {
                headEntity += (i == 0 ? "" : " ") + parts[i];
            }
            for (int i = relPos + 1; i < parts.size(); i++) {
                tailEntity += (i == relPos + 1 ? "" : " ") + parts[i];
            }

            int headVex = LocateVex(G, headEntity);
            int tailVex = LocateVex(G, tailEntity);
            if (headVex == -1 || tailVex == -1) continue;

            int relIdx = LocateRelationship(relation);
            if (relIdx == -1) continue;

            InsertArc(G, headVex, tailVex, relIdx);
            InsertArc(G, tailVex, headVex, relIdx);
        }

        infile.close();
    }

    void PrintGraph(ALGraph &G) {
        for (int i = 0; i < G.vexnum; i++) {
            VNode currVex = G.vertices[i];
            string headEntity = currVex.info;
            ArcNode *p = currVex.firstarc;
            while (p != NULL) {
                int tailVex = p->adjvex;
                string tailEntity = G.vertices[tailVex].info;
                int relIdx = p->relationship;
                cout << headEntity << " " << relIdx << " " << tailEntity << endl;
                p = p->nextarc;
            }
        }
    }


    void lingjiebiao() {
        ALGraph G;
        InitALGraph(G);
        CreateAdjList(G, "D:/file/学习/大二下/数据结构/课了个设/课设数据2025/课设数据2025/知识图谱/entity.txt");
        CreateUDG(G, "D:/file/学习/大二下/数据结构/课了个设/课设数据2025/课设数据2025/知识图谱/relation.txt");
        PrintGraph(G);
    }
}

#endif