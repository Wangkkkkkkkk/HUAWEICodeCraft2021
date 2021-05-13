#include<fstream>
#include<sstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <time.h>
#include <algorithm>
#include <assert.h>
#include<set>
#include<math.h>
using namespace std;

//可调参数


int a_bound = 150;//请求数小于这个值时，使用A*算法

float father_wt = 0.4;   //越大规划越快
int p_wt = 2;  //越大越排斥买新的服务器
int max_open_list_size = 500;  //越小规划越快,越大规划地越好
float G_wt;


float value_piancha;//弥补普遍核数比内存数少的量纲问题，认为核数量纲为内存的v_p倍
float piancha_size = 0;//弥补尺寸越小，灵活性越小的问题。
float piancha_run_purchse = 0.5; //越大purchase越重要


class IntIntInt
{
public:
    IntIntInt(int vm_id,int server_id,int node)
    {
        this ->m_vmid = vm_id;
        this ->m_serverid = server_id;
        this ->m_node = node;
    }
    int m_vmid;
    int m_serverid;
    int m_node;
};


class IntInt
{
public:
    IntInt(int id,int node)
    {
        this ->m_id = id;
        this ->m_node = node;
    }
    int m_id;
    int m_node;
};

class FVIII  //{F,v{ {serverid or purchase_num,oper}...... }}
{
    public:
    FVIII(float F,vector<IntIntInt> OP)
    {
        this ->m_F = F;
        
        this ->m_OP = OP;
    }
    float m_F;
    vector<IntIntInt> m_OP;
};

class StrInt
{
public:
    StrInt(string Type,int num)
    {
        this ->m_Type = Type;
        this ->m_num = num;
    }
    string m_Type;
    int m_num;
};
class StrFloat
{
public:
    StrFloat(string Type,float num)
    {
        this ->m_Type = Type;
        this ->m_num = num;
    }
    string m_Type;
    float m_num;
};
class SFI
{
public:
    SFI(string Type,float num1,int num2)
    {
        this ->m_Type = Type;
        this ->m_num1 = num1;
        this ->m_num2 = num2;
    }
    string m_Type;
    float m_num1;
    int m_num2;
};
class SFII
{
public:
    SFII(string Type,float num1,int num2,int num3)
    {
        this ->m_Type = Type;
        this ->m_num1 = num1;
        this ->m_num2 = num2;
        this ->m_num3 = num3;
    }
    string m_Type;
    float m_num1;
    int m_num2;
    int m_num3;
};

class SFIII
{
public:
    SFIII(string Type,float num1,int num2,int num3,int num4)
    {
        this ->m_Type = Type;
        this ->m_num1 = num1;
        this ->m_num2 = num2;
        this ->m_num3 = num3;
        this ->m_num4 = num4;
    }
    string m_Type;
    float m_num1;
    int m_num2;
    int m_num3;
    int m_num4;
};

class LongInt
{
public:
    LongInt( long long id,int num)
    {
        this ->m_id = id;
        this ->m_num = num;
    }
    long long  m_id;
    int m_num;
};

class compareFVIII
{
    public:
    bool operator()(const FVIII&fv1 ,const FVIII&fv2) const
    {
        // 排序方式为升序

        if(fv1.m_F !=fv2.m_F) //先尝试按F值升序排列
        {return fv1.m_F <fv2.m_F;}
        else if(fv1.m_OP.size()!=fv2.m_OP.size())                    //如果F值相同，则尺寸大的排前面                       
        {return fv1.m_OP.size() >fv2.m_OP.size();}
        else if(fv1.m_OP.back().m_serverid!=fv2.m_OP.back().m_serverid)     //如果F，尺寸都相同，则服务器id靠前的排前面（注意purchase时id为0）
        {return fv1.m_OP.back().m_serverid<fv2.m_OP.back().m_serverid;} 
        else if(fv1.m_OP.back().m_node!=fv1.m_OP.back().m_node)    //如果F，尺寸,服务器id都相同，则操作数小的排前面（这会导致优先塞入A节点）
        {return fv1.m_OP.back().m_node<fv2.m_OP.back().m_node;}  
        else                                                    //如果F，尺寸,服务器id,操作数都相同，按vm_id排序
        {return fv1.m_OP.back().m_vmid<fv2.m_OP.back().m_vmid;}  
               
        
    }
};

class compareStrInt
{
public:
    bool operator()(const StrInt&si1 ,const StrInt&si2) const
    {
        // 排序方式为降序
        return si1.m_num >si2.m_num;
    }
};
class compareStrFloat   ///不完善，如果要用需要添加排序规则
{
public:
    bool operator()(const StrFloat&sf1 ,const StrFloat&sf2) const
    {
        // 排序方式为升序
        return sf1.m_num <sf2.m_num;
    }
};

class compareSFI
{
public:
    bool operator()(const SFI&sfi1 ,const SFI&sfi2) const
    {
        // 排序方式为升序
        if(sfi1.m_num1!=sfi2.m_num1)
        {return sfi1.m_num1 <sfi2.m_num1;}
        else
        {return sfi1.m_num2<sfi2.m_num2;}
        
    }
};
class compareSFII
{
public:
    bool operator()(const SFII&sfii1 ,const SFII&sfii2) const
    {
        // 排序方式为升序
        return sfii1.m_num1 <sfii2.m_num1;
    }
};
class compareSFIII
{
public:
    bool operator()(const SFIII&sfiii1 ,const SFIII&sfiii2) const
    {
        // 排序方式为升序
        return sfiii1.m_num1 <sfiii2.m_num1;
    }
};




// 服务器信息
unordered_map<string,vector<int>> server_type_map;
// 虚拟机信息
unordered_map<string,vector<int>> vm_type_map;
// 请求信息
vector<vector<string>> requestInfos;
// 已有服务器信息及备份
unordered_map<int,vector<int>> servers_info_map;

// 已有虚拟机数值信息及备份
unordered_map<int,vector<int>> vms_info_map;

//服务器类型运行成本表
set<SFI,compareSFI> serverTypeCostSet;
//虚拟机类型大小表
set<StrInt,compareStrInt> vmTypeSizeSet;
//已部署虚拟机信息
unordered_map<string,string> vmIdType_map;
//全部请求信息
unordered_map<int,vector<vector<int>>> reqs_map;

// 高性价比服务器列表
set<SFIII,compareSFIII> purchase_type_set;

//保存请求信息表
unordered_map<int, vector<IntInt>> set_vm_map;

//每天的迁移指令表
vector<IntIntInt> mig_list;

// 保存所有信息表//////////////////////////////////////////////////
unordered_map<int,vector<StrInt>> ALL_map;


//A*算法开集组件
set<FVIII,compareFVIII> open_list;
vector<IntIntInt> oper_list;

//A*算法闭集组件
set<FVIII,compareFVIII> close_list;


// 每台服务器里的虚拟机信息和备份
unordered_map<int,vector<vector<int>>> server_vminfo_map;  //severid:{{vm_id,cores,mems,0orAorB},......}

//需要用到的全局变量
int vm_info_map_size;
int max_need_mem=0;
int max_need_core =0;
int server_id_count=0;
int purchase_type_num = 0; //定义购买请求的类型数
int server_num =0;
int max_core_mem=0;
int max_mem_core=0;
int total_day;
int today;
int purchase_num;
int all_vmcores = 0;
int all_vmmem =0;
int max_vmcores = 0;
int max_vmmem =0;
string best_server_type;
int str2int(string s)
{
    int num;
    stringstream ss(s);
    ss>>num;
    return num;
}
string int2str(int num)
{
    string s;
    stringstream ss;
    ss<<num;
    ss>>s;
    return s;
}


