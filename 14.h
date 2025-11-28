#include <bits/stdc++.h>
#include <cstring>

namespace fourteen {

const int MAXSIZE = 10000;
    using namespace std;

    // GBK环境：中文占2字节（高字节 >= 0x81），英文/符号占1字节（<= 0x7F）
    // 计算字符串的字符数（中文算1个，英文/符号算1个）
    int getCharCount(const char *str) {
        int count = 0;
        int len = strlen(str);
        for (int i = 0; i < len;) {
            if ((unsigned char)str[i] >= 0x81) {
                // GBK中文（2字节）：高字节范围 0x81-0xFE，低字节 0x40-0x7E、0x80-0xFE
                // 简化判断：高字节 >=0x81 即视为中文（忽略非法GBK编码）
                count++;
                i += 2;
            } else {
                // 英文/数字/符号（1字节）
                count++;
                i += 1;
            }
        }
        return count;
    }

    // 截取字符串的前n个字符（中文算1个，英文/符号算1个），返回新字符串
    char *substrChar(const char *str, int n) {
        char *res = new char[MAXSIZE];
        int resIdx = 0;
        int len = strlen(str);
        int count = 0;
        for (int i = 0; i < len && count < n;) {
            if ((unsigned char)str[i] >= 0x81) {
                // GBK中文（2字节）
                res[resIdx++] = str[i++];
                res[resIdx++] = str[i++];
                count++;
            } else {
                // 英文/符号（1字节）
                res[resIdx++] = str[i++];
                count++;
            }
        }
        res[resIdx] = '\0';
        return res;
    }

    // BF算法：在文本S中从pos位置开始匹配模式串T（按字节匹配）
    // 返回匹配成功的起始字节索引，失败返回-1
    int BF(const char *S, const char *T, int pos) {
        int sLen = strlen(S);
        int tLen = strlen(T);
        int i = pos; // 文本指针
        int j = 0; // 模式串指针

        while (i < sLen && j < tLen) {
            if (S[i] == T[j]) {
                // 字节匹配成功，双指针后移
                i++;
                j++;
            } else {
                // 匹配失败，文本指针回溯，模式串重置
                i = i - j + 1;
                j = 0;
            }
        }

        // 模式串完全匹配，返回起始索引
        if (j == tLen) {
            return i - j;
        }
        return -1; // 匹配失败
    }

    // 解析规则T，拆分出前缀、后缀和实体长度（*的个数）
    void parseRule(const char *T, char *prefix, char *suffix, int &entityLen) {
        int tLen = strlen(T);
        int starStart = -1, starEnd = -1;

        // 找到所有*的起始和结束位置（*是连续的，1字节）
        for (int i = 0; i < tLen;) {
            if (T[i] == '*') {
                if (starStart == -1) starStart = i;
                starEnd = i;
                i++; // *是1字节
            } else if ((unsigned char)T[i] >= 0x81) {
                // GBK中文跳过2字节
                i += 2;
            } else {
                // 其他1字节字符跳过
                i += 1;
            }
        }

        // 计算实体长度（*的个数）
        entityLen = 0;
        for (int i = starStart; i <= starEnd && i < tLen; i++) {
            if (T[i] == '*') entityLen++;
        }

        // 提取前缀（*左侧部分）
        if (starStart != -1) {
            strncpy(prefix, T, starStart);
            prefix[starStart] = '\0';
        } else {
            prefix[0] = '\0'; // 无前缀
        }

        // 提取后缀（*右侧部分）
        if (starEnd != -1) {
            strcpy(suffix, T + starEnd + 1);
        } else {
            suffix[0] = '\0'; // 无后缀
        }
    }

