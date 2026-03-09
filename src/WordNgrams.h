#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <iostream>
#include <chrono>
#include <omp.h>

using namespace std;

double extractWordNgrams(const vector<string>& words, int n,
                         unordered_map<string,int>& histogram);

void printTopKWords(const unordered_map<string,int>& histogram, int k = 25);
double parallelExtractWordNgrams(const vector<string>& words, int n,
                                  unordered_map<string,int>& histogram,
                                  int numThreads,
                                  int scheduleType,  // 0=static, 1=dynamic, 2=guided
                                  int chunkSize);  // 0=auto, 512, 4096