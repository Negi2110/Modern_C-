#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <chrono>
#include <atomic>
#include <vector>

// ============================================================
// try_lock + PRODUCER-CONSUMER (DIS PLUGIN CONTEXT) - NOTES
// ============================================================

// ============================================================
// SECTION 1: lock() vs try_lock() - THE KEY DIFFERENCE
// ============================================================

// lock():
// -> BLOCKS until mutex is available
// -> thread goes to sleep, OS wakes it when free
// -> always succeeds (eventually)
// -> USE: when you MUST do the work (no alternative)
//
// try_lock():
// -> returns IMMEDIATELY (non-blocking)
// -> returns TRUE if lock acquired
// -> returns FALSE if already locked (someone else holds it)
// -> USE: when you have USEFUL WORK to do if lock is busy
//         OR when skipping is acceptable
//         OR to avoid deadlock (try both locks, back off if fail)

// ============================================================
// SECTION 2: YOUR CODE EXPLAINED
// ============================================================

std::mutex g_lock;

void job1()
{
    g_lock.lock();          // BLOCKS until free (will always execute)
    std::cout << "job1 is executed...\n";
    g_lock.unlock();
}

void job2()
{
    if (g_lock.try_lock())  // NON-BLOCKING: try to get lock
    {
        // GOT the lock: execute the job
        std::cout << "job2 is executed...\n";
        g_lock.unlock();
    }
    // DIDN'T get lock: job2 is SKIPPED silently
    // (no output, no wait, just moves on)
}

// POSSIBLE OUTPUTS:
// CASE 1: job1 runs first, finishes, job2 gets lock
// "job1 is executed..."
// "job2 is executed..."
//
// CASE 2: job2 runs first, try_lock fails (job1 holds it)
// "job1 is executed..."
// (job2 skipped - no output for job2)
//
// CASE 3: job2 runs first before job1 starts
// "job2 is executed..."
// "job1 is executed..."

// ============================================================
// SECTION 3: try_lock REAL USE CASES
// ============================================================

// USE CASE 1: DO OTHER WORK if lock busy
void realWorldTryLock(std::mutex& m, std::atomic<int>& counter)
{
    if (m.try_lock()) {
        // got lock: do the protected work
        counter++;
        m.unlock();
    } else {
        // lock busy: do something ELSE useful instead of waiting
        // e.g., process lower-priority items
        std::this_thread::yield();  // let other thread finish
        // retry later...
    }
}

// USE CASE 2: AVOID DEADLOCK (lock both or neither)
std::mutex m1, m2;
void safeMultiLock()
{
    while (true) {
        if (m1.try_lock()) {
            if (m2.try_lock()) {
                // GOT BOTH: do work safely
                // critical section here
                m2.unlock();
                m1.unlock();
                break;  // done!
            }
            m1.unlock();  // couldn't get m2, release m1 and retry
        }
        std::this_thread::sleep_for(std::chrono::microseconds(1));
        // back off briefly before retry (prevents livelock)
    }
    // BETTER: use std::lock(m1, m2) which does this automatically!
}

// USE CASE 3: DIS PLUGIN - TRY to update display, skip if busy
// If render thread holds the display lock and sim tick wants to update:
// -> try_lock: if render busy, skip this frame's update (not critical)
// -> lock: if PDU receive is critical (must not miss), always wait

// ============================================================
// SECTION 4: PRODUCER-CONSUMER IN YOUR DIS PLUGIN
// ============================================================

// YOUR QUESTION: "producer consumer from interview perspective
//                 and I think I faced it in DIS plugin"
//
// YES! Your DIS plugin is a CLASSIC producer-consumer:
//
// PRODUCER = Network receive thread
//   -> receives UDP packets (DIS PDUs) from the network
//   -> doesn't know when packets arrive (driven by network)
//   -> must NOT block (or we miss incoming PDUs)
//   -> PRODUCES: EntityStatePDU, FirePDU, DetPDU etc.
//
// CONSUMER = Simulation/TDF thread
//   -> processes PDUs to update entity states
//   -> runs on a tick (60Hz or similar)
//   -> CONSUMES: PDUs from the shared queue
//   -> updates tactical display
//
// SHARED BUFFER = PDU queue
//   -> queue between producer and consumer
//   -> must be thread-safe (both threads access it)

