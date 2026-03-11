#pragma once
#include <vector>
#include <unordered_map>
#include <string>

using namespace std;

bool validateChar(const vector<int>& seqHist, const vector<int>& parHist);
bool validateWord(const unordered_map<string,int>& seqHist,
                  const unordered_map<string,int>& parHist);