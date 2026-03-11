//
// Created by murray on 15/04/23.
//

#pragma once


#include <stdexcept>

class ControlException : public std::runtime_error
{
public:
    explicit ControlException(const std::string& what) : std::runtime_error(what) {}
};
