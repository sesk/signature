
#ifndef _SIGNATURETHREAD_
#define _SIGNATURETHREAD_

#include <fstream>
#include "Configurations.h"

#include <iostream>
#include <fstream>

using std::streampos;

/**
  * Functor class for calculation signature per block
  * work is divided to blocks and calculates from begin block to end block
  */
class SignatureTask
{
	streampos _begin_block;
	streampos _end_block;
	streampos _extra_block;
	unsigned long long _blocksize;
	unsigned long long _extra_block_size;
	const char* _in_file;
	char* _out_buffer;
	streampos _processed_blocks;
	streampos _in_offset;

	std::unique_ptr<char[]> _in_buffer;

	int calculateHash(const char* block, unsigned long long blocksize);

	void processBlock(unsigned long long blocksize, streampos fl_offset, streampos bf_offset);

	void processBlock(streampos& number);

public:
	SignatureTask(streampos& begin_block, // first block to process
		          streampos& end_block,   // block next to last for processing
		          streampos& extra_block, // number of extra block (last block with different size if it exists)
		          unsigned long long blocksize,         // size of usual blocks
		          unsigned long long extra_block_size,  // size of extra block
		          const char* in_file,        // file name for input file
		          char* out_buffer,       // buffer to output data
		          streampos& processed_blocks, // processed blocks (already)
		          streampos in_offset)    // offset of input   
		: _begin_block(begin_block), 
		_end_block(end_block), 
		_extra_block(extra_block), 
		_blocksize(blocksize), 
		_extra_block_size(extra_block_size),
		_in_file(in_file), 
		_out_buffer(out_buffer), 
		_processed_blocks(processed_blocks), 
		_in_offset(in_offset)
	{
	}

	void operator()();
};

#endif
