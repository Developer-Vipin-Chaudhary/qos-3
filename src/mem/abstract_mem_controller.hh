#ifndef __MEM_ABSTRACT_MEMORY_CONTROLLER_HH__
#define __MEM_ABSTRACT_MEMORY_CONTROLLER_HH__

#include "mem/mem_object.hh"
#include "params/AbstractMemoryController.hh"

class AbstractMemoryController : public MemObject {
private:
    struct RequestHistory {
        int cpu_id;
        Tick timestamp;
    };

    std::vector<RequestHistory> request_history;
    std::vector<double> buffer_ratios;
    Tick last_update;
    static const Tick UPDATE_INTERVAL = 100000000; // 100ms in ticks
    int total_buffers;
    std::vector<int> allocated_buffers;

public:
    void recordRequest(PacketPtr pkt);
    void updateBufferAllocation();
    bool canAcceptRequest(PacketPtr pkt);
};


#endif // __MEM_ABSTRACT_MEMORY_CONTROLLER_HH__
