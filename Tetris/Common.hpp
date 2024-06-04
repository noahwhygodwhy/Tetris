#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <wrl.h>
#include <dxgi1_6.h>

#include <unordered_map>
#include <string>
#include <vector>
#include <string>
#include <format>
#include <chrono>
#include <random>
#include <map>

using namespace std;

inline void OutputDebugStringF(const char* fmt, ...)
{
    char buffer[1 << 16];
    va_list args;
    va_start(args, fmt);
    int rc = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    OutputDebugStringA(buffer);
}

inline void ThrowIfFailed(HRESULT hr, uint32_t line, const char* file)
{
    if (FAILED(hr))
    {
        char buff[500];
        snprintf(buff, sizeof(buff), "HR 0X%08X at line %u of %s\n", hr, line, file);
        OutputDebugStringA(buff);
        throw std::runtime_error(string(buff));
    }
}

inline void ThrowIfError(HRESULT hr, const char* errorMessage, uint32_t line, const char* file)
{
    if (FAILED(hr))
    {
        char buff[500];
        snprintf(buff, sizeof(buff), "HR 0X%08X at line %u of %s\n%s\n", hr, line, file, errorMessage);
        OutputDebugStringA(buff);
        throw std::runtime_error(string(buff));
    }
}


#define TIF(hr) ThrowIfFailed(hr, __LINE__, __FILE__)
#define TIE(hr, txt) ThrowIfError(hr, txt, __LINE__, __FILE__);

#define SAFE_RELEASE(p) if (p) (p)->Release() //unless it doesn't have release ugh

#define printvec(v) glm::to_string(v).c_str()