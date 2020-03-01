#include "DuckInterpreter.h"

using namespace std;


/**/
/*
 void DuckInterpreter::RunInterpreter()
 
 NAME
    RunInterpreter - Starts the interpreter and executes all the statements of the Duck program one at a time
 
 SYNOPSIS
    void DuckInterpreter::RunInterpreter()
 
 DESCRIPTION
    This function starts the interpreter and executes all the statements in the Duck program until an error occurs or a stop statement is encountered
 
 RETURNS
    void
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
void DuckInterpreter::RunInterpreter() {
    m_endFlag = false;
	int nextStatement = 0;
    string statement;
	while (true)
    {    
        try {
            statement = m_statements.GetStatement(nextStatement);
        } catch(string e) {
            //stop statement should have arrived before the end of a file
            cerr << "Error: No more lines to execute. " << endl;
            exit(1);
        }
        if(m_endFlag) {
            cerr << "Error: There can be no statements after the end statement." << endl;
            exit(1);
        }
        try {
            nextStatement = ExecuteStatement(statement, nextStatement);
        } catch(DuckInterpreterException& e) {
            cerr << endl;
            cerr << "Error: Line " << nextStatement + 1 << ": \"" << statement << "\": " << endl;
            cerr << "      " << e.what() << endl << endl;
            exit(1);
        }
		
	}
}/** void DuckInterpreter::RunInterpreter() **/


/**/
/*
 int DuckInterpreter::ExecuteStatement(const string& a_statement, int a_nextStatement)
 
 NAME
    ExecuteStatement - Executes the given statement
 
 SYNOPSIS
    int DuckInterpreter::ExecuteStatement(const string& a_statement, int a_nextStatement)
        a_statement --> The statement to be executed
        a_nextStatement --> The line number of the current statement
 
 DESCRIPTION
    This function first figures out the type of the given statement and then executes it according to the type. It also returns the 
    the next line to be executed
 
 RETURNS
    The next line to be executed
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
int DuckInterpreter::ExecuteStatement(const string& a_statement, int a_nextStatement)
{
    
    //check if the statement ends with a semi-colon
    if(a_statement[a_statement.length() - 1] != ';') {
        throw DuckInterpreterException("';' is missing from end of statement");
    }
    StatementType type = GetStatementType(a_statement );

	// Based on the type, execute the statement
	switch (type)
	{
	case StatementType::ArithmeticStat:
        EvaluateArithmeticStatement(a_statement);
		return a_nextStatement + 1;
        break;
	case StatementType::IfStat:
		return EvaluateIfStatement(a_statement, a_nextStatement);
        break;
    case StatementType::gotoStat:
        return EvaluateGotoStatement(a_statement);
        break;
	case StatementType::StopStat:
        ExecuteStopStatement(a_statement);
        break;
	case StatementType::EndStat:
        EvaluateEndStatement(a_statement);
        break;
	case StatementType::PrintStat:
        ExecutePrintStatement(a_statement);
        return a_nextStatement + 1;
        break;
	case StatementType::ReadStat:
        ExecuteReadStatement(a_statement);
        return a_nextStatement + 1;
        break;
    case StatementType::arrayDeclarationStat:
        EvaluateArrayDeclarationStatement(a_statement);
        return a_nextStatement + 1;
        break;
	default:
		cerr << "Line " << a_statement << ": BUGBUG - program terminate: invalid return value from GetStatementType for the statement: " << a_statement << endl;
		exit(1);
	}
    return a_nextStatement + 1;
} /** int DuckInterpreter::ExecuteStatement(const string& a_statement, int a_nextStatement) **/


/**/
/*
 int DuckInterpreter::ParseNextElement(const string &a_statement, int a_nextPos, string &a_stringValue, double &a_numValue)
 
 NAME
    ParseNextElement - Retrieves the next element in a statement (whether it be a number or a word) 
        and returns the index of the starting position of the next element
 
 SYNOPSIS
    int DuckInterpreter::ParseNextElement(const string &a_statement, int a_nextPos, string &a_stringValue, double &a_numValue)
        a_statement --> The statement whose element is to be parsed
        a_nextPos --> The index of the starting position of the element to be parsed
        a_stringValue --> The element that has been parsed 
        a_numValue --> The number that has been parsed
 DESCRIPTION
    This function retrieves the next element in the statement starting at the given index. It stores the element in either a_stringValue (it is not a number)
    or in a_numValue (if it is a number).
 
 RETURNS
    The starting position of the next element in the statement. If there are no elements left, then it return -1.
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
int DuckInterpreter::ParseNextElement(const string &a_statement, int a_nextPos, string &a_stringValue, double &a_numValue) {
    if(a_nextPos < 0) {
        return -1;
    }
    int indexOfStartOfElement = a_nextPos;
    //skip any white space starting from the index to the beginning of the closest element
    while((a_statement.substr(indexOfStartOfElement)).length() != 0 && (a_statement[indexOfStartOfElement] == ' ' || a_statement[indexOfStartOfElement] == '\t')){
        indexOfStartOfElement++;
    }
    
    //If no element is left in the statement after removing white space
    // Note: This case should not occur because:
    //      i)in a previous call to ParseNextElement, a_nextPos must have been set to -1 and the user of this function should not
    //              be calling ParseNextElement if a_nextPos is -1.
    //      ii)No statement stored in m_statements is entirely empty. So even the first call to ParseNextElement must contain at least 1 non-white-space character
    if((a_statement.substr(indexOfStartOfElement)).length() == 0) {
        return -2;
    }
    
    //since we calculated the index of where the element starts, let's calculate the length of the element
    //length of the element is at least 1 by default since we have guaranteed that it is not white space
    unsigned long lengthOfElement = GetLengthOfElement(a_statement, indexOfStartOfElement);
    
    
    //now, check to see if string, digit, variable, or syntactic character
    if(a_statement[indexOfStartOfElement] == '\"') {
        a_stringValue = a_statement.substr(indexOfStartOfElement, lengthOfElement);
    }
    else if(IsAValidNumber(a_statement.substr(indexOfStartOfElement, lengthOfElement))) {
        a_stringValue = "";
        a_numValue = stod(a_statement.substr(indexOfStartOfElement, lengthOfElement));
    }
    else if(IsAReservedKeyword(a_statement.substr(indexOfStartOfElement, lengthOfElement))) {
        a_stringValue = a_statement.substr(indexOfStartOfElement, lengthOfElement);
    }
    else if (IsAValidVariableName(a_statement.substr(indexOfStartOfElement, lengthOfElement))) {
        a_stringValue = a_statement.substr(indexOfStartOfElement, lengthOfElement);
    }
    else if (IsASyntacticCharacter(a_statement.substr(indexOfStartOfElement, lengthOfElement))) {
        a_stringValue = a_statement.substr(indexOfStartOfElement, lengthOfElement);
    }
    else {
        string message = "'" + a_statement.substr(indexOfStartOfElement, lengthOfElement) + "' is an unrecognized expression.";
        throw DuckInterpreterException(message);
    }
    
    int newNextPos = indexOfStartOfElement + static_cast<int>(lengthOfElement);
    
    //Skip all white space to get the index to the next element in the statement
    while((a_statement.substr(newNextPos)).length() > 0 && (a_statement[newNextPos] == ' ' || a_statement[newNextPos] == '\t')) {
        newNextPos++;
    }
    
    //If there is nothing after the current element
    if((a_statement.substr(newNextPos)).length() == 0) {
        newNextPos = -1;
    }
    
    return newNextPos;
} /** int DuckInterpreter::ParseNextElement(const string &a_statement, int a_nextPos, string &a_stringValue, double &a_numValue) **/