//删除一个vm
void del_vm(int server_id,int vm_id ,int vm_node,int vm_core,int vm_mem)
{
    if(vm_node==0)
    {
        auto it_temp = vms_info_map.find(vm_id);
        if(it_temp !=vms_info_map.end())
        {vms_info_map.erase(it_temp);}//删除虚拟机信息
        servers_info_map[server_id][0] += vm_core/2;
        servers_info_map[server_id][1] += vm_core/2;
        servers_info_map[server_id][2] += vm_mem/2;
        servers_info_map[server_id][3] += vm_mem/2;
        servers_info_map[server_id][4] -= vm_core;
        servers_info_map[server_id][5] -= vm_mem;
        for(auto it_temp2 =server_vminfo_map[server_id].begin();it_temp2!=server_vminfo_map[server_id].end();it_temp2++)
        {   
            if((*it_temp2)[0]==vm_id)
            {server_vminfo_map[server_id].erase(it_temp2);
            break;} //删除服务器虚拟机信息
        }
    }
    if(vm_node==1)
    {
        auto it_temp = vms_info_map.find(vm_id);
        if(it_temp !=vms_info_map.end())
        {vms_info_map.erase(it_temp);}//删除虚拟机信息
        servers_info_map[server_id][0] += vm_core;
        servers_info_map[server_id][2] += vm_mem;
        servers_info_map[server_id][4] -= vm_core;
        servers_info_map[server_id][5] -= vm_mem;
        for(auto it_temp2 =server_vminfo_map[server_id].begin();it_temp2!=server_vminfo_map[server_id].end();it_temp2++)
        {   
            if((*it_temp2)[0]==vm_id)
            {server_vminfo_map[server_id].erase(it_temp2);
            break;} //删除服务器虚拟机信息
        }
    }
    if(vm_node==2)
    {
        // set_vm_map[today].push_back(IntInt(-1,-1)); //表示无需操作
        auto it_temp = vms_info_map.find(vm_id);
        if(it_temp !=vms_info_map.end())
        {vms_info_map.erase(it_temp);}//删除虚拟机信息
        servers_info_map[server_id][1] += vm_core;
        servers_info_map[server_id][3] += vm_mem;
        servers_info_map[server_id][4] -= vm_core;
        servers_info_map[server_id][5] -= vm_mem;
        for(auto it_temp2 =server_vminfo_map[server_id].begin();it_temp2!=server_vminfo_map[server_id].end();it_temp2++)
        {   
            if((*it_temp2)[0]==vm_id)
            {server_vminfo_map[server_id].erase(it_temp2);
            break;} //删除服务器虚拟机信息
        }
    }
}




//A*相关函数、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、
void add_vm(int server_id,int vm_id,int vm_node,int vm_core ,int vm_mem)
{
switch(vm_node)
{
    case 0:
        vms_info_map[vm_id]=vector<int>{vm_core,vm_mem,server_id,0};
        servers_info_map[server_id][0] -= vm_core/2;
        servers_info_map[server_id][1] -= vm_core/2;
        servers_info_map[server_id][2] -= vm_mem/2;
        servers_info_map[server_id][3] -= vm_mem/2;
        servers_info_map[server_id][4] += vm_core;
        servers_info_map[server_id][5] += vm_mem;
        server_vminfo_map[server_id].push_back(vector<int>{vm_id,vm_core,vm_mem,0});
        break;
    case 1:
        vms_info_map[vm_id]=vector<int>{vm_core,vm_mem,server_id,1};
        servers_info_map[server_id][0] -= vm_core;
        servers_info_map[server_id][2] -= vm_mem;
        servers_info_map[server_id][4] += vm_core;
        servers_info_map[server_id][5] += vm_mem;
        server_vminfo_map[server_id].push_back(vector<int>{vm_id,vm_core,vm_mem,1});
        break;
    case 2:
        vms_info_map[vm_id]=vector<int>{vm_core,vm_mem,server_id,2};
        servers_info_map[server_id][1] -= vm_core;
        servers_info_map[server_id][3] -= vm_mem;
        servers_info_map[server_id][4] += vm_core;
        servers_info_map[server_id][5] += vm_mem;
        server_vminfo_map[server_id].push_back(vector<int>{vm_id,vm_core,vm_mem,2});
        break;
}

}



// void del_vm_temp(int server_id,int vm_id ,int vm_node,int vm_core,int vm_mem)//A*组件，在模拟世界下删除一个vm
// {
//     if(vm_node==0)
//     {
//         auto it_temp = vms_info_map.find(vm_id);
//         if(it_temp !=vms_info_map.end())
//         {vms_info_map.erase(it_temp);}//删除虚拟机信息
//         servers_info_map[server_id][0] += vm_core/2;
//         servers_info_map[server_id][1] += vm_core/2;
//         servers_info_map[server_id][2] += vm_mem/2;
//         servers_info_map[server_id][3] += vm_mem/2;
//         servers_info_map[server_id][4] -= vm_core;
//         servers_info_map[server_id][5] -= vm_mem;
//         for(auto it_temp2 =server_vminfo_map[server_id].begin();it_temp2!=server_vminfo_map[server_id].end();it_temp2++)
//         {   
//             if((*it_temp2)[0]==vm_id)
//             {server_vminfo_map[server_id].erase(it_temp2);
//             break;} //删除服务器虚拟机信息
//         }
//     }
//     if(vm_node==1)
//     {
//         // set_vm_map[today].push_back(IntInt(-1,-1)); //表示无需操作
//         auto it_temp = vms_info_map.find(vm_id);
//         if(it_temp !=vms_info_map.end())
//         {vms_info_map.erase(it_temp);}//删除虚拟机信息
//         servers_info_map[server_id][0] += vm_core;
//         servers_info_map[server_id][2] += vm_mem;
//         servers_info_map[server_id][4] -= vm_core;
//         servers_info_map[server_id][5] -= vm_mem;
//         for(auto it_temp2 =server_vminfo_map[server_id].begin();it_temp2!=server_vminfo_map[server_id].end();it_temp2++)
//         {   
//             if((*it_temp2)[0]==vm_id)
//             {server_vminfo_map[server_id].erase(it_temp2);
//             break;} //删除服务器虚拟机信息
//         }
//     }
//     if(vm_node==2)
//     {
//         auto it_temp = vms_info_map.find(vm_id);
//         if(it_temp !=vms_info_map.end())
//         {vms_info_map.erase(it_temp);}//删除虚拟机信息
//         servers_info_map[server_id][1] += vm_core;
//         servers_info_map[server_id][3] += vm_mem;
//         servers_info_map[server_id][4] -= vm_core;
//         servers_info_map[server_id][5] -= vm_mem;
//         for(auto it_temp2 =server_vminfo_map[server_id].begin();it_temp2!=server_vminfo_map[server_id].end();it_temp2++)
//         {   
//             if((*it_temp2)[0]==vm_id)
//             {server_vminfo_map[server_id].erase(it_temp2);
//             break;} //删除服务器虚拟机信息
//         }
//     }
// }
int do_opers_temp(vector<IntIntInt> _oper_list,vector<vector< int>>_reqs_list)  //A*组件，在模拟世界下实现oper_list向量里所有的操作
{
    int _count = _oper_list.size();
    int _purchase_num =0;
    int _req_size = _reqs_list.size();
    for(int i=1;i<_count;i++)
    {
        int _req_count = i-_purchase_num-1;
        int _server_id = _oper_list[i].m_serverid;
        int _vm_id = _reqs_list[_req_count][4];
        int _core_vm = _reqs_list[_req_count][1];
        int _mem_vm =  _reqs_list[_req_count][2];
        switch(_oper_list[i].m_node)
        {
            case -1: //删除
                del_vm(_server_id,_vm_id,0,_core_vm,_mem_vm);
                break;
            case -2: //删除
                del_vm(_server_id,_vm_id,1,_core_vm,_mem_vm);
                break;
            case -3: //删除
                del_vm(_server_id,_vm_id,2,_core_vm,_mem_vm);
            break;
            case 0: //_server_id双节点添加
                add_vm(_server_id,_vm_id,0,_core_vm,_mem_vm);
                break;
            case 1://_server_idA节点添加
                add_vm(_server_id,_vm_id,1,_core_vm,_mem_vm);
                break;
            case 2://_server_idB节点添加
                add_vm(_server_id,_vm_id,2,_core_vm,_mem_vm);
                break;
            case 3://购买
                _purchase_num++;
                int half_core = server_type_map[best_server_type][0]/2;
                int half_mem = server_type_map[best_server_type][1]/2;
                servers_info_map[server_id_count] = vector<int>{half_core,half_core,half_mem,half_mem,0,0 };
                server_id_count++;
                break;
        }
    }
    return _count -_purchase_num - 1; //输出接下来应该读取的请求序号（因为已经执行了这么些步骤了嘛，不可能又从头开始读请求）
}


