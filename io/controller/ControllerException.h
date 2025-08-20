//
// Created by murray on 15/04/23.
//

#ifndef CONTROLLEREXCEPTION_H
#define CONTROLLEREXCEPTION_H

#include <stdexcept>

class ControllerException : public std::runtime_error
{
public:
    explicit ControllerException(const std::string& what);
};


#endif //CONTROLLEREXCEPTION_H
