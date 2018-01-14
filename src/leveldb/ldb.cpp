//
// Created by coder on 1/14/18.
//

#include<iostream>
#include<string>
#include"leveldb/db.h"

using namespace std;
using namespace leveldb;

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
    string key = "name",val = "ciaos";
    s = db->Put(WriteOptions(),key,val);
    if(!s.ok()){
        cerr << s.ToString() << endl;exit(-1);
    }
    s = db->Get(ReadOptions(),key,&val);
    if(s.ok()){
        cout << key << "=" << val << endl;
        s = db->Put(leveldb::WriteOptions(),"key2",val);
        if(s.ok()){
            s = db->Delete(leveldb::WriteOptions(),key);
            if(!s.ok()){
                cerr << s.ToString() << endl;exit(-1);
            }
        }
    }
}