/**/
/*
 unsigned long DuckInterpreter::GetLengthOfElement(const string& a_statement, int a_indexOfStartOfElement)
 
 NAME
    GetLengthOfElement - Returns the length of an element starting at the given index from the given statement
 
 SYNOPSIS
    unsigned long DuckInterpreter::GetLengthOfElement(const string& a_statement, int a_indexOfStartOfElement)
        a_statement --> The statement from which the element's length is to be calculated
        a_indexOfStartOfElement --> The index from which the element starts
        
 DESCRIPTION
    Calculates the length of the element starting at a_indexOfStartOfElement from the given statement.
 
 RETURNS
    The length of the next element
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
unsigned long DuckInterpreter::GetLengthOfElement(const string& a_statement, int a_indexOfStartOfElement) {
    
    //if no element starting at index, return 0;
    if(a_statement.substr(a_indexOfStartOfElement).length() == 0) {
        return 0;
    }
    
    //now, length of element is one by default
    unsigned long lengthOfElement = 1;
    
    //First, check if the element is a string
    if(a_statement[a_indexOfStartOfElement] == '\"') {
        //find the second '"' character marking the end of the string
        size_t indexOfEndOfString = a_statement.find('\"', a_indexOfStartOfElement + 1);
        //if not found, throw error
        if(indexOfEndOfString == string::npos) {
            throw DuckInterpreterException("Bad Syntax. End of string not defined. '\"' character is missing.");
        } else {
            //if found, adjust length of string accordingly
            lengthOfElement = (indexOfEndOfString - a_indexOfStartOfElement) + 1;
        }
        
        //If not a string, check if the element is a syntactic operator (like +, -, ;, ==, etc)
    } else if(IsASyntacticCharacter(a_statement.substr(a_indexOfStartOfElement, 1))) {
        //make sure the current element is not the final element in the statement
        if((a_statement.substr(a_indexOfStartOfElement + 1)).length() > 0) {
            //if it is a single character operator (like (, - , +, =, etc.), then length of element stays one
            //if it is two character long operator (like ==, <=, etc), then increase length by one
            if(IsASyntacticCharacter(a_statement.substr(a_indexOfStartOfElement, 2))) {
                lengthOfElement++;
            }
        }
    }
    
    //If not a syntactic operator or a string, the element must either be a variable, a number, or a keyword (like goto, read, print, etc)
    else {
        //extract the length of the element by looping through and looking to see if the next character is a space or an operator, which would mark the end of the element
        for(int i = a_indexOfStartOfElement; i < a_statement.length(); i++) {
            
            //check if the current element is the final element in the statement
            if((a_statement.substr(i + 1)).length() == 0) {
                break;
            }
            //Now, check for operators that mark the end of an element
            //f we find a match, it means that we have arrived at the end of the element
            if(IsASyntacticCharacter(a_statement.substr(i + 1, 1))) {
                break;
            }
            lengthOfElement++;
        }
    }
    
    return lengthOfElement;
}/** unsigned long DuckInterpreter::GetLengthOfElement(const string& a_statement, int indexOfStartOfElement) **/



/**/
/*
 void DuckInterpreter::EvaluateArithmeticStatement(const string &a_statement)
 
 NAME
    EvaluateArithmeticStatement - Executes the given arithmetic statement, assigning the result of the expression to 
        the right side of "=" to the variable/array at the left side of "="
 
 SYNOPSIS
    void DuckInterpreter::EvaluateArithmeticStatement(const string &a_statement)
        a_statement --> The arithmetic statement to be evaluated
 
 DESCRIPTION
    Executes the arithmetic statement by evaluating the arithmetic expression and assigning the resultant value to the variable/array to the left of "="
 
 RETURNS
    void
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
void DuckInterpreter::EvaluateArithmeticStatement(const string &a_statement)
{
    // Record the variable that we will be assignning a value.
    int nextPos = 0;
    string variableOrArrayName;
    double placeHolder;
    nextPos = ParseNextElement(a_statement, nextPos, variableOrArrayName, placeHolder);
    
    //made sure of this when determining the type of statement
    assert(!variableOrArrayName.empty());
    
    string nextString;
    nextPos = ParseNextElement(a_statement, nextPos, nextString, placeHolder);
    
    //check if array
    bool isAnArray = false;
    double index = -1;
    if(nextString == "[") {
        index = EvaluateArithmeticExpression(a_statement, nextPos);
        isAnArray = true;
        nextPos = ParseNextElement(a_statement, nextPos, nextString, placeHolder);
        assert(nextString == "]");
        nextPos = ParseNextElement(a_statement, nextPos, nextString, placeHolder);
    }
    
    //made sure of this when determining the type of statement
    assert(nextString == "=");
    double result;
    
    //now, evaluate everything to the right of the assignment operator to get the result
    result = EvaluateArithmeticExpression(a_statement, nextPos);
    
    // Record the result into the variable or the array
    if(isAnArray) {
        m_arrayTable.AddToArray(variableOrArrayName, index, result);
    } else {
        m_symbolTable.RecordVariableValue(variableOrArrayName, result);
    }
    
    //check for the correctness of syntax of the remaining statement
    nextPos = ParseNextElement(a_statement, nextPos, nextString, placeHolder);
    if(nextPos >= 0) {
        throw DuckInterpreterException("Extraneous elements at the end of arithmetic statement.");
    }
    
} /** void DuckInterpreter::EvaluateArithmeticStatement(const string &a_statement) **/


