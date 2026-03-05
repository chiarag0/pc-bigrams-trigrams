#pragma once
#include <vector>
#include <string>
#include <queue>
#include <cmath>
#include <iostream>
#include <chrono>

using namespace std;

int encodeNgram(const string& text, int pos, int n);
string decodeNgram(int idx, int n);
double extractCharNgrams(const string& text, int n, vector<int>& histogram);
void printTopKChar(const vector<int>& histogram, int n, int k = 25);