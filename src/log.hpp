//
// Created by radue on 1/23/2024.
//

#pragma once

#include <string>
#include <iostream>

class Log
{
public:
    static void info(const std::string &message)
    {
        std::cout << "[INFO]\t| " << message << std::endl;
    }

    static void error(const std::string &message)
    {
        std::cerr << "[ERROR]\t| " << message << std::endl;
    }

    static void warning(const std::string &message)
    {
        std::cout << "[WARNING]\t| " << message << std::endl;
    }
};
