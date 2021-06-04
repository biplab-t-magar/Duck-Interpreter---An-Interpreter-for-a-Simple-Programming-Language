duckInterp: ArrayTable.o DuckInterp.o DuckInterpreter.o DuckInterpreterException.o Statement.o SymbolTable.o
	g++ -std=c++0x ArrayTable.o DuckInterp.o DuckInterpreter.o DuckInterpreterException.o Statement.o SymbolTable.o -o duckInterp

ArrayTable.o: ArrayTable.cpp ArrayTable.h
	g++ -c -std=c++0x ArrayTable.cpp

DuckInterpreter.o: DuckInterpreter.cpp DuckInterpreter.h
	g++ -c -std=c++0x DuckInterpreter.cpp

DuckInterpreterException.o: DuckInterpreterException.cpp DuckInterpreterException.h
	g++ -c -std=c++0x DuckInterpreterException.cpp

Statement.o: Statement.cpp Statement.h
	g++ -c -std=c++0x Statement.cpp

SymbolTable.o: SymbolTable.cpp SymbolTable.h
	g++ -c -std=c++0x SymbolTable.cpp

DuckInterp.o: DuckInterp.cpp 
	g++ -c -std=c++0x DuckInterp.cpp

clean:
	rm *.o duckInterp

