#include <iostream>
#include <cassert>
#include "rocksdb/db.h"
#include "rocksdb/write_batch.h"

using namespace std;

int main(int argc, char const *argv[])
{
	rocksdb::DB *db;
	rocksdb::Options options;
	options.create_if_missing = true;
	options.error_if_exists = true;

	rocksdb::Status status = rocksdb::DB::Open(options, "c:/testdb", &db);
	assert(status.ok());

	string value;
	rocksdb::Status s = db->Get(rocksdb::ReadOptions(), "key1", &value);
	if(s.ok()) {
		rocksdb::WriteBatch batch;
		batch.Delete("key1");
		batch.put("key2", value);
		s = db->Write(rocksdb::WriteOptions(), &batch);
	}

	delete db;
	return 0;
}