#include <memory>
#include <thread>
#include <chrono>
#include <semaphore>


class StorageZone {
private:
    std::unique_ptr<std::counting_semaphore<>> sem;
    
public:
    StorageZone(int max_terminals) 
        : sem(std::make_unique<std::counting_semaphore<>>(max_terminals)) {}

    bool useTerminal(int work_time) { 
        if(sem->try_acquire_for(std::chrono::seconds(5))) {
            std::this_thread::sleep_for(std::chrono::seconds(work_time));
            sem->release();
            return true;
        }
        return false;
    }
};
