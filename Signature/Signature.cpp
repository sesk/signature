// Signature.cpp: определяет точку входа для приложения.
//

#include "Signature.h"
#include <sstream>
#include "SignatureMaker.h"
#include "Configurations.h"

using namespace std;

int main(int argc, char* argv[])
{
	try
	{
		if (argc < 3)
		{
			throw exception("You must give at least two parameters: in, out files and block size (in megabytes) optional");
		}

		char* inf = argv[1];
		char* outf = argv[2];

		unsigned long long blocksize = SIZE_1_MB;
		if (argc > 3)
		{
			std::istringstream isstr(argv[3]);
			isstr >> blocksize;

			if (blocksize < 1)
				throw exception("The size of block cannot be less than 1");
		}

		SignatureMaker maker(inf, outf, blocksize);
		maker.makeSignature();
	}
	catch (exception& e)
	{
		cerr << e.what() << endl;
		return 1;
	}

	return 0;
}
