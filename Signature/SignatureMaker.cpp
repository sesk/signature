
#include "SignatureMaker.h"
#include <list>
#include "SignatureTask.h"
#include "Configurations.h"
#include <iostream>
#include <future>
#include <memory>

using namespace std;

void SignatureMaker::makeSignature()
{
	streampos input_size = inputSize();
	_input_offset = 0;

	_ofile.open(_output_file, ios::binary);
	if (!_ofile.is_open())
		throw exception("Can't open file for writing");

	streampos read_size = MAX_READ_BUFFER_SIZE;
	if (read_size + _input_offset > input_size)
		read_size = input_size - _input_offset;

	unique_ptr<char[]> buffer(new char[read_size]);

	while (read_size)
	{
		readPortion(read_size, buffer.get());
		processPortion(read_size, buffer.get());

		_input_offset += read_size;
		cout << _input_offset << " bytes from " << input_size << " are processed" << endl;

		if (read_size + _input_offset > input_size)
			read_size = input_size - _input_offset;
	}

	_ofile.close();
}

streampos SignatureMaker::inputSize()
{
	ifstream ifile(_input_file, ios::in | ios::binary | ios::ate);
	if (!ifile.is_open())
		throw exception("Can't open file for reading");

	streampos size = ifile.tellg();
	ifile.close();
	return size;
}

void SignatureMaker::readPortion(streampos read_size, char* buffer)
{
	list<future<void>> waits;
	streampos size_per_thread = read_size / THREADS_COUNT;
	if (read_size % THREADS_COUNT)
		size_per_thread += 1;

	streampos readed = 0;
	while (size_per_thread)
	{
		waits.push_back(async([=] {
			std::ifstream inf(_input_file, std::ios::in | std::ios::binary);
			if (!inf.is_open())
				throw std::exception("Can't open file");
			inf.seekg(_input_offset + readed);
			inf.read(buffer + readed, size_per_thread);
			inf.close();
		}));

		readed += size_per_thread;
		if (size_per_thread > read_size - readed)
			size_per_thread = read_size - readed;

	}

	for_each(waits.begin(), waits.end(), [](future<void>& w)
	{
		w.get();
	});
}

void SignatureMaker::processPortion(streampos size, char* buffer)
{
	streampos smooth_blocks_count = size / _blocksize;
	streampos blocks_count = smooth_blocks_count;

	if (size % _blocksize)
		blocks_count += 1;

	streampos output_size = blocks_count * HASH_SIZE;

	streampos processed_blocks = 0;
	
	while (output_size > 0)
	{
		streampos write_size = output_size;
		streampos process_blocks_count = blocks_count;

		if (write_size > MAX_BUFFER_SIZE)
		{
			process_blocks_count = MAX_BUFFER_SIZE / HASH_SIZE;
			write_size = process_blocks_count * HASH_SIZE;
		}

		processed_blocks += processBlocks(process_blocks_count, size % _blocksize, smooth_blocks_count, write_size, processed_blocks, buffer);
		output_size -= write_size;
		blocks_count -= process_blocks_count;
	}
}

streampos SignatureMaker::processBlocks(streampos blocks_count, unsigned long long extra_block_size, streampos extra_block_number, streampos write_size, 
	streampos processed_blocks, char* buffer)
{
	streampos blocks_per_thread = blocks_count / THREADS_COUNT;
	int additional_blocks = blocks_count % THREADS_COUNT;

	int extra_blocks = 0;

	list<future<void>> waits;
	unique_ptr<char[]> out_buffer(new char[write_size]);

	for (int t = 0; t < THREADS_COUNT; ++t)
	{
		streampos begin_block = t * blocks_per_thread + extra_blocks;
		streampos end_block = (t + 1) * blocks_per_thread + extra_blocks;
		if (additional_blocks != 0)
		{
			end_block += 1;
			++extra_blocks;
			--additional_blocks;
		}

		if (begin_block == end_block)
			break;
		waits.push_back(async(launch::async, SignatureTask(begin_block, end_block, extra_block_number, _blocksize, extra_block_size, buffer, out_buffer.get(), processed_blocks)));
	}

	for_each(waits.begin(), waits.end(), [](future<void>& w)
	{
		w.get();
	});

	_ofile.write(out_buffer.get(), write_size);

	return THREADS_COUNT * blocks_per_thread + extra_blocks;
}