int do_opers(vector<IntIntInt> _oper_list,vector<vector< int>>_reqs_list) //确定了最佳oper_list后，在真实世界复现这些操作
{
    int _count = _oper_list.size();
    int _purchase_num =0;
    int _req_size =_reqs_list.size();
    for(int i=1;i<_count;i++)
    {
        int _req_count = i-_purchase_num-1;
        int _server_id = _oper_list[i].m_serverid;
        int _vm_id = _reqs_list[_req_count][4];
        int _core_vm = _reqs_list[_req_count][1];
        int _mem_vm =  _reqs_list[_req_count][2];
        switch(_oper_list[i].m_node)
        {
            case -1: //删除
                del_vm(_server_id,_vm_id,0,_core_vm,_mem_vm);
                break;
            case -2: //删除
                del_vm(_server_id,_vm_id,1,_core_vm,_mem_vm);
                break;
            case -3: //删除
                del_vm(_server_id,_vm_id,2,_core_vm,_mem_vm);
            case 0: //_server_id双节点添加
                add_vm(_server_id,_vm_id,0,_core_vm,_mem_vm);
                break;
            case 1://_server_idA节点添加
                add_vm(_server_id,_vm_id,1,_core_vm,_mem_vm);
                break;
            case 2://_server_idB节点添加
                add_vm(_server_id,_vm_id,2,_core_vm,_mem_vm);
                break;
            case 3://购买
                _purchase_num++;
                int half_core = server_type_map[best_server_type][0]/2;
                int half_mem = server_type_map[best_server_type][1]/2;
                servers_info_map[server_id_count] = vector<int>{half_core,half_core,half_mem,half_mem,0,0 };
                server_id_count++;
                break;
        }
    }
    return _purchase_num; //返回oper_list中购买操作的数量，用于输出购买请求
}
void cancel_opers(vector<IntIntInt> _oper_list,vector<vector< int>>_reqs_list,int _req_head)
{
    int _count = _oper_list.size()-1;
    if(_count>=0)
    {int _req_count = _req_head-1;
    int _purchase_num =0;
    for(int i=_count;i>0;i--)
    {
        int _server_id = _oper_list[i].m_serverid;
        int _vm_id = _reqs_list[_req_count][4];
        int _core_vm = _reqs_list[_req_count][1];
        int _mem_vm =  _reqs_list[_req_count][2];

        switch(_oper_list[i].m_node)
        {
            case -1:
                add_vm(_server_id,_vm_id,0,_core_vm,_mem_vm);
                _req_count-=1;
                break;
            case -2:
                add_vm(_server_id,_vm_id,1,_core_vm,_mem_vm);
                _req_count-=1;
                break;
            case -3:
                add_vm(_server_id,_vm_id,2,_core_vm,_mem_vm);
                _req_count-=1;
                break;
            case 0:
                del_vm(_server_id,_vm_id,0,_core_vm,_mem_vm);
                _req_count-=1;
                break;
            case 1:
                del_vm(_server_id,_vm_id,1,_core_vm,_mem_vm);
                _req_count-=1;
                break;
            case 2:
                del_vm(_server_id,_vm_id,2,_core_vm,_mem_vm);
                _req_count-=1;
                break;
            case 3:
                server_id_count-=1;
                auto it = servers_info_map.find(server_id_count);
                servers_info_map.erase(it);
                break;
        }
    }}
}

