#include "SymbolTable.h"

using namespace std;

/**/
/*
 bool SymbolTable::GetVariableValue(string a_variable, double &a_value)
 
 NAME
 SymbolTable::GetVariableValue - Retrieves the value stored in the given variable
 
 SYNOPSIS
    bool SymbolTable::GetVariableValue(string a_variable, double &a_value)
        a_variable --> The variable whose value is to be retrieved
        a_value --> The value to be retrieved
 
 DESCRIPTION
    Retrieves the value stored in the given variable. If no such variable exists, then returns False;
 
 RETURNS
    True if the variable of which the value is to be retrieved exists. False otherwise
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
bool SymbolTable::GetVariableValue(string a_variable, double &a_value)
{
    if(m_SymbolTable.count(a_variable) == 0) {
        return false;
    } else {
        a_value = m_SymbolTable.find(a_variable)->second;
        return true;
    }
} /** bool SymbolTable::GetVariableValue(string a_variable, double &a_value) **/
