#include "Benchmark.h"
#include <iostream>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <ctime>
#include <omp.h>

using namespace std;

// 5 total non-discarded runs
static const int TOTAL_RUNS = 7;
static const int WARMUP_RUNS = 2;  // avoid cold cache effects

static void computeStats(const vector<double>& times,
                          double& mean, double& stdDev,
                          double& minT, double& maxT) {
    double sum = accumulate(times.begin(), times.end(), 0.0);
    mean = sum / times.size();

    double sq_sum = 0;
    for (int i = 0; i < times.size(); i++) {
        double t = times[i];
        sq_sum += (t - mean) * (t - mean);
    }
    stdDev = sqrt(sq_sum / times.size());

    minT = *min_element(times.begin(), times.end());
    maxT = *max_element(times.begin(), times.end());
}

double meanSeqChar(const string& text, int n, vector<int>& histogram) {
    vector<double> times;

    for (int r = 0; r < TOTAL_RUNS; r++) {
        double t = extractCharNgrams(text, n, histogram);
        if (r >= WARMUP_RUNS)
            times.push_back(t);
    }

    double mean, stdDev, minT, maxT;
    computeStats(times, mean, stdDev, minT, maxT);

    cout << "Sequential char " << n << "-gram: mean=" << mean << "s" << endl;
    return mean;
}

double meanSeqWord(const vector<string>& words, int n,
                   unordered_map<string,int>& histogram) {
    vector<double> times;

    for (int r = 0; r < TOTAL_RUNS; r++) {
        double t = extractWordNgrams(words, n, histogram);
        if (r >= WARMUP_RUNS)
            times.push_back(t);
    }

    double mean, stdDev, minT, maxT;
    computeStats(times, mean, stdDev, minT, maxT);

    cout << "Sequential word " << n << "-gram: mean=" << mean << "s" << endl;
    return mean;
}

void runCharBenchmark(const string& text, int n, int numThreads,
                      double seqTime, ofstream& csv) {
    vector<int> histogram;
    vector<double> wallTimes;
    vector<double> cpuTimes;

    for (int r = 0; r < TOTAL_RUNS; r++) {
        // cpu time is measured using clock() which counts cpu ticks
        clock_t cpuStart = clock(); 
        double wallTime = parallelExtractCharNgrams(text, n, histogram, numThreads);
        clock_t cpuEnd = clock();
        double cpuTime = (double)(cpuEnd - cpuStart) / CLOCKS_PER_SEC;

        if (r >= WARMUP_RUNS) {
            wallTimes.push_back(wallTime);
            cpuTimes.push_back(cpuTime);
        }
    }

    double wallMean, wallStd, wallMin, wallMax;
    computeStats(wallTimes, wallMean, wallStd, wallMin, wallMax);

    double cpuMean, cpuStd, cpuMin, cpuMax;
    computeStats(cpuTimes, cpuMean, cpuStd, cpuMin, cpuMax);

    double speedup = seqTime / wallMean;

    cout << "Char " << n << "-gram | threads=" << numThreads << endl;
    cout << "  wall: mean=" << wallMean << "s std=" << wallStd
         << "s min=" << wallMin << "s max=" << wallMax << "s" << endl;
    cout << "  cpu:  mean=" << cpuMean << "s" << endl;
    cout << "  speedup: " << speedup << "x" << endl;

    csv << "char," << n << "," << numThreads << ",static,auto,"
        << wallMean << "," << wallStd << "," << wallMin << "," << wallMax << ","
        << cpuMean << "," << speedup << "\n";
}

void runWordBenchmark(const vector<string>& words, int n, int numThreads,
                      int scheduleType, int chunkSize,
                      double seqTime, ofstream& csv) {
    unordered_map<string,int> histogram;
    vector<double> wallTimes;
    vector<double> cpuTimes;

    for (int r = 0; r < TOTAL_RUNS; r++) {
        clock_t cpuStart = clock();
        double wallTime = parallelExtractWordNgrams(words, n, histogram,
                                                 numThreads, scheduleType, chunkSize);
        clock_t cpuEnd = clock();
        double cpuTime = (double)(cpuEnd - cpuStart) / CLOCKS_PER_SEC;

        if (r >= WARMUP_RUNS) {
            wallTimes.push_back(wallTime);
            cpuTimes.push_back(cpuTime);
        }
    }

    double wallMean, wallStd, wallMin, wallMax;
    computeStats(wallTimes, wallMean, wallStd, wallMin, wallMax);

    double cpuMean, cpuStd, cpuMin, cpuMax;
    computeStats(cpuTimes, cpuMean, cpuStd, cpuMin, cpuMax);

    double speedup = seqTime / wallMean;

    string sched = (scheduleType == 0) ? "static" :
                   (scheduleType == 1) ? "dynamic" : "guided";
    string chunk = (chunkSize == 0) ? "auto" : to_string(chunkSize);

    cout << "Word " << n << "-gram | threads=" << numThreads
         << " | " << sched << " | chunk=" << chunk << endl;
    cout << "  wall: mean=" << wallMean << "s std=" << wallStd
         << "s min=" << wallMin << "s max=" << wallMax << "s" << endl;
    cout << "  cpu:  mean=" << cpuMean << "s" << endl;
    cout << "  speedup: " << speedup << "x" << endl;

    csv << "word," << n << "," << numThreads << "," << sched << "," << chunk << ","
        << wallMean << "," << wallStd << "," << wallMin << "," << wallMax << ","
        << cpuMean << "," << speedup << "\n";
}