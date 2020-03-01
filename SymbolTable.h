/*SymbolTable.h
 SymbolTable.h contains the SymbolTable class.
 SymbolTable is a class that represents a collection of variables and the values they store. It is utilized by the 
 DuckInterpreter program to store and retreive the values stored in variables.
 */


#pragma once

#include <unordered_map>
#include <string>

// This class will provide a mapping between the variables and their associated data.
class SymbolTable
{
public:
    SymbolTable(){}
    ~SymbolTable(){}

    /**/
    /*
    void RecordVariableValue(std::string a_variable, double a_value)
     
    NAME
        RecordVariableValue - Assigns the given variable to the given value
     
    SYNOPSIS
        void RecordVariableValue(std::string a_variable, double a_value)
        a_variable --> The variable whose value is to be assigned the given value
        a_value --> The value that is to be assigned to the given variable
     
    DESCRIPTION
        This function assigns a value to a variable and records it
     
    RETURNS
        void
     
    AUTHOR
        Biplab Thapa Magar
     */
    /**/
    void RecordVariableValue(std::string a_variable, double a_value) {
		m_SymbolTable[a_variable] = a_value;
	}/*void RecordVariableValue(std::string a_variable, double a_value)*/
    
	// Get the value of a variable.  Returns false if the variable does not exist.
    bool GetVariableValue( std::string a_variable, double &a_value );

private:
    //container to store all variables and the values they store as a key/value pair
    std::unordered_map<std::string, double> m_SymbolTable;
};             
