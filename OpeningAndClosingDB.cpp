#include <iostream>
#include <cassert>
#include <string>
#include "rocksdb/db.h"

using namespace std;

int main(int argc, char const *argv[])
{
        rocksdb::DB* db;
        rocksdb::Options options;
        options.create_if_missing = true;
        rocksdb::Status status =  rocksdb::DB::Open(options, "/mnt/testdb", &db);
        assert(status.ok());

        for(long i = 0; i < 1000000; i++) {
                string key = to_string(i);
                string value = "value" + to_string(i);
                status = db->Put(rocksdb::WriteOptions(), key, value);
        }

        string value;
        status = db->Get(rocksdb::ReadOptions(), "key1", &value);
        if(status.ok()) {
                status = db->Put(rocksdb::WriteOptions(), "key3", value);
                if(status.ok()) {
                        status = db->Delete(rocksdb::WriteOptions(), "key1");
                }
        }



        //closing the DB
        //delete db;
        return 0;
}
