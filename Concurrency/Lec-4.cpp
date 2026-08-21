#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <string>

// ============================================================
// PRODUCER-CONSUMER PROBLEM - COMPLETE EXPLANATION + CODE
// ============================================================

// ============================================================
// SECTION 1: WHAT IS THE PROBLEM?
// ============================================================

// PRODUCER: thread that CREATES data and puts it in a shared buffer
// CONSUMER: thread that TAKES data from the shared buffer and processes it
//
// THE PROBLEM: both access the SAME buffer (shared resource)
// -> Producer might add while Consumer is reading  = RACE CONDITION
// -> Consumer might read from EMPTY buffer         = CRASH
// -> Producer might fill a FULL buffer             = OVERFLOW
// -> Consumer might check "is empty?" and another
//    consumer steals the item before it can take   = RACE CONDITION
//
// REAL WORLD EXAMPLES:
// -> Logger: many threads produce log messages, one thread writes to file
// -> Network: network thread produces packets, sim thread consumes them
// -> YOUR ISSST: DIS PDU receiver (producer) + entity processor (consumer)
// -> Print queue: apps produce jobs, printer consumes them
// -> Web server: requests arrive (produced), worker threads handle (consume)

// ============================================================
// SECTION 2: THE WRONG WAY (NO SYNCHRONIZATION)
// ============================================================

// NEVER DO THIS:
namespace wrong
{
    std::queue<int> buffer;  // SHARED - no protection!

    void producer() {
        for (int i = 0; i < 5; i++) {
            buffer.push(i);        // RACE: what if consumer reads simultaneously?
        }
    }

    void consumer() {
        while (!buffer.empty()) {  // RACE: empty() might be true here
            int val = buffer.front(); // but another thread might pop() here!
            buffer.pop();            // and now front() returned garbage
            std::cout << val;
        }
    }
}
// Problems:
// 1. Two threads writing/reading std::queue at same time = data corruption
// 2. Consumer checks empty() then another consumer steals the item
// 3. No ordering guarantees

// ============================================================
// SECTION 3: THE THREE TOOLS NEEDED TO FIX IT
// ============================================================

// TOOL 1: std::mutex
// -> mutual exclusion lock
// -> only ONE thread can hold the lock at a time
// -> others WAIT (block) until lock is released
// -> prevents simultaneous access to shared data
//
// TOOL 2: std::lock_guard / std::unique_lock
// -> RAII wrapper for mutex
// -> lock_guard: acquires lock on construction, releases on destruction
// -> unique_lock: like lock_guard but more flexible (can unlock manually)
// -> prevents forgetting to unlock (even if exception thrown)
//
// TOOL 3: std::condition_variable
// -> allows threads to WAIT for a condition to become true
// -> consumer waits when buffer EMPTY (instead of spinning)
// -> producer NOTIFIES consumer when it adds data
// -> consumer waits when buffer FULL
// -> much more efficient than busy-waiting (spinning)

// ============================================================
// SECTION 4: THE CORRECT SOLUTION
// ============================================================

// SHARED STATE (protected by mutex):
std::queue<int>         g_buffer;        // the shared buffer
std::mutex              g_mutex;          // protects g_buffer
std::condition_variable g_cv;            // for producer-consumer signaling
bool                    g_done = false;  // signals producer is finished
const int               MAX_BUFFER = 5;  // max items in buffer at once

// ============================================================
// SECTION 5: PRODUCER THREAD
// ============================================================

void producer(int numItems)
{
    for (int i = 0; i < numItems; i++)
    {
        // STEP 1: acquire the lock (only this thread can modify buffer now)
        std::unique_lock<std::mutex> lock(g_mutex);
        //         ^^^^^^^^^^^^^
        //         unique_lock (not lock_guard) because we need
        //         to pass it to condition_variable::wait()

        // STEP 2: wait if buffer is FULL (don't overflow!)
        g_cv.wait(lock, []() {
            return g_buffer.size() < MAX_BUFFER;
        });
        // wait(lock, condition):
        // -> if condition TRUE: proceed (buffer has space)
        // -> if condition FALSE: ATOMICALLY release lock AND sleep
        // -> when notified: re-acquire lock AND re-check condition
        //    (re-check = "spurious wakeup" protection)

        // STEP 3: produce an item (we hold the lock, buffer has space)
        g_buffer.push(i);
        std::cout << "[Producer] produced: " << i
                  << " | buffer size: " << g_buffer.size() << '\n';

        // STEP 4: notify consumer that data is available
        g_cv.notify_all();
        // notify_all: wake up ALL waiting threads (consumers check condition)
        // notify_one: wake up ONE waiting thread (more efficient if one consumer)

        // STEP 5: lock automatically released here (unique_lock destructor)
        // -> consumer can now acquire lock and take the item

        // Simulate production time (optional - makes output readable)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Signal that producer is done:
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_done = true;
        std::cout << "[Producer] DONE producing\n";
    }
    g_cv.notify_all();  // wake consumers so they can see g_done = true
}

// ============================================================
// SECTION 6: CONSUMER THREAD
// ============================================================