// ============================================================
// SECTION 5: DIS PLUGIN PRODUCER-CONSUMER IMPLEMENTATION
// ============================================================

// Types (simplified from your actual DIS code):
struct EntityStatePDU {
    uint32_t entityID;
    float x, y, z;
    uint8_t pduType;
};

// THE SHARED BUFFER:
class ThreadSafePDUQueue
{
    std::queue<EntityStatePDU> m_queue;
    mutable std::mutex          m_mutex;
    std::condition_variable     m_cv;
    std::atomic<bool>           m_running{true};
    static const size_t         MAX_SIZE = 1000;  // prevent memory overflow

public:
    // PRODUCER calls this (network thread):
    bool push(EntityStatePDU pdu)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_queue.size() >= MAX_SIZE) {
            // queue full: DROP oldest PDU (or drop newest, or block)
            // In DIS: dropping old state is usually acceptable
            // (next PDU will have fresh state)
            m_queue.pop();  // remove oldest
        }

        m_queue.push(std::move(pdu));
        m_cv.notify_one();  // wake up consumer
        return true;
    }

    // CONSUMER calls this (sim/TDF thread):
    // BLOCKING version: wait until PDU available
    bool pop(EntityStatePDU& pdu)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [this]{
            return !m_queue.empty() || !m_running;
        });

        if (!m_running && m_queue.empty()) return false; // shutdown

        pdu = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    // NON-BLOCKING version: try_lock approach
    // USE: when sim tick can't wait (must render even without new PDUs)
    bool tryPop(EntityStatePDU& pdu)
    {
        std::unique_lock<std::mutex> lock(m_mutex, std::try_to_lock);
        // std::try_to_lock = try_lock on the unique_lock

        if (!lock.owns_lock() || m_queue.empty()) {
            return false;  // busy or empty: skip this tick
        }

        pdu = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    // Pop ALL available PDUs (process everything in one tick):
    std::vector<EntityStatePDU> popAll()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<EntityStatePDU> batch;
        while (!m_queue.empty()) {
            batch.push_back(std::move(m_queue.front()));
            m_queue.pop();
        }
        return batch;  // process entire batch without holding lock
    }

    void shutdown() {
        m_running = false;
        m_cv.notify_all();  // wake all waiting threads
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }
};

// ============================================================
// SECTION 6: COMPLETE DIS PLUGIN THREADING EXAMPLE
// ============================================================

class DISPlugin
{
    ThreadSafePDUQueue  m_pduQueue;
    std::atomic<bool>   m_running{false};
    std::thread         m_networkThread;

    // Entity state (protected by separate mutex):
    std::unordered_map<uint32_t, EntityStatePDU> m_entities;
    mutable std::mutex m_entityMutex;

public:
    void start()
    {
        m_running = true;

        // PRODUCER thread: receive PDUs from network
        m_networkThread = std::thread([this]() {
            while (m_running) {
                EntityStatePDU pdu = receiveFromUDP();  // blocks ~1ms
                m_pduQueue.push(pdu);
                // push() notifies consumer via condition_variable
            }
        });
    }

    // Called from SIMULATION TICK (consumer role):
    void processPDUs()
    {
        // Process ALL available PDUs in one tick:
        auto batch = m_pduQueue.popAll();

        std::lock_guard<std::mutex> lock(m_entityMutex);
        for (const auto& pdu : batch) {
            m_entities[pdu.entityID] = pdu;
            // update entity state
        }
        // lock released, render thread can now read entities
    }

