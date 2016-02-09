#ifndef DEBUG_HPP
#define DEBUG_HPP
#include <iostream>
#include <string>
#ifdef DEBUG
#define DEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif

#endif