void Astar_setvm(vector<vector<int>> reqs_aday)
{
    open_list.clear();
    oper_list.clear();
    float F = 0.0 ;  //定义估价函数值 
    int H ;     //定义启发函数值
    int G ;   //定义代价函数值
    int G_temp; //用于保存过程量
    int H_temp;
    int _reqs_size =reqs_aday.size();
    //读取当天第一条请求的信息
    int core_vm = reqs_aday[0][1];
    int mem_vm = reqs_aday[0][2];
    int vm_isdouble = reqs_aday[0][3];
    int vm_id = reqs_aday[0][4];
    int oper;
    int open_list_size = 0;
    while(1) //一般执行不到尾部就会break出来
    {    //首先进行open_list赋初值
        G = 0;
        H = _reqs_size;

        if(reqs_aday[0][0]==0) //如果是删除请求
        {
            H = _reqs_size-1;
            F = (float)G+H;
            int _loc_id = vms_info_map[vm_id][2];
            int _loc_node = - vms_info_map[vm_id][3]-1; 
            open_list.insert(FVIII(F,{{0,G,H},{vm_id,_loc_id,_loc_node}}));
            // 删除请求要保存loc——id和node，不然回头cancel无法复现

        }


        if(reqs_aday[0][0]==1)  //如果是添加请求
        {
            //计算这个请求的所有可选操作：
            if(reqs_aday[0][3]==0)    //如果该虚拟机为单节点型
            {
                if(open_list_size>max_open_list_size)
                {break;}
                for(int j=0;j<server_id_count;j++)  //遍历所有服务器id
                {
                    if(open_list_size>max_open_list_size)
                    {break;}
                    if(servers_info_map[j][0]>=core_vm&&servers_info_map[j][2]>=mem_vm) //如果j服务器A节点放得下
                    {
                        if(!(servers_info_map[j][4]||servers_info_map[j][5]))
                        {
                            G = 1;
                            H = _reqs_size - 1;
                            F = (float)G+H;
                            open_list.insert(FVIII(F,vector<IntIntInt>{{0,G,H},{vm_id,j,1}}));
                            open_list_size++;
                        }
                        else
                        {
                            G = 0;
                            H = _reqs_size - 1;
                            F = (float)G+H;
                            open_list.insert(FVIII(F,vector<IntIntInt>{{0,G,H},{vm_id,j,1}}));
                            open_list_size ++;
                        }
                    }
                    if(servers_info_map[j][1]>=core_vm&&servers_info_map[j][3]>=mem_vm) //如果j服务器A放不下的话，看B节点放得下不，后续如果启发函数涉及了节点层面，这里可以把else去掉增加可能性
                    {

                        if(servers_info_map[j][4]==0&&servers_info_map[j][5]==0)
                        {
                            G = 1;
                            H = _reqs_size - 1;
                            F = (float)G+H;
                            open_list.insert(FVIII(F,vector<IntIntInt>{{0,G,H},{vm_id,j,2}}));
                            open_list_size ++;
                        }
                        else
                        {
                            G = 0;
                            H = _reqs_size - 1;
                            F = (float)G+H;
                            open_list.insert(FVIII(F,vector<IntIntInt>{{0,G,H},{vm_id,j,2}}));
                            open_list_size ++;
                        }
                    }
                }
            }
            // cout<<"openlist_size = "<<open_list.size()<<endl;
            // cout<<"当前server_id_count为："<<server_id_count<<endl;
            if(reqs_aday[0][3]==1) //如果该虚拟机是双节点型
            {
                if(open_list_size>max_open_list_size)
                {break;}
                for(int j=0;j<server_id_count;j++)
                {
                    if(open_list_size>max_open_list_size)
                    {break;}
                    if(servers_info_map[j][0]>=core_vm/2&&servers_info_map[j][2]>=mem_vm/2)
                    {
                        if(servers_info_map[j][1]>=core_vm/2&&servers_info_map[j][3]>=mem_vm/2)
                        {
                            if(!servers_info_map[j][4]&&!servers_info_map[j][5])
                            {
                                // cout<<"使用了一个新服务器的可能。新服务器id："<<j<<endl;
                                // cout<<server_id_count<<endl;
                                // open_list.insert(FVII(0.0,{{0,-1}}));
                                G = 1;
                                H = _reqs_size - 1;
                                F = (float)G+H;
                                open_list.insert(FVIII(F,vector<IntIntInt>{{0,G,H},{vm_id,j,0}}));
                                open_list_size ++;
                            }
                            else
                            {
                                // cout<<"一种不使用新服务器的可能。新服务器id："<<j<<endl;
                                G = 0;
                                H = _reqs_size - 1;
                                F = (float)G+H;
                                open_list.insert(FVIII(F,vector<IntIntInt>{{0,G,H},{vm_id,j,0}}));
                                open_list_size ++;
                            }
                        }
                    }
                }
            }
            G = p_wt;
            H = _reqs_size;
            F = (float)G+H;
            open_list.insert(FVIII(F,{{0,G,H},{vm_id,0,3}})); //购买请求,购买应该比部署进新服务器代价更高
            open_list_size ++;
        }
        break;
    }
    // cout<<"初始化的openlist尺寸："<<open_list.size()<<endl;
    int _req_head =0;
    vector<IntIntInt> oper_list_copy;
    while(_req_head <_reqs_size) //直到有长度等于今天的请求数或者第一条长度为请求数+purchase_num
    {
        //1.读取openlist第一条，备份今天的起始状态（vms_info_map,servers_info_map,server_vminfo_map三表的temp），实现第一条的操作
        // servers_info_map = servers_info_map;
        // vms_info_map = vms_info_map;
        // server_vminfo_map = server_vminfo_map;
        // copy(servers_info_map.begin(),servers_info_map.end(),inserter(servers_info_map,servers_info_map.begin()));
        
        // copy(vms_info_map.begin(),vms_info_map.end(),inserter(vms_info_map,vms_info_map.begin()));
        
        // copy(server_vminfo_map.begin(),server_vminfo_map.end(),inserter(server_vminfo_map,server_vminfo_map.begin()));
        
        cancel_opers(oper_list,reqs_aday,_req_head);
        // server_id_count = server_id_count;
        F = open_list.begin()->m_F;
        oper_list = open_list.begin()->m_OP;
        G = oper_list.front().m_serverid;//第一位
        H = oper_list.front().m_node;//第二位

        oper_list_copy = oper_list;
        
        _req_head = do_opers_temp(oper_list_copy,reqs_aday); //复现open_list第一条,并得到接下来应该读的请求序号
        
        
        // cout<<"本次请求总数："<<_reqs_size<<endl;
        // cout<<"本次读取的oper长度："<<oper_list_copy.size()<<endl;
        // cout<<"本次读取的oper的F值："<<F<<endl;
        // cout<<"本次oper执行完后应该读取的请求序号："<<_req_head<<endl;
        // cout<<"open_list的大小："<<open_list.size()<<endl;

        if(_req_head==_reqs_size)
        {   
            purchase_num = oper_list_copy.size() - _req_head -1;
            break;}
        
        //2.然后删去openlist第一条（或者移到close_list）
        open_list.erase(open_list.begin());//删去第一条

        //3.实现操作之后，遍历接下来所有可能的操作，并将这些操作添加到读取的open_list第一条后面生产新的操作，添加到open_list中

        //读取当前请求的信息
        oper = reqs_aday[_req_head][0];
        core_vm = reqs_aday[_req_head][1];
        mem_vm = reqs_aday[_req_head][2];
        vm_isdouble = reqs_aday[_req_head][3];
        vm_id = reqs_aday[_req_head][4];
        

        open_list_size =0;
        while(1) //一般执行不到尾部就会break出来,删除操作时则靠尾部的break跳出来
        {
            if(open_list_size>max_open_list_size)
            {break;}
            if(oper==0) //如果是删除请求
            {
                int server_id = vms_info_map[vm_id][2];
                int vm_node = -vms_info_map[vm_id][3]-1;
                
                oper_list_copy.push_back({vm_id,server_id,vm_node});

                G_temp = G;
                H_temp = _reqs_size - _req_head-1;
                F = (float)G_temp + father_wt*(H+H_temp);
                open_list.insert(FVIII( F, oper_list_copy  ));
                
            }
            if(oper==1)  //如果是添加请求
            {
                if(open_list_size>max_open_list_size)
                {break;}
                //计算这个请求的所有可选操作：
                if(reqs_aday[_req_head][3]==0)    //如果该虚拟机为单节点型
                {
                    
                    for(int j=0;j<server_id_count;j++)  //遍历所有服务器id
                    {
                        if(open_list_size>max_open_list_size)
                        {break;}
                        if(servers_info_map[j][0]>=core_vm&&servers_info_map[j][2]>=mem_vm) //如果j服务器A节点放得下
                        {         
                            if(!servers_info_map[j][4]&&!servers_info_map[j][5]) //如果是空服务器
                            {
                                oper_list_copy = oper_list;
                                oper_list_copy.push_back({vm_id,j,1});
                                G_temp = G+1;
                                H_temp = _reqs_size - _req_head-1;
                                F = (float)G_temp + father_wt*(H+H_temp);
                                open_list.insert(FVIII(F,oper_list_copy));
                                open_list_size++;
                            }
                            else
                            {
                                oper_list_copy = oper_list;
                                oper_list_copy.push_back(IntIntInt(vm_id,j,1));
                                G_temp = G;
                                H_temp = _reqs_size - _req_head-1;
                                F = (float)G_temp + father_wt*(H+H_temp);
                                open_list.insert(FVIII(F,oper_list_copy));
                                open_list_size++;
                            }
                        }
                        else if(servers_info_map[j][1]>=core_vm&&servers_info_map[j][3]>=mem_vm) //如果j服务器A放不下的话，看B节点放得下不，后续如果启发函数涉及了节点层面，这里可以把else去掉增加可能性
                        {
                        

                            if(!servers_info_map[j][4]&&!servers_info_map[j][5])
                            {
                                oper_list_copy = oper_list;
                                oper_list_copy.push_back({vm_id,j,2});
                                G_temp = G+1;
                                H_temp = _reqs_size - _req_head-1;
                                F = (float)G_temp + father_wt*(H+H_temp);
                                open_list.insert(FVIII(F,oper_list_copy));
                                open_list_size++;
                            }
                            else
                            {

                                oper_list_copy = oper_list;
                                oper_list_copy.push_back({vm_id,j,2});
                                G_temp = G;
                                H_temp = _reqs_size - _req_head-1;
                                F = (float)G_temp + father_wt*(H+H_temp);
                                open_list.insert(FVIII(F,oper_list_copy));
                                open_list_size++;
                            }


                        }
                    }
                }
                if(reqs_aday[_req_head][3]==1) //如果该虚拟机是双节点型
                {
                    for(int j=0;j<server_id_count;j++)
                    {
                        if(open_list_size>max_open_list_size)
                        {break;}
                        if(servers_info_map[j][0]>=core_vm/2&&servers_info_map[j][2]>=mem_vm/2)
                        {
                            if(servers_info_map[j][1]>=core_vm/2&&servers_info_map[j][3]>=mem_vm/2)
                            {
                                if(!(servers_info_map[j][4]||servers_info_map[j][5]))
                                {
                                    oper_list_copy = oper_list;
                                    oper_list_copy.push_back({vm_id,j,0});
                                    G_temp = G+1;
                                    H_temp = _reqs_size - _req_head-1;
                                    F = (float)G_temp + father_wt*(H+H_temp);
                                    open_list.insert(FVIII(F,oper_list_copy));
                                    open_list_size++;
                                }
                                else
                                {
                                    oper_list_copy = oper_list;
                                    oper_list_copy.push_back({vm_id,j,0});
                                    G_temp = G;
                                    H_temp = _reqs_size - _req_head-1;
                                    F = (float)G_temp + father_wt*(H+H_temp);
                                    open_list.insert(FVIII(F,oper_list_copy));
                                    open_list_size++;
                                }
                            }
                        }
                    }
                }
                oper_list_copy.push_back({vm_id,0,3});
                G_temp = G+p_wt;
                H_temp = _reqs_size - _req_head;
                F = (float)G_temp + father_wt*(H+H_temp);
                open_list.insert(FVIII(F,oper_list_copy)); //稳定添加一项购买请求,但除非万不得已，不会采用他
            }
            break;
        }
    }

    // purchase_num = do_opers(oper_list_copy,reqs_aday);

    if(purchase_num ==0)
    {cout<<"(purchase, 0)"<<endl;}
    else
    {
        cout<<"(purchase, 1)"<<endl;
        string _purchase_body ="(, )";
        _purchase_body.insert(3,int2str(purchase_num));
        _purchase_body.insert(1,best_server_type);
        cout<<_purchase_body<<endl;
    }
    // 输出迁移指令
    string mig_head = "(migration, )";
    mig_head.insert(12,int2str(mig_list.size()));
    cout<<mig_head<<endl;
    if(mig_list.size()!=0)
    {
        for(int ij =0; ij<mig_list.size();ij++)
        {
            if(mig_list[ij].m_node==0)
            {cout<<"("<<mig_list[ij].m_vmid<<", "<<mig_list[ij].m_serverid<<")"<<endl;}
            else if(mig_list[ij].m_node==1)
            {cout<<"("<<mig_list[ij].m_vmid<<", "<<mig_list[ij].m_serverid<<", A)"<<endl;}
            else if(mig_list[ij].m_node==2)
            {cout<<"("<<mig_list[ij].m_vmid<<", "<<mig_list[ij].m_serverid<<", B)"<<endl;}
        }
    }
    // 输出部署指令
    // cout<<"今日待处理请求数："<<set_vm_map[today].size()<<endl;
    // for(auto it = set_vm_map[today].begin();it!=set_vm_map[today].end();it++)
    int _oper_size = oper_list_copy.size();
    for(int i=1;i<_oper_size;i++)
    {
        
        int _id = oper_list_copy[i].m_serverid ;//////
        // _id = 0;

        int oper = oper_list_copy[i].m_node;
        // if(_id>(server_id_count-1))
        // {cout<<"出现服务器id溢出了！!!!!!!!!!!!!!!"<<endl;}
        // string out = "()";
        switch(oper)
        {
            case -1:break;
            case -2:break;
            case -3:break;
            case 0 :cout<< "(" <<_id<< ")"<<endl;
                    break;
            case 1 :cout<< "(" <<_id<< ", " <<"A"<< ")"<<endl;
                    break;
            case 2 :cout<< "(" <<_id<< ", " <<"B"<< ")"<<endl;
                    break;
            case 3 :break;
        }
    }
}



