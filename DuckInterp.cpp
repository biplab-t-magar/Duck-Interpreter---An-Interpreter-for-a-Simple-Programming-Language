// DuckInterp.cpp : Defines the entry point for the console application.
//

#include"DuckInterpreter.h"

using namespace std;


int main( int argc, char *argv[] )
{
	if (argc != 2)
	{
		cerr << "Usage: DuckInterp <filename>" << endl;
		return 1;
	}
	// Create the interpreter object and use it to record the statements and
	// execute them.
	DuckInterpreter duckInt;

	duckInt.RecordStatements(argv[1]);
	duckInt.RunInterpreter();

    return 0;
}