/**/
/*
 double DuckInterpreter::EvaluateArithmeticExpression(const string &a_statement, int &a_nextPos)
 
 NAME
    EvaluateArithmeticExpression - Evaluates an arithmetic expression in a statement until it encounters a character that denotes
        the end of an arithmetic expression. 
 
 SYNOPSIS
    double DuckInterpreter::EvaluateArithmeticExpression(const string &a_statement, int &a_nextPos)
        a_statement --> The statement containing the expression to be evaluated
        a_nextPos --> The position at which the expression starts
 
 DESCRIPTION
    Evaluates the arithmetic expression starting at the given index and ending at a character that denotes the end of an arithmetic expression 
    (namely: ',' ';' 'goto' and ']'). Returns the result of the expression. Also, updates the value of a_nextPos to be the position of the first
    element after the end of the expression
 
 RETURNS
    The result of the arithmetic expression
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
double DuckInterpreter::EvaluateArithmeticExpression(const string &a_statement, int &a_nextPos) {
    vector<string> operatorStack;
    vector<double> numberStack;
    string operatorOrVariable;
    double numericOperand;
    double result;
    
    //we want a temporaryNextPos so that a_nextPos can trail behind one step so that we don't lose the index of a comma or a semicolon when we return to the calling
    //function. This is done in case the calling function wants to continue from where this function left off
    int tempNextPos = a_nextPos;
    tempNextPos = ParseNextElement(a_statement, tempNextPos, operatorOrVariable, numericOperand);
    
    result = numericOperand;
    //if, at the beginning, the algorithmic expression is empty, then throw error because no value can be reasonably assigned to the receiving variable
    if(tempNextPos < 0) {
        throw DuckInterpreterException("Arithmetic expression missing.");
    }
    
    //To accomodate for unary operators, we need to keep track of whether the last element in the expression was a number or another operator
    //If the current element is an operator and the last element was also an operator, then the current element is a unary operator (unless the current operator is
    //a ')' character). We use this variable to keep track of this
    string lastElementInExpression = "";
    
    //shunting yard algorithm to determine final value of expression
    //loop until end of arithmetic expression: the conditions in the while loop all mark the end of an arithmetic expression in various types of statements
    while(tempNextPos >= 0 && operatorOrVariable != "," && operatorOrVariable != ";" && operatorOrVariable != "goto" && operatorOrVariable != "]") {
        
        //if the next element is not a number
        if(operatorOrVariable.length() != 0  ) {
            
            //if not an arithmetic operator, check if it is a variable
            if(!IsAnArithmeticOperation(operatorOrVariable)) {
                
                //check to see if variable is valid
                if(!IsAValidVariableName(operatorOrVariable)) {
                    //if the string variable is not a valid variable name, then error
                    throw DuckInterpreterException("Invalid syntax for arithmetic expression.");
                }
                
                //extract value from the variable/array
                numericOperand = ExtractValueFromVariableOrArray(a_statement, tempNextPos, operatorOrVariable);
                
                if(lastElementInExpression == "number") {
                    throw DuckInterpreterException ("Invalid arithmetic expression. Numbers must have an operand between them.");
                }
                //push value to numberStack
                numberStack.push_back(numericOperand);
                lastElementInExpression = "number";
            //if the string is an operator
            } else {
                //handles the operators and the stack for us, as well as updating all the arguments sent to it
                HandleOperator(operatorOrVariable, lastElementInExpression, operatorStack, numberStack);
            }
        //if next element in the expression is a number
        } else {
            if(lastElementInExpression == "number") {
                throw DuckInterpreterException ("Invalid arithmetic expression. Numbers must have an operand between them.");
            }
            numberStack.push_back(numericOperand);
            lastElementInExpression = "number";
        }
        
        a_nextPos = tempNextPos;
        tempNextPos = ParseNextElement(a_statement, tempNextPos, operatorOrVariable, numericOperand);
    }
    
    //finished iterating through the entire arithmetic expression
    //check for possible error
    if(lastElementInExpression == "operator") {
        throw DuckInterpreterException("Invalid arithmetic expression. Arithmetic expressions cannot end with operators.");
    }
    
    //evaluate the rest of the results left in the stack
    while(!operatorStack.empty() && !numberStack.empty()) {
        if(operatorStack.back() == "(") {
            throw DuckInterpreterException("Invalid Arithmetic Expression. Ending parenthesis missing.");
        }
        EvaluateTopOfStack(operatorStack, numberStack);
    }
    
    
    //if there are elements (operands or operator) still left, throw error
    if(numberStack.size() > 1 || !operatorStack.empty()) {
        throw DuckInterpreterException("Invalid Arithmetic Expression");
    }
    
    result = numberStack.back();
    return result;
} /* double DuckInterpreter::EvaluateArithmeticExpression(const string &a_statement, int &a_nextPos) */



/**/
/*
 void DuckInterpreter::HandleOperator(string& a_operator, string& a_lastElement, vector<string>& a_operatorStack, vector<double>& a_numberStack)
 
 NAME
    HandleOperator - Depending on the given operator, this function evaluates the numbers and operators in the stacks, and updates the
        operator stack and number stack accordingly.
 
 SYNOPSIS
    void DuckInterpreter::HandleOperator(string& a_operator, string& a_lastElement, vector<string>& a_operatorStack, vector<double>& a_numberStack)
        a_operator --> The next operator in the arithmetic expression
        a_lastElement --> String to inform whether the last element in the arithmetic expression was a number or operator
        a_operatorStack --> The operator stack being used to evaluate the arithmetic expression
        a_numberStack --> The number stack being used to evaluate the arithmetic expression
 
 DESCRIPTION
    When the ArithmeticExpression function comes across an operator, it call this function to handle the evaluation of the operators and the two stacks
    depending on the type of the operator and the last element in the arithmetic expression. This function also updates the a_lastElement value and the
    two stacks to reflect the effect of the operator
 
 RETURNS
    void
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
void DuckInterpreter::HandleOperator(string& a_operator, string& a_lastElement, vector<string>& a_operatorStack, vector<double>& a_numberStack) {
    //shunting yard algorithm
    if(a_operator == "(") {
        
        //check for possible syntax error
        if(a_lastElement == "number") {
            throw DuckInterpreterException("Invalid arithmetic expression.");
        }
        
        //push into stack
        a_operatorStack.push_back("(");
        a_lastElement = "operator";
    } else if (a_operator == ")") {
        //check for possible syntax error
        if(a_operatorStack.back() == "(") {
            throw DuckInterpreterException("Invalid arithmetic expression. There must be an arithmetic expression inside parentheses.");
        }
        if(a_lastElement != "number") {
            throw DuckInterpreterException("Invalid arithmetic expression.");
        }
        
        //now, pop operator and number stack to evaluate the elements at the top of the stack
        //push evaluated number back into number stack
        //do this until we find the opening bracket
        while(!a_operatorStack.empty() && !a_numberStack.empty() && a_operatorStack.back() != "(") {
            EvaluateTopOfStack(a_operatorStack, a_numberStack);
        }
        //if the opening bracket is not found, throw error
        if(a_operatorStack.empty()) {
            throw DuckInterpreterException("Invalid arithmetic expression. Opening parenthesis missing.");
        }
        //pop opening bracket
        a_operatorStack.pop_back();
        //a pair of parentheses resolve into a number
        a_lastElement = "number";
    } else {
        //check if the current operator is a unary operator by checking if the last element was an operator  or by checking if this is the first element in the
        //arithmetic expression
        if(a_lastElement == "operator" || a_lastElement == "") {
            //if previous operator was also unary, then throw error
            if(!a_operatorStack.empty() && (a_operatorStack.back()).substr(0, 1) == "u") {
                string message = (a_operatorStack.back()).substr(5) + a_operator + " is an invalid operation";
                throw DuckInterpreterException(message);
            }
            
            //check if this operator is +, -, or !. If not, then it is an error
            if(a_operator == "+" || a_operator == "-" || a_operator == "!") {
                //mark as unary
                a_operator = "unary" + a_operator;
            } else {
                throw DuckInterpreterException("Invalid Arithmetic Expression");
            }
        }
        
        //now, pop and evaluate all operators with a precedence greater than or equal to the current operator
        while(!a_operatorStack.empty() && !a_numberStack.empty() && FindPrecedence(a_operator) <= FindPrecedence(a_operatorStack.back())) {
            EvaluateTopOfStack(a_operatorStack, a_numberStack);
        }
        a_operatorStack.push_back(a_operator);
        a_lastElement = "operator";
    }
} /* void DuckInterpreter::HandleOperator(string& a_operator, string& a_lastElement, vector<string>& a_operatorStack, vector<double>& a_numberStack) */


