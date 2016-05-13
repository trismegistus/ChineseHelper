#include "Converter.h"
#include <iconv.h>
#include <errno.h>
#include <string>
#include <stdexcept>
#include <stdint.h>

Converter::Converter(void)
{
    this->utf8Converter = iconv_open("UTF-8", "UCS-4LE");
    if(this->utf8Converter == (iconv_t)-1)
    {
        throw std::runtime_error("Unsupported conversion");
    }

    this->codepointConverter = iconv_open("UCS-4LE", "UTF-8");
    if(this->codepointConverter == (iconv_t)-1)
    {
        throw std::runtime_error("Unsupported conversion");
    }
}

Converter::~Converter()
{
    iconv_close(this->utf8Converter);
    iconv_close(this->codepointConverter);
}

std::string Converter::toUTF8(uint32_t codepoint)
{
    char utf8[5];
    size_t inSize = 4;
    size_t outSize = 4;
    char *inBuffer = (char*)&codepoint;
    char *outBuffer = utf8;
    size_t err;

    //a codepoint is 4 bytes and the resulting utf-8 could be between 1 to 4 bytes
    err = iconv(this->utf8Converter, &inBuffer, &inSize, &outBuffer, &outSize);
    if(err == -1)
    {
        switch(errno)
        {
            case E2BIG:
                throw std::runtime_error("Output buffer not big enough");
                break;

            case EILSEQ:
                throw std::runtime_error("Invalid sequence");
                break;
            
            case EINVAL:
                throw std::runtime_error("Incomplete sequence");
                break;
            
            default:
                throw std::runtime_error("Unknow error");
                break;
        }
    }

    utf8[4 - outSize] = '\0';
    return std::string(utf8);
}

unsigned int Converter::toCodepoint(const std::string& utf8)
{
    size_t inSize = 4;
    size_t outSize = 16;
    uint32_t out[4];
    char *inBuffer = (char*)utf8.c_str();
    char *outBuffer = (char*)out;
    size_t err;

    //the sizes are a little weird because a utf-8 character can be up to 4 bytes, so i need at least 4 bytes for that
    //however, 4 bytes could be four different charactecs, and since a codepoint is 4 bytes, the output needs to be 16 bytes
    //otherwise, iconv will complain
    err = iconv(this->codepointConverter, &inBuffer, &inSize, &outBuffer, &outSize);
    if(err == -1)
    {
        switch(errno)
        {
            case E2BIG:
                throw std::runtime_error("Output buffer not big enough");
                break;

            case EILSEQ:
                throw std::runtime_error("Invalid sequence");
                break;
            
            case EINVAL:
                throw std::runtime_error("Incomplete sequence");
                break;
            
            default:
                throw std::runtime_error("Unknow error");
                break;
        }
    }

    return out[0];
}
