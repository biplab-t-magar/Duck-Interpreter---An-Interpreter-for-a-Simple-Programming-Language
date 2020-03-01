/*Statement.h
 Statement.h contains the Statement class
 Statement is a class that represents the list of all lines of code in a Duck language source file. It is utilized by 
 the DuckInterpreter program to store all the lines of code from a Duck language source file and retreive statements in the given
 Duck code so that these statements can be executed by the DuckInterpreter class. Statement also stores all the labels in a Duck program
 source file and matches them to corresponding lines of code.
*/

#pragma once

#include<string>
#include<iostream>
#include <map>
#include <fstream>
#include <vector>
#include "DuckInterpreterException.h"

class Statement
{
public:
    Statement(){}
    ~Statement(){}

    //This function stores all the statements in the Duck language source file
    void RecordStatements(std::string a_sourceFileName);
    
    
    /**/
    /*
     std::string GetStatement(int a_statementNum)
     
    NAME
        GetStatement - Records the statements that are in the specified file.
     
    SYNOPSIS
        std::string GetStatement(int a_statementNum)
            a_statementNum --> The line number of the statement to be returned
     
    DESCRIPTION
        This function retreives the statement of the Duck program that corresponds to the given line
     
    RETURNS
        The statement that corresponds to the given line
     
    AUTHOR
        Biplab Thapa Magar
     */
    /**/
    std::string GetStatement(int a_statementNum) {
        if(a_statementNum >= m_statements.size()) {
            throw "Out of Lines.";
        }
        return m_statements[a_statementNum];
	}/*std::string GetStatement(int a_statementNum)*/
    
    //Returns the line that a label is correspondent to
    int GetLabelLocation(std::string a_labelName);

private:
    //container to store all the statements in a Duck language program
    std::vector<std::string> m_statements;
    
    //container to store all the labels paired with the lines they correspond to
    std::map<std::string, int> m_labelToStatement;
    
    //Returns true if a the string is a legal label name
    bool ValidLabelName(std::string a_labelName);
    
    //Removes white space from the start and beginning of a string
    void RemoveExtraneousWhiteSpace(std::string& a_line);
    
    //Removes the comments from the given statement
    void RemoveComments(std::string& a_statement);
    
    //Extracts and separates the label from a given statement
    void ExtractLabel(std::string& a_statement, std::string &a_labelName);
};