/**/
/*
 double DuckInterpreter::ExtractValueFromVariableOrArray(const string& a_statement, int &a_nextPos, string& arrayOrVariableName)
 
 NAME
    ExtractValueFromVariableOrArray - Takes in a statement, the index of the location of the variable/array, and the variable/array name and
        returns the value found in the variable/array
 
 SYNOPSIS
    double DuckInterpreter::ExtractValueFromVariableOrArray(const string& a_statement, int &a_nextPos, string& arrayOrVariableName)
        a_statement --> The statement containing the variable or the array
        a_nextPos --> The position of element right after the name of the variable (so, in case of an array, it denotes the position of the '[' character)
        arrayOrVariableName --> The name of the variable/array
 
 DESCRIPTION
    This array figures out whether a string denotes a variable or an array and retrieves the value stored in the variable or array. It takes in the statement
    containing the variable, the position at which the name of the variable or array ends, and the name of the variable or array to retrieve the variable/array
 
 RETURNS
    The value stored in the variable or array
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
double DuckInterpreter::ExtractValueFromVariableOrArray(const string& a_statement, int &a_nextPos, string& arrayOrVariableName) {
    
    //the value to be extracted from the variable or array
    double returnValue;
    
    //first, check if the arrayOrVariableName denotes an array by reading the next element in the statement and seeing if it is a "[" character
    string arrayIndexString;
    double placeHolder;
    int tempNextPosForArray = ParseNextElement(a_statement, a_nextPos, arrayIndexString, placeHolder);
    if(arrayIndexString == "[") {
        //evaluate the index
        double index = EvaluateArithmeticExpression(a_statement, tempNextPosForArray);
        //now, get the ']' character
        tempNextPosForArray  = ParseNextElement(a_statement, tempNextPosForArray, arrayIndexString, placeHolder);
        if(arrayIndexString!= "]") {
            throw DuckInterpreterException("Invalid syntax for using arrays. ']' character is missing. ");
        }
        a_nextPos = tempNextPosForArray;
        if(!m_arrayTable.GetValue(arrayOrVariableName, index, returnValue)) {
            //if array value not specified, throw error
            string message = "The value at index " + to_string(static_cast<int>(index)) + " of the array " + arrayOrVariableName + " has not been initialized.";
            throw DuckInterpreterException(message);
        }
        
    }
    //if not an array, it must be a variable
    //if variable has not been assigned a value, then throw error
    else if(!(m_symbolTable.GetVariableValue(arrayOrVariableName, returnValue))) {
        string message = "The variable " + arrayOrVariableName + " has not been assigned a value.";
        throw DuckInterpreterException(message);
    }
    
    return returnValue;
}/* double DuckInterpreter::ExtractValueFromVariableOrArray(const string& a_statement, int &a_nextPos, string& arrayOrVariableName) */


/**/
/*
 int DuckInterpreter::FindPrecedence(string a_op)
 
 NAME
    FindPrecedence - Returns the precedence value of an operator
 
 SYNOPSIS
    int DuckInterpreter::FindPrecedence(string a_op)
        a_op --> The operator whose precedence is to be calculated
 
 DESCRIPTION
    Returns the precedence of an operator. If the operator is invalid, throws an exception
 
 RETURNS
    The value stored in the variable or array
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
int DuckInterpreter::FindPrecedence(string a_op) {
    if(a_op.length() >= 2) {
        if(a_op.substr(0, 2) == "==") {
            return 2;
        }
        if(a_op.substr(0, 2) == ">=") {
            return 2;
        }
        if(a_op.substr(0, 2) == "<=") {
            return 2;
        }
        if(a_op.substr(0, 2) == "!=") {
            return 2;
        }
        if(a_op.substr(0, 6) == "unary+") {
            return 5;
        }
        if(a_op.substr(0, 6) == "unary-") {
            return 5;
        }
        if(a_op.substr(0, 6) == "unary!") {
            return 5;
        }
        
    }
    
    if(a_op.length() == 1) {
        switch(a_op[0]) {
            case '<':
                return 2;
                break;
            case '>':
                return 2;
                break;
            case ')':
                return 1;
                break;
            case '(':
                return 1;
                break;
            case '+':
                return 3;
                break;
            case '-':
                return 3;
                break;
            case '/':
                return 4;
                break;
            case '*':
                return 4;
                break;
            case '[':
                return 0;
                break;
            case ']':
                return 0;
                break;
            default:
                return -1;
                break;
        }
    }
    
    //if the operator is not recognized, throw error
    throw DuckInterpreterException("Invalid operator");
    return -1;
} /* int DuckInterpreter::FindPrecedence(string a_op) */


/**/
/*
 void DuckInterpreter::EvaluateTopOfStack(vector<string>& a_operatorStack, vector<double>& a_numberStack)
 
 NAME
    EvaluateTopOfStack - Evaluates the numbers at the top of the number stack with respect to the operator at 
        the top of the operator stack and pushes the result back into the number stack
 
 SYNOPSIS
    void DuckInterpreter::EvaluateTopOfStack(vector<string>& a_operatorStack, vector<double>& a_numberStack)
        a_operatorStack --> The stack containing the operators 
        a_numberStack --> The stack containing the operands
 
 DESCRIPTION
    This function evaluates the numbers at the top of the number stack using the operator at the top of the 
        operator stack. Then, it pushes the resulting number back into the number stack.
 
 RETURNS
    void
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
void DuckInterpreter::EvaluateTopOfStack(vector<string>& a_operatorStack, vector<double>& a_numberStack) {
    double result;
    if(a_operatorStack.back().substr(0, 5) == "unary") {
        
        //if unary, evaluate only one number per operator
        //throw error if could not evaluate
        if(!EvaluateOperation(a_operatorStack.back(), result, a_numberStack.back())) {
            string message = "Could not evaluate the operator '" + a_operatorStack.back() + "' with the operand " + to_string(a_numberStack.back());
            throw DuckInterpreterException(message);
        }
        a_operatorStack.pop_back();
        a_numberStack.pop_back();
        a_numberStack.push_back(result);
    } else {
        //if binary, evaluate two numbers per operator
        if(!EvaluateOperation(a_operatorStack.back(), result, a_numberStack[a_numberStack.size() - 2], a_numberStack[a_numberStack.size() - 1])){
            string message = "Could not evaluate the operator '" + a_operatorStack.back() + "' with the operands " + to_string(a_numberStack[a_numberStack.size() - 2])+ " and " + to_string(a_numberStack[a_numberStack.size() - 1]);
            throw DuckInterpreterException(message);
        }
        a_operatorStack.pop_back();
        a_numberStack.pop_back();
        a_numberStack.pop_back();
        a_numberStack.push_back(result);
    }
} /* void DuckInterpreter::EvaluateTopOfStack(vector<string>& a_operatorStack, vector<double>& a_numberStack) */


