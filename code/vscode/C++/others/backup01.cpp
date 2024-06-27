#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::ios;
using std::ofstream;
using std::string;
using std::vector;
class FCB
{
public:
    int first_block;
    string fileName;
    int size;
    int real_size;
    FCB(int first_block0, const string &fileName0, int size0, int real_size0) : first_block(first_block0),
                                                                                fileName(fileName0),
                                                                                size(size0),
                                                                                real_size(real_size0 * 4)
    {
    }
}; // 文件控制块
class DirItem
{ // 文件树
    string name;
    vector<FCB *> files;    // 目录下的文件
    vector<DirItem *> dirs; // 目录下的文件夹，树节点
public:
    DirItem(const string &name0) : name(name0) {}
    void addFile(int first_block, const string &fileName, int size, int real_size)
    {
        for (auto i = files.begin(); i != files.end(); i++)
        {
            if ((*i)->fileName == fileName)
            {
                cout << "same file name" << endl;
                return;
            }
        }
        files.push_back(new FCB(first_block, fileName, size, real_size));
    }
    void addDir(DirItem *dir)
    {
        dirs.push_back(dir);
    }
    vector<DirItem *> getDirs()
    {
        return dirs;
    }
    vector<FCB *> getFiles()
    {
        return files;
    }
    string getName()
    {
        return name;
    }
    void del_file(const string &fileName)
    {
        for (auto i = files.begin(); i != files.end(); i++)
        {
            if ((*i)->fileName == fileName)
            {
                files.erase(i);
                break;
            }
        }
    }
    void del_dir(const string &fileName)
    {
        for (auto i = dirs.begin(); i != dirs.end(); i++)
        {
            if ((*i)->getName() == fileName)
            {
                dirs.erase(i);
                break;
            }
        }
    }
    void clear()
    {
        for (DirItem *item : dirs)
        {
            delete item;
        }
        for (FCB *item : files)
        {
            delete item;
        }
        dirs.clear();
        files.clear();
    } // 递归删除时用到，删除当前目录下的目录和文件的指针
};

