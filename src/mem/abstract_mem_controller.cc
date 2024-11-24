#include "mem/abstract_mem_controller.hh"
#include "base/trace.hh"
#include "debug/MemController.hh"

AbstractMemoryController::AbstractMemoryController(const AbstractMemoryControllerParams* p)
    : MemObject(p),
      last_update(0),
      total_buffers(p->buffer_size),
      allocated_buffers(64, p->buffer_size/64) // Initially equal distribution
{
    DPRINTF(MemController, "Creating memory controller with %d buffers\n",
            total_buffers);
}

void
AbstractMemoryController::recordRequest(PacketPtr pkt) {
    RequestHistory entry;
    entry.cpu_id = pkt->req->masterId();
    entry.timestamp = curTick();
    request_history.push_back(entry);

    if (curTick() - last_update >= UPDATE_INTERVAL) {
        updateBufferAllocation();
        last_update = curTick();
    }
}

void
AbstractMemoryController::updateBufferAllocation() {
    // Write history to file
    std::ofstream history_file("request_history.txt");
    for (const auto& entry : request_history) {
        history_file << entry.cpu_id << "," << entry.timestamp << "\n";
    }
    history_file.close();

    // Execute Python script
    system("python3 calculate_ratios.py");

    // Read new ratios
    std::ifstream ratio_file("buffer_ratios.txt");
    buffer_ratios.clear();
    double ratio;
    while (ratio_file >> ratio) {
        buffer_ratios.push_back(ratio);
    }

    // Update buffer allocations
    for (size_t i = 0; i < allocated_buffers.size(); i++) {
        allocated_buffers[i] = static_cast<int>(total_buffers * buffer_ratios[i]);
    }

    // Clear history for next period
    request_history.clear();

    // Log the allocation
    std::ofstream log_file("buffer_allocation.log", std::ios::app);
    for (size_t i = 0; i < allocated_buffers.size(); i++) {
        log_file << i << "," << allocated_buffers[i] << ","
                << UPDATE_INTERVAL << "," << curTick() << "\n";
    }
}

bool
AbstractMemoryController::canAcceptRequest(PacketPtr pkt) {
    int cpu_id = pkt->req->masterId();
    // Count current usage for this CPU
    int current_usage = std::count_if(
        request_history.begin(),
        request_history.end(),
        [cpu_id](const RequestHistory& entry) {
            return entry.cpu_id == cpu_id;
        }
    );
    return current_usage < allocated_buffers[cpu_id];
}
