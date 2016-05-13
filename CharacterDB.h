//the CharacterDB class loads all required information from the database files and provides access to it

#ifndef _CharacterDB
#define _CharacterDB

#include <unordered_map>
#include <string>
#include <set>
#include <stdint.h>
#include <vector>

class CharacterDB
{
    std::unordered_map<uint32_t, std::unordered_map<std::string, std::string>> characters; //each character has a map of key value pairs
    std::unordered_map<uint32_t, int> frequency; //frequency data for each character
    std::unordered_map<std::string, std::set<uint32_t>> readings; //holds the characters associated with each reading

    void parseUnihan(void);
    void parseFrequency(void);

    void generateReadings(const std::string&);

    public:
    CharacterDB(void);
    ~CharacterDB();

    std::set<uint32_t> const& lookupReading(const std::string&);
    int lookupFrequency(uint32_t);

    std::vector<uint32_t> lookupSortedCharacters(const std::string&);

};
#endif
