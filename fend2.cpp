#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <time.h>
#include <algorithm>
#include <assert.h>
#include <set>
#include <list>
#include <omp.h>
#include<math.h>
using namespace std;

// ofstream outfile("output_old.txt", ios::trunc);

//可调参数

int server_choose_times =1;
int a_bound = 10000000;//请求数小于这个值时，使用爬山算法
int max_open_list_size = 2000;  //越小规划越快,越大规划地越好

float value_piancha;//弥补普遍核数比内存数少的量纲问题，认为核数量纲为内存的v_p倍
float piancha_size = 0;//弥补尺寸越大，灵活性越小的问题。

//灵魂调参点
float piancha_run_purchse = 3; //越大purchase越重要
float purchase_wt= 6;         //越大购买价格更重要
float vl_wt = 2;  //增大核数内存量纲比
float vl_wt2 = 2;//用在部署的量纲扩张系数

//f2.0添加-------------------------------------------------------------------------------------------f2.0添加
int max_ser_search_size = 10;  //完美匹配探索的服务器数量，越大越好小于server_type_num，耗时也越长 
int setvm_minsize = 1; //决定完美匹配方案的探索起点
int setvm_maxsize = 5; //决定完美匹配方案的探索终点 
int cm_wt = 3;   //完美匹配里决定虚拟机大小的核数量纲系数



int add_size_yesterday_max = 0;
bool super_mig_flag =0;

typedef struct A
{
    string type;
    int cpu;
    int memory;
    int cost;
    int cost_day;
}server;

typedef struct B
{
    string type;
    int cpu;
    int memory;
    int is_double_node;
}vm;

typedef struct C
{
    string type;
    int A_cpu;
    int B_cpu;
    int A_memory;
    int B_memory;
    int cpu;
    int memory;
}server_info;

typedef struct D
{
    string type;
    int vm_id;
    int vm_core;
    int vm_mem;
    int is_double_node;
}server_vminfo;

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
class compareIII
{
    public:
    bool operator()(const IntIntInt&iii1 ,const IntIntInt&iii2) const
    {
        // 排序方式
        return iii1.m_vmid<iii2.m_vmid;         //按虚拟机序号升序排序

    }
};

