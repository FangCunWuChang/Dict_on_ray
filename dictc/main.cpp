extern "C"
{
#include "getargs.h"
}

#include "diction.h"

#include <string>

int main(int argc, char* argv[])
{
	if (conf_get_options(argc, argv) != 0) {
		return 0;
	}
	std::string inFile(conf_get_input()), outFile(conf_get_output());

	const char* w_str = conf_get_word(), * t_str = conf_get_translation();
	std::string wStr("word"), tStr("translation");
	if (w_str != NULL) {
		printf("Using word title: %s\n", w_str);
		wStr = w_str;
	}
	else {
		printf("Using default word title: word\n");
	}
	if (t_str != NULL) {
		printf("Using translation title: %s\n", t_str);
		tStr = t_str;
	}
	else {
		printf("Using default translation title: translation\n");
	}
	
	Diction dict(wStr, tStr);
	
	try {
		printf("Loading input file: %s\n", inFile.c_str());
		dict.load(inFile);
		
		printf("Parsing dictionary...\n");
		dict.parse();
		
		printf("Saving: %s\n", outFile.c_str());
		dict.save(outFile);
		
		printf("Complete!\n");
	}
	catch (std::bad_alloc&) {
		printf("Out of memory!\n");
		exit(-2);
	}
	
	return 0;
}