/////////////////////////////////////////////////////////////////////////////////////////////////通用函数
void match_another_server(SFI server_temp,int need_core,int need_mem)
{
    set<SFI,compareSFI> match_server_set;
    if(server_temp.m_num2 < 0)
    {
        // cout<<"err<0"<<endl;
        float match_num;
        float cost_ave;

        for(auto it_servermatch = serverTypeCostSet.begin();it_servermatch!=serverTypeCostSet.end();it_servermatch++)
        {
            if(server_type_map[it_servermatch->m_Type][0]>=need_core&&server_type_map[it_servermatch->m_Type][1]>=need_mem)
            {if((it_servermatch->m_num2)>0)
            {
                match_num = -(float)(server_temp.m_num2)/(it_servermatch->m_num2);
                cost_ave = (server_temp.m_num1+match_num*(it_servermatch->m_num1))/(1+match_num);
                match_server_set.insert(SFI(it_servermatch->m_Type,cost_ave,it_servermatch->m_num2));
            }}
        }
    }
    if(server_temp.m_num2 > 0)
    {
        float match_num;
        float cost_ave;
        // cout<<"err>0"<<endl;
        for(auto it_servermatch = serverTypeCostSet.begin();it_servermatch!=serverTypeCostSet.end();it_servermatch++)
        {
            if(server_type_map[it_servermatch->m_Type][0]>=need_core||server_type_map[it_servermatch->m_Type][1]>=need_mem)
            {if((it_servermatch->m_num2)<0)
            {
                match_num = - (float)(server_temp.m_num2)/(it_servermatch->m_num2);
                
                cost_ave = (server_temp.m_num1+match_num*(it_servermatch->m_num1))/(1+match_num);
                
                match_server_set.insert(SFI(it_servermatch->m_Type,cost_ave,it_servermatch->m_num2));
            }}
        }
    }
    string f_type = match_server_set.begin()->m_Type;
    float f_cost = 0.3;
    int f_cm_err = match_server_set.begin()->m_num2;
    
    int f_core = server_type_map[f_type][0];
    int f_mem = server_type_map[f_type][1];
    // cout<<"匹配到的服务器的type和运行成本和cm_err和core和mem: "<<f_type<<" "<<f_cost<<" "<<f_cm_err<<" "<<f_core<<" "<<f_mem<<endl;
    // cout<<"purchase_set.size_test:"<<purchase_type_set.size()<<endl;
    purchase_type_set.insert(SFIII(f_type,f_cost,f_cm_err,f_core,f_mem));

}


