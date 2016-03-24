#ifndef VALIDATE_HPP
#define VALIDATE_HPP
#include <iostream>
#include <string>
#ifdef VALIDATE
#define VALIDATE_MSG(str) do { std::cout << "\x1b[36m" << str << "\x1b[0m" << std::endl; } while( false )
#define VALIDATE_MSG_FAIL(str) do { std::cout << "\x1b[31m" << str << "\x1b[0m" << std::endl; } while( false )
#else
#define VALIDATE_MSG(str) do { } while ( false )
#define VALIDATE_MSG_FAIL(str) do { } while ( false )
#endif

#endif
