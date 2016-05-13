#include "Vowels.h"
#include <vector>
#include <string>

const std::string iSuggestions[5] = {"\xC4\xAB", "\xC3\xAD", "\xC7\x90", "\xC3\xAC", "i"}; //ī, í, ǐ, ì
const std::string uSuggestions[5] = {"\xC5\xAB", "\xC3\xBA", "\xC7\x94", "\xC3\xB9", "u"}; //ū, ú, ǔ, ù
const std::string aSuggestions[5] = {"\xC4\x81", "\xC3\xA1", "\xC7\x8E", "\xC3\xA0", "a"}; //ā, á, ǎ, à
const std::string oSuggestions[5] = {"\xC5\x8D", "\xC3\xB3", "\xC7\x92", "\xC3\xB2", "o"}; //ō, ó, ǒ, ò 
const std::string eSuggestions[5] = {"\xC4\x93", "\xC3\xA9", "\xC4\x9B", "\xC3\xA8", "e"}; //ē, é, ě, è

const std::string vSuggestions[5] = {"\xC7\x96", "\xC7\x98", "\xC7\x9A", "\xC7\x9C", "\xC3\xBC"}; //ǖ, ǘ, ǚ, ǜ, ü
const std::string blankSuggestions[5] = {" ", " ", " ", " ", " "};

bool isVowel(char vowel)
{
    return vowel == 'i' || vowel == 'u' || vowel == 'a' || vowel == 'o' || vowel == 'e' || vowel == 'v';
}

const std::string *vowelSuggestions(char vowel)
{
    const std::string *suggestions;

    switch(vowel)
    {
        case 'i':
            suggestions = iSuggestions;
        break;

        case 'u':
            suggestions = uSuggestions;
        break;

        case 'a':
            suggestions = aSuggestions;
        break;

        case 'o':
            suggestions = oSuggestions;
        break;

        case 'e':
            suggestions = eSuggestions;
        break;

        case 'v':
            suggestions = vSuggestions;
        break;

        default:
            suggestions = blankSuggestions;
        break;
    }

    return suggestions;
}