void First_purchase_pro(int max_vmcore,int max_vmmem,int need_core,int need_mem )
{
    // cout<<need_core<<endl;
    // cout<<need_mem<<endl;
        for(auto it=serverTypeCostSet.begin();it!= serverTypeCostSet.end();it++) //遍历当前服务器性价比表
        {
            if(server_type_map[it->m_Type][0]>=need_core&&server_type_map[it->m_Type][1]>=need_mem) //如果该服务器核数和内存大于need—core和mneed-mem
            {
                if(it->m_num2!=0)   //如果核数内存数差值不为0 （为了方便能够和子服务器配合，拟合最大核数内存差）
                {string f_type = it->m_Type;
                float f_cost = it->m_num1;
                int f_cm_err = it->m_num2;
                int f_core = server_type_map[f_type][0];
                int f_mem = server_type_map[f_type][1];
                // cout<<"purchase_set.size1:"<<purchase_type_set.size()<<endl;
                // cout<<"母服务器的type和运行成本和cm_err和core和mem: "<<f_type<<" "<<f_cost<<" "<<f_cm_err<<" "<<f_core<<" "<<f_mem<<endl;
                purchase_type_set.insert(SFIII(f_type,f_cost,f_cm_err,f_core,f_mem)); //将该服务器信息塞进购买服务器列表中
                match_another_server(SFI(f_type,f_cost,f_cm_err),need_core,need_mem); //找到母服务器匹配的子服务器，同样塞进购买服务器列表中
                break;}
            }
        }
    int _all_core = 0; 
    int _all_mem = 0;
    int _core = 0;
    int _mem = 0;
    int _num = 0;
    bool full_flag =0;
    int err_cm_temp = -(max_vmcore-max_vmmem);
    int _n = purchase_type_set.size();
    // cout<<_n<<endl;
    purchase_type_num = 0;
    int purchase_list[100] ={0} ;
    string purchase_type_list[100];
    while(full_flag!=1)  //直到购买资源超过峰值需求
    {        
        for(auto it=purchase_type_set.begin();it!= purchase_type_set.end();it++)
        {
            
            _core = server_type_map[it->m_Type][0];
            _mem = server_type_map[it->m_Type][1];
            
            
            if(err_cm_temp*(it->m_num2)<=0) //err异号，可以减轻cm差距
            {
                _all_core +=_core;
                _all_mem += _mem;
                err_cm_temp += (it->m_num2);
                // cout<<"当前core-mem偏差值："<<err_cm_temp<<endl;
                

                purchase_list[_num%_n] += 1;
                if(purchase_type_list[_num%_n].size()==0){
                    purchase_type_list[_num%_n] =it->m_Type ;
                    purchase_type_num++;
                }
            }
            _num++;

            if(_all_core>=max_vmcore &&_all_mem>=max_vmmem)  //可以改进？
            {
                full_flag = 1;
                break;
            }
        }
    }
    string purchase_head = "(purchase, )";
    purchase_head.insert(11,int2str(purchase_type_num));
    cout<<purchase_head<<endl;
    for(int jj = 0;jj<100;jj++)
    {
        string purchase_body = "(, )";
        if(purchase_list[jj]!=0)
        {
            purchase_body.insert(3,int2str(purchase_list[jj]));
            purchase_body.insert(1,purchase_type_list[jj]);
            cout<<purchase_body<<endl;
            for(int i = 0;i<purchase_list[jj];i++)
            {
                int half_core = server_type_map[purchase_type_list[jj]][0]/2; 
                int half_mem = server_type_map[purchase_type_list[jj]][1]/2;
                servers_info_map[server_id_count] = vector<int>{half_core,half_core,half_mem,half_mem,0,0 };
                server_id_count ++;
            }
        }
    }
}


string find_best_server(int max_core,int max_mem,int today,int total_day) //找到今天的最佳服务器器，返回其类型字符串
{
    //新的服务器类型运行成本表
    set<SFI,compareSFI> serverTypeCostSet_today;
    int _core_vm = max_core;
    int _mem_vm = max_mem;
    // int _vm_id = req[4];
    for(auto it =server_type_map.begin();it!=server_type_map.end();it++)
    {
        
        float cost_perday = 0.0;
        
        string type_temp = it->first;
        int purchasecost_temp = it->second[2];
        int runcost_temp = it->second[3];
        int server_core_temp =  it->second[0];
        int server_mem_temp = it->second[1];
        if(server_core_temp>=_core_vm&&server_mem_temp>=_mem_vm)
        {        
            int core_mem_err = server_core_temp - server_mem_temp;
            cost_perday = (float)(purchasecost_temp/(total_day - today) + runcost_temp*piancha_run_purchse)/(server_core_temp*value_piancha+server_mem_temp);
            serverTypeCostSet_today.insert(SFI(type_temp,cost_perday,core_mem_err));
        }

    }
    string best_sever_type = serverTypeCostSet_today.begin()->m_Type;
    return best_sever_type;
}



void generateVm(string vmType,string vmCpuCores,string vmMemory,string vm_isdouble){
    string _vmType ;
    _vmType = vmType.substr(1,vmType.size()-2);
    int _vmCpuCores = 0,_vmMemory=0,_vm_isdouble=0;
    _vmCpuCores = str2int(vmCpuCores.substr(0,vmCpuCores.size()-1));
    _vmMemory = str2int(vmMemory.substr(0,vmMemory.size()-1));
    _vm_isdouble = str2int(vm_isdouble.substr(0,vm_isdouble.size()-1));
    vm_type_map[_vmType] = vector<int>{_vmCpuCores,_vmMemory,_vm_isdouble};
    int _vmSize = _vmCpuCores +_vmMemory + _vm_isdouble*10000; //计算vm大小，使双节点恒大于单节点，先部署大
    vmTypeSizeSet.insert(StrInt(_vmType,_vmSize));
}
void generateServer(string serverType,string cpuCores,string memorySize,string serverCost,string runCost){
    string _serverType;
    _serverType = serverType.substr(1,serverType.size()-2);
    int _cpuCores=0,_memorySize=0,_serverCost=0,_runCost=0;
    _cpuCores = str2int(cpuCores.substr(0,cpuCores.size()-1));
    _memorySize = str2int(memorySize.substr(0,memorySize.size()-1));
    _serverCost = str2int(serverCost.substr(0,serverCost.size()-1));
    _runCost = str2int(runCost.substr(0,runCost.size()-1));
    // 更新服务器类型表和服务器类型性价比排序表
    server_type_map[_serverType] = vector<int>{_cpuCores,_memorySize,_serverCost,_runCost};
}