class diskMgr
{                       // 磁盘
    vector<int> blocks; // fat表
    int n = 16;
    int block_num = n * n;
    int block_size = 4;                         // 每块4B
    int disk_capacity = block_num * block_size; // 1M
    vector<vector<int>> bit_map;                // 0空，1有，位视图
public:
    vector<char> disk;
    diskMgr()
    {
        disk = vector<char>(disk_capacity, '\0');
        blocks = vector<int>(block_num, -1);
        bit_map = vector<vector<int>>(n, vector<int>(n, 0));
    }
    int getSize(int blockNum)
    {
        int n = 0;
        while (blockNum != -1)
        {
            n++;
            blockNum = blocks[blockNum];
        }
        return n;
    } // 当前块号开始的文件一个有多少块儿
    int find_empty_block()
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (bit_map[i][j] == 0)
                {
                    return n * i + j;
                }
            }
        }
        return -1;
    } // 找到一个空闲盘块，从位视图
    void update_bit_map(int x, int notempty)
    {
        int i = x / n, j = x % n;
        bit_map[i][j] = notempty;
    } // 更新位视图
    void del(int blockNum)
    {
        while (blockNum != -1)
        {
            for (int i = 0; i < block_size; i++)
            {
                disk[blockNum * block_size + i] = '\0';
            }
            update_bit_map(blockNum, 0);
            int t = blocks[blockNum];
            blocks[blockNum] = -1;
            blockNum = t;
        }
    } // 删除文件
    vector<char> read(int blockNum)
    {
        vector<char> data;
        while (blockNum != -1)
        {
            for (int i = 0; i < block_size; i++)
            {
                data.push_back(disk[blockNum * block_size + i]);
            }
            blockNum = blocks[blockNum];
        }
        return data;
    } // 读取文件
    int write(vector<char> data)
    {
        int first_block = find_empty_block(); // 找到初始位置
        if (first_block == -1)
            return -1;
        int len = int(data.size());
        int blockn = len % block_size == 0 ? len / block_size : len / block_size + 1; // 一共需要的盘块儿数
        int next_block = first_block;
        int temp = next_block;
        int fix = (block_size - len % block_size) % block_size;

        for (int i = 0; i < fix; ++i)
        { // 数据长度对齐
            data.push_back('*');
        }
        for (int i = 0; i < blockn; i++)
        { // 存数据
            if (temp == -1)
            {
                break;
            }
            next_block = temp;
            for (int j = 0; j < block_size; j++)
            {
                disk[next_block * block_size + j] = data[i * block_size + j];
            }
            update_bit_map(next_block, 1);
            temp = find_empty_block();
            blocks[next_block] = temp;
        }
        blocks[next_block] = -1;
        return first_block;
    }
};
#define chart_head std::left << std::setw(len + 1) << std::setfill('-') << ""
#define chart_cell "|" << std::left << std::setw(len) << std::setfill(' ')
class fileMgr
{
    DirItem *root;   // 根节点
    diskMgr dm;      // 磁盘管理器
    DirItem *cur;    // 当前命令行执的工作目录
    string cur_path; // 工作命令
    DirItem *addmdir(DirItem *node, string name)
    {
        DirItem *newNode = new DirItem(name);
        node->addDir(newNode);
        return newNode;
    }

public:
    fileMgr()
    {
        root = new DirItem("/");
        DirItem *temp = addmdir(addmdir(root, "apps"), "tencent");
        addmdir(temp, "qq");
        addmdir(temp, "qqgame");
        addmdir(temp, "qqmusic");
        temp = addmdir(root, "docs");
        addmdir(temp, "words");
        addmdir(temp, "ppts");
        addmdir(temp, "excels");

        root->addDir(new DirItem("pics"));
        root->addDir(new DirItem("musics"));
        cur = root;
        cur_path = "/";
        cout_hint();
    } // 初始化一些文件夹
    void ls()
    {
        int len = 20;
        cout
            << chart_head << chart_head << chart_head << chart_head << endl
            << chart_cell << "file/dir name"
            << chart_cell << "type"
            << chart_cell << "size"
            << chart_cell << "real size" << "|" << std::endl
            << chart_head << chart_head << chart_head << chart_head << endl;
        vector<DirItem *> dirs = cur->getDirs();
        for (auto i = dirs.begin(); i != dirs.end(); i++)
        {
            cout
                << chart_cell << (*i)->getName()
                << chart_cell << "directory"
                << chart_cell << "--"
                << chart_cell << "--" << "|" << std::endl;
        }
        vector<FCB *> files = cur->getFiles();
        for (auto i = files.begin(); i != files.end(); i++)
        {
            cout
                << chart_cell << (*i)->fileName
                << chart_cell << "file"
                << chart_cell << (*i)->size
                << chart_cell << (*i)->real_size << "|" << std::endl;
        }
        cout
            << chart_head << chart_head << chart_head << chart_head << endl;
        cout_hint();
    }
    void cout_hint()
    {
        cout << "\n"
             << cur_path << "> ";
    } // 输出命令行提示符
    void cd(string name)
    {
        if (name == "..")
        {
            if (cur_path == "/")
            {
                cout_hint();
            }
            else
            {
                if (*cur_path.rbegin() == '/')
                {
                    cur_path.pop_back();
                }
                cur_path = cur_path.substr(0, cur_path.rfind('/'));
                if (cur_path.length() == 0)
                {
                    cur_path = "/";
                }
                cur = find_dir_node(cur_path);
                cout_hint();
                return;
            }
        }
        DirItem *node = cur;
        for (DirItem *item : node->getDirs())
        {
            if (item->getName() == name)
            {
                cur = item;
                cur_path += name + "/";
                cout_hint();
                return;
            }
        }
        cout << "no such dir";
        cout_hint();
    }
    void cdn(string path)
    {
        cur = find_dir_node(path);
        if (*path.rbegin() != '/')
        {
            path.push_back('/');
        }
        if (*path.begin() != '/')
        {
            path = "/" + path;
        }
        cur_path = path;
        cout_hint();
    }
    DirItem *find_dir_node(string path)
    {
        if (path[0] == '/')
        {
            path = path.substr(1, path.length() - 1);
        }
        if (*path.rbegin() != '/')
        {
            path.push_back('/');
        }
        int r = 0;
        DirItem *node = root;
        if (path == "/")
            return node;
        while (r != -1)
        {
            r = path.find('/');
            if (r != -1)
            {
                string dir = path.substr(0, r);
                bool find = false;
                for (DirItem *item : node->getDirs())
                {
                    if (item->getName() == dir)
                    {
                        node = item;
                        find = true;
                        break;
                    }
                }
                if (!find)
                {
                    return nullptr;
                }
                path = path.substr(r + 1, path.length() - r - 1);
            }
            else
            {
                break;
            }
        }
        return node;
    }
    void create_file(const string &fileName, const vector<char> &data)
    {
        DirItem *node = cur;
        if (node != nullptr)
        {
            int fb = dm.write(data);
            if (fb == -1)
            {
                cout << "no enough disk storage";
                cout_hint();
            }
            int size = dm.getSize(fb);
            node->addFile(fb, fileName, data.size(), size);
        }
        cout_hint();
    }
    void create_file(string path, const string &fileName, const vector<char> &data)
    {
        DirItem *node = find_dir_node(path);
        if (node != nullptr)
        {
            int fb = dm.write(data);
            int size = dm.getSize(fb);
            node->addFile(fb, fileName, data.size(), size);
        }
    }
    void create_dir(const string &dirName)
    {
        if (cur != nullptr)
        {
            cur->addDir(new DirItem(dirName));
        }
        cout_hint();
    }
    void create_dir(string path, const string &dirName)
    {
        DirItem *node = find_dir_node(path);
        if (node != nullptr)
        {
            node->addDir(new DirItem(dirName));
        }
    }
    void del_dir(DirItem *node)
    {
        if (node != nullptr)
        {
            for (FCB *fcb : node->getFiles())
            {
                dm.del(fcb->first_block);
                node->del_file(fcb->fileName);
            }
            for (auto i : node->getDirs())
            {
                del_dir(i);
            }
            node->clear();
        }
    }
    void del_dir(const string &fileName)
    {
        if (cur != nullptr)
        {
            for (auto i : cur->getDirs())
            {
                if (i->getName() == fileName)
                {
                    del_dir(i);
                    cur->del_dir(i->getName());
                    cout_hint();
                    return;
                }
            }
        }
        cout << "no such directory";
        cout_hint();
    }
    void del_file(const string &fileName)
    {
        DirItem *node = cur;
        if (node != nullptr)
        {
            for (FCB *fcb : node->getFiles())
            {
                if (fcb->fileName == fileName)
                {
                    dm.del(fcb->first_block);
                    node->del_file(fileName);
                    cout_hint();
                    return;
                }
            }
        }
    }
    void del_file(string path, const string &fileName)
    {
        DirItem *node = find_dir_node(path);
        if (node != nullptr)
        {
            for (FCB *fcb : node->getFiles())
            {
                if (fcb->fileName == fileName)
                {
                    dm.del(fcb->first_block);
                    node->del_file(fileName);
                    return;
                }
            }
        }
    }
    void show(const vector<char> &v)
    {
        for (char i : v)
        {
            cout << i;
        }
    }
    void read(const string &fileName)
    {
        if (cur != nullptr)
        {
            for (FCB *fcb : cur->getFiles())
            {
                if (fcb->fileName == fileName)
                {
                    vector<char> data = dm.read(fcb->first_block);
                    show(data);
                    cout_hint();
                    return;
                }
            }
        }
        cout << "file not found";
        cout_hint();
    }
    vector<char> read(string path, const string &fileName)
    {
        DirItem *node = find_dir_node(path);
        if (node != nullptr)
        {
            for (FCB *fcb : node->getFiles())
            {
                if (fcb->fileName == fileName)
                {
                    return dm.read(fcb->first_block);
                }
            }
        }
        return {};
    }
};
int main()
{

    fileMgr fm;
    /*ifstream Myfile2;
    Myfile2.open("file_sys.disk",ios::binary);
    //二进制打开，缺省为文本，ios::out,ios::in,文本输入输出用<<,>>
    Myfile2.read((char *)&fm,sizeof(fileMgr));
    Myfile2.close();*/
    string ins, para;
    while (cin >> ins)
    {
        if (ins == "ls")
        {
            fm.ls();
        }
        else if (ins == "cd")
        {
            cin >> para;
            fm.cd(para);
        }
        else if (ins == "read")
        {
            cin >> para;
            fm.read(para);
        }
        else if (ins == "mkdir")
        {
            cin >> para;
            fm.create_dir(para);
        }
        else if (ins == "rmdir")
        {
            cin >> para;
            fm.del_dir(para);
        }
        else if (ins == "mkfile")
        {
            cin >> para;
            string c = "";
            cout << "contents:\n";
            string data;
            while (c != "#")
            {
                data += c;
                std::getline(cin, c);
                data += '\n';
            }
            int k = 0;
            while (data[k] == '\n')
                k++;
            data = data.substr(k, data.length() - k);
            vector<char> vdata;
            for (char i : data)
            {
                vdata.push_back(i);
            }
            fm.create_file(para, vdata);
        }
        else if (ins == "rmfile")
        {
            cin >> para;
            fm.del_file(para);
        }
        else if (ins == "exit" || ins == "quit")
        {
            break;
        }
        else if (ins == "cdn")
        {
            cin >> para;
            fm.cdn(para);
        }
        else
        {
            string c;
            std::getline(cin, c);
            ins += " " + c;
            system(ins.c_str());
            cout << "unknown cmd:" << ins << endl;
            fm.cout_hint();
        }
    }

    /*ofstream file;
    file.open("file_sys.disk",ios::binary);
    //缓存的类型是 unsigned char *，需要类型转换
    file.write((char *)&fm,sizeof(fileMgr));  //winServer为类对象
    file.close();*/
    return 0;
}