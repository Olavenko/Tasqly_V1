#include <iostream>

int main()
{
#ifdef _GLIBCXX_USE_CXX11_ABI
    std::cout << "[ABI DETECT] _GLIBCXX_USE_CXX11_ABI = " << _GLIBCXX_USE_CXX11_ABI << std::endl;
#else
    std::cout << "[ABI DETECT] _GLIBCXX_USE_CXX11_ABI not defined" << std::endl;
#endif

#ifdef _MSC_VER
    std::cout << "[COMPILER] MSVC version: " << _MSC_VER << std::endl;
#endif

#ifdef __MINGW32__
    std::cout << "[COMPILER] MinGW detected" << std::endl;
#endif

#ifdef __GLIBCXX__
    std::cout << "[LIBC++] libstdc++ (GNU)" << std::endl;
#elif defined(_LIBCPP_VERSION)
    std::cout << "[LIBC++] libc++ (LLVM)" << std::endl;
#else
    std::cout << "[LIBC++] Unknown STL implementation" << std::endl;
#endif

#ifdef _DEBUG
    std::cout << "[BUILD] Debug mode" << std::endl;
#else
    std::cout << "[BUILD] Release mode" << std::endl;
#endif

    return 0;
}