class IntInt
{
public:
    IntInt(){};
    IntInt(int id,int node)
    {
        this ->m_id = id;
        this ->m_node = node;
    }
    int m_id;
    int m_node;
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

class rate
{
public:
    rate(int id,int rate)
    {
        this ->m_id = id;
        this ->m_rate = rate;
    }
    int m_id;
    int m_rate;
};

class SFI
{
public:
    SFI(string Type,float num1,int num2,double rate)
    {
        this ->m_Type = Type;
        this ->m_num1 = num1;
        this ->m_num2 = num2;
	this ->m_num3 = rate;
    }
    string m_Type;
    float m_num1;
    int m_num2;
    double m_num3;
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


class FIII
{
public:
    FIII(float num1,int num2,int num3,int num4)
    {
        this ->m_F = num1;
        this ->m_vmid = num2;
        this ->m_serverid = num3;
        this ->m_oper = num4;
    }
    float m_F;
    int m_vmid;
    int m_serverid;
    int m_oper;
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
class VI
{
    public:
        VI(vector<int> vec)
        {
            this->m_vec = vec;
        }
        vector<int> m_vec;
};
class compareVI9
{
    public:
    bool operator()(const VI&vi1 ,const VI&vi2) const
    {
        // 排序方式
        if(vi1.m_vec[8] !=vi2.m_vec[8])
        {return vi1.m_vec[8] >vi2.m_vec[8];}
        else{return vi1.m_vec[0]>vi2.m_vec[0];}    
    }
};
class compareVI34
{
    public:
    bool operator()(const VI&vi1 ,const VI&vi2) const
    {
        // 排序方式
        if(vi1.m_vec[3] !=vi2.m_vec[3])
        {return vi1.m_vec[3] >vi2.m_vec[3];}
        else{return vi1.m_vec[4]>vi2.m_vec[4];}    
    }
};
class compareVIcore
{
    public:
    bool operator()(const VI&vi1 ,const VI&vi2) const
    {
        // 排序方式
        if(vi1.m_vec[4] !=vi2.m_vec[4])         //先按单双节点排序，双节点在前
        {return vi1.m_vec[4] >vi2.m_vec[4];}
        else if(vi1.m_vec[6]!=vi1.m_vec[6])     //再按cmerr排序
        {return vi1.m_vec[6]>vi2.m_vec[6];}
        else                                    //保底按vm序号排序
        {return vi1.m_vec[1]<vi2.m_vec[1];}    
    }
};

class compareVImem
{
    public:
    bool operator()(const VI&vi1 ,const VI&vi2) const
    {
        // 排序方式
        if(vi1.m_vec[4] !=vi2.m_vec[4])         //先按单双节点排序，双节点在前
        {return vi1.m_vec[4] >vi2.m_vec[4];}
        else if(vi1.m_vec[6]!=vi1.m_vec[6])     //再按cmerr排序
        {return vi1.m_vec[6]<vi2.m_vec[6];}
        else
        {return vi1.m_vec[1]<vi2.m_vec[1];}    //保底按vm序号排序
    }
};
class VIVII
{
    public:
        VIVII(vector<int> vec1,vector<IntInt>vec2 )
        {
            this->m_info = vec1;  //配置方案总信息向量
            this->m_set = vec2;  //配置方案向量
        }
        vector<int> m_info;
        vector<IntInt> m_set;
};

class SLVIVII
{
    public:
        SLVIVII(string type,list<VIVII> set_list)
        {
            this->m_type = type;
            this->m_set_list = set_list;
        }
        string m_type;
        list<VIVII> m_set_list;
};


class SVIVII
{
    public:
        SVIVII(){};
        SVIVII(string type,vector<int> vec1,vector<IntInt>vec2 )
        {
            this->m_type = type;  //服务器类型
            this->m_info = vec1;  //配置方案总信息向量
            this->m_set = vec2;  //配置方案向量
        }
        string m_type;
        vector<int> m_info;
        vector<IntInt> m_set;
};
class compareSVIVII
{
    public:
    bool operator()(const SVIVII&vivii1 ,const SVIVII&vivii2) const
    {
        // 排序方式
        if(vivii1.m_info[1] !=vivii2.m_info[1])   //先按函数值排序
        {return vivii1.m_info[1] >vivii2.m_info[1];}
        else{return vivii1.m_info[0]>vivii2.m_info[0];}    //保底用虚拟机数量排序
    }
};

class compareFVIII
{
    public:
    bool operator()(const FVIII&fv1 ,const FVIII&fv2) const
    {
        // 排序方式为升序

        if(fv1.m_F !=fv2.m_F)                                       //先尝试按F值升序排列
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

class compareFIII
{
    public:
    bool operator()(const FIII&fiii1 ,const FIII&fiii2) const
    {
        // 排序方式为升序

        if(fiii1.m_F !=fiii2.m_F)                                       //先尝试按F值升序排列
        return fiii1.m_F <fiii2.m_F;
        else if(fiii1.m_serverid!=fiii2.m_serverid)                    //如果F值相同，则按服务器id升序排名                       
        return fiii1.m_serverid<fiii2.m_serverid;
        else                                                    //如果F，服务器id都相同，按vm_id排序
        return fiii1.m_vmid<fiii2.m_vmid; 
               
        
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
class compareStrFloat
{
public:
    bool operator()(const StrFloat&sf1 ,const StrFloat&sf2) const
    {
        // 排序方式为升序
        return sf1.m_num <sf2.m_num;
    }
};

class comparerate
{
public:
    bool operator()(const rate&sf1 ,const rate&sf2) const
    {
        // 排序方式为升序
        return sf1.m_rate > sf2.m_rate;
    }
};

class compareSFI
{
public:
    bool operator()(const SFI&sfi1 ,const SFI&sfi2) const
    {
        // 排序方式为升序
        if(sfi1.m_num1 !=sfi2.m_num1)
        return sfi1.m_num1 <sfi2.m_num1;
        else
        return sfi1.m_num3>sfi2.m_num3;

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

//虚拟机消失时间map
unordered_map<int,IntInt> vms_time;
unordered_map<int,int> add_vmsize;

//用于完美匹配的set和list
// set<SVIVII,compareSVIVII> perfect_sets;  //存放和排序部署方案的set
set<VI,compareVIcore> unset_vms_core;   //核数大于内存数的未布置vms
set<VI,compareVImem> unset_vms_mem;     //内存数大于核数的未布置vms
list<SLVIVII> pur_set_list;               //今天的购买、部署方案
SVIVII perfect_set;                     //存放最佳的购买部署方案
set<IntIntInt,compareIII> insert_set;              //用于插入部署向量


// 服务器信息
unordered_map<string,server> server_type_map;
// 虚拟机信息
unordered_map<string,vm> vm_type_map;
// 请求信息
vector<vector<string>> requestInfos;
// 已有服务器信息
unordered_map<int,server_info> servers_info_map;
// 已有虚拟机数值信息
unordered_map<int,vector<int>> vms_info_map;
//服务器类型运行成本表
set<SFI,compareSFI> serverTypeCostSet;
set<SFI,compareSFI> serverTypeCostSet_today;
//服务器利用率表
set<rate,comparerate> serverrateSet;
//虚拟机类型大小表
set<StrInt,compareStrInt> vmTypeSizeSet;
//已部署虚拟机信息
unordered_map<string,string> vmIdType_map;
//已购买服务器编号和类型信息
unordered_map<int,string> serverIdType_map;
//全部请求信息
unordered_map<int,vector<vector<int>>> reqs_map;

// 高性价比服务器列表
set<SFIII,compareSFIII> purchase_type_set;

//每天的迁移指令表
vector<IntIntInt> mig_list;


// 每台服务器里的虚拟机信息
unordered_map<int,vector<vector<int>>> server_vminfo_map;  //severid:{{vm_id,cores,mems,0orAorB},......}

//A*算法开集组件
// set<FVIII,compareFVIII> open_list;
vector<IntIntInt> oper_list_copy;
set<FIII,compareFIII> open_list_cb;
vector<FIII> oper_list_cb;

vector<IntIntInt> oper_list_scti;
set<FVIII,compareFVIII> open_list_sct;
vector<FIII> oper_list_sctf;
FVIII oper_list_sct = {0.0,{{0,0,0}}};

string best_server_type;
int max_need_mem=0;
int max_need_core =0;
int server_id_count=0;

int purchase_type_num = 0; //定义购买请求的类型数
int server_num =0;
int vm_num=0;

int max_core_mem=0;
int max_mem_core=0;
int total_day;
int today;
int first_read_lenth;

int all_vmcores = 0;
int all_vmmem =0;
int max_vmcores = 0;
int max_vmmem =0;

int migration_num=0;
int migration_num_max=0;
long long SERVERCOST = 0,POWERCOST=0,TOTALCOST =0;
clock_t start, finish;

int purchase_num_climb;

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


void generateVm(string vmType,string vmCpuCores,string vmMemory,string vmTwoNodes){
    string _vmType ;


    _vmType = vmType.substr(1,vmType.size()-2);

    int _vmCpuCores = 0,_vmMemory=0,_vmTwoNodes=0;

    _vmCpuCores = str2int(vmCpuCores.substr(0,vmCpuCores.size()-1));

    _vmMemory = str2int(vmMemory.substr(0,vmMemory.size()-1));

    _vmTwoNodes = str2int(vmTwoNodes.substr(0,vmTwoNodes.size()-1));

    vm_type_map[_vmType].cpu = _vmCpuCores;
    vm_type_map[_vmType].memory = _vmMemory;
    vm_type_map[_vmType].is_double_node = _vmTwoNodes;

    int _vmSize = _vmCpuCores +_vmMemory + _vmTwoNodes*10000; //计算vm大小，使双节点恒大于单节点，先部署大
    vmTypeSizeSet.insert(StrInt(_vmType,_vmSize));

}

void generateServer(string serverType,string cpuCores,string memorySize,string serverCost,string powerCost){
    string _serverType;

    _serverType = serverType.substr(1,serverType.size()-2);

    int _cpuCores=0,_memorySize=0,_serverCost=0,_powerCost=0;
    _cpuCores = str2int(cpuCores.substr(0,cpuCores.size()-1));
    _memorySize = str2int(memorySize.substr(0,memorySize.size()-1));
    _serverCost = str2int(serverCost.substr(0,serverCost.size()-1));
    _powerCost = str2int(powerCost.substr(0,powerCost.size()-1));

    // 更新服务器类型表和服务器类型性价比排序表
    server_type_map[_serverType].cpu = _cpuCores;
    server_type_map[_serverType].memory = _memorySize;
    server_type_map[_serverType].cost = _serverCost;
    server_type_map[_serverType].cost_day = _powerCost;


}
void del_vm(int server_id,int vm_id ,int vm_node,int vm_core,int vm_mem)
{
    if(vm_node==0)
    {
        // set_vm_map[today].push_back(IntInt(-1,-1)); //表示无需操作
        auto it_temp = vms_info_map.find(vm_id);
        if(it_temp !=vms_info_map.end())
        {vms_info_map.erase(it_temp);}//删除虚拟机信息
        servers_info_map[server_id].A_cpu += vm_core/2;
        servers_info_map[server_id].B_cpu += vm_core/2;
        servers_info_map[server_id].A_memory += vm_mem/2;
        servers_info_map[server_id].B_memory += vm_mem/2;
        servers_info_map[server_id].cpu -= vm_core;
        servers_info_map[server_id].memory -= vm_mem;
        for(auto it_temp2 =server_vminfo_map[server_id].begin();it_temp2!=server_vminfo_map[server_id].end();it_temp2++)
        {   
            // cout<<"看看指针取了个啥出来："<<(*it_temp2)[0]<<endl;
            if((*it_temp2)[0]==vm_id)
            {server_vminfo_map[server_id].erase(it_temp2);
            break;} //删除服务器虚拟机信息
        }

    }
    if(vm_node==1)
    {
        // set_vm_map[today].push_back(IntInt(-1,-1)); //表示无需操作
        auto it_temp = vms_info_map.find(vm_id);
        if(it_temp !=vms_info_map.end())
        {vms_info_map.erase(it_temp);}//删除虚拟机信息
        servers_info_map[server_id].A_cpu += vm_core;
        servers_info_map[server_id].A_memory += vm_mem;
        servers_info_map[server_id].cpu -= vm_core;
        servers_info_map[server_id].memory -= vm_mem;
        
        for(auto it_temp2 =server_vminfo_map[server_id].begin();it_temp2!=server_vminfo_map[server_id].end();it_temp2++)
        {   
            // cout<<"看看指针取了个啥出来："<<(*it_temp2)[0]<<endl;
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
        servers_info_map[server_id].B_cpu += vm_core;
        servers_info_map[server_id].B_memory += vm_mem;
        servers_info_map[server_id].cpu -= vm_core;
        servers_info_map[server_id].memory -= vm_mem;
        for(auto it_temp2 =server_vminfo_map[server_id].begin();it_temp2!=server_vminfo_map[server_id].end();it_temp2++)
        {   
            // cout<<"看看指针取了个啥出来："<<(*it_temp2)[0]<<endl;
            if((*it_temp2)[0]==vm_id)
            {server_vminfo_map[server_id].erase(it_temp2);
            break;} //删除服务器虚拟机信息
        }

    }
    vm_num--;
}


void migration(int max_mignum)
{
    int mig_num = 0;

    int max_mig_num =max_mignum ;
    // max_mig_num -= 1;
    int key = 1;
    double cpuRate=0.0;
    double memoryRate=0.0;
    double cpumemRate=0.0;
    string Type;
    server _serverInfo;
    server_info _useserver;

    double Rate_err=0.1; 
    int Vast_vmid=0;
    int Vast_vmcpu=0;
    int Vast_vmmemory=0;
    int Vast_vmnode=0;
    int point=0;

        serverrateSet.clear();
	for(int i=0;i<server_id_count;i++ )
    	{
	    int rate_all = (servers_info_map[i].A_cpu+servers_info_map[i].B_cpu) - (servers_info_map[i].A_memory+servers_info_map[i].B_memory);
	    if(rate_all<0)
	    {rate_all = -rate_all;}
	    serverrateSet.insert(rate(i,rate_all));
	}

    	for(auto it=serverrateSet.begin();it!=serverrateSet.end();it++)
    	{
	    int i=it->m_id; 
	    if(mig_num>=max_mig_num)
            {
		break;
            }
	    if( server_vminfo_map[i].size()==0 )
	    {
		continue;
	    }
	    	int rate_all = (servers_info_map[i].A_cpu+servers_info_map[i].B_cpu) - (servers_info_map[i].A_memory+servers_info_map[i].B_memory);
        // int rate_all = it->m_rate;
		if(rate_all>0)
	    	{
		    point = 1;
		}
		if(rate_all<0)
		{
		    point = -1;
		}
		if(point==0)
		{
		    continue;
		}
		Vast_vmid = 0; 
		Vast_vmcpu = 0;
		Vast_vmmemory = 0;
		Vast_vmnode = 0;
	    	for(int j=0;j<server_vminfo_map[i].size();j++) 
                {
		    int vm_id = server_vminfo_map[i][j][0];
                    int vm_core = server_vminfo_map[i][j][1];
                    int vm_mem = server_vminfo_map[i][j][2];
                    int node = server_vminfo_map[i][j][3];
		    if(vm_core>Vast_vmcpu && point==1)
		    {
			Vast_vmid = vm_id; 
			Vast_vmcpu = vm_core;
			Vast_vmmemory = vm_mem;
			Vast_vmnode = node;
		    }
		    if(vm_mem>Vast_vmmemory && point==-1)
		    {
			Vast_vmid = vm_id; 
			Vast_vmcpu = vm_core;
		        Vast_vmmemory = vm_mem;
			Vast_vmnode = node;
		    }
		}
		//for(int k=i+1;k<server_id_count;k++)      
                //{
		for(auto its=serverrateSet.end();its!=serverrateSet.begin();its--)
    	        {
	            int k=its->m_id;
		    if(server_vminfo_map[k].size()==0 || k==i)
                    {
			continue;
		    }
		    if(Vast_vmnode==0)//如果虚拟机为双节点
                    {
                       if(servers_info_map[k].A_cpu>=Vast_vmcpu/2 && servers_info_map[k].B_cpu>=Vast_vmcpu/2 && servers_info_map[k].A_memory>=Vast_vmmemory/2 && servers_info_map[k].B_memory>=Vast_vmmemory/2)
                       {
                           mig_list.push_back(IntIntInt(Vast_vmid,k,0));
                           del_vm(i,Vast_vmid,Vast_vmnode,Vast_vmcpu,Vast_vmmemory);
                           vms_info_map[Vast_vmid]=vector<int>{Vast_vmcpu,Vast_vmmemory,k,0};
                           servers_info_map[k].A_cpu -= Vast_vmcpu/2;
                           servers_info_map[k].B_cpu -= Vast_vmcpu/2;
                           servers_info_map[k].A_memory -= Vast_vmmemory/2;
                           servers_info_map[k].B_memory -= Vast_vmmemory/2;
                           servers_info_map[k].cpu += Vast_vmcpu;
                           servers_info_map[k].memory += Vast_vmmemory;
                           server_vminfo_map[k].push_back(vector<int>{Vast_vmid,Vast_vmcpu,Vast_vmmemory,0});
                           mig_num++;
                           break;
                        }
                    }
                    else if(Vast_vmnode!=0 && (servers_info_map[k].A_cpu+servers_info_map[k].A_memory)>(servers_info_map[k].B_cpu+servers_info_map[k].B_memory)) //如果虚拟机布置在A
                    {
                        if(servers_info_map[k].A_cpu>=Vast_vmcpu&&servers_info_map[k].A_memory>=Vast_vmmemory)
                        {
                            del_vm(i,Vast_vmid,Vast_vmnode,Vast_vmcpu,Vast_vmmemory);
                            mig_list.push_back(IntIntInt(Vast_vmid,k,1));
                            vms_info_map[Vast_vmid]=vector<int>{Vast_vmcpu,Vast_vmmemory,k,1};
                            servers_info_map[k].A_cpu -= Vast_vmcpu;
                            servers_info_map[k].A_memory -= Vast_vmmemory;
                            servers_info_map[k].cpu += Vast_vmcpu;
                            servers_info_map[k].memory += Vast_vmmemory;
                            server_vminfo_map[k].push_back(vector<int>{Vast_vmid,Vast_vmcpu,Vast_vmmemory,1});
                            mig_num++;
                            break;
                         }
                     }
                     else if(Vast_vmnode!=0 && (servers_info_map[k].A_cpu+servers_info_map[k].A_memory)<(servers_info_map[k].B_cpu+servers_info_map[k].B_memory)) //如果虚拟机布置在B
                     {
                         if(servers_info_map[k].B_cpu>=Vast_vmcpu&&servers_info_map[k].B_memory>=Vast_vmmemory)
                         {
                             del_vm(i,Vast_vmid,Vast_vmnode,Vast_vmcpu,Vast_vmmemory);
                             mig_list.push_back(IntIntInt(Vast_vmid,k,2));
                             vms_info_map[Vast_vmid]=vector<int>{Vast_vmcpu,Vast_vmmemory,k,2};
                             servers_info_map[k].B_cpu -= Vast_vmcpu;
                             servers_info_map[k].B_memory -= Vast_vmmemory;
                             servers_info_map[k].cpu += Vast_vmcpu;
                             servers_info_map[k].memory += Vast_vmmemory;
                             server_vminfo_map[k].push_back(vector<int>{Vast_vmid,Vast_vmcpu,Vast_vmmemory,2});
                             mig_num++;
                             break;
                         }
                     }
		 }
	}



        serverrateSet.clear();
	for(int i=0;i<server_id_count;i++ )
    	{
	    Type = serverIdType_map[i];
	    _serverInfo = server_type_map[Type];
	    int rate_all = servers_info_map[i].A_cpu+servers_info_map[i].B_cpu + servers_info_map[i].A_memory+servers_info_map[i].B_memory;
	    serverrateSet.insert(rate(i,rate_all));
	}
    	for(auto it=serverrateSet.begin();it!=serverrateSet.end();it++)
    	{
	    int i=it->m_id; 
            if(mig_num>=max_mig_num)
            {
		break;
	    }
	    if(server_vminfo_map[i].size()==0)  
            {
		continue;
	    }
                for(int j=0;j<server_vminfo_map[i].size();j++)
    	        {
                    if(mig_num>=max_mig_num)
                    {
			break;
		    }
                    int vm_id = server_vminfo_map[i][j][0];
                    int vm_core = server_vminfo_map[i][j][1];
                    int vm_mem = server_vminfo_map[i][j][2];
                    int node = server_vminfo_map[i][j][3];
                    for(auto its=serverrateSet.end();its!=serverrateSet.begin();its--)
    	            {
	                int k=its->m_id;
		    //for(int k=0;k<server_id_count;k++)
		    //{
                        if(k==i || server_vminfo_map[k].size()==0)
                        {
			    continue;
			}
                        if(node==0)//如果虚拟机为双节点
                        {
                            if(servers_info_map[k].A_cpu>=vm_core/2 && servers_info_map[k].B_cpu>=vm_core/2 && servers_info_map[k].A_memory>=vm_mem/2 && servers_info_map[k].B_memory>=vm_mem/2)
                            {
                                mig_list.push_back(IntIntInt(vm_id,k,0));
                                del_vm(i,vm_id,node,vm_core,vm_mem);
                                vms_info_map[vm_id]=vector<int>{vm_core,vm_mem,k,0};
                                servers_info_map[k].A_cpu -= vm_core/2;
                                servers_info_map[k].B_cpu -= vm_core/2;
                                servers_info_map[k].A_memory -= vm_mem/2;
                                servers_info_map[k].B_memory -= vm_mem/2;
                                servers_info_map[k].cpu += vm_core;
                                servers_info_map[k].memory += vm_mem;
                                server_vminfo_map[k].push_back(vector<int>{vm_id,vm_core,vm_mem,0});
                                mig_num++; 
				j=0;
                                break;

                            }
                        }
                        else if(node!=0 && (servers_info_map[k].A_cpu+servers_info_map[k].A_memory)>(servers_info_map[k].B_cpu+servers_info_map[k].B_memory)) //如果虚拟机布置在A
                        {
                            if(servers_info_map[k].A_cpu>=vm_core&&servers_info_map[k].A_memory>=vm_mem)
                            {
                                del_vm(i,vm_id,node,vm_core,vm_mem);
                                mig_list.push_back(IntIntInt(vm_id,k,1));
                                vms_info_map[vm_id]=vector<int>{vm_core,vm_mem,k,1};
                                servers_info_map[k].A_cpu -= vm_core;
                                servers_info_map[k].A_memory -= vm_mem;
                                servers_info_map[k].cpu += vm_core;
                                servers_info_map[k].memory += vm_mem;
                                server_vminfo_map[k].push_back(vector<int>{vm_id,vm_core,vm_mem,1});
                                mig_num++;
				j=0;
                                break;
                            }

                        }
                        else if(node!=0 && (servers_info_map[k].A_cpu+servers_info_map[k].A_memory)<(servers_info_map[k].B_cpu+servers_info_map[k].B_memory)) //如果虚拟机布置在B
                        {
                            if(servers_info_map[k].B_cpu>=vm_core&&servers_info_map[k].B_memory>=vm_mem)
                            {
                                del_vm(i,vm_id,node,vm_core,vm_mem);
                                mig_list.push_back(IntIntInt(vm_id,k,2));
                                vms_info_map[vm_id]=vector<int>{vm_core,vm_mem,k,2};
                                servers_info_map[k].B_cpu -= vm_core;
                                servers_info_map[k].B_memory -= vm_mem;
                                servers_info_map[k].cpu += vm_core;
                                servers_info_map[k].memory += vm_mem;
                                server_vminfo_map[k].push_back(vector<int>{vm_id,vm_core,vm_mem,2});
                                mig_num++;
				j=0;
                                break;
                            }

                        }
			
                    }
                }
        }
}

// 扩容时找到最佳服务器
string Find_server(int vm_allcore, int vm_allmemory, int vm_maxcore, int vm_maxmem,int sct)
{

    serverTypeCostSet_today.clear();
    int _needvm_core = vm_allcore;
    int _needvm_memory = vm_allmemory;

    int _vm_maxcore = vm_maxcore;
    int _vm_maxmemory = vm_maxmem;
    double vm_maxrate = double(_needvm_core) / double(_needvm_memory);
    double _cmrate=0.0;
    double min=100.0;
    string best_server;
    for(auto it=serverTypeCostSet.begin();it!= serverTypeCostSet.end();it++)
    {
	if(server_type_map[it->m_Type].cpu>=_vm_maxcore&&server_type_map[it->m_Type].memory>=_vm_maxmemory)
        {
            if(it->m_num2!=0)
            {	
                string f_type = it->m_Type;
                double f_rate = it->m_num3;
                if(vm_maxrate>=1)
                {
                        if( f_rate < 1 )
                        {
                            _cmrate = 100 + (vm_maxrate -f_rate)/vm_maxrate;
                            _cmrate += purchase_wt*(it->m_num1);
                            serverTypeCostSet_today.insert(SFI(f_type,float(_cmrate),it->m_num2,double(it->m_num1)));
                        }
                        else
                        {
                            if(vm_maxrate<f_rate)
                           { _cmrate =( f_rate -vm_maxrate)/vm_maxrate;
                           _cmrate += purchase_wt*(it->m_num1);
                            serverTypeCostSet_today.insert(SFI(f_type,float(_cmrate),it->m_num2,double(it->m_num1)));}
                            else
                           { _cmrate =( vm_maxrate- f_rate)/vm_maxrate ;
                           _cmrate += purchase_wt*(it->m_num1);
                            serverTypeCostSet_today.insert(SFI(f_type,float(_cmrate),it->m_num2,double(it->m_num1)));}

                        }
                }
                if(vm_maxrate<1)
                {
                        if( f_rate >= 1 )
                        {
                            _cmrate = 100 + ( f_rate -vm_maxrate)/vm_maxrate;
                            _cmrate += purchase_wt*(it->m_num1);
                            serverTypeCostSet_today.insert(SFI(f_type,float(_cmrate),it->m_num2,double(it->m_num1)));
                        }
                        else
                        {
                            if(vm_maxrate<f_rate)
                           { _cmrate =( f_rate -vm_maxrate)/vm_maxrate;
                           _cmrate += purchase_wt*(it->m_num1);
                            serverTypeCostSet_today.insert(SFI(f_type,float(_cmrate),it->m_num2,double(it->m_num1)));}
                            else
                           { _cmrate =( vm_maxrate- f_rate)/vm_maxrate;
                           _cmrate += purchase_wt*(it->m_num1);
                            serverTypeCostSet_today.insert(SFI(f_type,float(_cmrate),it->m_num2,double(it->m_num1)));}

                        }
                }
        
            }
	    }
    }
    int i =0;
    for(auto it_t=serverTypeCostSet_today.begin();it_t!=serverTypeCostSet_today.end();it_t++ )
    {
        if(i ==sct)
        {
            best_server =it_t->m_Type;
            break;
        }
        i+=1;
        
    }
    return best_server;
}

//A*相关函数、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、
void add_vm(int server_id,int vm_id,int vm_node,int vm_core ,int vm_mem)
{
switch(vm_node)
{
    case 0:
        vms_info_map[vm_id]=vector<int>{vm_core,vm_mem,server_id,0};
        servers_info_map[server_id].A_cpu -= vm_core/2;
        servers_info_map[server_id].B_cpu -= vm_core/2;
        servers_info_map[server_id].A_memory -= vm_mem/2;
        servers_info_map[server_id].B_memory -= vm_mem/2;
        servers_info_map[server_id].cpu += vm_core;
        servers_info_map[server_id].memory += vm_mem;
        server_vminfo_map[server_id].push_back(vector<int>{vm_id,vm_core,vm_mem,0});
        break;
    case 1:
        vms_info_map[vm_id]=vector<int>{vm_core,vm_mem,server_id,1};
        servers_info_map[server_id].A_cpu -= vm_core;
        servers_info_map[server_id].A_memory -= vm_mem;
        servers_info_map[server_id].cpu += vm_core;
        servers_info_map[server_id].memory += vm_mem;
        server_vminfo_map[server_id].push_back(vector<int>{vm_id,vm_core,vm_mem,1});
        break;
    case 2:
        vms_info_map[vm_id]=vector<int>{vm_core,vm_mem,server_id,2};
        servers_info_map[server_id].B_cpu -= vm_core;
        servers_info_map[server_id].B_memory -= vm_mem;
        servers_info_map[server_id].cpu += vm_core;
        servers_info_map[server_id].memory += vm_mem;
        server_vminfo_map[server_id].push_back(vector<int>{vm_id,vm_core,vm_mem,2});
        break;
}
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
        int _vm_id ;
        int _core_vm ;
        int _mem_vm ;
        switch(_oper_list[i].m_node)
        {
            case -1: //删除
                _vm_id = _reqs_list[_req_count][4];
                _core_vm = _reqs_list[_req_count][1];
                _mem_vm =  _reqs_list[_req_count][2];
                del_vm(_server_id,_vm_id,0,_core_vm,_mem_vm);
                break;
            case -2: //删除
                _vm_id = _reqs_list[_req_count][4];
                _core_vm = _reqs_list[_req_count][1];
                _mem_vm =  _reqs_list[_req_count][2];
                del_vm(_server_id,_vm_id,1,_core_vm,_mem_vm);
                break;
            case -3: //删除
                _vm_id = _reqs_list[_req_count][4];
                _core_vm = _reqs_list[_req_count][1];
                _mem_vm =  _reqs_list[_req_count][2];
                del_vm(_server_id,_vm_id,2,_core_vm,_mem_vm);
                break;
            case 0: //_server_id双节点添加
                _vm_id = _reqs_list[_req_count][4];
                _core_vm = _reqs_list[_req_count][1];
                _mem_vm =  _reqs_list[_req_count][2];
                add_vm(_server_id,_vm_id,0,_core_vm,_mem_vm);
                break;
            case 1://_server_idA节点添加
                _vm_id = _reqs_list[_req_count][4];
                _core_vm = _reqs_list[_req_count][1];
                _mem_vm =  _reqs_list[_req_count][2];
                add_vm(_server_id,_vm_id,1,_core_vm,_mem_vm);
                break;
            case 2://_server_idB节点添加
                _vm_id = _reqs_list[_req_count][4];
                _core_vm = _reqs_list[_req_count][1];
                _mem_vm =  _reqs_list[_req_count][2];
                add_vm(_server_id,_vm_id,2,_core_vm,_mem_vm);
                break;
            case 3://购买
                _purchase_num++;
                int half_core = server_type_map[best_server_type].cpu/2;
                int half_mem = server_type_map[best_server_type].memory/2;
                servers_info_map[server_id_count].A_cpu = half_core;
                servers_info_map[server_id_count].B_cpu = half_core;
                servers_info_map[server_id_count].A_memory = half_mem;
                servers_info_map[server_id_count].B_memory = half_mem;
                servers_info_map[server_id_count].cpu = 0;
                servers_info_map[server_id_count].memory = 0;
		//添加服务器编号类型信息
		serverIdType_map[server_id_count] = best_server_type;

                server_id_count++;
                break;
        }
    }
    return _purchase_num; //返回oper_list中购买操作的数量，用于输出购买请求
}

void cancel_opers(vector<IntIntInt> _oper_list,vector<vector< int>>_reqs_list,int _req_head)   //时间回溯，逆执行opers
{
    int _count = _oper_list.size()-1;
    if(_count>=0)
    {
    int _req_count = _req_head-1;
    int _purchase_num =0;
    for(int i=_count;i>0;i--)
    {
        int _server_id = _oper_list[i].m_serverid;
        int _vm_id ;
        int _core_vm ;
        int _mem_vm ;

        switch(_oper_list[i].m_node)
        {
            case -1:
                _vm_id = _reqs_list[_req_count][4];
                _core_vm = _reqs_list[_req_count][1];
                _mem_vm =  _reqs_list[_req_count][2];
                add_vm(_server_id,_vm_id,0,_core_vm,_mem_vm);
                _req_count-=1;
                break;
            case -2:
                _vm_id = _reqs_list[_req_count][4];
                _core_vm = _reqs_list[_req_count][1];
                _mem_vm =  _reqs_list[_req_count][2];
                add_vm(_server_id,_vm_id,1,_core_vm,_mem_vm);
                _req_count-=1;
                break;
            case -3:
                _vm_id = _reqs_list[_req_count][4];
                _core_vm = _reqs_list[_req_count][1];
                _mem_vm =  _reqs_list[_req_count][2];
                add_vm(_server_id,_vm_id,2,_core_vm,_mem_vm);
                _req_count-=1;
                break;
            case 0:
                _vm_id = _reqs_list[_req_count][4];
                _core_vm = _reqs_list[_req_count][1];
                _mem_vm =  _reqs_list[_req_count][2];
                del_vm(_server_id,_vm_id,0,_core_vm,_mem_vm);
                _req_count-=1;
                break;
            case 1:
                _vm_id = _reqs_list[_req_count][4];
                _core_vm = _reqs_list[_req_count][1];
                _mem_vm =  _reqs_list[_req_count][2];
                del_vm(_server_id,_vm_id,1,_core_vm,_mem_vm);
                _req_count-=1;
                break;
            case 2:
                _vm_id = _reqs_list[_req_count][4];
                _core_vm = _reqs_list[_req_count][1];
                _mem_vm =  _reqs_list[_req_count][2];
                del_vm(_server_id,_vm_id,2,_core_vm,_mem_vm);
                _req_count-=1;
                break;
            case 3:
                server_id_count-=1;
                auto it = servers_info_map.find(server_id_count);
                servers_info_map.erase(it);
                auto it1 = serverIdType_map.find(server_id_count);
                serverIdType_map.erase(it1);
                break;
        }
    }}
}
string find_perfectserver(int need_core,int need_mem,bool allused_flag)
{
    string _type;
    string perfect_server;
    if(!allused_flag)
    {if(need_core<max_need_core)
    {need_core =max_need_core;}
    if(need_mem<max_need_mem)
    {need_mem=max_need_mem;}}
    int cost = 0;
    int cost_min = 10000000;
    for(auto it = serverTypeCostSet.begin();it!=serverTypeCostSet.end();it++)
    {
        _type =it->m_Type;
        if(server_type_map[_type].cpu>=need_core&&server_type_map[_type].memory>=need_mem)
        { 
        cost = server_type_map[_type].cost+piancha_run_purchse*server_type_map[_type].cost_day*(total_day - today);
        if(cost<cost_min)
        {cost_min = cost;
        perfect_server = _type;}
        }
    }
    return perfect_server;
}


void perfect_purset()
{
    
    // bool mem_full = 0;
    // bool core_full = 0;
    // int A_core = 0;
    // int B_core = 0;
    // int A_mem = 0;
    // int B_mem = 0;
    

    int vm_num  = 0;
    int corevm_num = 0;
    int memvm_num = 0;
    int best_cvm_num =0;
    int best_mvm_num =0;
    int all_used_corevm_num=0,all_used_memvm_num=0,final_used_corevm_num=0,final_used_memvm_num=0;
    int F_ps = 0;
    int F_psmin = 1000000;
    int max_cancelday = 0; 
    int all_corevms_num = unset_vms_core.size();
    int all_memvms_num = unset_vms_mem.size();

        while(1) //一个循环购买并布置好一个服务器
        {
            if(all_used_memvm_num==all_memvms_num&&all_used_corevm_num==all_corevms_num)
            {break;}
            all_used_memvm_num = final_used_memvm_num;
            all_used_corevm_num = final_used_corevm_num;
            int A_core=0,A_mem =0,B_core=0,B_mem=0;
            int _server_core,_server_mem;
            int singvm_num =0;
            max_cancelday =0;
            corevm_num = 0;
            memvm_num = 0;
            F_psmin = 1000000;
            string _server_type; //保存最佳服务器
            vector<IntInt> vm_set ={} ; // {vm_num in reqs_aday,0orAorB}
            auto it_corevm = unset_vms_core.begin();
            auto it_memvm = unset_vms_mem.begin();
            int vm_mem = 0;
            int vm_core = 0;
            int vm_cancelday = 0;
            int vm_isdouble = 0;
            int catch_count = 0;
            int _count0 = 0;
            int free_Acore,free_Bcore,free_Amem,free_Bmem;
            bool A_flag=0,max_flag=0,allused_flag=0;
            while(_count0<max_ser_search_size) //建立多种购买部署方案，保留最优值
            {    
                if(all_used_memvm_num==all_memvms_num&&all_used_corevm_num==all_corevms_num)
                {break;}
                
                if(catch_count%2) //单数取mem型vm
                {
                    if(it_memvm!=unset_vms_mem.end()) 
                    {    
                        vm_core = it_memvm->m_vec[2];
                        vm_mem = it_memvm->m_vec[3];
                        vm_isdouble = it_memvm->m_vec[4];
                        vm_cancelday = it_memvm->m_vec[5];
                        if(vm_isdouble)
                        {
                            A_core +=vm_core/2;
                            B_core +=vm_core/2;
                            A_mem +=vm_mem/2;
                            B_mem +=vm_mem/2;
                            vm_set.push_back(IntInt(it_memvm->m_vec[1],0));
                            memvm_num++;
                            all_used_memvm_num++;
                            if(max_cancelday<vm_cancelday)
                            {max_cancelday = vm_cancelday;}

                            
                        }
                        else
                        {
                            int A_err = abs(A_core +vm_core-B_core)*cm_wt + abs(A_mem+vm_mem-B_mem);
                            int B_err = abs(A_core -vm_core-B_core)*cm_wt + abs(A_mem-vm_mem-B_mem); 
                            if(A_err>B_err)
                            {
                                A_flag = 0;
                                B_core+=vm_core;
                                B_mem+=vm_mem;
                                if(max_cancelday<vm_cancelday)
                                {max_cancelday = vm_cancelday;} 
                                vm_set.push_back(IntInt(it_memvm->m_vec[1],2));
                                memvm_num++;
                                singvm_num++;
                                all_used_memvm_num++;
                            }
                            else
                            {
                                A_flag=1;
                                A_core+=vm_core;
                                A_mem+=vm_mem;
                                if(max_cancelday<vm_cancelday)
                                {max_cancelday = vm_cancelday;} 
                                vm_set.push_back(IntInt(it_memvm->m_vec[1],1));
                                memvm_num++;
                                singvm_num++;
                                all_used_memvm_num++;
                            }
                        }
                        it_memvm++;
                    }
                    else
                    {catch_count++;
                        continue;}
                    catch_count++;

                }
                else //双数取core型vm
                {
                    if(it_corevm!=unset_vms_core.end()) 
                    {    
                        vm_core = it_corevm->m_vec[2];
                        vm_mem = it_corevm->m_vec[3];
                        vm_isdouble = it_corevm->m_vec[4];
                        vm_cancelday = it_corevm->m_vec[5];
                        if(vm_isdouble)
                        {
                            A_core +=vm_core/2;
                            B_core +=vm_core/2;
                            A_mem +=vm_mem/2;
                            B_mem +=vm_mem/2;
                            vm_set.push_back(IntInt(it_corevm->m_vec[1],0));
                            corevm_num++;
                            all_used_corevm_num++;
                            if(max_cancelday<vm_cancelday)
                            {max_cancelday = vm_cancelday;}
                        }
                        else
                        {
                            int A_err = abs(A_core +vm_core-B_core)*cm_wt + abs(A_mem+vm_mem-B_mem);
                            int B_err = abs(A_core -vm_core-B_core)*cm_wt + abs(A_mem-vm_mem-B_mem); 
                            if(A_err>B_err)
                            {
                                A_flag=0;
                                B_core+=vm_core;
                                B_mem+=vm_mem;
                                if(max_cancelday<vm_cancelday)
                                {max_cancelday = vm_cancelday;} 
                                vm_set.push_back(IntInt(it_corevm->m_vec[1],2));
                                corevm_num++;
                                singvm_num++;
                                all_used_corevm_num++;
                            }
                            else
                            {
                                A_flag =1;
                                A_core+=vm_core;
                                A_mem+=vm_mem;
                                if(max_cancelday<vm_cancelday)
                                {max_cancelday = vm_cancelday;} 
                                vm_set.push_back(IntInt(it_corevm->m_vec[1],1));
                                corevm_num++;
                                singvm_num++;
                                all_used_corevm_num++;
                            }
                        }
                        it_corevm++;
                    }
                    else
                    {catch_count++;
                    continue;}

                    catch_count++;
                }
                int _max_core = (A_core>=B_core)?A_core:B_core;
                int _max_mem = (A_mem>=B_mem)?A_mem:B_mem;
                
                //有bug：可能会直接从很小的值增加到很大的值然后就跳出去了，没有进行完美服务器匹配(解决方法：回溯到前一个虚拟机，并maxflag置1)
                //还是有bug:max_cancelday没有复原。 
                if(_max_core>=max_need_core||_max_mem>=max_need_mem)  //应该换成大于最大的服务器核数和内存数
                {
                    max_flag =1;
                    vm_set.pop_back();
                    if(vm_isdouble)
                    {
                        A_core-=vm_core/2;
                        B_core-=vm_core/2;
                        A_mem-=vm_mem/2;
                        B_mem-=vm_mem/2;
                    }
                    else
                    {
                        singvm_num--;
                        if(A_flag)
                        {A_core-=vm_core;
                        A_mem-=vm_mem;}
                        else
                        {B_core-=vm_core;
                        B_mem-=vm_mem;}
                    }
                    _max_core = (A_core>=B_core)?A_core:B_core;
                    _max_mem = (A_mem>=B_mem)?A_mem:B_mem;
                    if(catch_count%2)
                    {all_used_corevm_num--;
                    corevm_num--;}
                    else
                    {all_used_memvm_num--;
                    memvm_num--;}
                }

                if(all_used_memvm_num==all_memvms_num&&all_used_corevm_num==all_corevms_num)
                {allused_flag =1;}
                //下面这行有bug：如果虚拟机全用上了应该强制选一个服务器来装（已尝试解决）--------------------------------------------------------bug
                if((2*_max_core>=max_need_core&&2*_max_mem>=max_need_mem)||allused_flag||max_flag)
                {
                    _server_type = find_perfectserver(2*_max_core,2*_max_mem,allused_flag);
                    
                    _server_core = server_type_map[_server_type].cpu;
                    _server_mem = server_type_map[_server_type].memory;
                    int _purchase_cost = server_type_map[_server_type].cost;
                    int _run_cost = server_type_map[_server_type].cost_day;
                    F_ps = (_purchase_cost+piancha_run_purchse*_run_cost*(total_day-today))/((A_core+B_core)*cm_wt+A_mem+B_mem);    //可以加个量纲权重
                    
                    if((F_ps<F_psmin)||allused_flag)
                    {
                        free_Acore = _server_core/2 - A_core;
                        free_Bcore = _server_core/2 - B_core;
                        free_Amem = _server_mem/2 - A_mem;
                        free_Bmem = _server_mem/2 - B_mem;
                        F_psmin = F_ps;
                        perfect_set = SVIVII(_server_type,vector<int>{singvm_num,free_Acore,free_Bcore,free_Amem,free_Bmem,A_core+B_core,A_mem+B_mem,max_cancelday},vm_set);
                        best_mvm_num = memvm_num;
                        best_cvm_num = corevm_num;
                        final_used_corevm_num = all_used_corevm_num;
                        final_used_memvm_num = all_used_memvm_num;
                        
                    }
                    if(max_flag==1)
                    {break;}
                    _count0++;
                }
            }
            //试试剩下的所有虚拟机能不能加到这个最佳服务器里，能的话加进去 --------------很重要
                // int best_vm_no;
                // int best_vm_oper;
                int used_core = _server_core-free_Acore-free_Bcore;
                int used_mem = _server_mem-free_Amem-free_Bmem;

//此处有bug：all_used_vm_num要合理更新，建议新建一个final_used_vm_num（已尝试解决）
                // all_used_corevm_num = best_cvm_num;
                // all_used_memvm_num = best_mvm_num;
//下一行疑似bug：无法顺利跳出while------------------------------------------------------------------------------------------------------------bug
                
                while(it_memvm!=unset_vms_mem.end())
                {
                    
                    vm_core = it_memvm->m_vec[2];
                    vm_mem = it_memvm->m_vec[3];
                    vm_isdouble =  it_memvm->m_vec[4];
                    vm_cancelday = it_memvm->m_vec[5];
                    // int vm_cmerr = it_corevm->m_vec[6];
                    int vm_no = it_memvm->m_vec[1];
                    if(vm_isdouble)
                    {
                        
                        if(vm_core<=free_Acore*2&&vm_core<=free_Bcore*2&&vm_mem<=free_Bmem*2&&vm_mem<=free_Amem*2)
                        {
                            perfect_set.m_set.push_back(IntInt(vm_no,0));
                            final_used_memvm_num++;
                            // best_mvm_num++;
                            it_memvm = unset_vms_mem.erase(it_memvm);
                            used_core+=vm_core;
                            used_mem+=vm_mem;
                            free_Acore-=vm_core/2;
                            free_Amem-=vm_mem/2;
                            free_Bcore-=vm_core/2;
                            free_Bmem-=vm_mem/2;
                            if(max_cancelday<vm_cancelday)
                            {max_cancelday = vm_cancelday;}
                            continue;
                        }
                    }
                    else
                    {
                        if(vm_core<=free_Acore&&vm_mem<=free_Amem) //如果A节点放得下
                        {
                            perfect_set.m_set.push_back(IntInt(vm_no,1));
                            singvm_num++;
                            final_used_memvm_num++;
                            // best_mvm_num++;
                            it_memvm = unset_vms_mem.erase(it_memvm);
                            used_core+=vm_core;
                            used_mem+=vm_mem;
                            free_Acore-=vm_core;
                            free_Amem-=vm_mem;
                            if(max_cancelday<vm_cancelday)
                            {max_cancelday = vm_cancelday;}
                            continue;
                        }
                        else if(vm_core<=free_Bcore&&vm_mem<=free_Bmem) //如果B节点放得下
                        {
                            perfect_set.m_set.push_back(IntInt(vm_no,2));
                            singvm_num++;
                            final_used_memvm_num++;
                            // best_mvm_num++;
                            it_memvm = unset_vms_mem.erase(it_memvm);
                            used_core+=vm_core;
                            used_mem+=vm_mem;
                            free_Bcore-=vm_core;
                            free_Bmem-=vm_mem;
                            if(max_cancelday<vm_cancelday)
                            {max_cancelday = vm_cancelday;}
                            continue;
                        }

                    }
                    it_memvm++;
                }
                while(it_corevm!=unset_vms_core.end())
                {
                    float F_c = 0;
                    float F_cmin = 9000000;  //函数值越小越好
                    
                    vm_core = it_corevm->m_vec[2];
                    vm_mem = it_corevm->m_vec[3];
                    vm_isdouble =  it_corevm->m_vec[4];
                    vm_cancelday = it_corevm->m_vec[5];
                    // int vm_cmerr = it_corevm->m_vec[6];
                    int vm_no = it_corevm->m_vec[1];
                    if(vm_isdouble)
                    {
                        
                        if(vm_core<=free_Acore*2&&vm_core<=free_Bcore*2&&vm_mem<=free_Bmem*2&&vm_mem<=free_Amem*2)
                        {
                            // F_c =-vm_core-vm_mem;
                            // if(F_c<F_cmin)
                            // {F_cmin =F_c;
                            // best_vm_no = vm_no;
                            // best_vm_oper = 0;
                            // } 
                            
                            perfect_set.m_set.push_back(IntInt(vm_no,0));
                            
                            final_used_corevm_num++;
                            // best_cvm_num++;
                            it_corevm = unset_vms_core.erase(it_corevm);
                            
                            used_core+=vm_core;
                            used_mem+=vm_mem;
                            free_Acore-=vm_core/2;
                            free_Amem-=vm_mem/2;
                            free_Bcore-=vm_core/2;
                            free_Bmem-=vm_mem/2;
                            if(max_cancelday<vm_cancelday)
                            {max_cancelday = vm_cancelday;} 
                            continue;
                            

                        }
                    }
                    else
                    {
                        if(vm_core<=free_Acore&&vm_mem<=free_Amem) //如果A节点放得下
                        {
                            perfect_set.m_set.push_back(IntInt(vm_no,1));
                            singvm_num++;
                            final_used_corevm_num++;
                            // best_cvm_num++;
                            it_corevm = unset_vms_core.erase(it_corevm);
                            used_core+=vm_core;
                            used_mem+=vm_mem;
                            free_Acore-=vm_core;
                            free_Amem-=vm_mem;
                            if(max_cancelday<vm_cancelday)
                            {max_cancelday = vm_cancelday;}
                            continue;
                        }
                        else if(vm_core<=free_Bcore&&vm_mem<=free_Bmem) //如果B节点放得下
                        {
                            perfect_set.m_set.push_back(IntInt(vm_no,2));
                            singvm_num++;
                            final_used_corevm_num++;
                            // best_cvm_num++;
                            it_corevm = unset_vms_core.erase(it_corevm);
                            used_core+=vm_core;
                            used_mem+=vm_mem;
                            free_Bcore-=vm_core;
                            free_Bmem-=vm_mem;
                            if(max_cancelday<vm_cancelday)
                            {max_cancelday = vm_cancelday;}
                            continue;
                        }

                    }
                    it_corevm++;
                }
                
                perfect_set.m_info[0] = singvm_num;
                perfect_set.m_info[1] = free_Acore;
                perfect_set.m_info[2] = free_Bcore;
                perfect_set.m_info[3] = free_Amem;
                perfect_set.m_info[4] = free_Bmem;
                perfect_set.m_info[5] = used_core;
                perfect_set.m_info[6] = used_mem;
                perfect_set.m_info[7] = max_cancelday;

//看看完美匹配法的匹配效果：
                // float _core_rate = 1 - float(free_Acore+free_Bcore)/_server_core;
                // float _mem_rate = 1 - float(free_Amem+free_Bmem)/_server_mem;

                // cout<<"完美法配置好了一个服务器,信息如下："<<endl;
                // cout<<"服务器大小（核数,内存数）："<<_server_core<<" , "<<_server_mem<<endl;

                // cout<<"各节点空余资源(AC,BC AM,BM)："<<free_Acore<<","<<free_Bcore<<"  "<<free_Amem<<","<<free_Bmem<<endl;
                // cout<<"核数利用率："<<_core_rate<<endl;
                // cout<<"内存利用率："<<_mem_rate<<endl;

// ..检查是否清零了该清零的map，以及是否能够准时结束跳出循环
            //读取保留下来的最佳购买部署方案，存到pur_set_list中
            bool add_type = 1;
            for(auto it = pur_set_list.begin();it!=pur_set_list.end();it++)
            {
                if(it->m_type==perfect_set.m_type)
                {
                    it->m_set_list.push_back(VIVII(perfect_set.m_info,perfect_set.m_set));
                    add_type = 0;
                    break;
                }
            }
            if(add_type)
            {pur_set_list.push_back(SLVIVII(perfect_set.m_type,list<VIVII>{VIVII(perfect_set.m_info,perfect_set.m_set)}));}

            //清除用掉的vms
            for(int pop_count =0;pop_count<best_mvm_num;pop_count++)
            {   
                unset_vms_mem.erase(unset_vms_mem.begin());
            }
            for(int pop_count =0;pop_count<best_cvm_num;pop_count++)
            {unset_vms_core.erase(unset_vms_core.begin());}
            

        }//一个循环购买并部署好一个服务器

    //整理购买方案，更新三个世界map:servers_info;vms_info;server_vminfo
    for(auto it_ps=pur_set_list.begin();it_ps!=pur_set_list.end();it_ps++)
    {
        string _type = it_ps->m_type;
        list<VIVII> set_list =it_ps->m_set_list;
        int _server_num = set_list.size();
        
        for(auto it_sl=set_list.begin();it_sl!=set_list.end();it_sl++)
        {
            vector<IntInt> vms_set =it_sl->m_set;
            int _size = vms_set.size();
            int _vm_no = vms_set[0].m_id;
            int _vm_node = vms_set[0].m_node;
            int _vm_id =  reqs_map[today][_vm_no][4];
            int _vm_core = reqs_map[today][_vm_no][1];
            int _vm_mem = reqs_map[today][_vm_no][2];
            
            servers_info_map[server_id_count].A_cpu = it_sl->m_info[1];
            servers_info_map[server_id_count].B_cpu = it_sl->m_info[2];
            servers_info_map[server_id_count].A_memory = it_sl->m_info[3];
            servers_info_map[server_id_count].B_memory = it_sl->m_info[4];
            servers_info_map[server_id_count].cpu = it_sl->m_info[5];
            servers_info_map[server_id_count].memory = it_sl->m_info[6];




            insert_set.insert(IntIntInt(_vm_no,server_id_count,_vm_node));
            server_vminfo_map[server_id_count] = vector<vector<int>>{{_vm_id,_vm_core,_vm_mem,_vm_node}};
            vms_info_map[_vm_id] = vector<int>{_vm_core,_vm_mem,server_id_count,_vm_node};
            for(int i0 =1;i0<_size;i0++)
            {   
                _vm_no = vms_set[i0].m_id;
                _vm_node = vms_set[i0].m_node;
                _vm_id =  reqs_map[today][_vm_no][4];
                _vm_core = reqs_map[today][_vm_no][1];
                _vm_mem = reqs_map[today][_vm_no][2];
                
                insert_set.insert(IntIntInt(_vm_no,server_id_count,_vm_node));
                server_vminfo_map[server_id_count].push_back(vector<int>{_vm_id,_vm_core,_vm_mem,_vm_node});
                vms_info_map[_vm_id] = vector<int>{_vm_core,_vm_mem,server_id_count,_vm_node};
            
            
            }
            server_id_count++;
        }
        
    }

}


void climbmount_setvm_pro(vector<vector<int>> reqs_aday,int sct)
{

    
    // purchase_aday_map.clear();
    bool add_type_flag = 1;
    int purchase_type_num = 0;
    int server_id_temp = server_id_count;


    open_list_sct.clear();
    for(int i_sct =0;i_sct<sct;i_sct++)
    {
        //清空完美匹配表
        // perfect_sets.clear();
        unset_vms_core.clear();
        unset_vms_mem.clear();
        pur_set_list.clear();
        insert_set.clear();



        oper_list_sct=FVIII (0.0,{{i_sct,i_sct,i_sct}});
        float F_sct = 0.0;
        oper_list_cb.clear();
        float F = 0.0 ;                 //定义估价函数值 
        int _reqs_size =reqs_aday.size(); 
        purchase_num_climb =0;      //清零今日购买数量
        bool type_set =0;           //0 表示还没有确定今天的购置类型
        for(int i =0;i<_reqs_size;i++) //遍历今天的每一步
        {    //读取请求信息
            float F_min = 90000000;
            add_type_flag=1;
            open_list_cb.clear();   //清零用于保存这一步所有可能操作的set
            // 读取这一步的所有信息
            int core_vm = reqs_aday[i][1];
            int mem_vm = reqs_aday[i][2];
            int vm_isdouble = reqs_aday[i][3];
            int vm_id = reqs_aday[i][4];
            int oper;
            int open_list_size = 0;
            int cm_err_s;
            int cm_err_v = core_vm - mem_vm;
            // float rate_v=(float)core_vm/mem_vm;
            float rate_s;
            while(open_list_size<max_open_list_size) //限制探索的可能数量  
            {    //首先进行open_list赋初值

                if(reqs_aday[i][0]==0) //如果是删除请求
                {
                    F = 0.0;
                    int _loc_id = vms_info_map[vm_id][2];
                    int _loc_node = vms_info_map[vm_id][3]; 
                    _loc_node = -_loc_node-1;
                    open_list_cb.emplace(F,vm_id,_loc_id,_loc_node);   //把这个操作可能插入到open_list_cb中
                }

                if(reqs_aday[i][0]==1)  //如果是添加请求
                {
                    //计算这个请求的所有可选操作：
                    if(vm_isdouble==0)    //如果该虚拟机为单节点型
                    {
                        // if(open_list_size>max_open_list_size)
                        // {break;}
                        for(int j=0;j<server_id_temp;j++)  //遍历所有服务器id
                        {
                            
                            if(open_list_size>max_open_list_size)
                            {break;}
                            int _server_core_A = servers_info_map[j].A_cpu;
                            int _server_core_B = servers_info_map[j].B_cpu;
                            int _server_mem_A = servers_info_map[j].A_memory;
                            int _server_mem_B = servers_info_map[j].B_memory;
                            // int _server_core_A = servers_info_map[j].A_cpu;
                            

                            if(_server_core_A>=core_vm&&_server_mem_A>=mem_vm) //如果j服务器A节点放得下
                            {
                                if(servers_info_map[j].cpu!=0) //如果j服务器不为空服务器
                                {
                                    
                                    F= vl_wt2*value_piancha*(servers_info_map[j].A_cpu+servers_info_map[j].B_cpu)+servers_info_map[j].A_memory+servers_info_map[j].B_memory;
                                    cm_err_s = _server_core_A-_server_mem_A;
                                    if(cm_err_s*cm_err_v<0)
                                    {F+=1000;}
                                    if(F<F_min)
                                    {
                                        F_min = F;
                                        open_list_cb.emplace(F,vm_id,j,1);}  //把这个操作可能插入到open_list_cb中
                                    open_list_size++;
                                    // break;
                                }
                                else
                                {
                                    F= vl_wt2*value_piancha*(servers_info_map[j].A_cpu+servers_info_map[j].B_cpu)+servers_info_map[j].A_memory+servers_info_map[j].B_memory  ;
                                    cm_err_s = _server_core_A-_server_mem_A;
                                    if(cm_err_s*cm_err_v<0)
                                    {F+=1000;}
                                    F +=10000;//占用了空服务器的函数值
                                    if(F<F_min)
                                    {
                                        F_min = F;
                                        open_list_cb.emplace(F,vm_id,j,1);}//把这个操作可能插入到open_list_cb中
                                    open_list_size++;
                                }

                            }
                            if(servers_info_map[j].B_cpu>=core_vm&&servers_info_map[j].B_memory>=mem_vm) //如果j服务器A放不下的话，看B节点放得下不，后续如果启发函数涉及了节点层面，这里可以把else去掉增加可能性
                            {
                                if(servers_info_map[j].cpu!=0) //如果j服务器不为空服务器
                                {
                                    F= vl_wt2*value_piancha*(servers_info_map[j].A_cpu+servers_info_map[j].B_cpu)+servers_info_map[j].A_memory+servers_info_map[j].B_memory  ;
                                    // F =j;  
                                    cm_err_s = _server_core_B-_server_mem_B;
                                    if(cm_err_s*cm_err_v<0)
                                    {F+=1000;}
                                    if(F<F_min)
                                    {
                                        F_min = F;
                                        open_list_cb.emplace(F,vm_id,j,2);}  //把这个操作插入到open_list_cb中
                                    open_list_size++;
                                }
                                else
                                {
                                    F= vl_wt2*value_piancha*(servers_info_map[j].A_cpu+servers_info_map[j].B_cpu)+servers_info_map[j].B_memory+servers_info_map[j].A_memory  ;
                                    F +=10000;
                                    cm_err_s = _server_core_B-_server_mem_B;
                                    if(cm_err_s*cm_err_v<0)
                                    {F+=1000;}
                                    if(F<F_min)
                                    {
                                        F_min = F;
                                        open_list_cb.emplace(F,vm_id,j,2);} //把这个操作插入到open_list_cb中
                                    open_list_size++;
                                }
                            }

                        }
                    }
                    if(vm_isdouble) //如果该虚拟机是双节点型
                    {
                        // if(open_list_size>max_open_list_size)
                        // {break;}
                        for(int j=0;j<server_id_temp;j++)
                        {
                            int _server_core_A = servers_info_map[j].A_cpu;
                            int _server_core_B = servers_info_map[j].B_cpu;
                            int _server_mem_A = servers_info_map[j].A_memory;
                            int _server_mem_B = servers_info_map[j].B_memory;
                            if(open_list_size>max_open_list_size)
                            {break;}
                            if(servers_info_map[j].A_cpu>=core_vm/2&&servers_info_map[j].A_memory>=mem_vm/2)
                            {
                                if(servers_info_map[j].B_cpu>=core_vm/2&&servers_info_map[j].B_memory>=mem_vm/2)
                                {
                                    if(servers_info_map[j].cpu!=0) //如果j服务器不为空服务器
                                    {
                                        
                                        // F =j;  
                                        F= vl_wt2*value_piancha*(servers_info_map[j].A_cpu+servers_info_map[j].B_cpu)+servers_info_map[j].A_memory+servers_info_map[j].B_memory;
                                        cm_err_s = _server_core_A+_server_core_B-_server_mem_B-_server_mem_A;
                                        if(cm_err_s*cm_err_v<0)
                                        {F+=1000;}
                                        if(F<F_min)
                                        {
                                            F_min = F;
                                            open_list_cb.emplace(F,vm_id,j,0);} //把这个操作插入到open_list_cb中
                                        open_list_size++;
                                    }
                                    else
                                    {
                                        F= vl_wt2*value_piancha*(servers_info_map[j].A_cpu+servers_info_map[j].B_cpu)+servers_info_map[j].A_memory+servers_info_map[j].B_memory;
                                        F +=10000;
                                        cm_err_s = _server_core_A+_server_core_B-_server_mem_B-_server_mem_A;
                                        if(cm_err_s*cm_err_v<0)
                                        {F+=1000;}
                                        if(F<F_min)
                                        {
                                            F_min = F;
                                            open_list_cb.emplace(F,vm_id,j,0);} //把这个操作插入到open_list_cb中
                                        open_list_size++;
                                    }
                                }
                            }
                        }
                    }
                    F = (float)1000000.0;
                    open_list_cb.emplace(F,vm_id,0,3); //购买请求,购买应该比部署进新服务器代价更高
                    open_list_size ++;
                }
                break;
            }
            // oper_list = open_list_cb.begin();
            F = open_list_cb.begin()->m_F;
            int _server_id = open_list_cb.begin()->m_serverid;
            // vm_id = open_list_cb.begin()->m_vmid;
            // int _serverid = open_list_cb.begin()->m_serverid;
            int _oper = open_list_cb.begin()->m_oper;
            // cout<<"选取的 F = "<<F<<endl;
            // cout<<"此时的服务器数量："<<server_id_count<<endl;
            oper_list_cb.emplace_back(F,vm_id,_server_id,_oper) ;

            if(_oper!=3)
            {oper_list_sct.m_OP.emplace_back(vm_id,_server_id,_oper);}

            // if(vm_id == 886962355)
            // {
            //     cout<<"找到问题虚拟机"<<endl;
            //     cout<<"问题虚拟机所在位置："<<_server_id<<endl;
            // }
            switch(_oper)
            {
                case -1: //删除
                    del_vm(_server_id,vm_id,0,core_vm,mem_vm);
                    break;
                case -2: //删除
                    del_vm(_server_id,vm_id,1,core_vm,mem_vm);
                    break;
                case -3: //删除
                    del_vm(_server_id,vm_id,2,core_vm,mem_vm);
                    break;
                case 0: //_server_id双节点添加
                    add_vm(_server_id,vm_id,0,core_vm,mem_vm);
                    break;
                case 1://_server_idA节点添加
                    add_vm(_server_id,vm_id,1,core_vm,mem_vm);
                    break;
                case 2://_server_idB节点添加
                    add_vm(_server_id,vm_id,2,core_vm,mem_vm);
                    break;
                case 3://把放不下的虚拟机存起来
                    // purchase_num_climb++;
                    if(core_vm>=mem_vm)
                    {
                        int cancel_day = vms_time[vm_id].m_node;
                        unset_vms_core.insert(vector<int>{vm_id,i,core_vm,mem_vm,vm_isdouble,cancel_day,cm_wt*core_vm+mem_vm});
                    }
                    if(core_vm<mem_vm)
                    {
                        int cancel_day = vms_time[vm_id].m_node;
                        unset_vms_mem.insert(vector<int>{vm_id,i,core_vm,mem_vm,vm_isdouble,cancel_day,cm_wt*core_vm+mem_vm});
                    }

                    //添加服务器编号类型信息
                    // serverIdType_map[server_id_temp] = best_server_type;
                    // server_id_temp++;
                    break;
            }

        } //读取完了一轮今天的请求

        perfect_purset();

        //根据insert_set,插入部署指令。
        for(auto it0 = insert_set.begin();it0!=insert_set.end();it0++)
        {
            oper_list_sct.m_OP.insert(oper_list_sct.m_OP.begin()+it0->m_vmid+1,*it0);
        }

        
    }//循环完了sct次部署购买方案

    //输出购买指令
    purchase_type_num = pur_set_list.size();
    if(purchase_type_num ==0)
    {
        cout<<"(purchase, 0)"<<endl;
    // outfile<<"(purchase, 0)"<<endl;
    }
    else
    {
        
        cout<<"(purchase, "<<purchase_type_num<<")"<<endl;

        for(auto it_ps = pur_set_list.begin();it_ps!=pur_set_list.end();it_ps++)
        {
            string _purchase_body = "(, )";
            string _purchase_type = it_ps->m_type;
            int _server_num = it_ps->m_set_list.size();
            _purchase_body.insert(3,int2str(_server_num));
            _purchase_body.insert(1,_purchase_type);
            cout<<_purchase_body<<endl;
        }

    }
    // 输出迁移指令
    string mig_head = "(migration, )";
    mig_head.insert(12,int2str(mig_list.size()));
    cout<<mig_head<<endl;
    // outfile<<mig_head<<endl;
    if(mig_list.size()!=0)
    {
        for(int ij =0; ij<mig_list.size();ij++)
        {
            if(mig_list[ij].m_node==0)
            {
                cout<<"("<<mig_list[ij].m_vmid<<", "<<mig_list[ij].m_serverid<<")"<<endl;
            // outfile<<"("<<mig_list[ij].m_vmid<<", "<<mig_list[ij].m_serverid<<")"<<endl;
            }
            else if(mig_list[ij].m_node==1)
            {
                cout<<"("<<mig_list[ij].m_vmid<<", "<<mig_list[ij].m_serverid<<", A)"<<endl;
            // outfile<<"("<<mig_list[ij].m_vmid<<", "<<mig_list[ij].m_serverid<<", A)"<<endl;
            }
            else if(mig_list[ij].m_node==2)
            {
                cout<<"("<<mig_list[ij].m_vmid<<", "<<mig_list[ij].m_serverid<<", B)"<<endl;
            // outfile<<"("<<mig_list[ij].m_vmid<<", "<<mig_list[ij].m_serverid<<", B)"<<endl;
            }
        }
    }
    // 输出部署指令
    // cout<<"今日待处理请求数："<<set_vm_map[today].size()<<endl;
    // for(auto it = set_vm_map[today].begin();it!=set_vm_map[today].end();it++)
    // for(auto it_ps = pur_set_list.begin();it_ps!=pur_set_list.end();it_ps++)
    // {
        
    // }

    int _oper_size = oper_list_sct.m_OP.size();
    
    for(int i=1;i<_oper_size;i++)
    {
        
        int _id = oper_list_sct.m_OP[i].m_serverid ;//////
        // _id = 0;

        int oper = oper_list_sct.m_OP[i].m_node;
        // if(_id>(server_id_count-1))
        // {cout<<"出现服务器id溢出了！!!!!!!!!!!!!!!"<<endl;}
        // string out = "()";
        switch(oper)
        {
            case -1:break;
            case -2:break;
            case -3:break;
            case 0 :
            cout<< "(" <<_id<< ")"<<endl;
                    // outfile<< "(" <<_id<< ")"<<endl;
                    break;
            case 1 :
            cout<< "(" <<_id<< ", " <<"A"<< ")"<<endl;
                    // outfile<< "(" <<_id<< ", " <<"A"<< ")"<<endl;
                    break;
            case 2 :
            cout<< "(" <<_id<< ", " <<"B"<< ")"<<endl;
                    // outfile<< "(" <<_id<< ", " <<"B"<< ")"<<endl;
                    break;
            case 3 :break;
        }
    }
}







void climbmount_setvm(vector<vector<int>> reqs_aday,int sct)
{
    open_list_sct.clear();
    for(int i_sct =0;i_sct<sct;i_sct++)
    {
        
        oper_list_sct=FVIII (0.0,{{i_sct,i_sct,i_sct}});

        float F_sct = 0.0;
        

        oper_list_cb.clear();
        float F = 0.0 ;                 //定义估价函数值 
        int _reqs_size =reqs_aday.size(); 
        purchase_num_climb =0;      //清零今日购买数量
        bool type_set =0;           //0 表示还没有确定今天的购置类型
        for(int i =0;i<_reqs_size;i++) //遍历今天的每一步
        {    //读取请求信息
            open_list_cb.clear();   //清零用于保存这一步所有可能操作的set
            float F_min = 90000000;
            // 读取这一步的所有信息
            int core_vm = reqs_aday[i][1];
            int mem_vm = reqs_aday[i][2];
            int vm_isdouble = reqs_aday[i][3];
            int vm_id = reqs_aday[i][4];
            int oper;
            int open_list_size = 0;
            int cm_err_s;
            int cm_err_v = core_vm - mem_vm;
            // float rate_v=(float)core_vm/mem_vm;
            float rate_s;
            while(open_list_size<max_open_list_size) //限制探索的可能数量  
            {    //首先进行open_list赋初值

                if(reqs_aday[i][0]==0) //如果是删除请求
                {
                    F = 0.0;
                    int _loc_id = vms_info_map[vm_id][2];
                    int _loc_node = vms_info_map[vm_id][3]; 
                    _loc_node = -_loc_node-1;

                    open_list_cb.emplace(F,vm_id,_loc_id,_loc_node);   //把这个操作可能插入到open_list_cb中

                }


                if(reqs_aday[i][0]==1)  //如果是添加请求
                {
                    //计算这个请求的所有可选操作：
                    if(reqs_aday[i][3]==0)    //如果该虚拟机为单节点型
                    {
                        // if(open_list_size>max_open_list_size)
                        // {break;}
                        for(int j=0;j<server_id_count;j++)  //遍历所有服务器id
                        {
                            
                            if(open_list_size>max_open_list_size)
                            {break;}
                            int _server_core_A = servers_info_map[j].A_cpu;
                            int _server_core_B = servers_info_map[j].B_cpu;
                            int _server_mem_A = servers_info_map[j].A_memory;
                            int _server_mem_B = servers_info_map[j].B_memory;
                            // int _server_core_A = servers_info_map[j].A_cpu;
                            

                            if(_server_core_A>=core_vm&&_server_mem_A>=mem_vm) //如果j服务器A节点放得下
                            {
                                if(servers_info_map[j].cpu!=0) //如果j服务器不为空服务器
                                {
                                    
                                    F= vl_wt2*value_piancha*(servers_info_map[j].A_cpu+servers_info_map[j].B_cpu)+servers_info_map[j].A_memory+servers_info_map[j].B_memory;
                                    cm_err_s = _server_core_A-_server_mem_A;
                                    if(cm_err_s*cm_err_v<0)
                                    {F+=1000;}
                                    if(F<F_min)
                                    {
                                        F_min = F;
                                        open_list_cb.emplace(F,vm_id,j,1);}  //把这个操作可能插入到open_list_cb中
                                    open_list_size++;
                                    // break;
                                }
                                else
                                {
                                    F= vl_wt2*value_piancha*(servers_info_map[j].A_cpu+servers_info_map[j].B_cpu)+servers_info_map[j].A_memory+servers_info_map[j].B_memory;
                                    cm_err_s = _server_core_A-_server_mem_A;
                                    if(cm_err_s*cm_err_v<0)
                                    {F+=1000;}
                                    F +=10000;//占用了空服务器的函数值
                                    if(F<F_min)
                                    {
                                        F_min = F;
                                        open_list_cb.emplace(F,vm_id,j,1);}//把这个操作可能插入到open_list_cb中
                                    open_list_size++;
                                }

                            }
                            if(servers_info_map[j].B_cpu>=core_vm&&servers_info_map[j].B_memory>=mem_vm) //如果j服务器A放不下的话，看B节点放得下不，后续如果启发函数涉及了节点层面，这里可以把else去掉增加可能性
                            {
                                if(servers_info_map[j].cpu!=0) //如果j服务器不为空服务器
                                {
                                    F= vl_wt2*value_piancha*(servers_info_map[j].A_cpu+servers_info_map[j].B_cpu)+servers_info_map[j].A_memory+servers_info_map[j].B_memory;
                                    // F =j;  
                                    cm_err_s = _server_core_B-_server_mem_B;
                                    if(cm_err_s*cm_err_v<0)
                                    {F+=1000;}
                                    if(F<F_min)
                                    {
                                        F_min = F;
                                        open_list_cb.emplace(F,vm_id,j,2);}  //把这个操作插入到open_list_cb中
                                    open_list_size++;
                                }
                                else
                                {
                                    F= vl_wt2*value_piancha*(servers_info_map[j].A_cpu+servers_info_map[j].B_cpu)+servers_info_map[j].A_memory+servers_info_map[j].B_memory;
                                    F +=10000;
                                    cm_err_s = _server_core_B-_server_mem_B;
                                    if(cm_err_s*cm_err_v<0)
                                    {F+=1000;}
                                    if(F<F_min)
                                    {
                                        F_min = F;
                                        open_list_cb.emplace(F,vm_id,j,2);} //把这个操作插入到open_list_cb中
                                    open_list_size++;
                                }
                            }

                        }
                    }
                    if(reqs_aday[i][3]==1) //如果该虚拟机是双节点型
                    {
                        // if(open_list_size>max_open_list_size)
                        // {break;}
                        for(int j=0;j<server_id_count;j++)
                        {
                            int _server_core_A = servers_info_map[j].A_cpu;
                            int _server_core_B = servers_info_map[j].B_cpu;
                            int _server_mem_A = servers_info_map[j].A_memory;
                            int _server_mem_B = servers_info_map[j].B_memory;
                            if(open_list_size>max_open_list_size)
                            {break;}
                            if(servers_info_map[j].A_cpu>=core_vm/2&&servers_info_map[j].A_memory>=mem_vm/2)
                            {
                                if(servers_info_map[j].B_cpu>=core_vm/2&&servers_info_map[j].B_memory>=mem_vm/2)
                                {
                                    if(servers_info_map[j].cpu!=0) //如果j服务器不为空服务器
                                    {
                                        
                                        // F =j;  
                                        F= vl_wt2*value_piancha*(servers_info_map[j].A_cpu+servers_info_map[j].B_cpu)+servers_info_map[j].A_memory+servers_info_map[j].B_memory;
                                        cm_err_s = _server_core_A+_server_core_B-_server_mem_B-_server_mem_A;
                                        if(cm_err_s*cm_err_v<0)
                                        {F+=1000;}
                                        if(F<F_min)
                                        {
                                            F_min = F;
                                            open_list_cb.emplace(F,vm_id,j,0);} //把这个操作插入到open_list_cb中
                                        open_list_size++;
                                    }
                                    else
                                    {
                                        F= vl_wt2*value_piancha*(servers_info_map[j].A_cpu+servers_info_map[j].B_cpu)+servers_info_map[j].A_memory+servers_info_map[j].B_memory;
                                        F +=10000;
                                        cm_err_s = _server_core_A+_server_core_B-_server_mem_B-_server_mem_A;
                                        if(cm_err_s*cm_err_v<0)
                                        {F+=1000;}
                                        if(F<F_min)
                                        {
                                            F_min = F;
                                            open_list_cb.emplace(F,vm_id,j,0);} //把这个操作插入到open_list_cb中
                                        open_list_size++;
                                    }
                                }
                            }
                        }
                    }
                    
                    F = (float)1000000.0;
                    
                    open_list_cb.emplace(F,vm_id,0,3); //购买请求,购买应该比部署进新服务器代价更高}
                    open_list_size ++;
                }
                break;
            }
            // oper_list = open_list_cb.begin();
            F = open_list_cb.begin()->m_F;
            int _server_id = open_list_cb.begin()->m_serverid;
            // vm_id = open_list_cb.begin()->m_vmid;
            // int _serverid = open_list_cb.begin()->m_serverid;
            int _oper = open_list_cb.begin()->m_oper;
            // cout<<"选取的 F = "<<F<<endl;
            // cout<<"此时的服务器数量："<<server_id_count<<endl;
            oper_list_cb.emplace_back(F,vm_id,_server_id,_oper) ;

            oper_list_sct.m_OP.emplace_back(vm_id,_server_id,_oper);


            switch(_oper)
            {
                case -1: //删除
                    del_vm(_server_id,vm_id,0,core_vm,mem_vm);
                    break;
                case -2: //删除
                    del_vm(_server_id,vm_id,1,core_vm,mem_vm);
                    break;
                case -3: //删除
                    del_vm(_server_id,vm_id,2,core_vm,mem_vm);
                    break;
                case 0: //_server_id双节点添加
                    add_vm(_server_id,vm_id,0,core_vm,mem_vm);
                    break;
                case 1://_server_idA节点添加
                    add_vm(_server_id,vm_id,1,core_vm,mem_vm);
                    break;
                case 2://_server_idB节点添加
                    add_vm(_server_id,vm_id,2,core_vm,mem_vm);
                    break;
                case 3://购买
                    purchase_num_climb++;
                    if(type_set ==0)
                        {
                            int day_allvm_cores =0;
                            int day_allvm_mem =0;
                            int max_req_cores=0;
                            int max_req_mems=0;
                            for(int ii=i;ii<_reqs_size;ii++)
                            {
                                int core_vm = reqs_aday[ii][1];
                                int mem_vm = reqs_aday[ii][2];
                                int vm_isdouble = reqs_aday[ii][3];
                                int vm_id = reqs_aday[ii][4];
                                day_allvm_cores += core_vm;
                                day_allvm_mem += mem_vm;
                                if(vm_isdouble==1)
                                {
                                    if(max_req_cores<core_vm)
                                    {max_req_cores = core_vm;}
                                    if(max_req_mems<mem_vm)
                                    {max_req_mems = mem_vm;}
                                }
                                if(vm_isdouble==0)
                                {
                                    if(max_req_cores<core_vm*2)
                                    {max_req_cores = core_vm*2;}
                                    if(max_req_mems<mem_vm*2)
                                    {max_req_mems = mem_vm*2;}
                                }
                            }
                        //得到当前性价比最高的可以装下当前请求的服务器
                            best_server_type = Find_server(day_allvm_cores,day_allvm_mem,max_need_core,max_need_mem,i_sct);
                            type_set =1;
                            // outfile<<"天数：剩余未部署核数，内存数   ：   "<<today<<" : "<<day_allvm_cores<<","<<day_allvm_mem<<endl;
                        }
                    // cout<<"今天的最佳服务器为："<<best_server_type<<endl;
                    int half_core = server_type_map[best_server_type].cpu/2;
                    int half_mem = server_type_map[best_server_type].memory/2;

                    oper_list_sct.m_F+=(float)server_type_map[best_server_type].cost/(total_day-today)+piancha_run_purchse*server_type_map[best_server_type].cost_day;
                    servers_info_map[server_id_count].A_cpu = half_core;
                    servers_info_map[server_id_count].B_cpu = half_core;
                    servers_info_map[server_id_count].A_memory = half_mem;
                    servers_info_map[server_id_count].B_memory = half_mem;
                    servers_info_map[server_id_count].cpu = 0;
                    servers_info_map[server_id_count].memory = 0;
                    i-=1;
                    //添加服务器编号类型信息
                    serverIdType_map[server_id_count] = best_server_type;

                    server_id_count++;
                    break;
            }


        
        }
        open_list_sct.insert(oper_list_sct);
        if(server_choose_times !=1)
        {cancel_opers(oper_list_sct.m_OP,reqs_aday,reqs_aday.size());}
        
    }
    if(server_choose_times!=1)
    {oper_list_sct = *open_list_sct.begin();
    purchase_num_climb =do_opers(oper_list_sct.m_OP,reqs_aday);}
    int choose_type_num =0;
    for(auto it_t =serverTypeCostSet_today.begin();it_t!=serverTypeCostSet_today.end();it_t++)
    {
        if(choose_type_num == oper_list_sct.m_OP.front().m_serverid)
        {
            best_server_type = it_t->m_Type;
            break;
        }
        choose_type_num++;
    }

    // purchase_num = do_opers(oper_list_copy,reqs_aday);
    // cout<<"今天的天数："<<today<<endl;
    if(purchase_num_climb ==0)
    {
        cout<<"(purchase, 0)"<<endl;
        // outfile<<"(purchase, 0)"<<endl;
    }
    else
    {
        cout<<"(purchase, 1)"<<endl;
        // outfile<<"(purchase, 1)"<<endl;
        string _purchase_body ="(, )";
        _purchase_body.insert(3,int2str(purchase_num_climb));
        _purchase_body.insert(1,best_server_type);
        cout<<_purchase_body<<endl;
        // outfile<<_purchase_body<<endl;

    }
    // 输出迁移指令
    string mig_head = "(migration, )";
    mig_head.insert(12,int2str(mig_list.size()));
    cout<<mig_head<<endl;
    // outfile<<mig_head<<endl;
    if(mig_list.size()!=0)
    {
        for(int ij =0; ij<mig_list.size();ij++)
        {
            if(mig_list[ij].m_node==0)
            {
                cout<<"("<<mig_list[ij].m_vmid<<", "<<mig_list[ij].m_serverid<<")"<<endl;
            // outfile<<"("<<mig_list[ij].m_vmid<<", "<<mig_list[ij].m_serverid<<")"<<endl;
            }
            else if(mig_list[ij].m_node==1)
            {
                cout<<"("<<mig_list[ij].m_vmid<<", "<<mig_list[ij].m_serverid<<", A)"<<endl;
            // outfile<<"("<<mig_list[ij].m_vmid<<", "<<mig_list[ij].m_serverid<<", A)"<<endl;
            }
            else if(mig_list[ij].m_node==2)
            {
                cout<<"("<<mig_list[ij].m_vmid<<", "<<mig_list[ij].m_serverid<<", B)"<<endl;
            // outfile<<"("<<mig_list[ij].m_vmid<<", "<<mig_list[ij].m_serverid<<", B)"<<endl;
            }
        }
    }
    // 输出部署指令
    // cout<<"今日待处理请求数："<<set_vm_map[today].size()<<endl;
    // for(auto it = set_vm_map[today].begin();it!=set_vm_map[today].end();it++)
    int _oper_size = oper_list_sct.m_OP.size();
    
    for(int i=1;i<_oper_size;i++)
    {
        
        int _id = oper_list_sct.m_OP[i].m_serverid ;//////
        // _id = 0;

        int oper = oper_list_sct.m_OP[i].m_node;
        // if(_id>(server_id_count-1))
        // {cout<<"出现服务器id溢出了！!!!!!!!!!!!!!!"<<endl;}
        // string out = "()";
        switch(oper)
        {
            case -1:break;
            case -2:break;
            case -3:break;
            case 0 :
            cout<< "(" <<_id<< ")"<<endl;
                    // outfile<< "(" <<_id<< ")"<<endl;
                    break;
            case 1 :
            cout<< "(" <<_id<< ", " <<"A"<< ")"<<endl;
                    // outfile<< "(" <<_id<< ", " <<"A"<< ")"<<endl;
                    break;
            case 2 :
            cout<< "(" <<_id<< ", " <<"B"<< ")"<<endl;
                    // outfile<< "(" <<_id<< ", " <<"B"<< ")"<<endl;
                    break;
            case 3 :break;
        }
    }
}



void read_req_map()
{
    
    vector<vector<int>> reqs_aday ;
    
    // total_day = reqs_map.size();
    
    for(today = 0;today<total_day;today++)
    {
        mig_list.clear();
        int max_mignum;
        max_mignum = floor(vms_info_map.size()*30/1000) ;

        if(3*today>total_day)
        {
            int add_size_yesterday = add_vmsize[today-1];
            if((!super_mig_flag)&&(add_size_yesterday>1000))
            {
                max_mignum = vms_info_map.size();
                super_mig_flag =1;
            }
        }
        
        
        migration(max_mignum);
/*
//输出服务器使用率   //------------------------------------------------------------------------------------------------------------提交注释
    for(int i=0;i<server_id_count;i++)
    {
	double cpuRate=0.0;
	double memoryRate=0.0;
	//if( server_vminfo_map[i].size()>0 )
	//{
	    string Type = serverIdType_map[i];
	    server _serverInfo = server_type_map[Type];
	    server_info _useserver = servers_info_map[i];
	    cpuRate = 1 - double(_useserver.A_cpu+_useserver.B_cpu)/double(_serverInfo.cpu);
	    memoryRate = 1 - double(_useserver.A_memory+_useserver.B_memory)/double(_serverInfo.memory);
	    //if( (cpuRate-memoryRate)>0.3 || (cpuRate-memoryRate)<-0.3)
	    //{
	    	//cout<< "server " <<i<< ": "<<"cpuRate: "<<cpuRate<<" memoryRate: "<<memoryRate<<endl;
		outfile<<"server " <<i<< ": "<<"cpuRate: "<<cpuRate<<" memoryRate: "<<memoryRate<<endl;
	    //}
	//}
    }
*/

        vector<string> purchase_list;
        reqs_aday.clear();
        reqs_aday = reqs_map[today];


        int add_size_today = add_vmsize[today];
        if(add_size_today>1000)
        {   
            // for(int sct =0;sct<server_choose_times;sct++)
            // {
                // oper_list_sct=FVIII (0.0,{{sct,sct,sct}});
                climbmount_setvm_pro(reqs_aday,server_choose_times);
            // }
        }
        else
        {
            climbmount_setvm(reqs_aday,server_choose_times);
        }


        //更新map
        if(today<total_day - first_read_lenth)
        {
            string s;
            string type_temp; 
            string id_temp;
            cin>>s;
            int reqs_num = str2int(s);
            for(int j =0;j<reqs_num;j++)
            {
                int core_vm;
                int mem_vm;
                int is_double;
                int need_core;
                int need_mem;
                cin>>s;
                if(s[1]=='a')
                {
                    cin>>type_temp>>id_temp;
                    type_temp=type_temp.substr(0,type_temp.size()-1);
                    id_temp=id_temp.substr(0,id_temp.size()-1);
                    core_vm = vm_type_map[type_temp].cpu;
                    mem_vm = vm_type_map[type_temp].memory;
                    all_vmcores += core_vm;
                    all_vmmem += mem_vm;
                    if(all_vmcores>max_vmcores){max_vmcores=all_vmcores;};
                    if(all_vmmem>max_vmmem){max_vmmem=all_vmmem;};
                    is_double = vm_type_map[type_temp].is_double_node;
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
                    //保存请求
                    vms_time[str2int(id_temp)] = IntInt(today+first_read_lenth,1001);
                    reqs_map[today+first_read_lenth].push_back({1,core_vm,mem_vm,is_double,str2int(id_temp)});


                }
                else if(s[1]=='d')
                {
                    cin>>id_temp;
                    id_temp=id_temp.substr(0,id_temp.size()-1);
                    type_temp = vmIdType_map[id_temp];
                    core_vm = vm_type_map[type_temp].cpu;
                    mem_vm = vm_type_map[type_temp].memory;
                    is_double = vm_type_map[type_temp].is_double_node;
                    all_vmcores -= core_vm;
                    all_vmmem -= mem_vm;
                    //保存删除请求
                    vms_time[str2int(id_temp)].m_node = today+first_read_lenth;
                    reqs_map[today+first_read_lenth].push_back({0,core_vm,mem_vm,is_double,str2int(id_temp)});
                }
            }
            value_piancha = (float)(vl_wt*max_vmmem/max_vmcores);
            add_vmsize[today+first_read_lenth]=0;
            if(all_vmcores>max_vmcores)
            {
                add_vmsize[today+first_read_lenth]+=all_vmcores-max_vmcores;
                max_vmcores=all_vmcores;}
            if(all_vmmem>max_vmmem)
            {
                add_vmsize[today+first_read_lenth]+=all_vmmem-max_vmmem;
                max_vmmem=all_vmmem;}
        }

/*
//输出服务器使用率   //------------------------------------------------------------------------------------------------------------提交注释
    for(int i=0;i<server_id_count;i++)
    {
	double cpuRate=0.0;
	double memoryRate=0.0;
	//if( server_vminfo_map[i].size()>0 )
	//{
	    string Type = serverIdType_map[i];
	    server _serverInfo = server_type_map[Type];
	    server_info _useserver = servers_info_map[i];
	    cpuRate = 1 - double(_useserver.A_cpu+_useserver.B_cpu)/double(_serverInfo.cpu);
	    memoryRate = 1 - double(_useserver.A_memory+_useserver.B_memory)/double(_serverInfo.memory);
	    //if( (cpuRate-memoryRate)>0.3 || (cpuRate-memoryRate)<-0.3)
	    //{
	    	//cout<< "server " <<i<< ": "<<"cpuRate: "<<cpuRate<<" memoryRate: "<<memoryRate<<endl;
		outfile<<"server " <<i<< ": "<<"cpuRate: "<<cpuRate<<" memoryRate: "<<memoryRate<<endl;
	    //}
	//}
    }
*/

    }
}


int main()
{
    int reqs_count = 0;
    int day = 0;
    int num = 0;
    int amount = 0;
    string s;

    // start = clock();
    // 读取服务器和虚拟机类型数据，建立服务器类型表和虚拟机类型表--------------------------------------------------------------------------------------

    // std::freopen("/home/kai/file/HuaWeiCodeCraft2021/training-data/training-1.txt","rb",stdin);
    // std::freopen("training-1.txt","rb",stdin);
    // std::freopen("trainig-1.txt","rb",stdin);


    cin>>s;
    amount = str2int(s);

    



    for (int i = 0; i < amount; i++)
    {
        string type_temp,core_temp,mem_temp,purchasecost_temp,runcost_temp;
        cin>>type_temp>>core_temp>>mem_temp>>purchasecost_temp>>runcost_temp;
        generateServer(type_temp,core_temp,mem_temp,purchasecost_temp,runcost_temp);
    }
    // cout<<"服务器类型总数 ： "<<server_type_map.size()<<endl;
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

    //根据数据集选择灵魂参数
    if(total_day<900)
    {
        piancha_run_purchse = 2; //越大runcost越重要
        purchase_wt= 8;         //越大购买价格更重要
        vl_wt = 2;  //增大核数内存量纲比
        vl_wt2 = 2;//用在部署的量纲扩张系数
    }
    cin>>s;
    first_read_lenth = str2int(s);

    //读取第一批请求数据，计算内存和核数出现的最大值

    int need_mem =0;
    int need_core =0;
    for (int i =0;i<first_read_lenth;i++)
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
                core_vm = vm_type_map[type_temp].cpu;
                mem_vm = vm_type_map[type_temp].memory;
                all_vmcores += core_vm;
                all_vmmem += mem_vm;
                // if(all_vmcores>max_vmcores){max_vmcores=all_vmcores;};
                // if(all_vmmem>max_vmmem){max_vmmem=all_vmmem;};
                is_double = vm_type_map[type_temp].is_double_node;
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
                //保存请求和虚拟机建立时间

                vms_time[str2int(id_temp)] = IntInt(i,1001);
                reqs_map[i].push_back({1,core_vm,mem_vm,is_double,str2int(id_temp)});


            }
            else if(s[1]=='d')
            {
                cin>>id_temp;
                id_temp=id_temp.substr(0,id_temp.size()-1);
                type_temp = vmIdType_map[id_temp];
                core_vm = vm_type_map[type_temp].cpu;
                mem_vm = vm_type_map[type_temp].memory;
                is_double = vm_type_map[type_temp].is_double_node;
                all_vmcores -= core_vm;
                all_vmmem -= mem_vm;

                //保存删除请求和虚拟机删除时间
                vms_time[str2int(id_temp)].m_node = i;

                reqs_map[i].push_back({0,core_vm,mem_vm,is_double,str2int(id_temp)});
            }
            add_vmsize[i]=0;
            if(all_vmcores>max_vmcores)
            {
                add_vmsize[i]+=all_vmcores-max_vmcores;
                max_vmcores=all_vmcores;}
            if(all_vmmem>max_vmmem)
            {
                add_vmsize[i]+=all_vmmem-max_vmmem;
                max_vmmem=all_vmmem;}


        }
    }
    
    int err_cm =max_vmcores-max_vmmem;

    //计算各服务器相对性价比，并建立服务器类型-性价比排序表（相对性价比，服务器类型名）
    value_piancha = (float)(vl_wt*max_vmmem/max_vmcores);
    for(auto it =server_type_map.begin();it!=server_type_map.end();it++)
    {
        float cost_perday = 0.0;
        string type_temp = it->first;
        int servercost_temp = it->second.cost;
        int runcost_temp = it->second.cost_day;
        int server_core_temp =  it->second.cpu;
        int server_mem_temp = it->second.memory;
        int core_mem_err = server_core_temp - server_mem_temp;
	    double server_rate = double(server_core_temp)/double(server_mem_temp);
        cost_perday = (float)(servercost_temp/total_day + runcost_temp*piancha_run_purchse)/(server_core_temp*value_piancha+server_mem_temp);
        serverTypeCostSet.insert(SFI(type_temp,cost_perday,core_mem_err,server_rate));
    }

    read_req_map();

// /**/
//     finish = clock();
//     printf("\nusr time: %f s \n",double(finish - start) / CLOCKS_PER_SEC);
    // fclose(stdin);
    // outfile.close();
    return 0 ;
}
