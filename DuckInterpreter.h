/* DuckInterpreter.h
 DuckInterpreter.h contains the DuckInterpreter class
 The DuckInterpreter is the class that is used to interpret a Duck language file. It interprets the code, produces required outputs, and handles all errors 
 in the user's Duck code.
*/


#pragma once
#include "Statement.h"
#include "SymbolTable.h"
#include "ArrayTable.h"
#include<string>
#include<iostream>
#include <assert.h>
#include <vector>
#include "DuckInterpreterException.h"

class DuckInterpreter
{
public:
    DuckInterpreter() {}
    ~DuckInterpreter(){}

    /**/
    /*
    void RecordStatements(std::string a_filename)
     
    NAME
        RecordStatements - Records the statements that are in the specified file.
    
    SYNOPSIS
        RecordStatements(std::string a_filename)
            a_filename --> The name of the Duck program source file 
     
    DESCRIPTION
        This function stores all the statements in the Duck language source file using a Statement object.
     
    RETURNS 
        void
     
    AUTHOR
        Biplab Thapa Magar
    */
    /**/
    void RecordStatements(std::string a_fileName) {
		m_statements.RecordStatements(a_fileName);
	}/*void RecordStatements(std::string a_filename)*/
    
    
    
	// Runs the interpreter on the recorded statements.
	void RunInterpreter();

private:

	// The statement object that holds the recorded statements.
	Statement m_statements;

	// The sysmbol table object that holds all the variable names and their values.
	SymbolTable m_symbolTable;
    
    //The array table object that holds all the arrays and the values they contain
    ArrayTable m_arrayTable;

	// Stacks for the operators and numbers.  These will be used in evaluating statements.
    
    //this is set to true if the end of the program (marked by the end statement) is reached
    bool m_endFlag;

    //enum class that is used to communicate the type of a statement and to evaluate/execute the statement accordingly
	enum class StatementType
	{
		ArithmeticStat,
		IfStat,
		ReadStat,
		PrintStat,
		StopStat,
		EndStat,
		gotoStat,
        arrayDeclarationStat,
	};
	
	// Executes the statement at the specified location.  Returns the location of the next statement to be executed.
	int ExecuteStatement(const std::string &a_statement, int a_StatementLoc);

	// Returns the next element in the statement.  Returns the next location to be accessed.
	int ParseNextElement(const std::string &a_statement, int a_nextPos, std::string &a_stringValue, double &a_numValue);
    
    //Gets the length of an element in a statement starting from the given index
    unsigned long GetLengthOfElement(const std::string& a_statement, int a_indexOfStartOfElement) ;

	// Evaluate an arithmetic statement.
	void EvaluateArithmeticStatement(const std::string &a_statement );
    
    // Evaluate an arithmetic expression.  Return the value.  The variable a_nextPos is index to the element after the end of the expression
    double EvaluateArithmeticExpression(const std::string &a_statement, int &a_nextPos);

    //Sees what operator is found in an arithmetic expression, then evaluates the numbers and operators in the stacks, and adjusts the
    //operator stack and number stack accordingly.
    void HandleOperator(std::string& a_operator, std::string& a_lastElement, std::vector<std::string>& a_operatorStack, std::vector<double>& a_numberStack);

    //Takes in a statement, the index of the location of the variable/array, and the variable/array name and
    //returns the value found in the variable/array
    double ExtractValueFromVariableOrArray(const std::string& a_statement, int &a_nextPos, std::string& arrayOrVariableName);
    
    // Returns the precedence of an operator.
    int FindPrecedence(std::string a_op);

    //Evaluates the numbers at the top of the number stack with respect to the operator at the top of the operator stack and pushes the result back into the number stack
    void EvaluateTopOfStack(std::vector<std::string>& a_operatorStack, std::vector<double>& a_numberStack);

    //Evaluates the result of two (or one, in the case of unary operators) numbers with respect to the given operator
    //returns true if the operation is defined, false otherwise
    bool EvaluateOperation(std::string a_operation, double& a_result, double a_leftOperand, double a_rightOperand = 0);

    //Evaluate an if statement. Return the next line to execute depending on the result of the if condition
    int EvaluateIfStatement(const std::string& a_statement, int a_nextStatement);

    //Evaluate a goto statement. Returns the next line to execute
    int EvaluateGotoStatement(const std::string& a_statement, int a_nextPos = 0);

    //Execute the stop statement, causing the program to terminate
    void ExecuteStopStatement(const std::string& a_statement);

    //Evaluates the end statmenet.
    void EvaluateEndStatement(const std::string& a_statement);

    //Executes a print statement
    void ExecutePrintStatement(const std::string &a_statement);
    
    //Execute a read statement and stores input into given variables/arrays
    void ExecuteReadStatement(const std::string& a_statement);
    
    //Evaluates the declaration of an array.
    void EvaluateArrayDeclarationStatement(const std::string& a_statement);

    // Determines the type of the next statement.
    StatementType GetStatementType(const std::string &a_string);

    //Returns true if the given string is a legal variable name. Returns false otherwise
    bool IsAValidVariableName(std::string a_variableName);
    
    //Returns true if the given string contains a valid number. Returns false otherwise
    bool IsAValidNumber(std::string a_number);
    
    //Returns true if the given string contains an arithmetic operator like +, -, ==, <=, !, etc.
    bool IsAnArithmeticOperation(std::string a_stringToCheck);
    
    //Returns true if the given string contains a syntactic character like ;, =, +, *, -, [, ], etc
    bool IsASyntacticCharacter(const std::string &a_stringToCheck);
    
    //Returns true if the given string contains a reserved keyword that cannot taken as variable names, like goto, stop, end, etc.
    bool IsAReservedKeyword(std::string a_stringToCheck);

    //Removes the white space from the beginning and end of the given string
    void RemoveExtraneousWhiteSpace(std::string& a_line);
    
   

};
