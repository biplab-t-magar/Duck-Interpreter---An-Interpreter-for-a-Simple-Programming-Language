/*
 ArrayTable.h
 ArrayTable.h contains the ArrayTable class.
 ArrayTable is a class that represents a collection of arrays. It is utilized by the DuckInterpreter program to
 store and retreive arrays and their values at various indexes.
*/

#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include "DuckInterpreterException.h"
#include "SymbolTable.h"

// This class will provide a mapping between the arrays and their associated data.
class ArrayTable
{
public:
    ArrayTable() {}
    ~ArrayTable(){}
    
    //Stores a new array (with all indexes uninitialized)
    void AddNewArray(std::string a_arrayName, double a_size);
    
    //Add a value to a specific index in a specific array that is stored in this class
    void AddToArray(std::string a_arrayName, double a_index, double value);
    
    //Get a value from a specific index of a specific array; return true if retreival was successful, false if the index of the given array was uninitialized
    bool GetValue(std::string a_arrayName, double a_index, double &a_value);

private:
    //The container in which the array name and its size is stored as a key-value pair
    std::unordered_map<std::string, int> m_ArraySizeTable;
    
    //The container in which the values of all the initialized indexes of all the arrays are stored
    //All arrays in ArrayTable share the same m_arrayTable
    SymbolTable m_arrayTable;
    
    //This function is used to verify if the array specified by the user has been initialized and/or the index specified is not out-of-bounds
    //if these conditions are not met, it throws an error
    void verifyArrayIndex(std::string a_arrayName, double a_index);
};
