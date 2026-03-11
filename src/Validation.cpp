#include "Validation.h"
#include <iostream>

using namespace std;

bool validateChar(const vector<int>& seqHist, const vector<int>& parHist) {
    if (seqHist.size() != parHist.size()) {
        cerr << "Validation failed: histogram sizes differ" << endl;
        return false;
    }

    for (int i = 0; i < (int)seqHist.size(); i++) {
        if (seqHist[i] != parHist[i]) {
            cerr << "Validation failed: mismatch at index " << i
                 << " (seq=" << seqHist[i] << " vs par=" << parHist[i] << ")" << endl;
            return false;
        }
    }

    return true;
}

bool validateWord(const unordered_map<string,int>& seqHist,
                  const unordered_map<string,int>& parHist) {
    if (seqHist.size() != parHist.size()) {
        cerr << "Validation failed: histogram sizes differ" << endl;
        return false;
    }

    for (const auto& kv : seqHist) {
        const string& ngram = kv.first;
        int seqCount = kv.second;

        auto it = parHist.find(ngram);  // find key and return iterator
        if (it == parHist.end()) {
            cerr << "Validation failed: n-gram '" << ngram << "' missing in parallel histogram" << endl;
            return false;
        }

        int parCount = it->second;  // get value from iterator
        if (seqCount != parCount) {
            cerr << "Validation failed: mismatch for n-gram '" << ngram
                 << "' (seq=" << seqCount << " vs par=" << parCount << ")" << endl;
            return false;
        }
    }

    return true;
}