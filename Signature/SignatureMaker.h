
#ifndef _SIGNATUREMAKER_H_
#define _SIGNATUREMAKER_H_

#include <string>
#include <fstream>

using std::string;
using std::ofstream;
using std::streampos;

/**
 *  This class calculates signature per blocks of input file and writes it to the output file
 */
class SignatureMaker
{
	string _input_file;
	string _output_file;
	unsigned long long _blocksize;

	streampos _input_offset;
	ofstream _ofile;

	void readPortion(streampos read_size, char* buffer);
	void processPortion(streampos size, char* buffer);
	streampos processBlocks(streampos blocks_count, unsigned long long extra_block_size, streampos extra_block_number, streampos write_size, streampos processed_blocks, char* buffer);

	streampos inputSize();
public:
	// You need provide input and output file and size of block for calculatien
	SignatureMaker(const string& input_file, const string& output_file, unsigned long long blocksize)
		:_input_file(input_file), _output_file(output_file), _blocksize(blocksize)
	{}

	// Call this to calculate signature and put it to output file
	void makeSignature();
};

#endif
