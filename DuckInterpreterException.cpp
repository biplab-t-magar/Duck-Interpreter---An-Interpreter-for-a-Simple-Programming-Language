//
//  DuckInterpreterExceptions.cpp
//  DuckInterpreter
//
//  Created by Biplab Thapa Magar on 5/7/19.
//  Copyright © 2019 Biplab Thapa Magar. All rights reserved.
//

#include "DuckInterpreterException.h"

/**/
/*
 DuckInterpreterException::DuckInterpreterException(std::string message)
 
 NAME
 DuckInterpreterException::DuckInterpreterException - The constructor for the DuckInterpreterException class
 
 SYNOPSIS
    DuckInterpreterException::DuckInterpreterException(std::string message)
        a_message --> The description of the exception
 
 DESCRIPTION
    Creates a DuckInterpreterException object along with a description of the type of exception
 
 RETURNS
 
 AUTHOR
 Biplab Thapa Magar
 */
/**/
DuckInterpreterException::DuckInterpreterException(std::string a_message) {
    this->m_errorMessage = a_message;
}/** DuckInterpreterException::DuckInterpreterException(std::string a_message) **/



/**/
/*
 std::string DuckInterpreterException::what()
 
 NAME
 DuckInterpreterException::what - Returns the description of the exception
 
 SYNOPSIS
    std::string DuckInterpreterException::what()
 
 DESCRIPTION
    Returns the description of the exception
 
 RETURNS
    The value stored in m_errorMessage
 
 AUTHOR
    Biplab Thapa Magar
 */
/**/
std::string DuckInterpreterException::what() {
    return m_errorMessage;
}/** std::string DuckInterpreterException::what() **/
