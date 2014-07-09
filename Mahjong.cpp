#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iterator>
//#include "jsoncpp/json.h"

using namespace std;

vector<string> cards; //手中的牌
int myID;
int notfrozen; //手中没有杠、碰的牌
int operID;
int cnt[3][10]; //手中每张牌的计数，按码牌习惯，0-万，1-条。2-饼
int sea[3][10]; //排池（已经被打出去的牌）和手中的牌的总数，这个AI版本暂时用不到
int chk[5]; //0不用，chk[i]表示手中张数为i的牌的组数，比如ch[3] = 2 表示手里有两组三张一样的牌
int value[19]; //牌估价，表示有value[i]个句子包含cards[i]
string output;
string now;

inline int hsh(char T)
{
    if(T == 'W') return 0;
    if(T == 'T') return 1;
    if(T == 'B') return 2;
}

inline int a(int i)
{
    return hsh(cards[i][0]);
}

inline int b(int i)
{
    return cards[i][1]-'0';
}

void numcheck()
{
    memset(cnt,0,sizeof(cnt));
    notfrozen = 0;
    sort(cards.begin(),cards.end());
    int i = 0;
    while(cards[i][0] < 'a')
    {
        ++notfrozen;
        ++cnt[a(i)][b(i)];
        ++cnt[a(i)][0];
    }
    for(int i = 0;i <= 2;++i)
        for(int j = 1;j <= 9;++j)
            ++chk[cnt[i][j]];
}

void valuecheck()
{
    for(int i = 0;i < notfrozen;++i)
    {
        value[i] += (cnt[a(i)][b(i)] > 2); //是碰……
        if(b(i) <= notfrozen - 2)value[i] += (cnt[a(i)][b(i)+1] > 0 && cnt[a(i)][b(i)+2] > 0);
        if(b(i) <= notfrozen - 1 && b(i) >= 1) value[i] += (cnt[a(i)][b(i)+1] > 0 && cnt[a(i)][b(i)-1] > 0);
        if(b(i) >= 2)value[i] += (cnt[a(i)][b(i)-1] > 0 && cnt[a(i)][b(i)-2] > 0);
    }
}

int minvalue()
{
    for(int i = 0;i < notfrozen;++i)
        if(value[i] == 0 && count(cards.begin(),cards.end(),cards[i]) == 1) return i;
    for(int i = 0;i < notfrozen;++i)
        if(value[i] == 1 && count(cards.begin(),cards.end(),cards[i]) == 1) return i;
    for(int i = 0;i < notfrozen;++i)
        if(value[i] == 0 && count(cards.begin(),cards.end(),cards[i]) >= 3) return i;
    for(int i = 0;i < notfrozen;++i)
        if(value[i] == 0) return i;
    for(int i = 0;i < notfrozen;++i)
        if(value[i] == 1) return i;
    for(int i = 0;i < notfrozen;++i)
        if(value[i] == 2) return i;
    for(int i = 0;i < notfrozen;++i)
        if(value[i] == 3) return i;
}

int minvalue(string T)
{
    for(int i = 0;i < notfrozen;++i)
    {
        if(cards[i] == T) continue;
        if(value[i] == 0 && count(cards.begin(),cards.end(),cards[i]) == 1) return i;
    }
    for(int i = 0;i < notfrozen;++i)
    {
        if(cards[i] == T) continue;
        if(value[i] == 1 && count(cards.begin(),cards.end(),cards[i]) == 1) return i;
    }
    for(int i = 0;i < notfrozen;++i)
    {
        if(cards[i] == T) continue;
        if(value[i] == 0 && count(cards.begin(),cards.end(),cards[i]) >= 3) return i;
    }
    for(int i = 0;i < notfrozen;++i)
    {
        if(cards[i] == T) continue;
        if(value[i] == 0) return i;
    }
    for(int i = 0;i < notfrozen;++i)
    {
        if(cards[i] == T) continue;
        if(value[i] == 1) return i;
    }
    for(int i = 0;i < notfrozen;++i)
    {
        if(cards[i] == T) continue;
        if(value[i] == 2) return i;
    }
    for(int i = 0;i < notfrozen;++i)
    {
        if(cards[i] == T) continue;
        if(value[i] == 3) return i;
    }
}

bool checkhu(int num[10]) //感觉是Sample里一个很nice的实现，抄过来，传参直接传cnt[i]就好
{
/*
 大概意思：
 只判断一种花色
 num[0]表示距离胡牌还差几张,初始为这种花色牌总数
 num[1]到num[9]表示这张牌还有几张
 另外这个函数判断的是能否胡，是胡牌的必要条件而不是充分条件
*/

    for (int i = 1; i <= 9; i++)
		if (num[i] < 0)
			return false;
	if (num[0] == 0)
		return true;
	if (num[0] == 1)
		return false;
	if (num[0] == 2)
	{
		// 剩下两张将牌
		for (int i = 1; i <= 9; i++)
			if (num[i] == 2)
				return true;
		return false;
	}

	for (int i = 1; i <= 9; i++)
	{
		if (num[i] > 0)
		{
			if (i <= 7)
			{
				// ABC型句子
				num[i]--, num[i + 1]--, num[i + 2]--;
				num[0] = num[0] - 3;
				if (checkhu(num))
					return true;
				num[0] = num[0] + 3;
				num[i]++, num[i + 1]++, num[i + 2]++;
			}
			if (num[i] >= 3)
			{
				// AAA型句子
				num[i] = num[i] - 3;
				num[0] = num[0] - 3;
				if (checkhu(num))
					return true;
				num[0] = num[0] + 3;
				num[i] = num[i] + 3;
			}
		}
	}

	return false;
}

inline bool hu()
{
    return(checkhu(cnt[1]) && checkhu(cnt[2]) && checkhu(cnt[3]));
}

void decide()
{
    if(hu()) {output = "HU"; return;}
    int sl = count(cards.begin(),cards.end(),now);
    int pos = 0;
    while(pos < cards.size() && cards[pos] != now) ++pos;
    if(operID == 2)
    {
        if(sl == 3 && value[pos] == 1) {output = "GANG "; output += now;}
        else {output = "PLAY "; output += cards[minvalue()];}
    }
    else if(operID == 3)
    {
        if(sl == 2 && chk[2] >= 2) {output = "PENG ";output += cards[minvalue(now)];}
        else if(sl == 3 && value[pos] == 1) {output = "GANG";}
    }
}

int main()
{

    /*
    输入要实现的功能:
    第一回合获取编号后放在myID里；
    第二回合获取手牌后放在cards里；
    其他回合把手牌放在cards里，待处理的牌（别人打出的或者自己摸到的）放在now里，把输入信息中的第一个数字放在operID里;
    复盘的时候要保证cards里面没有空元素
    如果是只需要输出PASS的话请直接goto输出部分……
    */
    cards.push_back(now);
    numcheck();
    valuecheck();
    decide();
    /*
    输出部分已经放在output里，不过我感觉会有很多bug，建议先判一下是不是空
    */

    return 0;
}