    // Called from RENDER thread (reader):
    std::vector<EntityStatePDU> getEntities() const
    {
        std::lock_guard<std::mutex> lock(m_entityMutex);
        std::vector<EntityStatePDU> result;
        for (const auto& [id, pdu] : m_entities) {
            result.push_back(pdu);
        }
        return result;  // copy released before lock unlock
    }

    void stop()
    {
        m_running = false;
        m_pduQueue.shutdown();
        if (m_networkThread.joinable())
            m_networkThread.join();
    }

private:
    EntityStatePDU receiveFromUDP() {
        // actual UDP receive code here
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        return EntityStatePDU{};
    }
};

// ============================================================
// SECTION 7: PRODUCER-CONSUMER INTERVIEW QUESTIONS
// ============================================================

// THE CLASSIC INTERVIEW QUESTION:
// "Implement a thread-safe producer-consumer queue"
//
// INTERVIEWER LOOKS FOR:
// 1. mutex to protect the queue
// 2. condition_variable for efficient waiting (not busy-spin)
// 3. Handling empty queue (consumer waits)
// 4. Handling full queue (producer blocks or drops)
// 5. Clean shutdown mechanism
// 6. Exception safety (what if push() throws?)
//
// FOLLOW-UP QUESTIONS:
// "What if there are multiple producers and consumers?"
// -> same code works! mutex handles N producers, M consumers
//
// "What if the queue gets too large?"
// -> add MAX_SIZE, producer blocks (back-pressure) or drops oldest
//
// "How would you avoid a deadlock?"
// -> release lock BEFORE notify_one() (best practice)
// -> never hold multiple locks without std::lock()
//
// "What is the difference between notify_one and notify_all?"
// -> notify_one: one consumer wakes (efficient for one consumer)
// -> notify_all: all consumers wake (use when all should check)
//
// "How is this related to std::async?"
// -> std::async is high-level producer-consumer!
//    producer: the async task computes
//    consumer: get() retrieves the result
//    buffer: the std::future holds the result

// ============================================================
// SECTION 8: try_lock IN DIS PLUGIN CONTEXT
// ============================================================

// WHERE try_lock helps in your ISSST simulation:
//
// SCENARIO 1: TDF display update
// Render runs at 60Hz. Simulation updates entity state.
// If render frame is being rendered when sim tries to update:
// -> try_lock: skip this update (render gets priority)
// -> next sim tick: try again
// -> display is 16ms stale: ACCEPTABLE for tactical display
//
// void simTick(std::mutex& displayMutex, EntityMap& entities) {
//     if (displayMutex.try_lock()) {
//         updateDisplayEntities(entities);  // fast update
//         displayMutex.unlock();
//     }
//     // if try_lock fails: skip, display shows previous frame
// }
//
// SCENARIO 2: PDU logging (non-critical)
// Received PDU: MUST process (use lock())
// Log the PDU: can skip (use try_lock())
//
// void receivePDU(EntityStatePDU pdu) {
//     // MUST process - use blocking lock:
//     { std::lock_guard<std::mutex> l(entityLock); entities[pdu.entityID]=pdu; }
//
//     // NICE TO LOG but not critical - use try_lock:
//     if (logMutex.try_lock()) {
//         logPDU(pdu);
//         logMutex.unlock();
//     }
//     // if logMutex busy: skip logging this PDU
// }

// ============================================================
// SECTION 9: INTERVIEW QUESTIONS - try_lock
// ============================================================

// Q1: What is try_lock and how does it differ from lock()?
//     -> try_lock(): non-blocking, returns true/false immediately.
//        lock(): blocking, waits until mutex available.
//        Use try_lock when you can do useful work if lock is busy.

// Q2: In your DIS plugin, where would you use try_lock vs lock()?
//     -> lock(): receiving PDUs (must not miss), updating entity state.
//        try_lock(): non-critical logging, optional display updates,
//        background statistics collection.

