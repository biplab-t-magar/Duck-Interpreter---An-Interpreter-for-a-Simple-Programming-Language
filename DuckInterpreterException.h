/* DuckInterpreterException.h
 DuckInterpreterException.h contains the DuckInterpreterException class
 DuckInterpreterException is a class that represents all the different exceptions that may arise in the DuckInterpreter class while
 executing the Duck Language source code. It is used by the DuckInterpreter class to handle these exceptions that arise from
 faulty Duck Language code. It inherits from the standard C++ exception class
*/
#pragma once

#include <string>
#include <exception>

class DuckInterpreterException : public std::exception
{
public:
    
    //Constructor that takes in the exception description as message
    DuckInterpreterException(std::string a_message);
    
    //Returns a description of the exception that occured
    std::string what();
private:
    //stores a description of the type of exception that occured
    std::string m_errorMessage;
};