/**/
/*
 bool DuckInterpreter::EvaluateOperation(string a_operation, double& a_result, double a_leftOperand, double a_rightOperand)
 
 NAME
    EvaluateOperation - Evaluates the result of two (or one, in the case of unary operators) numbers with respect to the given operator. 
 
 SYNOPSIS
    bool DuckInterpreter::EvaluateOperation(string a_operation, double& a_result, double a_leftOperand, double a_rightOperand)
        a_operation --> The operation with respect to which the operands are evaluated
        a_result --> The result of the operation
        a_leftOperand --> The operand at the left of the operation
        a_rightOperand --> The operand at the right of the operation. In the case of unary operations, this parameter is defaulted to 0
 
 DESCRIPTION
    Evaluates the operation on the given operand/s. Returns true if the operation was successful, false otherwise
 
 RETURNS
    Returns true if the operation was successful. False otherwise.
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
bool DuckInterpreter::EvaluateOperation(string a_operation, double& a_result, double a_leftOperand, double a_rightOperand) {
    assert(IsAnArithmeticOperation(a_operation));
    //deal with two-character long operations
    if(a_operation.length() >= 2) {
        if(a_operation.substr(0, 2) == "==") {
            a_result = (a_leftOperand == a_rightOperand);
            return true;
        }
        else if(a_operation.substr(0, 2) == ">=") {
            a_result = (a_leftOperand >= a_rightOperand);
            return true;
        }
        else if(a_operation.substr(0, 2) == "<=") {
            a_result = (a_leftOperand <= a_rightOperand);
            return true;
        }
        else if(a_operation.substr(0, 2) == "!=") {
            a_result = (a_leftOperand != a_rightOperand);
            return true;
        } else if(a_operation.substr(0, 6) == "unary-") {
            a_result = -1 * a_leftOperand;
            return true;
        } else if(a_operation.substr(0, 6) == "unary+") {
            a_result = a_leftOperand;
            return true;
        }else if(a_operation.substr(0, 6) == "unary!") {
            if(a_leftOperand != 0) {
                a_result = 0;
            } else {
                a_result = 1;
            }
            return true;
        }else {
            return false;
        }
        
    }
    
    if(a_operation.length() == 1) {
        switch(a_operation[0]) {
            case '!':
                if(a_leftOperand != 0) {
                    a_result = 0;
                } else {
                    a_result = 1;
                }
                return true;
            case '<':
                a_result = (a_leftOperand < a_rightOperand);
                return true;
                break;
            case '>':
                a_result = (a_leftOperand > a_rightOperand);
                return true;
                break;
            case '+':
                a_result = (a_leftOperand + a_rightOperand);
                return true;
                break;
            case '-':
                a_result = (a_leftOperand - a_rightOperand);
                return true;
                break;
            case '/':
                a_result = (a_leftOperand / a_rightOperand);
                return true;
                break;
            case '*':
                a_result = (a_leftOperand * a_rightOperand);
                return true;
                break;
            default:
                return false;
                break;
        }
    }
    return false;
} /* bool DuckInterpreter::EvaluateOperation(string a_operation, double& a_result, double a_leftOperand, double a_rightOperand) */


/**/
/*
 int DuckInterpreter::EvaluateIfStatement(const string& a_statement, int a_nextStatement)
 
 NAME
    EvaluateIfStatement - Evaluate an if statement
 
 SYNOPSIS
    int DuckInterpreter::EvaluateIfStatement(const string& a_statement, int a_nextStatement)
        a_statement --> The if statement to be evaluated
        a_nextStatement --> The line number of this statement
 
 DESCRIPTION
    Evaluated an if statement by seeing if the condition was met. If it was met, then the goto statement is evaluated and the number of the next
        line is calculated. If the condition has not been met, the function returns the value a_nextStatement + 1 (i.e. the number of the line that comes
        right after this line
 
 RETURNS
    The number of the next line to execute/evaluate
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
int DuckInterpreter::EvaluateIfStatement(const string& a_statement, int a_nextStatement) {
    // Get past of the "if" in the if statement.
    int nextPos = 0;
    string resultString;
    double placeHolder;
    nextPos = ParseNextElement(a_statement, nextPos, resultString, placeHolder);
    assert(resultString == "if" );
    
    if(a_statement[nextPos] != '(') {
        throw DuckInterpreterException("If statement conditions must be inclosed by parentheses.");
    }
    
    //get result
    double result = EvaluateArithmeticExpression(a_statement, nextPos);
    
    if (result == 0)
    {
        return a_nextStatement + 1;
    }
    
    int labelLocation = EvaluateGotoStatement(a_statement, nextPos);
    
    return labelLocation;
} /* int DuckInterpreter::EvaluateIfStatement(const string& a_statement, int a_nextStatement) */



/**/
/*
 int DuckInterpreter::EvaluateGotoStatement(const string& a_statement, int a_nextStatement, int a_nextPos)
 
 NAME
    EvaluateGotoStatement - Evaluate a goto statement
 
 SYNOPSIS
    int DuckInterpreter::EvaluateGotoStatement(const string& a_statement, int a_nextStatement, int a_nextPos)
        a_statement --> The goto statement to be evaluated
        a_nextPos --> The index at which the goto statement starts (because goto statments can start in the second half of an if statement). a_nextPos has a
            default value of 0
 
 DESCRIPTION
    Evaluates a goto statement and returns the line number of the line that the label in the goto statement corresponds to.
 
 RETURNS
    The line number to jump to
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
int DuckInterpreter::EvaluateGotoStatement(const string& a_statement, int a_nextPos) {
    string resultString;
    double placeHolder;
    a_nextPos = ParseNextElement(a_statement, a_nextPos, resultString, placeHolder);
    if(resultString != "goto") {
        throw DuckInterpreterException("Invalid statement. If statement must have a goto.");
    }
    
    //extract label
    a_nextPos = ParseNextElement(a_statement, a_nextPos, resultString, placeHolder);
    
    //verify it is a variable name
    if(!IsAValidVariableName(resultString)) {
        throw DuckInterpreterException("Goto statement must be followed by a label");
    }
    
    int labelLocation;
    
    //throw an error if label does not exist
    labelLocation = m_statements.GetLabelLocation(resultString);
    
    //check the rest of the statement for errors
    a_nextPos = ParseNextElement(a_statement, a_nextPos, resultString, placeHolder);
    if(a_nextPos >= 0) {
        throw DuckInterpreterException("Extraneous elements at the end of an if statement.");
    }
    
    return labelLocation;
} /* int DuckInterpreter::EvaluateGotoStatement(const string& a_statement, int a_nextStatement, int a_nextPos) */


/**/
/*
 void DuckInterpreter::ExecuteStopStatement(const string& a_statement)
 
 NAME
 ExecuteStopStatement - Evaluates a stop statement
 
 SYNOPSIS
    void DuckInterpreter::ExecuteStopStatement(const string& a_statement)
        a_statement --> The stop statement to be evaluated
 
 DESCRIPTION
    Evaluates and executes a stop statement if it has correct syntax. This is the only function that causes a program without
        any error to end
 
 RETURNS
    void
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
void DuckInterpreter::ExecuteStopStatement(const string& a_statement) {
    //make sure that "stop;" is the only thing written in the statement
    double placeHolder;
    string nextElement;
    int nextPos = ParseNextElement(a_statement, 0, nextElement, placeHolder);
    if(nextElement != "stop") {
        string message = nextElement + " is undefined in the context of a stop statement";
        throw DuckInterpreterException(message);
    }
    nextPos = ParseNextElement(a_statement, nextPos, nextElement, placeHolder);
    if(nextElement != ";" || nextPos >= 0) {
        throw DuckInterpreterException("Invalid stop statement");
    }
    exit(0);
} /* void DuckInterpreter::ExecuteStopStatement(const string& a_statement) */


