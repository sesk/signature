
#include "SignatureTask.h"

#include <boost/crc.hpp>


void SignatureTask::operator()()
{
		for (streampos i = _begin_block; i < _end_block; i += 1)
		{
			processBlock(i);
		}
}

void SignatureTask::processBlock(streampos& number)
{
	if (number + _processed_blocks == _extra_block)
	{
		processBlock(_extra_block_size, number*_blocksize, number*HASH_SIZE);
	}
	else
	{
		processBlock(_blocksize, number*_blocksize, number*HASH_SIZE);
	}
}

void SignatureTask::processBlock(unsigned long long blocksize, streampos fl_offset, streampos bf_offset)
{
	int signature = calculateHash(_in_buffer + fl_offset, blocksize);

	memcpy(_out_buffer + bf_offset, &signature, HASH_SIZE); 
}

int SignatureTask::calculateHash(const char* block, unsigned long long blocksize)
{
	boost::crc_32_type result;
	result.process_bytes(block, blocksize);
	return result.checksum();
}


