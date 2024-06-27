#include <iostream> //完全背包基础版
using namespace std;
int main()
{
    int Weight, N, weight[100], value[100]; //背包容量，物品种类，物品的重量和价值(从1开始)
    cin >> Weight >> N;
    for (int i = 1; i <= N; i++)
        cin >> weight[i] >> value[i];
    int dp[100]; //进阶版：dp[j]的含义：当前背包剩余j容量，从前i个商品(i的信息包含在for循环中)里面选，所得到的最大价值

    for (int j = 0; j <= Weight; j++) //初始化
        dp[j] = 0;

    for (int i = 1; i <= N; i++) //动态规划填表，直到i=N，dp[Weight]
        for (int j = Weight; j >= weight[i]; j--)
            for (int k = 0; k <= j / weight[i]; k++) //考虑在i-1行的基础上，选多少个i能使价值最大
                dp[j] = max(dp[j], dp[j - k * weight[i]] + k * value[i]);

    cout << dp[Weight];
}