    // 实体识别核心函数：S为文本，T为规则，返回是否匹配成功
    bool EntityRecognition(const char *S, const char *T) {
        char prefix[MAXSIZE] = {0};
        char suffix[MAXSIZE] = {0};
        int entityLen = 0;
        bool hasMatch = false;

        // 解析规则：拆分前缀、后缀、实体长度
        parseRule(T, prefix, suffix, entityLen);
        int prefixLen = strlen(prefix);
        int suffixLen = strlen(suffix);

        // 情况1：规则只有*（如"***"），匹配所有长度为entityLen的连续中文
        if (prefixLen == 0 && suffixLen == 0) {
            int sCharCount = getCharCount(S);
            int currPos = 0; // 当前字节位置
            int currChar = 0; // 当前字符计数（中文算1个）

            while (currChar + entityLen <= sCharCount) {
                // 截取当前位置开始的entityLen个字符作为实体
                char *entity = substrChar(S + currPos, entityLen);
                // 验证实体是否全为中文（GBK编码）
                bool isAllChinese = true;
                int entityByteLen = strlen(entity);
                for (int i = 0; i < entityByteLen;) {
                    if ((unsigned char)entity[i] < 0x81) {
                        // 不是GBK中文
                        isAllChinese = false;
                        break;
                    }
                    i += 2; // 中文占2字节
                }
                if (isAllChinese) {
                    cout << entity << endl;
                    hasMatch = true;
                }
                delete[] entity;

                // 移动到下一个中文字符（跳过非中文字符）
                while (currPos < (int)strlen(S) && (unsigned char)S[currPos] < 0x81) {
                    currPos++; // 跳过英文/符号
                }
                if (currPos < (int)strlen(S)) {
                    currPos += 2; // 跳过当前中文字符（2字节）
                    currChar++;
                } else {
                    break;
                }
            }
            return hasMatch;
        }

        // 情况2：规则有前缀和/或后缀（如"****隶属于"）
        // 先匹配后缀，再向前截取实体长度，验证前缀
        int pos = 0;
        while (true) {
            // 用BF算法匹配后缀
            int suffixPos = BF(S, suffix, pos);
            if (suffixPos == -1) break; // 无更多后缀匹配

            // 计算实体的起始字节位置：后缀起始位置 - 实体字节长度
            // 实体字节长度 = 实体字符数 * 2（GBK中文占2字节）
            int entityByteLen = entityLen * 2;
            int entityStart = suffixPos - entityByteLen;
            if (entityStart < 0) {
                // 实体超出文本范围，跳过
                pos = suffixPos + 1;
                continue;
            }

            // 截取实体（从entityStart开始，entityByteLen字节）
            char entity[MAXSIZE] = {0};
            strncpy(entity, S + entityStart, entityByteLen);
            entity[entityByteLen] = '\0';

            // 验证实体是否全为中文（GBK）
            bool isAllChinese = true;
            for (int i = 0; i < entityByteLen;) {
                if ((unsigned char)entity[i] < 0x81) {
                    isAllChinese = false;
                    break;
                }
                i += 2;
            }
            if (!isAllChinese) {
                pos = suffixPos + 1;
                continue;
            }

            // 验证前缀（如果有前缀）
            if (prefixLen > 0) {
                int prefixEnd = entityStart; // 前缀结束位置 = 实体起始位置
                int prefixStart = prefixEnd - prefixLen;
                if (prefixStart < 0) {
                    // 前缀超出文本范围，跳过
                    pos = suffixPos + 1;
                    continue;
                }

                // 截取前缀部分
                char actualPrefix[MAXSIZE] = {0};
                strncpy(actualPrefix, S + prefixStart, prefixLen);
                actualPrefix[prefixLen] = '\0';

                // 用BF算法匹配前缀是否一致
                if (BF(actualPrefix, prefix, 0) != 0) {
                    pos = suffixPos + 1;
                    continue;
                }
            }

            // 所有条件满足，输出实体
            cout << entity << endl;
            hasMatch = true;
            pos = suffixPos + 1; // 继续查找下一个匹配
        }

        // 情况3：规则只有前缀（如"乐鑫***"），匹配前缀后向后截取实体
        if (prefixLen > 0 && suffixLen == 0) {
            pos = 0;
            while (true) {
                int prefixPos = BF(S, prefix, pos);
                if (prefixPos == -1) break;

                int entityStart = prefixPos + prefixLen;
                int entityByteLen = entityLen * 2; // GBK中文2字节/个
                int entityEnd = entityStart + entityByteLen;
                if (entityEnd > (int)strlen(S)) {
                    pos = prefixPos + 1;
                    continue;
                }

                char entity[MAXSIZE] = {0};
                strncpy(entity, S + entityStart, entityByteLen);
                entity[entityByteLen] = '\0';

                // 验证实体是否全为中文（GBK）
                bool isAllChinese = true;
                for (int i = 0; i < entityByteLen;) {
                    if ((unsigned char)entity[i] < 0x81) {
                        isAllChinese = false;
                        break;
                    }
                    i += 2;
                }
                if (isAllChinese) {
                    cout << entity << endl;
                    hasMatch = true;
                }

                pos = prefixPos + 1;
            }
        }

        return hasMatch;
    }

    void shitishibie() {
        char keyword[100];
        char text[10000];

        // 读取规则和文本（GBK编码下getline正常处理含空格的字符串）
        cin.ignore(); // 忽略缓冲区残留的换行符
        cin.getline(keyword, 100);
        cin.ignore(); // 忽略两次输入之间的换行符
        cin.getline(text, 10000);

        if (!EntityRecognition(text, keyword)) {
            cout << "无匹配" << endl;
        }
    }
}

