// src/env_win.cpp
#ifdef _WIN32
#include <cstdlib>
#include <cstring>
#include <cerrno>

// provide MinGW a _dupenv_s implementation
extern "C" errno_t __cdecl _dupenv_s(char** pValue,
                                     size_t* pLen,
                                     const char* name)
{
    if (!pValue) return EINVAL;
    const char* val = std::getenv(name);
    if (val) {
        size_t len = std::strlen(val) + 1;
        char* buf = (char*)std::malloc(len);
        if (!buf) return ENOMEM;
        std::memcpy(buf, val, len);
        *pValue = buf;
        if (pLen) *pLen = len;
        return 0;
    } else {
        *pValue = nullptr;
        if (pLen) *pLen = 0;
        return ENOENT;
    }
}
#endif
