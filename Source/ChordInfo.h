#pragma once
#include <vector>
#include <string>

struct ChordInfo
{
    double timestamp;
    std::string name;
    std::set<int> notes;
};