#include "help_tool.h"
#include "config/config.h"
#include "help_tool.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <winerror.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0602
#endif
#include <pathcch.h>
#include <windows.h>
static wchar_t canonical[DEFAULT_BLOCK_SIZE] = {0};
unsigned char Isspace(const int c)
{
    unsigned char res = 0;
    if (c != -1)
    {
        unsigned char uc = (unsigned char)c;
        if (uc == (unsigned char)' ' || uc == (unsigned char)'\t' || uc == (unsigned char)'\n' || uc == (unsigned char)'\v'
            || uc == (unsigned char)'\f' || uc == (unsigned char)'\r')
        {
            res = 1;
        }
    }
    return res;
}
unsigned char IsEmpty(const char* str)
{
    if (str[0] == '\0')
    {
        return 1U;
    }
    unsigned char isEmpty = 1U;
    size_t size = strlen(str);
    for (size_t i = 0U; i < size; i++)
    {
        if (Isspace(str[i]) == 0U)
        {
            isEmpty = 0;
            break;
        }
    }
    return isEmpty;
}
void ClearStdin(void)
{
    fflush(stdin);
}
static int IsTrimChar(char c)
{
    return Isspace(c) || (c == '"');
}
void Clear_dqm_spaces(char* str)
{
    if (!str)
        return;

    size_t len = strlen(str);

    size_t start = 0;
    while (start < len && IsTrimChar(str[start]))
        start++;

    if (start == len)
    {
        str[0] = '\0';
        return;
    }

    size_t end = len;
    while (end > start && IsTrimChar(str[end - 1]))
        end--;

    size_t new_len = end - start;
    if (start != 0)
        memmove(str, str + start, new_len);
    str[new_len] = '\0';
}
void Print_Cin_Clear(char* buf, const char* to_print)
{
    assert(buf != NULL);
    assert(to_print != NULL);
    printf_s(to_print);
    fgets(buf, DEFAULT_BLOCK_SIZE - 1ULL, stdin);
    Clear_dqm_spaces(buf);
    ClearStdin();
}
static int to_wide(const char* src, wchar_t* dst, size_t dst_count, DWORD* out_err)
{
    if (out_err)
        *out_err = 0;
    if (!src || !dst || dst_count == 0)
    {
        if (out_err)
            *out_err = ERROR_INVALID_PARAMETER;
        return 0;
    }
    if (dst_count > (size_t)INT_MAX)
    {
        if (out_err)
            *out_err = ERROR_INVALID_PARAMETER;
        return 0;
    }

    SetLastError(0);
    int n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, src, -1, dst, (int)dst_count);
    if (n > 0)
        return 1;
    DWORD e1 = GetLastError();

    SetLastError(0);
    n = MultiByteToWideChar(CP_ACP, 0, src, -1, dst, (int)dst_count);
    if (n > 0)
        return 1;
    DWORD e2 = GetLastError();

    if (out_err)
        *out_err = (e2 != 0) ? e2 : e1;
    return 0;
}
Path_Data Check_Path(const char* path, unsigned short* wc_path, int is_source, unsigned long long wc_size)
{
    assert((is_source == 0x01) || (is_source == 0x00));
    assert(path != NULL);
    Path_Data data = {0};
    data.m_flag |= is_source;
    /* ---------- 诊断 1：传入的 path 字节 ---------- */
    // fprintf(stderr, "[DIAG] path len=%zu bytes:", strlen(path));
    // for (size_t i = 0; path[i]; i++)
    //     fprintf(stderr, " %02X", (unsigned char)path[i]);
    // fprintf(stderr, "\n");

    /* ---------- 诊断 2：to_wide 返回值 ---------- */
    DWORD werr = 0;
    int ok = to_wide(path, wc_path, wc_size, &werr);
    // fprintf(stderr, "[DIAG] to_wide = %d, werr = %lu\n", (int)ok, (unsigned long)werr);
    if (!ok)
        return data;

    /* ---------- 诊断 3：转换后的宽串 ---------- */
    // fprintf(stderr, "[DIAG] wc_path = \"%ls\" (len=%zu)\n", wc_path, wcslen(wc_path));
    if (is_source)
    {
        DWORD Attr = GetFileAttributesW(wc_path);
        if ((Attr != INVALID_FILE_ATTRIBUTES) && ((Attr & FILE_ATTRIBUTE_DIRECTORY) != 0UL)) // 存在且为目录
        {
            data.m_flag |= 0x02;
            data.m_flag |= 0x04;
        }
        return data;
    }
    HRESULT hr = PathCchCanonicalizeEx(canonical,
                                       _countof(canonical),
                                       wc_path,
                                       PATHCCH_ALLOW_LONG_PATHS); // 关键：允许长路径，且不强制要求存在
    if (SUCCEEDED(hr))
    {
        data.m_flag |= 0x02;
    }
    return data;
}
int Check_Data(Path_Data data)
{
    if (data.m_flag & PATH_SOURCE)
    {

        if ((data.m_flag & 0x04) == 0)
        {
            (void)printf_s("[check path]: source was NOEXITS or ILLEGAL!\n");
            return 0;
        }
        if ((data.m_flag & 0x02) == 0)
        {
            (void)printf_s("[check path]: source was FILE!\n");
            return 0;
        }
    }
    else
    {
        if ((data.m_flag & 0x02) == 0)
        {
            (void)printf_s("[check path]: diraction was ILLEGAL!\n");
            return 0;
        }
    }
    return 1;
}