// Q3: What is the producer-consumer problem?
//     -> Producer creates data, consumer processes it.
//        Shared buffer between them must be thread-safe.
//        Problems: buffer empty (consumer waits), buffer full (producer waits).
//        Solution: mutex + condition_variable + bounded queue.

// Q4: How does condition_variable solve busy-waiting?
//     -> Without cv: while(queue.empty()) {} burns CPU.
//        With cv: thread sleeps until notified.
//        Producer calls notify_one() after push().
//        Consumer wakes up, processes, sleeps again if empty.

// Q5: What happens in your DIS plugin if the PDU queue grows unbounded?
//     -> Memory runs out, application crashes.
//        Fix: MAX_SIZE cap with drop policy (drop oldest/newest).
//        Or: producer blocks (back-pressure from consumer).
//        In real-time sim: dropping old state PDUs is usually safe.

// Q6: Is try_lock exception-safe?
//     -> try_lock() itself doesn't throw.
//        But if you hold the lock and throw: DEADLOCK (forgot unlock).
//        Fix: use unique_lock with try_to_lock:
//        std::unique_lock<std::mutex> lock(m, std::try_to_lock);
//        if (lock.owns_lock()) { /* safe: auto-unlocks! */ }

// Q7: What is std::try_to_lock?
//     -> A tag used with unique_lock to attempt try_lock:
//        std::unique_lock<std::mutex> lock(m, std::try_to_lock);
//        if (lock.owns_lock()) { /* got it */ }
//        RAII: auto-unlocks if lock was acquired (exception safe!)

// Q8: What is back-pressure in producer-consumer?
//     -> When consumer is slower than producer, queue grows.
//        Back-pressure: producer SLOWS DOWN or BLOCKS when queue full.
//        Prevents memory overflow.
//        Alternative: drop oldest items (acceptable in real-time sim).

// int main()
// {
//     // YOUR ORIGINAL CODE:
//     std::thread thread1(job1);
//     std::thread thread2(job2);

//     thread1.join();
//     thread2.join();

//     // POSSIBLE OUTPUTS (non-deterministic):
//     // "job1 is executed..." + "job2 is executed..."  (both ran)
//     // "job1 is executed..."                           (job2 skipped)
//     // "job2 is executed..." + "job1 is executed..."  (job2 first)

//     return 0;
// }
// ```

// **lock() vs try_lock() in one picture:**

// ```
// Thread calling lock():         Thread calling try_lock():
//   if (mutex free) → lock it     if (mutex free) → lock it, return true
//   if (mutex busy) → SLEEP... ←  if (mutex busy) → return false IMMEDIATELY
//                    SLEEP...                       do other work!
//                    SLEEP...
//   [notified]      → lock it
//   → continue
// ```

// **Your DIS Plugin = Producer-Consumer:**

// ```
// Network          │  Shared Queue      │  Simulation/TDF
// ─────────────────┼────────────────────┼─────────────────
// receiveUDP()     │  mutex             │  processTick()
//   push(PDU) ────→│→ [PDU1]            │→ popAll()
//   push(PDU) ────→│→ [PDU1][PDU2]      │  updateEntities()
//   push(PDU) ────→│→ [PDU1][PDU2][PDU3]│  renderDisplay()
//                  │  condition_var     │
//                  │  notify_one() ────→│ (wake consumer!)
//                  │  MAX_SIZE=1000     │
// ```

// > **The interview answer for DIS plugin**: "Yes, this is exactly the producer-consumer pattern. The network receive thread is the producer — it receives DIS PDUs asynchronously from UDP and pushes them to a thread-safe queue. The simulation tick thread is the consumer — it processes PDUs from the queue at each tick. The `condition_variable` ensures the consumer sleeps when the queue is empty (not busy-waiting), and `MAX_SIZE` prevents the queue from growing unbounded if the consumer falls behind the producer."