void old_setvm(vector<vector<int>> reqs_aday) //老部署方法
{
    int _reqs_size = reqs_aday.size();
    for(int i =0;i<_reqs_size;i++) //遍历所有请求
        {
            int core_vm = reqs_aday[i][1];
            int mem_vm = reqs_aday[i][2];
            int vm_isdouble = reqs_aday[i][3];
            int vm_id = reqs_aday[i][4];
            if(reqs_aday[i][0]==1)
            {
                bool OK_flag = 0;
                bool type_set = 0;
                if(reqs_aday[i][3]==0 && OK_flag==0)
                {
                    
                    for(int j=0;j<server_id_count;j++)
                    {
                        if(servers_info_map[j][0]>=core_vm&&servers_info_map[j][2]>=mem_vm)
                        {
                            set_vm_map[today].push_back(IntInt(j,1));
                            vms_info_map[vm_id]=vector<int>{core_vm,mem_vm,j,1};
                            servers_info_map[j][0] -= core_vm;
                            servers_info_map[j][2] -= mem_vm;
                            servers_info_map[j][4] += core_vm;
                            servers_info_map[j][5] += mem_vm;
                            server_vminfo_map[j].push_back(vector<int>{vm_id,core_vm,mem_vm,1});
                            OK_flag =1;
                            break;
                        }
                        else if(servers_info_map[j][1]>=core_vm&&servers_info_map[j][3]>=mem_vm)
                        {
                            set_vm_map[today].push_back(IntInt(j,2));
                            vms_info_map[vm_id]=vector<int>{core_vm,mem_vm,j,2};
                            servers_info_map[j][1] -= core_vm;
                            servers_info_map[j][3] -= mem_vm;
                            servers_info_map[j][4] += core_vm;
                            servers_info_map[j][5] += mem_vm;
                            server_vminfo_map[j].push_back(vector<int>{vm_id,core_vm,mem_vm,2});
                            OK_flag =1;
                            break;
                        }
                    }
                }
                if(reqs_aday[i][3]==1 && OK_flag==0)
                {
                    for(int j=0;j<server_id_count;j++)
                    {
                        if(servers_info_map[j][0]>=core_vm/2&&servers_info_map[j][2]>=mem_vm/2)
                        {
                            if(servers_info_map[j][1]>=core_vm/2&&servers_info_map[j][3]>=mem_vm/2)
                            {
                                set_vm_map[today].push_back(IntInt(j,0));
                                vms_info_map[vm_id]=vector<int>{core_vm,mem_vm,j,0};
                                servers_info_map[j][0] -= core_vm/2;
                                servers_info_map[j][1] -= core_vm/2;
                                servers_info_map[j][2] -= mem_vm/2;
                                servers_info_map[j][3] -= mem_vm/2;
                                servers_info_map[j][4] += core_vm;
                                servers_info_map[j][5] += mem_vm;
                                server_vminfo_map[j].push_back(vector<int>{vm_id,core_vm,mem_vm,0});
                                OK_flag =1;
                                break;
                            }
                        }
                    }
                }
                if(OK_flag==1) //处理添加请求结束，读取下一条消息
                {continue;}
                
                // 接下来写扩容函数
                if(OK_flag==0) 
                {
                
                    // kuorong(reqs_aday[i],best_server_type);//应把当前请求处理了,并更新三个表
                    purchase_num++;
                    int half_core = server_type_map[best_server_type][0]/2;
                    int half_mem = server_type_map[best_server_type][1]/2;
                    servers_info_map[server_id_count] = vector<int>{half_core,half_core,half_mem,half_mem,0,0 };
                    int _server_id = server_id_count;
                    if(reqs_aday[i][3]==0)
                    {
                        set_vm_map[today].push_back(IntInt(_server_id,1));
                        vms_info_map[vm_id]=vector<int>{core_vm,mem_vm,server_id_count,1};
                        servers_info_map[server_id_count][0] -= core_vm;
                        servers_info_map[server_id_count][2] -= mem_vm;
                        servers_info_map[server_id_count][4] += core_vm;
                        servers_info_map[server_id_count][5] += mem_vm;
                        server_vminfo_map[server_id_count].push_back(vector<int>{vm_id,core_vm,mem_vm,1});

                    }
                    if(reqs_aday[i][3]==1)
                    {
                        set_vm_map[today].push_back(IntInt(_server_id,0));
                        vms_info_map[vm_id]=vector<int>{core_vm,mem_vm,server_id_count,0};
                        servers_info_map[server_id_count][0] -= core_vm/2;
                        servers_info_map[server_id_count][1] -= core_vm/2;
                        servers_info_map[server_id_count][2] -= mem_vm/2;
                        servers_info_map[server_id_count][3] -= mem_vm/2;
                        servers_info_map[server_id_count][4] += core_vm;
                        servers_info_map[server_id_count][5] += mem_vm;
                        server_vminfo_map[server_id_count].push_back(vector<int>{vm_id,core_vm,mem_vm,0});

                    }
                    server_id_count++;
                }
            }
            if(reqs_aday[i][0]==0)
            {
                int server_id = vms_info_map[vm_id][2];
                int vm_node = vms_info_map[vm_id][3];
                del_vm(server_id,vm_id,vm_node,core_vm,mem_vm);
                
            }
        }
        //输出购买指令
        // cout<<"今天是第 "<<today<<" 天,共 "<<set_vm_map[today].size()<<"条请求"<<endl;
        if(today!=0)
        {    
            if(purchase_num ==0)
            {cout<<"(purchase, 0)"<<endl;}
            else
            {cout<<"(purchase, 1)"<<endl;
            string _purchase_body ="(, )";
            _purchase_body.insert(3,int2str(purchase_num));
            _purchase_body.insert(1,best_server_type);
            cout<<_purchase_body<<endl;
            }
        }
        // 输出迁移指令
        string mig_head = "(migration, )";
        mig_head.insert(12,int2str(mig_list.size()));
        cout<<mig_head<<endl;
        if(mig_list.size()!=0)
        {for(int ij =0; ij<mig_list.size();ij++)
        {
            if(mig_list[ij].m_node==0)
            {cout<<"("<<mig_list[ij].m_vmid<<", "<<mig_list[ij].m_serverid<<")"<<endl;}
            else if(mig_list[ij].m_node==1)
            {cout<<"("<<mig_list[ij].m_vmid<<", "<<mig_list[ij].m_serverid<<", A)"<<endl;}
            else if(mig_list[ij].m_node==2)
            {cout<<"("<<mig_list[ij].m_vmid<<", "<<mig_list[ij].m_serverid<<", B)"<<endl;}
        }}
        // 输出部署指令
        // cout<<"今日待处理请求数："<<set_vm_map[today].size()<<endl;
        // for(auto it = set_vm_map[today].begin();it!=set_vm_map[today].end();it++)
        for(int i=0;i<set_vm_map[today].size();i++)
        {
            
            int _id = set_vm_map[today][i].m_id ;
            int oper = set_vm_map[today][i].m_node;
            switch(oper)
            {
                case -1:break;
                case 0 :cout<< "(" <<_id<< ")"<<endl;
                        break;
                case 1 :cout<< "(" <<_id<< ", " <<"A"<< ")"<<endl;
                        break;
                case 2 :cout<< "(" <<_id<< ", " <<"B"<< ")"<<endl;
                        break;
            }
        }
}

