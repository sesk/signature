
#include "SignatureTask.h"

#include <boost/crc.hpp>


void SignatureTask::operator()()
{
		streampos size = (_end_block - _begin_block) * _blocksize;
		if (_end_block + _processed_blocks > _extra_block)
			size -= _blocksize - _extra_block_size;

		_in_buffer.reset(new char[size]);

		std::ifstream inf(_in_file, std::ios::in | std::ios::binary);
		if (!inf.is_open())
			throw std::exception("Can't open file");
		inf.seekg(_begin_block*_blocksize + _in_offset);
		inf.read(_in_buffer.get(), size);
		inf.close();

		for (streampos i = _begin_block; i < _end_block; i += 1)
		{
			processBlock(i);
		}
}

void SignatureTask::processBlock(streampos& number)
{
	if (number + _processed_blocks == _extra_block)
	{
		processBlock(_extra_block_size, (number - _begin_block)*_blocksize, number*HASH_SIZE);
	}
	else
	{
		processBlock(_blocksize, (number - _begin_block)*_blocksize, number*HASH_SIZE);
	}
}

void SignatureTask::processBlock(unsigned long long blocksize, streampos fl_offset, streampos bf_offset)
{
	int signature = calculateHash(_in_buffer.get() + fl_offset, blocksize);

	memcpy(_out_buffer + bf_offset, &signature, HASH_SIZE); 
}

int SignatureTask::calculateHash(const char* block, unsigned long long blocksize)
{
	boost::crc_32_type result;
	result.process_bytes(block, blocksize);
	return result.checksum();
}


