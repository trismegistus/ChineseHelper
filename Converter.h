//the converter class deals with converted from utf-8 to utf-32 (simply referred to as codepoints)
#ifndef _Converter
#define _Converter

#include <string>
#include <iconv.h>
#include <errno.h>
#include <stdint.h>

class Converter
{
    iconv_t utf8Converter;
    iconv_t codepointConverter;

    public:
    Converter(void);
    ~Converter();

    std::string toUTF8(uint32_t);
    uint32_t toCodepoint(const std::string&);
};

#endif
