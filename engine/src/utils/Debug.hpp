#pragma once

#include <execinfo.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <cxxabi.h>
#include <iostream>
#include <sstream>
#include <string>
#include <dlfcn.h>
#include <iomanip>
#include <cstdio>
#include <map>
#include <chrono>
#include <thread>
#include <mutex>

namespace Debug {
    // Enhanced debugging state
    inline std::string lastKnownLocation;
    inline std::map<std::thread::id, std::string> threadLocations;
    inline std::map<void*, std::string> addressHistory;
    inline std::map<std::string, size_t> functionCallCount;
    inline std::chrono::high_resolution_clock::time_point lastUpdateTime;
    inline std::recursive_mutex debugMutex;
    
    // Track memory access patterns
    inline void recordMemoryAccess(void* addr, const std::string& operation) {
        std::lock_guard<std::recursive_mutex> lock(debugMutex);
        std::stringstream ss;
        ss << "Thread " << std::this_thread::get_id() 
           << " " << operation << " at " 
           << lastKnownLocation << " [" 
           << std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::high_resolution_clock::now() - lastUpdateTime).count() 
           << "ms since last update]";
        addressHistory[addr] = ss.str();
    }
    
    inline void setLocation(const std::string& location) {
        std::lock_guard<std::recursive_mutex> lock(debugMutex);
        lastKnownLocation = location;
        threadLocations[std::this_thread::get_id()] = location;
        functionCallCount[location]++;
        lastUpdateTime = std::chrono::high_resolution_clock::now();
    }
    
    inline std::string getSymbolName(void* addr) {
        std::lock_guard<std::recursive_mutex> lock(debugMutex);
        Dl_info info;
        std::stringstream ss;
        
        if (dladdr(addr, &info)) {
            if (info.dli_sname) {
                int status;
                char* demangled = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
                if (status == 0 && demangled) {
                    ss << demangled;
                    free(demangled);
                } else {
                    ss << info.dli_sname;
                }
                
                ss << std::hex << " (+0x" << 
                    static_cast<std::ptrdiff_t>(
                        reinterpret_cast<char*>(addr) - 
                        reinterpret_cast<char*>(info.dli_saddr)
                    ) << ")";
                
                // Add call count information
                if (functionCallCount.find(info.dli_sname) != functionCallCount.end()) {
                    ss << " [Called " << functionCallCount[info.dli_sname] << " times]";
                }
            } else {
                ss << "?? ";
            }
            ss << " in " << info.dli_fname;
            
            // Add memory access history
            if (addressHistory.find(addr) != addressHistory.end()) {
                ss << "\n    Last accessed: " << addressHistory[addr];
            }
        } else {
            ss << "?? at " << addr;
        }
        return ss.str();
    }

    inline void printBacktrace() {
        std::lock_guard<std::recursive_mutex> lock(debugMutex);
        void* array[100];
        int size = backtrace(array, 100);
        
        std::cerr << "\n=== ULTIMATE DEBUG BACKTRACE ===\n";
        std::cerr << "Last known location: " << lastKnownLocation << "\n";
        
        // Print thread-specific locations
        std::cerr << "\nActive Thread Locations:\n";
        for (const auto& [threadId, location] : threadLocations) {
            std::cerr << "Thread " << threadId << ": " << location << "\n";
        }
        
        // Print function call statistics
        std::cerr << "\nFunction Call Statistics:\n";
        for (const auto& [func, count] : functionCallCount) {
            std::cerr << func << ": " << count << " calls\n";
        }
        
        std::cerr << "\nDetailed Stack Trace:\n";
        for (int i = 0; i < size; ++i) {
            std::cerr << "[" << std::setw(2) << i << "]: " << getSymbolName(array[i]) << "\n";
        }
        
        // Memory access patterns
        std::cerr << "\nRecent Memory Access Patterns:\n";
        for (const auto& [addr, history] : addressHistory) {
            std::cerr << "0x" << std::hex << addr << ": " << history << "\n";
        }
        
        char** messages = backtrace_symbols(array, size);
        if (messages) {
            std::cerr << "\nRaw backtrace for comparison:\n";
            for (int i = 0; i < size; ++i) {
                std::cerr << "[" << std::setw(2) << i << "]: " << messages[i] << "\n";
            }
            free(messages);
        }
        
        std::cerr << "\nTime since last update: " 
                  << std::chrono::duration_cast<std::chrono::milliseconds>(
                         std::chrono::high_resolution_clock::now() - lastUpdateTime).count() 
                  << "ms\n";
        
        std::cerr << "=== END ULTIMATE DEBUG BACKTRACE ===\n" << std::endl;
    }

    inline void signalHandler(int sig) {
        std::cerr << "\n!!!!! CRITICAL ERROR DETECTED !!!!!\n";
        std::cerr << "Signal " << sig << " (" << strsignal(sig) << ") caught!\n";
        printBacktrace();
        
        // Restore default handler and re-raise
        signal(sig, SIG_DFL);
        raise(sig);
    }

    inline void installSignalHandlers() {
        signal(SIGSEGV, signalHandler);
        signal(SIGABRT, signalHandler);
        signal(SIGFPE, signalHandler);
        signal(SIGILL, signalHandler);
        signal(SIGTERM, signalHandler);
        lastUpdateTime = std::chrono::high_resolution_clock::now();
    }
    
    // Memory tracking macros
    #define DEBUG_LOCATION(loc) Debug::setLocation(loc)
    #define DEBUG_MEMORY_READ(addr) Debug::recordMemoryAccess(addr, "read")
    #define DEBUG_MEMORY_WRITE(addr) Debug::recordMemoryAccess(addr, "write")
} 