/**/
/*
 void DuckInterpreter::EvaluateEndStatement(const string& a_statement)
 
 NAME
    EvaluateEndStatement - Evaluates a stop statement
 
 SYNOPSIS
    void DuckInterpreter::EvaluateEndStatement(const string& a_statement)
        a_statement --> The end statement to be evaluated
 
 DESCRIPTION
    Evaluates an end statement if it has correct syntax. Sets the endFlag to true so that if the interpreter tries to execute another line after this
    line, an error is thrown
 
 RETURNS
    void
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
void DuckInterpreter::EvaluateEndStatement(const string& a_statement) {
    //make sure that "end;" is the only thing written in the statement
    double placeHolder;
    string nextElement;
    int nextPos = ParseNextElement(a_statement, 0, nextElement, placeHolder);
    if(nextElement != "end") {
        string message = nextElement + " is undefined in the context of an end statement";
        throw DuckInterpreterException(message);
    }
    nextPos = ParseNextElement(a_statement, nextPos, nextElement, placeHolder);
    if(nextElement != ";" || nextPos >= 0) {
        throw DuckInterpreterException("Invalid end statement");
    }
    m_endFlag = true;
} /* void DuckInterpreter::EvaluateEndStatement(const string& a_statement) */



/**/
/*
 void DuckInterpreter::ExecutePrintStatement(const string &a_statement)
 
 NAME
    EvaluateEndStatement - Evaluates a print statement
 
 SYNOPSIS
    void DuckInterpreter::ExecutePrintStatement(const string &a_statement)
        a_statement --> The print statement to be evaluated and executed
 
 DESCRIPTION
    Executes a print statement. It prints string elements, variables, numbers, and array elements alike. It also evaluates any arithmetic expression in the
        statement and prints the result.
 
 RETURNS
    void
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
void DuckInterpreter::ExecutePrintStatement(const string &a_statement) {
    int nextPos = 0;
    string nextString;
    double nextNumber;
    
    
    nextPos = ParseNextElement(a_statement, nextPos, nextString, nextNumber);
    //make sure first element is print
    if(nextString != "print") {
        throw DuckInterpreterException("Invalid print statement.");
    }
    
    //nextPos stays one step behind tempNextPos so that we can send nextPos as an argument to EvaluateArithmeticExpression
    int tempNextPos = nextPos;
    do {
        nextPos = tempNextPos;
        tempNextPos = ParseNextElement(a_statement, tempNextPos, nextString, nextNumber);
        
        //if what follows after the print statement (or a comma) is not a string, variable, operator, or number, then throw error
        if(nextString[0] != '\"' && !IsAValidVariableName(nextString) && !IsAnArithmeticOperation(nextString) && nextString != "") {
            throw DuckInterpreterException("Invalid print statement.");
        }
        //if string, remove quotation marks and print
        if(nextString[0] == '\"') {
            cout << nextString.substr(1, nextString.length() - 2);
        }
        //if number, variable, or array, evaluate till the end of arithmetic expression
        else {
            cout << EvaluateArithmeticExpression(a_statement, nextPos);
            tempNextPos = nextPos;
        }
        tempNextPos = ParseNextElement(a_statement, tempNextPos, nextString, nextNumber);
        
    } while(nextString == ",");
    cout << endl;
    
    if(tempNextPos >= 0) {
        throw DuckInterpreterException("Extraneous elements at the end of print statement.");
    }
} /* void DuckInterpreter::ExecutePrintStatement(const string &a_statement) */


/**/
/*
 void DuckInterpreter::ExecuteReadStatement(const string& a_statement)
 
 NAME
    ExecuteReadStatement - Evaluates a read statement
 
 SYNOPSIS
    void DuckInterpreter::ExecuteReadStatement(const string& a_statement)
        a_statement --> The print statement to be evaluated and executed
 
 DESCRIPTION
    It evaluates the read statement and asks user for input/s. It stores the input/s into the variables and arrays listed in the statement
 
 RETURNS
    void
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
void DuckInterpreter::ExecuteReadStatement(const string& a_statement) {
    int nextPos = 0;
    string nextString;
    double nextNumber;
    
    nextPos = ParseNextElement(a_statement, nextPos, nextString, nextNumber);
    //make sure its a read statement
    
    assert(nextString == "read");
    
    nextPos = ParseNextElement(a_statement, nextPos, nextString, nextNumber);
    
    //print prompt
    if(nextString[0] == '\"') {
        cout << nextString.substr(1, nextString.length() - 2);
        nextPos = ParseNextElement(a_statement, nextPos, nextString, nextNumber);
        if(nextString != ",") {
            throw DuckInterpreterException("Need comma separation in read statement");
        }
        nextPos = ParseNextElement(a_statement, nextPos, nextString, nextNumber);
    }
    string inputValue;
    //go through all the variables
    while(IsAValidVariableName(nextString) || nextString == ",") {
        getline(cin, inputValue);
        //remove white space from beginning and end of input
        RemoveExtraneousWhiteSpace(inputValue);
        if(inputValue.length() == 0) {
            continue;
        }
        if(!IsAValidNumber(inputValue)) {
            throw DuckInterpreterException("Input value is not a valid number");
        }
        
        //check if variable is an array
        string tempString;
        int arrayNextPos = ParseNextElement(a_statement, nextPos, tempString, nextNumber);
        if(tempString == "[") {
            double index = EvaluateArithmeticExpression(a_statement, arrayNextPos);
            //add to input value to array
            m_arrayTable.AddToArray(nextString, index, stof(inputValue));
            //now, point nextPos to first the element after the array
            arrayNextPos = ParseNextElement(a_statement, arrayNextPos, tempString, nextNumber);
            nextPos = arrayNextPos;
        }
        //if not an array, it must be a variable
        else {
            m_symbolTable.RecordVariableValue(nextString, stof(inputValue));
        }
        
        
        nextPos = ParseNextElement(a_statement, nextPos, nextString, nextNumber);
        //skip comma
        if(nextString == ",") {
            nextPos = ParseNextElement(a_statement, nextPos, nextString, nextNumber);
        }
    }
    //if the final element is not a semicolon or if there are extraneous elements at the end of the read statement, then throw eror
    if(nextString != ";") {
        throw DuckInterpreterException("Invalid syntax for read statement");
    }
    
    if(nextPos >= 0) {
        throw DuckInterpreterException("Extraneous elements at the end of read statement.");
    }
} /* void DuckInterpreter::ExecuteReadStatement(const string& a_statement) */



