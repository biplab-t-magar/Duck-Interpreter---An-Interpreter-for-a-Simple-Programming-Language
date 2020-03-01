//
//  ArrayTable.cpp
//  DuckInterpreter
//
//  Created by Biplab Thapa Magar on 5/15/19.
//  Copyright © 2019 Biplab Thapa Magar. All rights reserved.
//

#include "ArrayTable.h"

using namespace std;


/**/
/*
 void ArrayTable::AddNewArray(string a_arrayName, double a_size)
 
 NAME
    ArrayTable::AddNewArray - Adds a new array to ArrayTable with all its indexes uninitialized
 
 SYNOPSIS
    void ArrayTable::AddNewArray(string a_arrayName, double a_size)
        a_arrayName --> The name of the new array to be added
        a_size --> The size of the array to be added
 
 DESCRIPTION
    This function adds a new array (with the specified name and size) to the ArrayTable.
    It throws exceptions if the array size is invalid or if the array name is already taken
 
 RETURNS
    void
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
void ArrayTable::AddNewArray(string a_arrayName, double a_size)
{
    //first, check if the size is valid
    if(a_size <= 0) {
        throw DuckInterpreterException("Size of array is invalid.");
    }
    
    //second, check if the size is a whole number
    int sizeInWholeNum = static_cast<int>(a_size);
    if(a_size - sizeInWholeNum != 0) {
        throw DuckInterpreterException("Size of array must be a whole number.");
    }
    
    //check if the array already exists
    if(m_ArraySizeTable.count(a_arrayName) > 0) {
        throw DuckInterpreterException("Cannot redeclare an existing array.");
    }
    
    //add vector and corresponding size to ArraySizeTable
    m_ArraySizeTable[a_arrayName] = sizeInWholeNum;
}/**void ArrayTable::AddNewArray(string a_arrayName, double a_size)**/



/**/
/*
 void ArrayTable::AddToArray(string a_arrayName, double a_index, double a_value)
 
 NAME
    ArrayTable::AddToArray - Assigns the given value to the specified index in the specified array
 
 SYNOPSIS
    void ArrayTable::AddToArray(string a_arrayName, double a_index, double a_value)        
        a_arrayName --> The name of the array
        a_index --> The index of the array at which the value is to be assigned
        a_value --> The value to be assigned at the specified index of the array
 
 DESCRIPTION
    This function assigns a value at the specified index of an existing array. If the array does not exist or if the index
    is invalid, then the function throws an error
 
 RETURNS
    void
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
void ArrayTable::AddToArray(string a_arrayName, double a_index, double a_value) {
    //first, verify the array name and index
    verifyArrayIndex(a_arrayName, a_index);
    
    //add value to the array table
    //each element in the array is stored as an independent variable in the arrayTable, with the key to obtain the value being <array name>[<index.]
    string keyVariable = a_arrayName + "[" + to_string(static_cast<int>(a_index)) + "]";
    m_arrayTable.RecordVariableValue(keyVariable, a_value);
}/**void ArrayTable::AddToArray(string a_arrayName, double a_index, double a_value)**/




/**/
/*
 bool ArrayTable::GetValue(string a_arrayName, double a_index, double &a_value)
 
 NAME
    ArrayTable::GetValue - Retrieves the value present at the specified index in the specified array
 
 SYNOPSIS
    bool ArrayTable::GetValue(string a_arrayName, double a_index, double &a_value)
        a_arrayName --> The name of the array
        a_index --> The index of the array at which the value is to be assigned
        a_value --> The value to be retreived from the specified index of the array
 
 DESCRIPTION
    This function retrieves the value assigned at the specified index of the specified array. If the array does not exist, if the index
    is invalid, then the function throws an error
 
 RETURNS
    Returns false if a value has not been assigned at the specified index of the array. Returns true otherwise
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
bool ArrayTable::GetValue(string a_arrayName, double a_index, double &a_value) {
    //first, verify the array name and index
    verifyArrayIndex(a_arrayName, a_index);
    
    //calculate the key so that we can retreive the value stored in the arrayTable
    string keyVariable = a_arrayName + "[" + to_string(static_cast<int>(a_index)) + "]";
    
    //next, assign the value at the given index of the array to a_value if the array has been initialized at the given index
    //if a value at the given index has not been assigned, throw error
    if(!m_arrayTable.GetVariableValue(keyVariable, a_value)) {
        return false;
    }
    return true;
}/**bool ArrayTable::GetValue(string a_arrayName, double a_index, double &a_value)**/



/**/
/*
 void ArrayTable::verifyArrayIndex(string a_arrayName, double a_index)
 
 NAME
    ArrayTable::verifyArrayIndex - Checks if the given array has been initalized and if the index specified is valid
 
 SYNOPSIS
    void ArrayTable::verifyArrayIndex(string a_arrayName, double a_index)
        a_arrayName --> The name of the array to be checked
        a_index --> The index to be checked
 
 DESCRIPTION
    This function checks if the given array has been initialized. If not, it throws an exception.
    It also checks of the index is out-of-bounds of the array or if the index is not a whole number. Throws an exception in both cases
 
 RETURNS
    void
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
void ArrayTable::verifyArrayIndex(string a_arrayName, double a_index) {
    //first, check if the array has been declared
    if(m_ArraySizeTable.count(a_arrayName) == 0) {
        string message = "'" + a_arrayName + "' has not been declared as an array";
        throw DuckInterpreterException(message);
    }
    
    //next, check to see if the index is valid
    //check to see if index is a whole number
    int indexInWholeNum = static_cast<int>(a_index);
    if(a_index - indexInWholeNum != 0) {
        string message = "The given array index, " + to_string(a_index) + ", is not a whole number. Index must be a whole number.";
        throw DuckInterpreterException(message);
    }
    //check to see if index is less than zero
    if(indexInWholeNum < 0) {
        string message = "The given array index, " + to_string(a_index) + ", is negative. Index must be a positive number.";
        throw DuckInterpreterException(message);
    }
    //check to see if index exceeds size
    int arraySize = m_ArraySizeTable.find(a_arrayName)->second;
    if(indexInWholeNum > arraySize - 1) {
        string message = "Array index '" + to_string(indexInWholeNum) + "' is out of bounds of the array '" + a_arrayName;
        message = message + "', which is of size " + to_string(arraySize) + ".";
        throw DuckInterpreterException(message);
    }
}/**void ArrayTable::verifyArrayIndex(string a_arrayName, double a_index)**/
