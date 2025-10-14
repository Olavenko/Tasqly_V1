#pragma once
#include <iostream>
#include <string>
#include <optional>
#include "domain/core/mappers/P1_TaskDto.h"

namespace tasqly::diagnostic {

inline void printRuntimeInfo()
{
    std::cout << "====================================================\n";
    std::cout << "Tasqly Runtime Diagnostic\n";
    std::cout << "----------------------------------------------------\n";

#if defined(__clang__)
    std::cout << "Compiler     : Clang/LLVM\n";
#elif defined(__GNUC__)
    std::cout << "Compiler     : GNU (MinGW64)\n";
#elif defined(_MSC_VER)
    std::cout << "Compiler     : MSVC\n";
#else
    std::cout << "Compiler     : Unknown\n";
#endif

#if defined(__VERSION__)
    std::cout << "Version      : " << __VERSION__ << "\n";
#endif

#if defined(__GLIBCXX__)
    std::cout << "LibC++       : libstdc++ (GNU)\n";
#elif defined(_LIBCPP_VERSION)
    std::cout << "LibC++       : libc++ (LLVM)\n";
#else
    std::cout << "LibC++       : Unknown\n";
#endif

#if defined(_GLIBCXX_USE_CXX11_ABI)
    std::cout << "ABI Mode     : _GLIBCXX_USE_CXX11_ABI = "
              << _GLIBCXX_USE_CXX11_ABI << "\n";
#else
    std::cout << "ABI Mode     : Not defined\n";
#endif

#if defined(_WIN32)
    std::cout << "Platform     : Windows\n";
#elif defined(__linux__)
    std::cout << "Platform     : Linux\n";
#elif defined(__APPLE__)
    std::cout << "Platform     : macOS\n";
#else
    std::cout << "Platform     : Unknown\n";
#endif

#ifdef _DEBUG
    std::cout << "Build Type   : Debug\n";
#else
    std::cout << "Build Type   : Release\n";
#endif

    using tasqly::domain::core::v1::TaskDto;
    std::cout << "sizeof(std::string)            = " << sizeof(std::string) << "\n";
    std::cout << "sizeof(std::optional<string>)  = " << sizeof(std::optional<std::string>) << "\n";
    std::cout << "sizeof(TaskDto)                = " << sizeof(TaskDto) << "\n";
    std::cout << "====================================================\n";
}

} // namespace tasqly::diagnostic
