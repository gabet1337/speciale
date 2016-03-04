#ifndef DEBUG_HPP
#define DEBUG_HPP
#include <iostream>
#include <string>
#ifdef DEBUG
#define DEBUG_MSG(str) do { std::cout << "\x1b[36m" << str << "\x1b[0m" << std::endl; } while( false )
#define DEBUG_MSG_FAIL(str) do { std::cout << "\x1b[31m" << str << "\x1b[0m" << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#define DEBUG_MSG_FAIL(str) do { } while ( false )
#endif

#endif
