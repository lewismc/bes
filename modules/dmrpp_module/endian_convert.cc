
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>

#include <XDRStreamUnMarshaller.h>

int main(int argc, char **argv) {
	bool debug = false;
	std::string file;
	int bflag = 0;
	int index;
	int c;

	opterr = 0;
	while ((c = getopt(argc, argv, "dc:")) != -1)
		switch (c) {
		case 'd':
			debug = true;
			break;

		case 'f':
			file = optarg;
			break;

		case '?':
			if (optopt == 'c')
				std::cerr << "Option '-"<< optopt << "' requires an argument." << std::endl;
			else if (isprint(optopt))
				std::cerr << "Unknown option '-"<< optopt << "'" << std::endl;
			else
				std::cerr << "Unknown option character '"<< optopt << "'" << std::endl;
			return 1;

		default:
			abort();
		}

	std::cout << "debug = "<< debug << " file = " << file << std::endl;
	if(file.empty()){
		std::cout << "File name must be set" << std::endl;
	}

	libdap::XDRStreamUnMarshaller xdm(cin);

	for (index = optind; index < argc; index++)
		std::cout << "Non-option argument " << argv[index] << std::endl;

	return 0;
}