void consumer(int id)
{
    while (true)  // keep consuming until producer is done AND buffer empty
    {
        // STEP 1: acquire the lock
        std::unique_lock<std::mutex> lock(g_mutex);

        // STEP 2: wait for data OR done signal
        g_cv.wait(lock, []() {
            return !g_buffer.empty() || g_done;
            // wake up when:
            // -> buffer has something to consume (can process)
            // OR
            // -> producer is done (time to exit even if empty)
        });

        // STEP 3: check exit condition
        if (g_buffer.empty() && g_done) {
            std::cout << "[Consumer " << id << "] exiting\n";
            break;  // done! no more items, producer finished
        }

        // STEP 4: consume one item (we hold the lock, buffer has data)
        int item = g_buffer.front();
        g_buffer.pop();
        std::cout << "[Consumer " << id << "] consumed: " << item
                  << " | buffer size: " << g_buffer.size() << '\n';

        // STEP 5: notify producer (buffer has space now)
        g_cv.notify_all();

        // STEP 6: lock released (unique_lock destructor)

        // Simulate processing time:
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
}

// ============================================================
// SECTION 7: RUNNING IT
// ============================================================

int main()
{
    std::cout << "=== Producer-Consumer Demo ===\n\n";

    // One producer, two consumers:
    std::thread producerThread(producer, 10);       // produces 10 items
    std::thread consumerThread1(consumer, 1);       // consumer 1
    std::thread consumerThread2(consumer, 2);       // consumer 2

    // Wait for all to finish:
    producerThread.join();
    consumerThread1.join();
    consumerThread2.join();

    std::cout << "\n=== All done ===\n";
    return 0;
}

// SAMPLE OUTPUT (order varies - concurrent!):
// [Producer] produced: 0 | buffer size: 1
// [Consumer 1] consumed: 0 | buffer size: 0
// [Producer] produced: 1 | buffer size: 1
// [Producer] produced: 2 | buffer size: 2
// [Consumer 2] consumed: 1 | buffer size: 1
// [Consumer 1] consumed: 2 | buffer size: 0
// ...
// [Producer] DONE producing
// [Consumer 1] exiting
// [Consumer 2] exiting
// === All done ===

// ============================================================
// SECTION 8: THE CONDITION VARIABLE MECHANISM EXPLAINED
// ============================================================

// WHY NOT JUST USE A while LOOP?
//
// BAD (busy-wait / spin-lock):
// while (g_buffer.empty()) {  // keeps checking continuously
//     // burns CPU doing NOTHING useful
//     // even while holding the lock (deadlock risk!)
// }
//
// GOOD (condition_variable):
// g_cv.wait(lock, condition);
// -> thread goes to SLEEP (releases CPU to other threads)
// -> OS wakes it up only when notified + condition true
// -> zero CPU waste while waiting
//
// HOW wait() WORKS INTERNALLY:
// 1. Check condition: if TRUE, return immediately (proceed)
// 2. If FALSE:
//    a. ATOMICALLY release the lock
//    b. Put THIS THREAD to sleep
//    c. When notify_one/notify_all called:
//    d. Wake up, RE-ACQUIRE the lock
//    e. RE-CHECK the condition (spurious wakeup protection!)
//    f. If condition now TRUE: proceed
//    g. If condition still FALSE: go back to sleep (step b)

// ============================================================
// SECTION 9: THE ISSST/DIS CONNECTION
// ============================================================

// YOUR REAL APPLICATION in DRDO simulation:
//
// struct EntityStatePDU { uint32_t entityID; float x,y,z; };
//
// std::queue<EntityStatePDU> g_pduQueue;
// std::mutex g_pduMutex;
// std::condition_variable g_pduCv;
// bool g_networkDone = false;
//
// // PRODUCER: network thread receiving DIS PDUs
// void networkReceiver() {
//     while (running) {
//         EntityStatePDU pdu = receiveFromUDP();  // blocks waiting for packet
//         {
//             std::lock_guard<std::mutex> lock(g_pduMutex);
//             g_pduQueue.push(pdu);
//         }
//         g_pduCv.notify_one();  // wake up simulator thread
//     }
// }
//
// // CONSUMER: simulation thread processing PDUs
// void simulationTick() {
//     while (running) {
//         std::unique_lock<std::mutex> lock(g_pduMutex);
//         g_pduCv.wait(lock, []{ return !g_pduQueue.empty(); });
//         EntityStatePDU pdu = g_pduQueue.front();
//         g_pduQueue.pop();
//         lock.unlock();  // release BEFORE processing (don't block network!)
//         updateEntityState(pdu);  // process without holding lock
//     }
// }
// ```

// **The problem in one picture:**

// ```
// WITHOUT SYNC:                    WITH SYNC (mutex + condvar):

// Producer  Consumer               Producer         Consumer
//    |          |                     |                  |
// push(1) ──────┤ ← RACE!          lock()            wait()  ← sleeping
//    |      pop()                  push(1)           [wakes up on notify]
//    |      (reads garbage!)       notify() ─────→   lock()
//                                  unlock()           pop(1)
//                                                     notify()
//                                                     unlock()
// ```

// **The condition_variable wait() explained:**

// ```cpp
// g_cv.wait(lock, []() { return !g_buffer.empty(); });

// // Internally:
// // if (!g_buffer.empty()) return;    // condition true → proceed
// // else:
// //   unlock(lock);                   // release lock (atomically!)
// //   sleep_until_notified();         // CPU free for other threads
// //   reacquire(lock);                // wake up, lock again
// //   if (g_buffer.empty()) sleep();  // check again (spurious wakeup!)
// ```

// **Compile and run:**
// ```bash
// g++ -std=c++17 -pthread producer_consumer.cpp -o pc && ./pc
// ```

// > **The ISSST takeaway**: your DIS network plugin is a classic producer-consumer — the UDP receive thread produces PDUs, the simulation/TDF thread consumes them. Without `mutex` + `condition_variable`, simultaneous access to the shared PDU queue causes data corruption. With them: network thread pushes safely, simulation thread sleeps when queue is empty, wakes instantly when a PDU arrives — exactly the low-latency behavior a real-time simulation needs.