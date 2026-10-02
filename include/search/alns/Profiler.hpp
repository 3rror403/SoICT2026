#pragma once
#include <chrono>
#include <string>
#include <map>
#include <iostream>

namespace soict::alns {

struct ProfileStats {
    long long calls = 0;
    long long total_ns = 0;
    long long candidates = 0;
    long long successes = 0;
    double gain = 0;
};

class Profiler {
    static inline std::map<std::string, ProfileStats> stats_;
public:
    static void add_stat(const std::string& name, long long ns, long long candidates = 0, long long successes = 0, double gain = 0.0) {
        auto& s = stats_[name];
        s.calls++;
        s.total_ns += ns;
        s.candidates += candidates;
        s.successes += successes;
        s.gain += gain;
    }
    static void print() {
        std::cerr << "--- PROFILING RESULTS ---\n";
        for (const auto& [name, s] : stats_) {
            double ms = s.total_ns / 1e6;
            double sec = s.total_ns / 1e9;
            std::cerr << name << ":\n  calls=" << s.calls 
                      << "\n  time=" << sec << "s (avg " << (ms / std::max(1LL, s.calls)) << "ms)"
                      << "\n  cands=" << s.candidates 
                      << "\n  succ=" << s.successes 
                      << "\n  gain=" << s.gain 
                      << "\n  gain/s=" << (sec > 0 ? s.gain / sec : 0) << "\n";
        }
        std::cerr << "-------------------------\n";
    }
};

struct ScopedTimer {
    std::string name;
    std::chrono::high_resolution_clock::time_point start;
    long long cands = 0;
    long long succ = 0;
    double gain = 0.0;
    ScopedTimer(std::string n) : name(n), start(std::chrono::high_resolution_clock::now()) {}
    ~ScopedTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        Profiler::add_stat(name, std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count(), cands, succ, gain);
    }
};

}
