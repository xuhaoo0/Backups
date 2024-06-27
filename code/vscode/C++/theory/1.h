/*
DFA的设计
1、设计出被接受的路径和沿途的状态
2、每个状态对于每个字母都有一个箭头指出，去补充这些情况即可；
3、若死机，指向一个死机状态

NFA->DFA
1、写出NFA状态转移表
2、确定DFA起始状态：NFA的起始状态和它在不输入的情况下达到的状态的集合
3、依据1和2来写DFA状态转移表
4、画出DFA图：只要包括了NFA接收状态的，都是DFA的接受状态

正则运算封闭性  p36
正则表达式->NFA  p42
1、并：添加一个新的起始状态，用e指向各个起始状态
2、连接：第一个机器的所有接收状态用e指向第二个的起始状态
3、星号：添加一个新的起始状态(并把它写为接受状态)，用e指向原来的起始状态，再把原来的接收状态用e指向原来的起始状态
4、单个字母：->起始状态->(输入a)接收状态

DFA->正则表达式  p46
1、添加新的起始状态s，接受状态a，改一下原图
2、逐个删去每个中间顶点，添加新路径

泵定理证明非正则语言  p49
格式：
任意p>0，
令w=...
任意的x,y,z(w=xyz,|y|>0,|xy|<p)
令i=...  (i>=0即可)
x yi z 不属于A
所以是非正则语言

给出格局序列（见作业）

给出图灵机判定器的说明（见作业）
格式：
构造如下图灵机
M=“对于输入串w
1）...
2）...
...
n）...拒绝...接受...
”
若...则停机接受；否则停机拒绝。
一方面，...是M的语言，另一方面，M对所有输入串都能停机。
所以是判定器。

证明图灵机的封闭性（见作业）
格式同上；
但要图灵机注意内容；

证明给定的语言可判定（见作业）：
格式同上；
1、构造该语言的DFA a
2、构造一个新的DFA b，使得L(b)=...
3、标记所有与起始状态连通的状态...
4、依据b的性质设计接受情况和拒绝情况

证明p
格式：最后一句话改为“所以M是多项式时间判定器，所以...∈P”

证明np
格式：
构造如下非确定图灵机
M=“
k)  非确定地选择...
”
若...则一定有分支接受；否则所有分支拒绝。
所以...是M的语言，且M的所有分支都在O(n2)时间内运行。
所以M是...的多项式时间非确定判定器。
所以...∈NP.

证明np完全性
举例：
1、证明HALF-CLIQUE∈NP
2、证明CLIQUE可以多项式时间映射归约到HALF-CLIQUE







*/