#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "CharNgrams.h"
#include "WordNgrams.h"
#include <fstream>
#include <ostream>

using namespace std;

// measures sequential baseline with warmup, returns mean time
double meanSeqChar(const string& text, int n, vector<int>& histogram);
double meanSeqWord(const vector<string>& words, int n, unordered_map<string,int>& histogram);

void runCharBenchmark(const string& text, int n, int numThreads,
                      double seqTime, ofstream& csv);

void runWordBenchmark(const vector<string>& words, int n, int numThreads,
                      int scheduleType, int chunkSize,
                      double seqTime, ofstream& csv);