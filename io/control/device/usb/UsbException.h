//
// Created by murray on 12/04/23.
//

#pragma once


#include <stdexcept>

class UsbException : public std::runtime_error
{
public:
    explicit UsbException(const std::string& what);
    explicit UsbException(const std::wstring& what);
};

