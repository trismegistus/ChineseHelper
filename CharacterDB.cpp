#include "CharacterDB.h"
#include "Converter.h"
#include <unordered_map>
#include <string>
#include <string.h>
#include <fstream>
#include <ctype.h>
#include <iostream>
#include <iconv.h>
#include <errno.h>
#include <vector>
#include <algorithm>

void CharacterDB::parseUnihan(void)
{
    std::ifstream file("db/unihan.txt");
    char buffer[512];

    while(file.good())
    {
        uint32_t codepoint;
        int index;
        std::string number;
        std::string key;
        std::string value;

        file.getline(buffer, 512);

        //data starts with U+, everything else is a comment
        if(strncmp(buffer, "U+", 2)) continue;

        index = 2;

        //data has this format
        //U+<codepoint> key value
        while(isxdigit(buffer[index]))
        {
            number += buffer[index];
            index++;
        }

        codepoint = std::stoi(number.c_str(), nullptr, 16);

        //make an entry for the character if it's not already there
        if(this->characters.count(codepoint) == 0)
        {
            this->characters[codepoint] = std::unordered_map<std::string, std::string>();
        }

        //eat up whitespace
        while(isspace(buffer[index])) index++;

        //get the key
        while(!isspace(buffer[index]))
        {
            key += buffer[index];
            index++;
        }

        //eat up whitespace
        while(isspace(buffer[index])) index++;

        //get the value
        while(buffer[index] != '\0')
        {
            value += buffer[index];
            index++;
        }

        this->characters[codepoint][key] = value;
    }

    file.close();
}

void CharacterDB::parseFrequency(void)
{
    std::ifstream file("db/frequency.txt");
    char buffer[512];
    Converter converter;

    while(file.good())
    {
        int index;
        std::string character;
        std::string freq;
        uint32_t codepoint;
        int frequency;

        file.getline(buffer, 512);

        //comments start with a #
        if(buffer[0] == '#') continue;

        //Data is formated as follows
        //row# character count percentile pronounciation definition

        //the first character will always be a number, so might as well skip it
        index = 1;

        //eat up the row number
        while(isdigit(buffer[index])) index++;

        //eat up whitespace
        while(isspace(buffer[index])) index++;

        //get the data for the character
        while(!isspace(buffer[index]))
        {
            character += buffer[index];
            index++;
        }

        //eat up whitespace
        while(isspace(buffer[index])) index++;

        //get the frequency
        while(isdigit(buffer[index]))
        {
            freq += buffer[index];
            index++;
        }

        frequency = std::stoi(freq.c_str(), nullptr, 10);

        codepoint = converter.toCodepoint(character);
        this->frequency[codepoint] = frequency;
    }
}

//name is the name of the language
void CharacterDB::generateReadings(const std::string& name)
{
    //create readings for each character
    for(auto& pair : this->characters)
    {
        uint32_t codepoint = pair.first;
        std::unordered_map<std::string, std::string>& dict = pair.second;
        std::string reading;

        if(dict.count(name))
        {
            reading = dict[name];

            //create a set of readings if one doesn't already exist
            if(!this->readings.count(reading))
            {
                this->readings[reading] = std::set<uint32_t>();
            }

            //insert the reading
            this->readings[reading].insert(codepoint);
        }
    }
}

CharacterDB::CharacterDB(void)
{
    this->parseUnihan();
    this->parseFrequency();
    //only care about mandarin
    this->generateReadings("kMandarin");
}

CharacterDB::~CharacterDB()
{
}

std::set<uint32_t> const& CharacterDB::lookupReading(const std::string& reading)
{
    return this->readings[reading];
}

int CharacterDB::lookupFrequency(uint32_t codepoint)
{
    if(this->frequency.count(codepoint))
    {
        return this->frequency[codepoint];
    }
    else
    {
        return 0;
    }
}

//returns a vector of characters that have reading as their reading, sorted by frequency
std::vector<uint32_t> CharacterDB::lookupSortedCharacters(const std::string& reading)
{
    std::set<uint32_t> const& characters = this->lookupReading(reading);
    std::vector<uint32_t> vCharacters;

    //store the found characters
    vCharacters.reserve(characters.size());
    for(uint32_t character : characters)
    {
        vCharacters.push_back(character);
    }

    //sort the character by frequency
    std::sort(vCharacters.begin(), vCharacters.end(), [this](uint32_t one, uint32_t two)
    {
        return this->lookupFrequency(one) > this->lookupFrequency(two);
    });
    return vCharacters;
}
