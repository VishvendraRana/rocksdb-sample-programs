#include <iostream>
#include <string>
#include <cassert>
#include "rocksdb/db.h"
#include "rocksdb/table.h"
#include "rocksdb/cache.h"
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

        rocksdb::BlockBasedTableOptions table_options;
        table_options.block_cache = rocksdb::NewLRUCache(4 * 1024 * 1024);
        options.table_factory.reset(NewBlockBasedTableFactory(table_options));

        rocksdb::Status status = rocksdb::DB::Open(options, "/mnt/testdb", &db);
        assert(status.ok());

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
        for (long i = 5000000; i < 10000000; ++i) {
                string key = to_string(i);
                key = key + KEY.substr(key.length());

                //write to the db
                status = db->Put(rocksdb::WriteOptions(), key, VALUE);
        }

        rocksdb::Iterator *it = db->NewIterator(readOptions);
        it->SeekToLast();
        cout<<it->key().ToString()<<endl;
        delete it;

        rocksdb::Iterator *iter = db->NewIterator(rocksdb::ReadOptions());
        iter->SeekToLast();
        cout<<iter->key().ToString()<<endl;

        return 0;
}
