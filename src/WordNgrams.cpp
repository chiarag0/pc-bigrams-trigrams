#include "WordNgrams.h"

double extractWordNgrams(const vector<string>& words, int n,
                         unordered_map<string,int>& histogram) {
    histogram.clear();

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i <= (int)words.size() - n; i++) {  // for each  valid starting position of n-gram
        string ngram = words[i];
        for (int j = 1; j < n; j++) {
            ngram += ' ';
            ngram += words[i + j];
        }
        histogram[ngram]++;  // increment count for this n-gram in the histogram
    }

    auto end = chrono::high_resolution_clock::now();
    return chrono::duration<double>(end - start).count();
}

void printTopKWords(const unordered_map<string,int>& histogram, int k) {
    priority_queue<pair<int,string>> q;
    for (auto& [ngram, count] : histogram) {
        q.push({count, ngram});
    }

    cout << "\n=== Top " << k << " word n-grams ===" << endl;
    int printed = 0;
    while (printed < k && !q.empty()) {
        auto [count, ngram] = q.top();
        q.pop();
        cout << ngram << ": " << count << endl;
        printed++;
    }
}


double parallelExtractWordNgrams(const vector<string>& words, int n,
                                  unordered_map<string,int>& histogram,
                                  int numThreads,
                                  int scheduleType,
                                  int chunkSize) {
    histogram.clear();

    // each thread has its own local map to avoid race conditions
    vector<unordered_map<string,int>> localHists(numThreads);
    for (auto& h : localHists)
        h.reserve(words.size() / numThreads);

    auto start = chrono::high_resolution_clock::now();

    #pragma omp parallel num_threads(numThreads) 
    {
        int id = omp_get_thread_num();
        unordered_map<string,int>& local = localHists[id];

        if (scheduleType == 0) {  // static scheduling
            if (chunkSize == 0) {  // auto chunk size determined by OpenMP
                #pragma omp for schedule(static)
                for (int i = 0; i <= (int)words.size() - n; i++) {
                    string ngram = words[i];
                    for (int j = 1; j < n; j++) { ngram += ' '; ngram += words[i+j]; }
                    local[ngram]++;
                }
            } else if (chunkSize == 64) {  // fixed chunk size (small)
                #pragma omp for schedule(static, 64)
                for (int i = 0; i <= (int)words.size() - n; i++) {
                    string ngram = words[i];
                    for (int j = 1; j < n; j++) { ngram += ' '; ngram += words[i+j]; }
                    local[ngram]++;
                }    
            } else if (chunkSize == 512) {  // fixed chunk size (medium)
                #pragma omp for schedule(static, 512)
                for (int i = 0; i <= (int)words.size() - n; i++) {
                    string ngram = words[i];
                    for (int j = 1; j < n; j++) { ngram += ' '; ngram += words[i+j]; }
                    local[ngram]++;
                }
            } else {
                #pragma omp for schedule(static, 4096)  // fixed chunk size (large)
                for (int i = 0; i <= (int)words.size() - n; i++) {
                    string ngram = words[i];
                    for (int j = 1; j < n; j++) { ngram += ' '; ngram += words[i+j]; }
                    local[ngram]++;
                }
            }
        } else if (scheduleType == 1) {  // dynamic scheduling
            if (chunkSize == 0) {
                #pragma omp for schedule(dynamic)
                for (int i = 0; i <= (int)words.size() - n; i++) {
                    string ngram = words[i];
                    for (int j = 1; j < n; j++) { ngram += ' '; ngram += words[i+j]; }
                    local[ngram]++;
                }
            } else if (chunkSize == 64) {
                #pragma omp for schedule(dynamic, 64)  // fixed chunk size (small)
                for (int i = 0; i <= (int)words.size() - n; i++) {
                    string ngram = words[i];
                    for (int j = 1; j < n; j++) { ngram += ' '; ngram += words[i+j]; }
                    local[ngram]++;
                }
            } else if (chunkSize == 512) {
                #pragma omp for schedule(dynamic, 512) // fixed chunk size (medium)
                for (int i = 0; i <= (int)words.size() - n; i++) {
                    string ngram = words[i];
                    for (int j = 1; j < n; j++) { ngram += ' '; ngram += words[i+j]; }
                    local[ngram]++;
                }
            } else {
                #pragma omp for schedule(dynamic, 4096)  // fixed chunk size (large)
                for (int i = 0; i <= (int)words.size() - n; i++) {
                    string ngram = words[i];
                    for (int j = 1; j < n; j++) { ngram += ' '; ngram += words[i+j]; }
                    local[ngram]++;
                }
            }
        } else {  // guided scheduling
            #pragma omp for schedule(guided)  // OpenMP determines chunk size dynamically, starting large and decreasing
            for (int i = 0; i <= (int)words.size() - n; i++) {
                string ngram = words[i];
                for (int j = 1; j < n; j++) { ngram += ' '; ngram += words[i+j]; }
                local[ngram]++;
            }
        }
    }

    // sequential merge
    histogram.reserve(words.size());
    for (auto& local : localHists) {
        for (auto& [k, v] : local) {
            histogram[k] += v;
        }
    }

    auto end = chrono::high_resolution_clock::now();
    return chrono::duration<double>(end - start).count();
}