#include <iostream>
#include <string>
#include <cassert>
#include "rocksdb/db.h"
#include "rocksdb/table.h"
#include "rocksdb/slice_transform.h"
using namespace std;

class Configuration
{
private:
        size_t write_buffer_size = 512*1024*1024;
        int max_write_buffer_number = 5;
        int min_write_buffer_number_to_merge = 1;

        int num_levels = 1;
        bool create_if_missing = true;
        bool error_if_exists = false;
public:
        Configuration(){}
        ~Configuration(){}

        size_t getWriteBufferSize() {
                return write_buffer_size;
        }

        int getMaxWriteBufferNumber() {
                return max_write_buffer_number;
        }

        int getMinWriteBufferNumberToMerge() {
                return min_write_buffer_number_to_merge;
        }

        int getNumLevels() {
                return num_levels;
        }

        bool getCreateIfMissing() {
                return create_if_missing;
        }

        bool getErrorIfExists() {
                return error_if_exists;
        }
};

string generateFixedSizeString(size_t size) {
        string val = "";
        for (int i = 0; i < size; ++i)  {
                val = val + '0';
        }
        return val;
}

int main(int argc, char const *argv[])
{
        Configuration conf;

        rocksdb::DB *db;
        rocksdb::Options options;
        options.create_if_missing = conf.getCreateIfMissing();
        options.error_if_exists = conf.getErrorIfExists();
        options.write_buffer_size = conf.getWriteBufferSize();
        options.max_write_buffer_number = conf.getMaxWriteBufferNumber();
        options.min_write_buffer_number_to_merge = conf.getMinWriteBufferNumberToMerge();
        options.info_log_level = rocksdb::InfoLogLevel::DEBUG_LEVEL;

        options.table_factory.reset(NewPlainTableFactory(rocksdb::PlainTableOptions()));
        options.prefix_extractor.reset(rocksdb::NewFixedPrefixTransform(8));

        rocksdb::Status status = rocksdb::DB::Open(options, "/mnt/testdb", &db);
        assert(status.ok());
        //cout<<status.ToString()<<endl;

        //Generate a 1Kb string for value
        const string VALUE = generateFixedSizeString(1024);
        //Generate a 32Bytes string for key
        const string KEY = generateFixedSizeString(32);

        //Generate 5 million key-value pairs
        for (long i = 0; i < 5000000; ++i) {
                string key = to_string(i);
                key = key + KEY.substr(key.length());

                //write to the db
                status = db->Put(rocksdb::WriteOptions(), key, VALUE);
        }

        //Take db snapshot
        rocksdb::ReadOptions readOptions;
        readOptions.snapshot = db->GetSnapshot();

        //write 1M key-value pairs
        for (long i = 9000000; i < 10000000; ++i) {
                string key = to_string(i);
                key = key + KEY.substr(key.length());

                //write to the db
                status = db->Put(rocksdb::WriteOptions(), key, VALUE);
        }

        string value;
        rocksdb::Iterator *it = db->NewIterator(readOptions);
        for(it->SeekToFirst();it->Valid();it->Next()) {
                value = it->key().ToString();
        }
        delete it;
        cout<<value<<endl;

        rocksdb::Iterator *iter = db->NewIterator(rocksdb::ReadOptions());
        for(iter->SeekToFirst();iter->Valid();iter->Next()) {
                value = iter->key().ToString();
        }
        cout<<value<<endl;

        return 0;
}
