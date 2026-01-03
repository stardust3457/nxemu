#pragma once

class stdstr;

#include <string>
#include <vector>

typedef std::vector<stdstr> strvector;

class stdstr :
    public std::string
{
public:
    enum
    {
        CODEPAGE_UTF8 = 65001,
    };

    stdstr();
    stdstr(const std::string & str);
    stdstr(const stdstr & str);
    stdstr(const char * str);

    strvector Tokenize(char delimiter) const;
    strvector Tokenize(const char * delimiter) const;
    void Format(const char * strFormat, ...);
    stdstr & ToLower();
    stdstr & ToUpper();

    stdstr & Replace(const char search, const char replace);
    stdstr & Replace(const char * search, const char replace);
    stdstr & Replace(const std::string & search, const std::string & replace);

    stdstr & Trim(const char * chars2remove = "\t ");
    stdstr & TrimLeft(const char * chars2remove = "\t ");
    stdstr & TrimRight(const char * chars2remove = "\t ");

#ifdef _WIN32
    stdstr & FromUTF16(const wchar_t * utf16Source, bool * success = nullptr);
    std::wstring ToUTF16(unsigned int codePage = CODEPAGE_UTF8, bool * success = nullptr) const;
#endif

    void ArgFormat(const char * format, va_list & args);
};

class stdstr_f : public stdstr
{
public:
    stdstr_f(const char * strFormat, ...);
};
