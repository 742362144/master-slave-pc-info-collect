//
// Created by coder on 1/14/18.
//

#include<iostream>
#include<string>
#include<sys/time.h>
#include"leveldb/db.h"

using namespace std;
using namespace leveldb;

#define TEST_DATA_NUM 100000
#define TEST_DATA_LEN 100

#define MAX_LEN 10000

struct timeval start,current;

int main()
{
    DB *db;
    Options options;
    options.create_if_missing = true;
    //options.error_if_exists = true;
    Status s = DB::Open(options,"/usr/local/download/leveldb/lvd.db",&db);
    if(!s.ok()){
        cerr << s.ToString() << endl;exit(-1);
    }

    int i;
    char k[100],v[MAX_LEN+1];
    string key,val;
    for(i = 0;i < MAX_LEN && i < TEST_DATA_LEN ;i ++)
    {
        v[i] = 'x';
    }
    v[i] = '\0';
    val = v;

    //write db test
    gettimeofday(&start,NULL);
    for(i = 0;i < TEST_DATA_NUM; i ++){
        snprintf(k,sizeof(k),"key%d",i);
        key = k;

        s = db->Put(WriteOptions(),key,val);
        if(!s.ok()){
            cerr << s.ToString() << endl;exit(-1);
        }
        if(i % 10000 == 0){
            gettimeofday(&start,NULL);
            double microsecs = 1000000*(current.tv_sec-start.tv_sec)+(current.tv_usec-start.tv_usec);
            printf("write db cost time %d %lf(ms) %lf(ops)\n",i,microsecs,i/(microsecs/1000000));

        }
    }

    //read db test
    gettimeofday(&start,NULL);
    for(i = 0;i < TEST_DATA_NUM; i ++){
        snprintf(k,sizeof(k),"key%d",i);
        key = k;

        s = db->Get(ReadOptions(),key,&val);
        if(!s.ok()){
            cerr << s.ToString() << endl;exit(-1);
        }
        if(i % 10000 == 0){
            gettimeofday(&start,NULL);
            double microsecs = 1000000*(current.tv_sec-start.tv_sec)+(current.tv_usec-start.tv_usec);
            printf("read db cost time %d %lf(ms) %lf(ops)\n",i,microsecs,i/(microsecs/1000000));

        }
    }
}