/**/
/*
 void DuckInterpreter::EvaluateArrayDeclarationStatement(const string& a_statement)
 
 NAME
    EvaluateArrayDeclarationStatement - Evaluates an array declaration statement
 
 SYNOPSIS
    void DuckInterpreter::EvaluateArrayDeclarationStatement(const string& a_statement)
        a_statement --> The array declaration statement to be evaluated
 
 DESCRIPTION
    It evaluates the array declaration statement. If the array and size specified are valid, then it adds a new array to m_arrayTable with the specified size
 
 RETURNS
    void
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
void DuckInterpreter::EvaluateArrayDeclarationStatement(const string& a_statement) {
    int nextPos = 0;
    string nextString;
    double nextNumber;
    
    nextPos = ParseNextElement(a_statement, nextPos, nextString, nextNumber);
    assert(nextString == "dim");
    
    nextPos = ParseNextElement(a_statement, nextPos, nextString, nextNumber);
    
    //check if the array name is valid
    if(!IsAValidVariableName(nextString)) {
        string message = "'" + nextString + "' is an invalid array name.";
        throw DuckInterpreterException(message);
    }
    //record arrayName
    string arrayName = nextString;
    
    nextPos = ParseNextElement(a_statement, nextPos, nextString, nextNumber);
    if(nextString != "[") {
        throw DuckInterpreterException("Array variable name must be of the form: 'dim <array name>[<array size>]'.");
    }
    
    int arraySize = EvaluateArithmeticExpression(a_statement, nextPos);
    
    m_arrayTable.AddNewArray(arrayName, arraySize);
   
    //check for the correctness of the syntax of the rest of the statement
    nextPos = ParseNextElement(a_statement, nextPos, nextString, nextNumber);
    if(nextString != "]") {
        throw DuckInterpreterException("Array variable name must be of the form: 'dim <array name>[<array size>]'.");
    }
    
    nextPos = ParseNextElement(a_statement, nextPos, nextString, nextNumber);
    if(nextString != ";" || nextPos >= 0) {
        throw DuckInterpreterException("Incorrect syntax for array declaration statement.");
    }
} /* void DuckInterpreter::EvaluateArrayDeclarationStatement(const string& a_statement) */




/**/
/*
 DuckInterpreter::StatementType DuckInterpreter::GetStatementType(const string &a_statement)
 
 NAME
    GetStatementType - Evaluates the type of the given statement
 
 SYNOPSIS
    DuckInterpreter::StatementType DuckInterpreter::GetStatementType(const string &a_statement)
        a_statement --> The statement whose type is to be evaluated
 
 DESCRIPTION
    It evaluates the type of the given statement and returns a StatementType value accordingly
 
 RETURNS
    The type of the given statement
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
DuckInterpreter::StatementType DuckInterpreter::GetStatementType(const string &a_statement) {
    
    //checking for read statement
    if(a_statement.substr(0, 4) == "read") {
        return StatementType::ReadStat;
    }
    if(a_statement.substr(0, 2) == "if") {
        return StatementType::IfStat;
    }
    
    if(a_statement.substr(0, 5) == "print") {
        return StatementType::PrintStat;
    }
    
    if(a_statement.substr(0, 4) == "stop") {
        return StatementType::StopStat;
    }
    
    if(a_statement.substr(0, 3) == "end") {
        return StatementType::EndStat;
    }
    
    if(a_statement.substr(0, 4) == "goto") {
        return StatementType::gotoStat;
    }
    
    if(a_statement.substr(0, 3) == "dim") {
        return StatementType::arrayDeclarationStat;
    }
    
    //checking for arithmetic statement
    
    //first, check for presence of '=' character
    size_t positionOfEqualOperator = a_statement.find("=");
    if(positionOfEqualOperator != string::npos) {
        string variableName = a_statement.substr(0, positionOfEqualOperator);
        //check to see if the assignment operator is preceded by quotations.
        if(variableName.find('\"') != string::npos) {
            //It doesn't make sense for a non-Print statement and a non-Read statement to have a string
            //It must be a statement with bad syntax
            throw DuckInterpreterException("Invalid syntax.");
        }
        int nextPos = 0;
        string nextString;
        double nextNumber;
        nextPos = ParseNextElement(a_statement, nextPos, nextString, nextNumber);
        
        if(!IsAValidVariableName(nextString)) {
            throw DuckInterpreterException("Invalid arithmetic statement. No proper variable or array to assign value to.");
        }
        
        nextPos = ParseNextElement(a_statement, nextPos, nextString, nextNumber);
        
        //check to see if the variable is an array
        if(nextString == "[") {
            nextPos = ParseNextElement(a_statement, nextPos, nextString, nextNumber);
            //if there is nothing in between [], throw error
            if(nextString == "]") {
                throw DuckInterpreterException("Array index unspecified");
            }
            //search for closing bracket
            while(nextString != "]" && nextPos != -1) {
                nextPos = ParseNextElement(a_statement, nextPos, nextString, nextNumber);
            }
            if(nextPos < 0) {
                if(nextString != "]") {
                    throw DuckInterpreterException("Missing ']' character to end array expression.");
                }
                throw DuckInterpreterException("Invalid arithmetic statement.");
            }
            nextPos = ParseNextElement(a_statement, nextPos, nextString, nextNumber);
        }
        
        //if there are more characters after the variable or array but before the '=' operator, throw error
        if(nextString != "=") {
            string message = a_statement.substr(0, positionOfEqualOperator) + " is an invalid expression to which to assign a value.";
            throw DuckInterpreterException(message);
        }
        return StatementType::ArithmeticStat;
    }
    throw DuckInterpreterException("Unable to determine statement type");
} /* DuckInterpreter::StatementType DuckInterpreter::GetStatementType(const string &a_statement) */



/**/
/*
 bool DuckInterpreter::IsAValidVariableName(string a_variableName)
 
 NAME
    IsAValidVariableName - Checks whether the given string is a valid variable name
 
 SYNOPSIS
    bool DuckInterpreter::IsAValidVariableName(string a_variableName)
        a_variableName --> The string to be checked for validity
 
 DESCRIPTION
    This function checks whether the given string is a valid variable name according the variable naming rules of the Duck language. It checks whether the
        first character of the string is a an alphabet, or an underscore, and whether the remaining characters are alphabets, digits, or underscores
 
 RETURNS
    True if the string is a valid variable name. False if not
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
bool DuckInterpreter::IsAValidVariableName(string a_variableName) {
    //return if empty variable name
    if(a_variableName.length() == 0) {
        return false;
    }
    
    
    //reserved keywords are not allowed to be variable names
    if(IsAReservedKeyword(a_variableName)) {
        return false;
    }
    
    //now, check to see if the first character of the label is not an "_" or a letter
    if(a_variableName[0] != '_') {
        if(a_variableName[0] < 'a' || a_variableName[0] > 'z') {
            if(a_variableName[0] < 'A' || a_variableName[0] > 'Z') {
                return false;
            }
        }
    }
    
    //now, check to see if the rest of the characters of the label is not an "_", a letter, or a number
    for(int i = 1; i < a_variableName.length(); i++) {
        if(a_variableName[i] != '_') {
            if(a_variableName[i] < 'a' || a_variableName[i] > 'z') {
                if(a_variableName[i] < 'A' || a_variableName[i] > 'Z') {
                    if(a_variableName[i] < '0' || a_variableName[i] > '9') {
                        return false;
                    }
                }
            }
        }
    }
    
    return true;
} /* bool DuckInterpreter::IsAValidVariableName(string a_variableName) */


