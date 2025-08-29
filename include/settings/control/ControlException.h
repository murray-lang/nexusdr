//
// Created by murray on 15/04/23.
//

#ifndef CONTROLEXCEPTION_H
#define CONTROLEXCEPTION_H

#include <stdexcept>

class ControlException : public std::runtime_error
{
public:
    explicit ControlException(const std::string& what) : std::runtime_error(what) {}
};


#endif //CONTROLEXCEPTION_H
