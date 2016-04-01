#include "iostream"
#include "string"
#include "cassert"
#include "rocksdb/db.h"

int main(int argc, char const *argv[])
{
        rocksdb::DB *db;
        rocksdb::Options options;
        options.create_if_missing = true;
        rocksdb::Status status = rocksdb::DB::Open(options, "/mnt/testdb", &db);
        assert(status.ok());

        rocksdb::Iterator *it = db->NewIterator(rocksdb::ReadOptions());
        int i = 0;

        //for(it->SeekToFirst(); it->Valid(), i < 1000; it->Next(), i++) {
        //for range queries
        for(it->Seek("1000"); it->Valid() , it->key().ToString() < "19999"; it->Next()) {
                std::cout<<it->key().ToString()<<" : "<<it->value().ToString()<<std::endl;
        }
        assert(it->status().ok());

        return 0;
}