/**/
/*
 bool DuckInterpreter::IsAValidNumber(string a_number)
 
 NAME
    IsAValidNumber - Checks whether the given string is a valid number
 
 SYNOPSIS
    bool DuckInterpreter::IsAValidNumber(string a_number)
        a_number --> The string to be checked for validity
 
 DESCRIPTION
    This function checks whether the given string is a valid number.
 
 RETURNS
    True if the string is a valid number. False if not
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
bool DuckInterpreter::IsAValidNumber(string a_number) {
    if(a_number.length() == 0) {
        return false;
    }
    
    //used to count number of decimal points. If more than one, then invalid number
    int dotCounter = 0;
    
    
    //now, check whether the first character in the number is a digit or a sign (+ or -)
    if((a_number[0] < '0' || a_number[0] > '9') && a_number[0] != '-' && a_number[0] != '+') {
        if(a_number[0] == '.') {
            dotCounter++;
        } else {
            return false;
        }
    }
    
    //if the first digit of the string is a + or - character, but if there are no digits following them, then it is not a number
    if((a_number[0] == '-' || a_number[0] == '+') && (a_number.length() == 1)) {
        return false;
    }
    
    //now, check to see if the rest of the characters are digits
    for(int i = 1; i < a_number.length(); i++) {
        if(a_number[i] < '0' || a_number[i] > '9') {
            //allow one decimal dot
            if(a_number[i] == '.' && dotCounter == 0) {
                dotCounter++;
            } else {
                return false;
            }
            
        }
    }
    return true;
} /* bool DuckInterpreter::IsAValidNumber(string a_number) */



/**/
/*
 bool DuckInterpreter::IsAnArithmeticOperation(string a_stringToCheck)
 
 NAME
    IsAnArithmeticOperation - Checks whether the given string is an arithmetic operation
 
 SYNOPSIS
    bool DuckInterpreter::IsAnArithmeticOperation(string a_stringToCheck)
        a_stringToCheck --> The string to be checked for being an arithmetic operation
 
 DESCRIPTION
    This function checks whether the given string is an arithmetic operation (such as ==, -, +, /, !, !=, <, etc)
 
 RETURNS
    True if the string is a valid arithmetic operation. False if not
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
bool DuckInterpreter::IsAnArithmeticOperation(string a_stringToCheck) {
    
    //First, check for >=, <=, ==, != (i.e. all operators that are 2 characters long
    if(a_stringToCheck.length() >= 2) {
        if(a_stringToCheck.substr(0, 2) == "==") {
            return true;
        }
        if(a_stringToCheck.substr(0, 2) == ">=") {
            return true;
        }
        if(a_stringToCheck.substr(0, 2) == "<=") {
            return true;
        }
        if(a_stringToCheck.substr(0, 2) == "!=") {
            return true;
        }
        if(a_stringToCheck.substr(0, 6) == "unary+") {
            return true;
        }
        if(a_stringToCheck.substr(0, 6) == "unary-") {
            return true;
        }
        if(a_stringToCheck.substr(0, 6) == "unary!") {
            return true;
        }
    }
    
    //Now, check for 1 character long operators that mark the end of an element (namely: ' ', '=', '!', '<', '>', '"', ',', ';', '(', ')',
    // '+', '-', '*', '/', '[' , and ']')
    if(a_stringToCheck.length() == 1) {
        switch(a_stringToCheck[0]) {
            case '<':
            case '>':
            case '!':
            case ')':
            case '(':
            case '+':
            case '-':
            case '/':
            case '*':
            
                return true;
                break;
            default:
                return false;
                break;
        }
    }

    return false;
    
} /* bool DuckInterpreter::IsAnArithmeticOperation(string a_stringToCheck)  */


/**/
/*
 bool DuckInterpreter::IsASyntacticCharacter(const string& a_stringToCheck)
 
 NAME
    IsASyntacticCharacter - Checks whether the given string is a syntactic character
 
 SYNOPSIS
    bool DuckInterpreter::IsASyntacticCharacter(const string& a_stringToCheck)
        a_stringToCheck --> The string to be checked for being a syntactic character
 
 DESCRIPTION
    This function checks whether the given string is an syntactic character (such as ' ', ';', '+', '=', etc)
 
 RETURNS
    True if the string is a valid syntactic character. False if not
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
bool DuckInterpreter::IsASyntacticCharacter(const string& a_stringToCheck) {
    
    ////First, check if it is an arithmetic operator
    if(IsAnArithmeticOperation(a_stringToCheck)) {
        return true;
    }
    
    //Now, check for other 1 character long characters that mark the end of an element (namely: ' ', '=', '"', ',', ';')
    if(a_stringToCheck.length() == 1) {
        switch(a_stringToCheck[0]) {
            case ' ':
            case '=':
            case ';':
            case ',':
            case '\"':
            case '[':
            case ']':
            case '\t':
                return true;
                break;
            default:
                return false;
                break;
        }
    }
    return false;
} /* bool DuckInterpreter::IsASyntacticCharacter(const string& a_stringToCheck) */

/**/
/*
 bool DuckInterpreter::IsAReservedKeyword(string a_stringToCheck)
 
 NAME
    IsAReservedKeyword - Checks whether the given string is a reserved keyword
 
 SYNOPSIS
    bool DuckInterpreter::IsAReservedKeyword(string a_stringToCheck)
        a_stringToCheck --> The string to be checked for being a reserved keyword
 
 DESCRIPTION
    This function checks whether the given string is a reserved keyword (such as "goto", "print", "read", "dim", etc)
 
 RETURNS
    True if the string is a reserved keyword. False if not.
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
bool DuckInterpreter::IsAReservedKeyword(string a_stringToCheck) {
    if(a_stringToCheck == "dim") {
        return true;
    }
    if(a_stringToCheck == "read") {
        return true;
    }
    
    if(a_stringToCheck == "goto") {
        return true;
    }
    
    if(a_stringToCheck == "if") {
        return true;
    }
    
    if(a_stringToCheck == "print") {
        return true;
    }
    
    if(a_stringToCheck == "stop") {
        return true;
    }
    
    if(a_stringToCheck == "end") {
        return true;
    }
    
    return false;
} /* bool DuckInterpreter::IsAReservedKeyword(string a_stringToCheck) */


/**/
/*
 void DuckInterpreter::RemoveExtraneousWhiteSpace(string& a_string)
 
 NAME
    RemoveExtraneousWhiteSpace - removes extraneous white space from the beginning and the end of a string
 
 SYNOPSIS
    void DuckInterpreter::RemoveExtraneousWhiteSpace(string& a_string)
        a_string --> The string from which the white space is to be removed
 
 DESCRIPTION
    This function removes extraneous white space from the beginning and the end of a string
 
 RETURNS
    void
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
void DuckInterpreter::RemoveExtraneousWhiteSpace(string& a_string) {
    //now, remove extraneous white space from the beginning of a_string
    while(a_string.length() != 0 && ((a_string[0] == ' ') || a_string[0] == '\t')) {
        a_string = a_string.substr(1);
    }
    //now, remove extraneous white space from the end of a_string
    while(a_string.length() != 0 && (a_string[a_string.length() - 1] == ' ' || a_string[a_string.length() - 1] == '\t')) {
        a_string = a_string.substr(0, a_string.length() - 1);
    }
} /* void DuckInterpreter::RemoveExtraneousWhiteSpace(string& a_string) */

