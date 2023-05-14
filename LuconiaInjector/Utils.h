#pragma once

#include "Window.h"

#include <stdio.h>
#include <string>
#include <Aclapi.h>
#include <Sddl.h>
#include <string>
#include <codecvt>
#include <ostream>

class Utils
{
public:
    static bool CALLBACK SetFont(HWND child, LPARAM font) {
        SendMessage(child, WM_SETFONT, font, true);
        return true;
    }
};

class String
{
public:
    char* data;
public:
    String() : data(new char[1])
    {
        data[0] = '\0';
    }

    String(const std::string& str) : data(new char[1 + std::strlen(str.c_str())])
    {
        std::strcpy(data, str.c_str());
    }

    String(const char* cstr) : data(new char[1 + std::strlen(cstr)])
    {
        std::strcpy(data, cstr);
    }

    String(const String& str)
    {
        data = str.data;
    }

    String(String a, String b) : data(new char[1 + std::strlen(a.data) + std::strlen(b.data)])
    {
        std::strcpy(data, a.data);
        std::strcat(data, b.data);
    }

    ~String()
    {
        delete[] data;
    }
public:
    std::wstring to_wstring()
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> wstrConv;
        return wstrConv.from_bytes(data);
    }

    std::string to_str()
    {
        return data;
    }

    char* c_str()
    {
        return data;
    }
public:
    const char* operator * () const
    {
        return data;
    }

    bool operator == (const char* str)
    {
        return data == str;
    }
public:
    friend std::ostream& operator << (std::ostream& out, String& str)
    {
        int index = 0;

        while (str.data[index] != '\0')
        {
            out << str.data[index];
            index++;
        }

        return out;
    }
};

class WString
{
public:
    wchar_t* data;
public:
    WString() : data(new wchar_t[1])
    {
        data[0] = '\0';
    }

    WString(const std::wstring& wstr) : data(new wchar_t[1 + lstrlen(wstr.c_str())])
    {
        lstrcpy(data, wstr.c_str());
    }

    WString(const wchar_t* cwstr) : data(new wchar_t[1 + lstrlen(cwstr)])
    {
        lstrcpy(data, cwstr);
    }

    WString(const WString& wstr)
    {
        data = wstr.data;
    }

    WString(WString a, WString b) : data(new wchar_t[1 + lstrlen(a.data) + lstrlen(b.data)])
    {
        lstrcpy(data, a.data);
        lstrcpy(data, b.data);
    }

    ~WString()
    {
        delete[] data;
    }
public:
    std::string to_string()
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strConv;
        return strConv.to_bytes(data);
    }

    std::wstring to_wstr()
    {
        return data;
    }

    const wchar_t* c_str()
    {
        return data;
    }
public:
    const wchar_t* operator * () const
    {
        return data;
    }

    bool operator == (const wchar_t* wstr)
    {
        return data == wstr;
    }
public:
    friend std::wostream& operator << (std::wostream& out, WString& wstr)
    {
        int index = 0;

        while (wstr.data[index] != '\0')
        {
            out << wstr.data[index];
            index++;
        }

        return out;
    }
};