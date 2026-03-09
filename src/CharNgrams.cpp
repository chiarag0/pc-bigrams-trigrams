#include "CharNgrams.h"

int encodeNgram(const string& word, int pos, int n) {
    int index = 0;
    for (int i = 0; i < n; i++) {
        char c = word[pos + i];
        int offset;
        if (c >= '0' && c <= '9') { // if digit, map '0'-'9' to 0-9
            offset = c - '0';
        } else if (c >= 'a' && c <= 'z') {  // if letter, map 'a'-'z' to 10-35
            offset = c - 'a' + 10;
        } else if (c == '_') {
            offset = 36;
        } else {
            return -1;  // invalid character for n-gram, skip
        }
        index = index * 37 + offset; // base-37 encoding
    }
    return index;
}

string decodeNgram(int idx, int n) {
    string ngram(n, ' ');
    for (int j = n - 1; j >= 0; j--) {  // decode in reverse order
        int offset = idx % 37; 
        if (offset < 10) // convert back to number or letter or _
            ngram[j] = '0' + offset;
        else if (offset < 36)
            ngram[j] = 'a' + offset - 10;
        else
            ngram[j] = '_'; 
        idx /= 37;  // move to the next character
    }
    return ngram;
}

double extractCharNgrams(const string& text, int n, vector<int>& histogram) {
    int numSlots = pow(37, n); // 37 possible characters for each of the n positions
    histogram.assign(numSlots, 0); // resize and initialize histogram

    auto start = chrono::high_resolution_clock::now();

    for (int p = 0; p <= (int)text.size() - n; p++) {
        int idx = encodeNgram(text, p, n);
        if (idx >= 0) histogram[idx]++;
    }

    auto end = chrono::high_resolution_clock::now();
    return chrono::duration<double>(end - start).count();
}

void printTopKChars(const vector<int>& histogram, int n, int k) {
    priority_queue<pair<int,int>> q;  // couple of (count, index)
    for (int i = 0; i < (int)histogram.size(); i++) {
        if (histogram[i] > 0)
            q.push({histogram[i], i});
    }

    cout << "\n=== Top " << k << " char " << n << "-grams ===" << endl;
    for (int i = 0; i < k && !q.empty(); i++) {
        auto [count, idx] = q.top(); // retrieve the most frequent n-gram
        q.pop();
        cout << decodeNgram(idx, n) << ": " << count << endl;
    }
}


double parallelExtractCharNgrams(const string& text, int n,
                                  vector<int>& histogram, int numThreads) {
    int numSlots = pow(37, n);
    histogram.assign(numSlots, 0);

    // each thread has its own local histogram to avoid race conditions
    vector<vector<int>> localHists(numThreads, vector<int>(numSlots, 0));

    auto start = chrono::high_resolution_clock::now();

    #pragma omp parallel num_threads(numThreads)
    {
        int id = omp_get_thread_num();
        vector<int>& local = localHists[id];

        #pragma omp for schedule(static)  // divide in equal chunks among threads
        for (int p = 0; p <= (int)text.size() - n; p++) {
            int idx = encodeNgram(text, p, n);
            if (idx >= 0) local[idx]++;
        }
    }

    // one thread merges local histograms into the global histogram
    // only sums, no need for parallelization
    for (int t = 0; t < numThreads; t++) {
        for (int i = 0; i < numSlots; i++) {
            histogram[i] += localHists[t][i];
        }
    }

    // measure time after merging
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration<double>(end - start).count();
}