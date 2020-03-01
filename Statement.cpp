#include "Statement.h"

using namespace std;


/**/
/*
 bool Statement::ValidLabelName(string a_labelName)
 
 NAME
    Statement::ValidLabelName - Checks if the given string is a valid candidate for a label name
 
 SYNOPSIS
    bool Statement::ValidLabelName(string a_labelName)
        a_labelName --> The string to be checked
 
 DESCRIPTION
    If the given string is a valid candidate for a label name and if the string is not a keyword associated with
    the duck language (like dim, read, print), then the function returns true. Else, it returns false
 
 RETURNS
    True if valid label name. False otherwise.
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
bool Statement::ValidLabelName(string a_labelName) {
    //return if empty labelName
    if(a_labelName.length() == 0) {
        return false;
    }
    
    //now, check to see if the first character of the label is not an "_" or a letter
    if(a_labelName[0] != '_') {
        if(a_labelName[0] < 'a' || a_labelName[0] > 'z') {
            if(a_labelName[0] < 'A' || a_labelName[0] > 'Z') {
                return false;
            }
        }
    }
    
    
    //now, check to see if the rest of the characters of the label is not an "_", a letter, or a number
    for(int i = 1; i < a_labelName.length(); i++) {
        if(a_labelName[i] != '_') {
            if(a_labelName[i] < 'a' || a_labelName[i] > 'z') {
                if(a_labelName[i] < 'A' || a_labelName[i] > 'Z') {
                    if(a_labelName[i] < '0' || a_labelName[i] > '9') {
                        return false;
                    }
                }
            }
        }
    }
    
    //check if label name is a reserved keyword
    
    if(a_labelName == "dim") {
        return false;
    }
    if(a_labelName == "read") {
        return false;
    }
    
    if(a_labelName == "goto") {
        return false;
    }
    
    if(a_labelName == "if") {
        return false;
    }
    
    if(a_labelName == "print") {
        return false;
    }
    
    if(a_labelName == "stop") {
        return false;
    }
    
    if(a_labelName == "end") {
        return false;
    }

    return true;
}/** bool Statement::ValidLabelName(string a_labelName) **/



/**/
/*
 void Statement::RecordStatements(string a_sourceFileName)
 
 NAME
 Statement::RecordStatements - Records all the statements from a given file so that they can be executed by the DuckInterpreter
 
 SYNOPSIS
    void Statement::RecordStatements(string a_sourceFileName)
        a_sourceFileName --> The name of the file from which the statements are to be taken to be recorded
 
 DESCRIPTION
    This function stores all the statements in the given source file. When storing the statments, it removes all comments and extracts
    and records all the labels separately. The labels are stored along with the line number they correspond to. Furthermore, this function
    also sees if there is at least one stop statement in the Duck program source file. Can throw multiple exceptions depending on validity of 
    labels or the absence of stop statements.
 
 RETURNS
    void
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
void Statement::RecordStatements(string a_sourceFileName) {
	ifstream inf(a_sourceFileName);
	if (!inf)
	{
        cerr << "Could not open the file: " << a_sourceFileName << endl;
		exit(1);
	}
    
    //To find errors caused by a lack of stop statements in the program
    int stopStatementCounter = 0;
    
	// Read through the file one line at a time.  Find labels and record them.
    string nextLine = "";
    string labelName = "";
    while(getline(inf, nextLine)) {
        RemoveComments(nextLine);
        RemoveExtraneousWhiteSpace(nextLine);
        
        //ignore empty line
        if(nextLine.length() == 0) {
            continue;
        }
        
        //Check for label and extract label
        ExtractLabel(nextLine, labelName);
        
        //now, remove extraneous white space from the beginning of nextLine
        RemoveExtraneousWhiteSpace(nextLine);
        
        //check for possible syntax error
        if(nextLine.length() == 0) {
            cerr <<  "Syntax Error: Line " << m_statements.size() << ": Labeled line has no adjoining statement." << endl;
            exit(1);
        }
        
        //check if the statement is a stop statement
        if(nextLine.substr(0, 4) == "stop") {
            stopStatementCounter++;
        }
        
        //add statement to m_statements vector
        m_statements.push_back(nextLine);
        
        //add corresponding label to labelToStatement vector (if label exists) and if label is not duplicate
        if(labelName.length() != 0) {
            
            //checking for duplicate
            if(m_labelToStatement.count(labelName) > 0) {
                map<string, int>::iterator it;
                it = m_labelToStatement.find(labelName);
                cerr << "Syntax Error: Lines " << it->second << " and " << m_statements.size() - 1<< ": Multiple labels with the same name" << endl;
                exit(1);
            }
            m_labelToStatement.insert(pair<string, int>(labelName, m_statements.size() - 1));
        }
    }
    
    if(stopStatementCounter == 0) {
        cerr << "Program Error: Missing stop statements." << endl;
        exit(1);
    }
    
    inf.close();
}/** void Statement::RecordStatements(string a_sourceFileName) **/


