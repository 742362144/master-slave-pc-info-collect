//
// Created by coder on 1/2/18.
//

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <regex.h>
#include <string.h>
#include "PCInfoTools.h"

using namespace std;

std::string tools::PCInfoTools::execute(std::string command) {
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    cout<<result<<endl;
    return result;
}


std::string tools::PCInfoTools::parse(string str, string pattern) {
    regex_t reg;    //定义一个正则实例
    regcomp(&reg, pattern.c_str(), REG_EXTENDED);    //编译正则模式串
    const char* buf = str.c_str();   //定义待匹配串
    const size_t nmatch = 1;    //定义匹配结果最大允许数
    regmatch_t pmatch[1];   //定义匹配结果在待匹配串中的下标范围
    int status = regexec(&reg, buf, nmatch, pmatch, 0); //匹配他

    if (status == REG_NOMATCH){ //如果没匹配上
        printf("No Match\n");
    }
    else if (status == 0){  //如果匹配上了

        int length = pmatch[0].rm_eo - pmatch[0].rm_so;
        string result = str.substr(pmatch[0].rm_so, length);    //sub str

        cout<<"result is :"<<result<<endl;
        regfree(&reg);  //释放正则表达式
        return result;
    }
    regfree(&reg);  //释放正则表达式
    return "";
}


tools::Cpu tools::PCInfoTools::parseCpu() {
    Cpu cpu;
    const char* p_int_num = R"(\b[1-9]\d*)";  // \b[0-9]*$
    const char* p_float_num = R"(\b[1-9][0-9]*\.[0-9]*|0\.[0-9]*[1-9][0-9]*)";
    const char* p_cpu = R"(\bCPU\(s\):[. \s]*[0-9]*)"; //定义cpu个数模式串
    const char* p_fre = R"(\bCPU MHz:\s*(\b[1-9][0-9]*\.[0-9]*|0\.[0-9]*[1-9][0-9]*))"; //定义frequency模式串
//    const char* p_fre = R"(\bCPU MHz:\s*[1-9][0-9]*\.[0-9]*)"; //定义frequency模式串
    //!!!!!!!!!!!!!!!!!!!!!!!!
    //dont match \d but [0-9]
    //dont match \n but \s

    //get shell result
    string str_exec = execute("lscpu");

    //get cpu num
    string cpu_match = parse(str_exec, p_cpu);
    if(!cpu_match.empty()){
        string str_cpu_num = parse(cpu_match, p_int_num);
        if(!str_cpu_num.empty()){
            cpu.setFrequency(atof(str_cpu_num.c_str()));
            cout<<str_cpu_num<<endl;
        }
    }
    //get fre
    string fre_match = parse(str_exec, p_fre);
    if(!fre_match.empty()){
        string str_fre_num = parse(fre_match, p_float_num);
        if(!str_fre_num.empty()){
            cpu.setCore(atoi(str_fre_num.c_str()));
            cout<<str_fre_num<<endl;
        }
    }
    return cpu;//todo
}
tools::Memory tools::PCInfoTools::parseMem() {
    Memory mem;
    const char* p_int_num = R"(\b[1-9]\d*)";  // \b[0-9]*$
    const char* p_float_num = R"(\b[1-9][0-9]*\.[0-9]*|0\.[0-9]*[1-9][0-9]*)";
    const char* p_mem = R"(\bMem:[\s 0-9]*)"; //定义根挂载点模式串

    // get shell result
    string str_exec = execute("free -m");
    //get memmory info
    string mem_match = parse(str_exec, p_mem);

    //analys match
    istringstream is(mem_match);
    list<string> list_str;
    string s;
    //根据空白符分隔开几个子串
    while(is>>s){
        list_str.push_back(s);
    }

    list<string>::iterator it = list_str.begin();

    advance(it,1);
    mem.setTotal(atoi((*it).c_str()));//迭代器往后移动1个
    advance(it,1);
    mem.setUsed(atoi((*it).c_str()));//迭代器往后移动2个
    advance(it,1);
    mem.setAvail(atoi((*it).c_str()));//迭代器往后移动3个

    cout<<mem.getTotal()<<" "<<mem.getUsed()<<" "<<mem.getAvail()<<" "<<endl;
    return mem;
}
tools::Disk tools::PCInfoTools::parseDisk() {
    Disk disk;
    const char* p_int_num = R"(\b[1-9]\d*)";  // \b[0-9]*$
    const char* p_float_num = R"(\b[1-9][0-9]*\.[0-9]*|0\.[0-9]*[1-9][0-9]*)";
    const char* p_disk = R"(/.*\s*.*[A-Z]\s*.*[A-Z]\s*.*[A-Z]\s*.*% /\s)"; //定义根挂载点模式串
    // /[a-z0-9]*\s*.*[A-Z]\s*.*[A-Z]\s*.*[A-Z]\s*.*% /\s

    // get shell result
    string str_exec = execute("df -lh");
    //get disk num
    string disk_match = parse(str_exec, p_disk);

    //analys match
    istringstream is(disk_match);
    list<string> list_str;
    string s;
    //根据空白符分隔开几个子串
    while(is>>s){
        list_str.push_back(s);
    }

    list<string>::iterator it = list_str.begin();

    advance(it,1);
    disk.setTotal(atof((*it).c_str()));//迭代器往后移动1个
    advance(it,1);
    disk.setUsed(atof((*it).c_str()));//迭代器往后移动2个
    advance(it,1);
    disk.setAvail(atof((*it).c_str()));//迭代器往后移动3个

    cout<<disk.getTotal()<<" "<<disk.getUsed()<<" "<<disk.getAvail()<<" "<<endl;

    return disk;
}
tools::Net tools::PCInfoTools::parseNet() {
    string str_exec = execute("ifconfig");
}

void tools::PCInfoTools::init() {
    //cpu
    parseCpu();
    //mem
    parseMem();
//    //disk
    parseDisk();
//    //net

}