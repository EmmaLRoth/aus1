#ifndef HEAP_HEAP_MONITOR_HPP
#define HEAP_HEAP_MONITOR_HPP

#ifndef NDEBUG

#include <cstdint>
#include <map>
#include <string>

namespace heap {

class HeapMonitor {
public:
    HeapMonitor(const HeapMonitor&) = delete;
    void operator= (const HeapMonitor&) = delete;
    HeapMonitor(HeapMonitor&&) = delete;
    void operator= (HeapMonitor&&) = delete;

    static HeapMonitor &getInstance() {
        static HeapMonitor instance;
        return instance;
    }

public:
    ~HeapMonitor();
    void logAllocation(void *p, std::string file, int line);
    void logDeletion(void *p);

private:
    HeapMonitor() = default;

private:
    std::map<std::intptr_t, std::string> m_allocations;
};

} // namespace heap

void *operator new (std::size_t sz, const char *file, int line);

void operator delete (void *p) noexcept;

void operator delete (void *p, std::size_t n) noexcept;

#define new new (__FILE__, __LINE__)

#endif // NDEBUG

#endif // HEAP-MONITOR_HPP