/**/
/*
 void Statement::ExtractLabel(string& a_statement, string &a_labelName)
 
 NAME
    Statement::ExtractLabel - Extracts a label from a given statement, stores the label, and removes the label from the original statement
 
 SYNOPSIS
    void Statement::ExtractLabel(string& a_statement, string &a_labelName)
    a_statement --> The statement from which to extract and remove the label
    a_labelName --> The label that is extracted from the statement
 
 DESCRIPTION
    This function extracts the label from a given statement, stores the label, and removes the label from the original statement.
    It also cleans up the label of any extraneous white space.
 
 RETURNS
    void
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
void Statement::ExtractLabel(string& a_statement, string &a_labelName) {
    size_t labelEndIndex = 0;
    //Check for label and extract label
    labelEndIndex = a_statement.find(':');
    //if the statement contains a ':' character that is not within an output string
    if(labelEndIndex != string::npos && (a_statement.substr(0, labelEndIndex)).find('\"') == string::npos) {
        //isolate label name
        a_labelName = a_statement.substr(0, labelEndIndex);
        //remove label from statement
        a_statement = a_statement.substr(labelEndIndex + 1);
        RemoveExtraneousWhiteSpace(a_labelName);
        if(!ValidLabelName(a_labelName)) {
            cerr << "Error: Line " << m_statements.size() << ": '" << a_labelName + "' is an invalid label name." << endl;
            exit(1);
        };
    } else {
        a_labelName = "";
    }
}/** void Statement::ExtractLabel(string& a_statement, string &a_labelName) **/


/**/
/*
 int Statement::GetLabelLocation(string a_string)
 
 NAME
    Statement::GetLabelLocation - Retrieves the line that the given label corresponds to
 
 SYNOPSIS
    int Statement::GetLabelLocation(string a_string)
        a_labelName --> The name of the label
 
 DESCRIPTION
    This function returns the line number that the given label corresponds to. If the label does not exist, it throws an error
 
 RETURNS
    The line number that the label corresponds to
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
int Statement::GetLabelLocation(string a_labelName) {
    if(m_labelToStatement.count(a_labelName) == 0) {
        string message = "'" + a_labelName + "' is an undefined label";
        throw DuckInterpreterException(message);
    }
    return m_labelToStatement[a_labelName];
} /** int Statement::GetLabelLocation(string a_string) **/



/**/
/*
 void Statement::RemoveExtraneousWhiteSpace(string& a_string)
 
 NAME
    Statement::RemoveExtraneousWhiteSpace - Removes all extraneous white space from the beginning and the end of the given string
 
 SYNOPSIS
    void Statement::RemoveExtraneousWhiteSpace(string& a_string)
        a_string --> The string from which extraneous white space is to be removed
 
 DESCRIPTION
    Removes all extraneous white space from the beginning and the end of the given string. This includes spaces as well as tab spaces
 
 RETURNS
    void
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
void Statement::RemoveExtraneousWhiteSpace(string& a_string) {
    //remove extraneous white space from the beginning of nextLine
    while(a_string.length() != 0 && (a_string[0] == ' ' || a_string[0] == '\t')) {
        a_string = a_string.substr(1);
    }
    //now, remove extraneous white space from the end of nextLine
    while(a_string.length() != 0 && (a_string[a_string.length() - 1] == ' ' || a_string[a_string.length() - 1] == '\t')) {
        a_string = a_string.substr(0, a_string.length() - 1);
    }
}/** void Statement::RemoveExtraneousWhiteSpace(string& a_string) **/



/**/
/*
 void Statement::RemoveComments(string& a_string)
 
 NAME
    Statement::RemoveComments - Removes all comments the given statement
 
 SYNOPSIS
    void Statement::RemoveComments(string& a_string)
        a_statement --> The statement from which to remove comments
 
 DESCRIPTION
    Removes the comments from a given statement. If the statement is entirely a comment, then the statement's value is assigned to an empty string
    Ignores comment characters that occur within a string
 
 RETURNS
    void
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
void Statement::RemoveComments(string& a_statement) {
    //if comment characters found
    if(a_statement.find("//") != string::npos) {
        size_t commentIndex = a_statement.find("//");
        //if comment starts at the beginning of the line, then empty the whole line
        if(commentIndex == 0) {
            a_statement = "";
            return;
        }
        
        //if comment starts anywhere else then search further:

        // here, quotationCharacterCounter is used to find out whether the comment characters occur within quotation marks
        int quotationCharacterCounter = 0;
        
        //repeat process until no more commment characters are found or until end of string
        while(commentIndex != string::npos && commentIndex < a_statement.length() - 1) {
            //make sure the comment characters are not part of a string
            //count the number of quotation marks that appear before the comment characters
            for(int i = 0; i < commentIndex; i++) {
                if(a_statement[i] == '\"') {
                    quotationCharacterCounter++;
                }
            }
            //if the comment characters are not part of a string (i.e. if there are an even number of quotation characters
            //before the comment characters,, then a comment has been found and we can remove it
            if((quotationCharacterCounter % 2) == 0) {
                a_statement = a_statement.substr(0, commentIndex);
                return;
            } else {
                //if the comment characters are part of a string, look for other comment characters later in the line
                commentIndex = a_statement.find("//", commentIndex + 2);
            }
        }
    }
} /** void Statement::RemoveComments(string& a_string) **/