void migration(int mig_size)
{
    int mig_num = 0;
    int num = 1;
    int max_mig_num = floor((float)vms_info_map.size()/200) ;
    int bound = (int)0;
    bool OK_flag =1;
    while(mig_num<max_mig_num&&OK_flag==1&&mig_size>=num)
    {
        num++;
        OK_flag =0;
        int k=0;
        for(int i = server_id_count-1;i>=0;i-- )//从后往前遍历服务器
        {
            if(mig_num>=max_mig_num)
            {break;}
            if(0<server_vminfo_map[i].size()<num)   //如果服务器不是空的
            // if(server_vminfo_map[i].size()==1||server_vminfo_map[i].size()==2||server_vminfo_map[i].size()==3)   //如果服务器不是空的
            {
                // int server_id = i ;
                for(int j=0;j<server_vminfo_map[i].size();j++)  //遍历这个服务器里所有的虚拟机
                {
                    if(mig_num>=max_mig_num)
                    {break;}
                    int vm_id = server_vminfo_map[i][j][0];
                    int vm_core = server_vminfo_map[i][j][1];
                    int vm_mem = server_vminfo_map[i][j][2];
                    int node = server_vminfo_map[i][j][3];

                    for( k =bound;k<server_id_count;k++)      //从bound开始遍历所有服务器
                    {
                        if(k==i)
                        {continue;}
                        if(node==0)//如果虚拟机为双节点
                        {
                            if(servers_info_map[k][0]>=vm_core/2&&servers_info_map[k][1]>=vm_core/2)
                            {
                                if(servers_info_map[k][2]>=vm_mem/2&&servers_info_map[k][3]>=vm_mem/2)
                                {
                                    mig_list.push_back(IntIntInt(vm_id,k,0));
                                    del_vm(i,vm_id,node,vm_core,vm_mem);
                                    vms_info_map[vm_id]=vector<int>{vm_core,vm_mem,k,0};
                                    servers_info_map[k][0] -= vm_core/2;
                                    servers_info_map[k][1] -= vm_core/2;
                                    servers_info_map[k][2] -= vm_mem/2;
                                    servers_info_map[k][3] -= vm_mem/2;
                                    servers_info_map[k][4] += vm_core;
                                    servers_info_map[k][5] += vm_mem;
                                    server_vminfo_map[k].push_back(vector<int>{vm_id,vm_core,vm_mem,0});
                                    mig_num++;
                                    OK_flag =1;
                                    break;
                                }
                            }
                        }
                        else  //如果虚拟机不是单节点型
                        {
                            if(servers_info_map[k][0]>=vm_core&&servers_info_map[k][2]>=vm_mem)
                            {
                                del_vm(i,vm_id,node,vm_core,vm_mem);
                                mig_list.push_back(IntIntInt(vm_id,k,1));
                                vms_info_map[vm_id]=vector<int>{vm_core,vm_mem,k,1};
                                servers_info_map[k][0] -= vm_core;
                                servers_info_map[k][2] -= vm_mem;
                                servers_info_map[k][4] += vm_core;
                                servers_info_map[k][5] += vm_mem;
                                server_vminfo_map[k].push_back(vector<int>{vm_id,vm_core,vm_mem,1});
                                mig_num++;
                                OK_flag =1;
                                break;
                            }
                            if(servers_info_map[k][1]>=vm_core&&servers_info_map[k][3]>=vm_mem)
                            {
                                del_vm(i,vm_id,node,vm_core,vm_mem);
                                mig_list.push_back(IntIntInt(vm_id,k,2));
                                vms_info_map[vm_id]=vector<int>{vm_core,vm_mem,k,2};
                                servers_info_map[k][1] -= vm_core;
                                servers_info_map[k][3] -= vm_mem;
                                servers_info_map[k][4] += vm_core;
                                servers_info_map[k][5] += vm_mem;
                                server_vminfo_map[k].push_back(vector<int>{vm_id,vm_core,vm_mem,2});
                                mig_num++;
                                OK_flag =1;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}
void build_reqsmap(int total_day)
{
    string s;
    //读取请求数据，计算内存和核数出现的最大值
    int need_mem =0;
    int need_core =0;
    int reqs_num;
    for (int i =0;i<total_day;i++)
    {
        string type_temp; 
        string id_temp;
        cin>>s;
        reqs_num = str2int(s);
        for(int j =0;j<reqs_num;j++)
        {
            int core_vm;
            int mem_vm;
            int is_double;
            cin>>s;
            if(s[1]=='a')
            {
                cin>>type_temp>>id_temp;
                type_temp=type_temp.substr(0,type_temp.size()-1);
                id_temp=id_temp.substr(0,id_temp.size()-1);
                core_vm = vm_type_map[type_temp][0];
                mem_vm = vm_type_map[type_temp][1];
                all_vmcores += core_vm;
                all_vmmem += mem_vm;
                if(all_vmcores>max_vmcores){max_vmcores=all_vmcores;};
                if(all_vmmem>max_vmmem){max_vmmem=all_vmmem;};
                if(is_double)
                {   need_core = core_vm;
                    need_mem = mem_vm;
                    if(max_need_core<need_core)
                    {max_need_core=need_core;
                    if(need_mem>max_core_mem)
                    {max_core_mem =need_mem; }
                    }
                    if(max_need_mem<need_mem)
                    {max_need_mem=need_mem;
                    if(need_core>max_mem_core)
                    {max_mem_core =need_core; }}
                }
                if(!is_double)
                {
                    need_core = core_vm*2;
                    need_mem = mem_vm*2;
                    if(max_need_core<need_core)
                    {max_need_core=need_core;
                    if(need_mem>max_core_mem)
                    {max_core_mem =need_mem; }
                    }
                    if(max_need_mem<need_mem)
                    {max_need_mem=need_mem;
                    if(need_core>max_mem_core)
                    {max_mem_core =need_core; }}
                }
                vmIdType_map[id_temp]=type_temp;
                is_double = vm_type_map[type_temp][2];
                //保存请求
                reqs_map[i].push_back({1,core_vm,mem_vm,is_double,str2int(id_temp)});
            }
            else if(s[1]=='d')
            {
                cin>>id_temp;
                id_temp=id_temp.substr(0,id_temp.size()-1);
                type_temp = vmIdType_map[id_temp];
                core_vm = vm_type_map[type_temp][0];
                mem_vm = vm_type_map[type_temp][1];
                is_double = vm_type_map[type_temp][2];
                all_vmcores -= core_vm;
                all_vmmem -= mem_vm;
                //保存删除请求
                reqs_map[i].push_back({0,core_vm,mem_vm,is_double,str2int(id_temp)});
            }
        }
    }
}

void read_req_map()
{
    vector<vector<int>> reqs_aday ; //用来保存一天的请求
    for(today = 0;today<total_day;today++) //遍历所有天
    {
        if(a_bound>0||today%10==0)
        {a_bound--;} //随着天数增加，减少A*算法的使用概率

        mig_list.clear();
        migration(4);
        purchase_num =0; //清零购买值

        //得到当前性价比最高的，可以装下最大虚拟机的服务器
        best_server_type = find_best_server(max_need_core,max_need_mem,today,total_day);
        reqs_aday = reqs_map[today];
        int _reqs_size = reqs_aday.size();

        if(_reqs_size>a_bound||today ==0) //第一天或者请求数很多时，只进行传统部署
        {
            old_setvm(reqs_aday); 
        }

        else                            //请求数较少，可以使用a*算法
        {
            Astar_setvm(reqs_aday);
        }
    } //结束一天请求读取的括号
}//函数结束的括号



int main()////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////主函数
{
    int reqs_count = 0;
    int day = 0;
    int num = 0;
    int amount = 0;
    string s;
    // 读取服务器和虚拟机类型数据，建立服务器类型表和虚拟机类型表--------------------------------------------------------------------------------------
    // std::freopen("training-1.txt","rb",stdin);
    // std::freopen("test.txt","rb",stdin);
    cin>>s; 
    amount = str2int(s);//获取服务器总数
    for (int i = 0; i < amount; i++)
    {
        string type_temp,core_temp,mem_temp,purchasecost_temp,runcost_temp;
        cin>>type_temp>>core_temp>>mem_temp>>purchasecost_temp>>runcost_temp;
        generateServer(type_temp,core_temp,mem_temp,purchasecost_temp,runcost_temp);
    }
    //建立虚拟机类型表
    cin>>s;
    amount = str2int(s);
    for (int i = 0; i < amount; i++)
    {
        string type_temp;
        string core_temp,mem_temp,isdouble;
        cin>>type_temp>>core_temp>>mem_temp>>isdouble;
        generateVm(type_temp,core_temp,mem_temp,isdouble);
    }
    int reqs_num = 0;
    cin>>s;
    total_day = str2int(s);

    build_reqsmap(total_day); //读取所有请求并建立reqs_map，并得到最大值

    //计算各服务器相对性价比，并建立服务器类型-性价比排序表（相对性价比，服务器类型名）
    value_piancha = (float)(2*max_vmmem/max_vmcores);
    for(auto it =server_type_map.begin();it!=server_type_map.end();it++)
    {
        float cost_perday = 0.0;
        string type_temp = it->first;
        int servercost_temp = it->second[2];
        int runcost_temp = it->second[3];
        int server_core_temp =  it->second[0];
        int server_mem_temp = it->second[1];
        int core_mem_err = server_core_temp - server_mem_temp;
        cost_perday = (float)(servercost_temp/total_day + runcost_temp*piancha_run_purchse)/(server_core_temp*value_piancha+server_mem_temp);  //性价比公式
        serverTypeCostSet.insert(SFI(type_temp,cost_perday,core_mem_err));
    }

    // 购置第一批服务器
    First_purchase_pro(max_vmcores,max_vmmem,max_need_core,max_need_mem);

    //开始读取reqs并处理
    read_req_map();




    return 0 ;
}