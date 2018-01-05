//
// Created by coder on 1/2/18.
//

#ifndef BRPC_PCINFOTOOLS_H
#define BRPC_PCINFOTOOLS_H

#include <iostream>

namespace tools {
    class Memory{
    private:
        float total = 0;
        float used = 0;
        float avail = 0;
    public:
        void setTotal(float total){
            this->total = total;
        }
        float getTotal(){
            return this->total;
        }
        void setUsed(float used){
            this->used = used;
        }
        float getUsed(){
            return this->used;
        }
        void setAvail(float avail){
            this->avail = avail;
        }
        float getAvail(){
            return avail;
        }
    };

    class Cpu{
    private:
        int core = 0;
        float frequency = 0;
    public:
        void setCore(int core){
            this->core = core;
        }
        int getCore(){
            return this->core;
        }
        void setFrequency(int fre){
            this->frequency = fre;
        }
        float getFrequency(){
            return this->frequency;
        }

        ~Cpu(){

        }
    };

    class Disk{
    private:
        float total = 0;
        float used = 0;
        float avail = 0;
    public:
        void setTotal(float total){
            this->total = total;
        }
        float getTotal(){
            return this->total;
        }
        void setUsed(float used){
            this->used = used;
        }
        float getUsed(){
            return this->used;
        }
        void setAvail(float avail){
            this->avail = avail;
        }
        float getAvail(){
            return avail;
        }
    };

    class Net{

    };

    class PCInfoTools{
    private:
        Cpu cpu;
        Memory mem;
        Net net;
        Disk disk;
    public:
        void setCpu(Cpu cpu){
            this->cpu = cpu;
        }
        Cpu getCpu(){
            return this->cpu;
        }
        void setMemory(Memory mem){
            this->mem = mem;
        }
        Memory getMemory(){
            return this->mem;
        }
        void setNet(Net net){
            this->net = net;
        }
        Net getNet(){
            return this->net;
        }
        void setDisk(Disk disk){
            this->disk = disk;
        }
        Disk getDisk(){
            return this->disk;
        }


        //zhi xing ming ling
        std::string execute(std::string command);
        //huo qu shell jiao ben zhi xing jie guo
        std::string parse(std::string result, std::string pattern);
        void init();
        Cpu parseCpu();
        Memory parseMem();
        Disk parseDisk();
        Net parseNet();
    };
}



#endif //BRPC_PCINFOTOOLS_H
