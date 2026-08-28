#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

// ============================================================
// CONDITION VARIABLE - WORKER/REPORTER PATTERN - COMPLETE NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS A CONDITION VARIABLE?
// ============================================================

// YOUR QUESTION: "what is worker or reporting thread?"
// YOUR COMMENT: "t1,t2,t3 can access the critical section at once
//                and keep checking if it is locked or free"
//
// PROBLEM: busy-waiting is wasteful
// BAD WAY (spin-lock = burns CPU):
// while (result == 0) { }  // thread keeps checking, wastes 100% CPU
// std::cout << result;
//
// GOOD WAY: condition variable = thread SLEEPS until notified
// -> reporter thread SLEEPS (releases CPU completely)
// -> worker thread does its work
// -> worker WAKES UP reporter when done
// -> reporter reads result (no wasted CPU!)
//
// YOUR 4 COMPONENTS:
// 1. bool notified          = the CONDITION (has work been done?)
// 2. std::unique_lock       = the LOCK (protects shared data)
// 3. std::condition_variable= the SIGNAL mechanism (sleep/wake)
// 4. Two threads:
//    WORKER   = does computation, notifies when done
//    REPORTER = waits for result, prints when available

// ============================================================
// SECTION 2: THE THREE OBJECTS
// ============================================================

std::mutex              gLock;              // protects shared state
std::condition_variable gConditionVariable; // for sleep/wake signaling

// ============================================================
// SECTION 3: YOUR CODE EXPLAINED - BUG + FIX
// ============================================================

// YOUR CODE HAS A SUBTLE BUG:
// if(!notified) {
//     gConditionVariable.wait(lock);  // wait WITHOUT condition!
// }
//
// PROBLEM WITH PLAIN wait():
// "Spurious wakeups" = thread wakes up for NO reason
// -> condition_variable can wake up randomly without notify
// -> if(!notified) checked once, then plain wait() sleeps
// -> spurious wakeup: thread wakes, but notified might still be false!
// -> reporter prints BEFORE worker finishes
//
// ANOTHER PROBLEM: RACE CONDITION
// Worker might call notify_one() BEFORE reporter reaches wait()
// -> reporter waits forever (missed the notification!)
//
// FIX: use wait() WITH PREDICATE (lambda):
// gConditionVariable.wait(lock, [&notified]{ return notified; });
// -> re-checks condition after EVERY wakeup (spurious or real)
// -> if notified=false after wakeup: goes back to sleep automatically

// ============================================================
// SECTION 4: CORRECTED CODE
// ============================================================

void correctExample()
{
    int result   = 0;
    bool notified = false;

    // REPORTER THREAD: waits for worker to finish
    std::thread reporter([&]
    {
        std::unique_lock<std::mutex> lock(gLock);
        // wait() atomically:
        // 1. CHECKS predicate (notified==true?)
        // 2. If FALSE: releases lock + puts thread to SLEEP
        // 3. When notified: re-acquires lock + re-checks predicate
        // 4. If TRUE: proceeds
        gConditionVariable.wait(lock, [&notified]{
            return notified;  // wake up ONLY when notified = true
        });
        // HERE: we have the lock AND notified is true
        // worker has finished and released the lock
        std::cout << "Reporter: result = " << result << '\n';
        // lock released automatically (unique_lock destructor)
    });

    // WORKER THREAD: does computation, then notifies
    std::thread worker([&]
    {
        std::unique_lock<std::mutex> lock(gLock);
        // We have the lock - do our work:
        result = 42 + 1 + 7;  // = 50

        // Simulate long computation:
        std::this_thread::sleep_for(std::chrono::seconds(5));
        // NOTE: we sleep WHILE HOLDING THE LOCK!
        // Reporter is blocked in wait() (which releases the lock)
        // so reporter CAN'T proceed while we sleep
        // (this is why reporter doesn't print before worker is done)

        // Signal completion:
        notified = true;
        std::cout << "Worker: work complete!\n";

        // Release lock BEFORE notify (best practice):
        lock.unlock();
        // WHY unlock before notify?
        // If we notify while holding lock:
        // reporter wakes up, tries to lock -> IMMEDIATELY blocked again
        // (we still hold it) -> extra context switch wasted
        // Unlock first -> reporter can lock immediately on wake

        gConditionVariable.notify_one();
        // wake up ONE waiting thread (the reporter)
    });

    reporter.join();
    worker.join();
    std::cout << "Program complete\n";
}

// ============================================================
// SECTION 5: SEQUENCE DIAGRAM - WHAT ACTUALLY HAPPENS
// ============================================================

// TIME →
// Reporter                    Worker                    gLock
//   |                           |                         FREE
//   | unique_lock(gLock)        |                         LOCKED by reporter
//   | wait(lock, pred)          |                         FREE (wait releases!)
//   | [sleeping...]             |                         ↑ key!
//   |                           | unique_lock(gLock)      LOCKED by worker
//   |                           | result = 50
//   |                           | sleep(5 seconds)        still locked!
//   |                           | notified = true
//   |                           | lock.unlock()           FREE
//   |                           | notify_one() ──→ wake! 
//   | [wakes up]                |
//   | re-acquires gLock         |                         LOCKED by reporter
//   | checks: notified==true? ✅|
//   | cout << result (50)       |
//   | lock released             |                         FREE
//   [reporter done]             [worker done]
//
// OUTPUT:
// Worker: work complete!
// Reporter: result = 50
// Program complete

// ============================================================
// SECTION 6: WHY unique_lock (NOT lock_guard)?
// ============================================================

// condition_variable::wait() REQUIRES unique_lock
// REASON: wait() needs to UNLOCK the mutex while sleeping
//         and RELOCK it when waking up
//
// lock_guard: cannot unlock (no unlock() method)
// unique_lock: CAN unlock/lock manually (and wait() uses this)
//
// wait() INTERNALLY does:
// 1. lock.unlock()        ← releases lock so others can proceed
// 2. sleep()              ← thread sleeps (zero CPU usage)
// 3. [notify_one called]
// 4. lock.lock()          ← reacquires lock
// 5. check predicate()    ← is condition really true?
// 6. if false: goto 1     ← spurious wakeup, sleep again

// ============================================================
// SECTION 7: SPURIOUS WAKEUPS - WHY THE PREDICATE MATTERS
// ============================================================

// SPURIOUS WAKEUP = thread wakes from wait() for NO REASON
// -> guaranteed to happen occasionally on all platforms
// -> POSIX spec explicitly allows this
// -> OS implementation detail (signal interruption, etc.)
//
// WITHOUT predicate (YOUR ORIGINAL CODE - BUGGY):
// gConditionVariable.wait(lock);
// Spurious wakeup -> reporter continues even though notified=false!
// -> reporter prints result=0 (WRONG! worker hasn't computed yet)
//
// WITH predicate (CORRECT):
// gConditionVariable.wait(lock, []{ return notified; });
// Spurious wakeup -> checks notified -> false -> goes BACK to sleep
// -> reporter only proceeds when notified is ACTUALLY true
//
// RULE: ALWAYS use wait() with a predicate lambda
// It is equivalent to:
// while (!predicate()) {
//     gConditionVariable.wait(lock);
// }

// ============================================================
// SECTION 8: notify_one vs notify_all
// ============================================================

// notify_one():
// -> wakes up ONE waiting thread (arbitrary which one)
// -> use when: one worker to be woken, producer-consumer
// -> YOUR CODE uses this (correct: only one reporter)
//
// notify_all():
// -> wakes up ALL waiting threads
// -> each thread re-checks its predicate
// -> only threads whose predicate is true proceed
// -> use when: multiple threads waiting for same condition
//
// EXAMPLE - multiple reporters:
// std::thread r1([&]{ gCV.wait(lock, pred); cout << "r1\n"; });
// std::thread r2([&]{ gCV.wait(lock, pred); cout << "r2\n"; });
// std::thread r3([&]{ gCV.wait(lock, pred); cout << "r3\n"; });
//
// notify_one() -> only ONE of r1,r2,r3 wakes (unpredictable)
// notify_all() -> ALL of r1,r2,r3 wake up -> all print

// ============================================================
// SECTION 9: ISSST/DIS CONNECTION
// ============================================================

// WORKER/REPORTER = common pattern in your simulation:
//
// WORKER  = network receive thread (receives DIS PDUs)
// REPORTER= simulation tick thread (waits for PDUs to process)
//
// std::mutex              pduMutex;
// std::condition_variable pduCV;
// std::queue<EntityStatePDU> pduQueue;
// bool newPDUAvailable = false;
//
// Network thread (WORKER):
// void receiveThread() {
//     while (running) {
//         EntityStatePDU pdu = receiveFromUDP();  // blocking
//         {
//             std::unique_lock<std::mutex> lock(pduMutex);
//             pduQueue.push(pdu);
//             newPDUAvailable = true;
//         }  // unlock BEFORE notify (best practice)
//         pduCV.notify_one();  // wake sim thread
//     }
// }
//
// Simulation thread (REPORTER):
// void simThread() {
//     while (running) {
//         std::unique_lock<std::mutex> lock(pduMutex);
//         pduCV.wait(lock, []{ return !pduQueue.empty(); });
//         // woke up AND queue has data
//         EntityStatePDU pdu = pduQueue.front();
//         pduQueue.pop();
//         lock.unlock();  // release before processing
//         processEntityState(pdu);  // update simulation
//     }
// }

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is a condition variable?
//     -> Synchronization primitive that allows threads to SLEEP
//        until a condition becomes true. Avoids busy-waiting.
//        Thread releases mutex while sleeping (efficient).
//        Another thread notifies to wake it up.

// Q2: Why must you use unique_lock with condition_variable?
//     -> wait() needs to release the mutex while sleeping and
//        reacquire it when waking. unique_lock supports manual
//        unlock()/lock(). lock_guard cannot unlock, so it can't
//        be used with condition_variable::wait().

// Q3: What is a spurious wakeup and how do you handle it?
//     -> Thread wakes from wait() without notify being called.
//        Guaranteed to happen occasionally on all platforms.
//        Handle: always use wait() with a predicate lambda.
//        Predicate re-checked after every wakeup (real or spurious).

// Q4: What is the difference between notify_one and notify_all?
//     -> notify_one(): wakes ONE arbitrary waiting thread.
//        notify_all(): wakes ALL waiting threads (each re-checks predicate).
//        Use notify_all when multiple threads wait for same condition.

// Q5: Should you unlock before or after notify_one()?
//     -> BEFORE is best practice (though both work).
//        Unlock then notify: woken thread immediately acquires lock.
//        Notify then unlock: woken thread wakes but immediately blocks
//        waiting for YOUR unlock (wasted context switch).

// Q6: What is the worker/reporter pattern?
//     -> Worker thread performs computation, reporter waits for result.
//        Reporter sleeps (releases CPU) until worker is done.
//        Worker notifies via condition_variable when finished.
//        Common in: simulation ticks, background computation, I/O.

// Q7: What is the bug in the original code?
//     -> Two issues:
//        1. wait() without predicate (spurious wakeup not handled).
//        2. notify_one() called while lock still held (inefficient).
//        Fix: wait(lock, pred) and lock.unlock() before notify.

// Q8: What are the three pieces needed for condition_variable?
//     -> 1. std::mutex:              protects shared state
//        2. std::unique_lock:        lockable/unlockable wrapper
//        3. std::condition_variable: sleep/wake signaling
//        Plus: shared data (result) and condition flag (notified).

int main()
{
    int result   = 0;
    bool notified = false;

    std::thread reporter([&]
    {
        std::unique_lock<std::mutex> lock(gLock);
        gConditionVariable.wait(lock, [&notified]{
            return notified;  // predicate: wake only when true
        });
        std::cout << "Reporter: result = " << result << '\n';
    });

    std::thread worker([&]
    {
        std::unique_lock<std::mutex> lock(gLock);
        result   = 42 + 1 + 7;  // 50
        notified = true;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout << "Worker: work complete!\n";
        lock.unlock();               // unlock BEFORE notify (best practice)
        gConditionVariable.notify_one();
    });

    reporter.join();
    worker.join();
    std::cout << "Program complete\n";
    return 0;
}
// ```

// **The condition_variable lifecycle:**

// ```
// Reporter Thread:                Worker Thread:
//   unique_lock(gLock)              unique_lock(gLock)
//   wait(lock, pred):               ← BLOCKED (reporter holds lock)
//     check pred → false
//     RELEASE lock ────────────────→ worker gets lock!
//     SLEEP 💤                       result = 50
//                                    notified = true
//                                    sleep(5 seconds)
//                                    lock.unlock()
//     WAKE UP 👁 ←───────────────── notify_one()
//     RE-ACQUIRE lock
//     check pred → true ✅
//     cout << result (50)
// ```

// **Always use wait with predicate:**

// ```cpp
// // BUGGY (spurious wakeup not handled):
// gCV.wait(lock);              // wakes randomly → might print result=0!

// // CORRECT (predicate re-checked every wakeup):
// gCV.wait(lock, []{ return notified; });
// // equivalent to:
// while (!notified) { gCV.wait(lock); }  // keeps sleeping until truly true
// ```

// > **The pattern in one sentence**: reporter calls `wait(lock, pred)` which atomically releases the lock and sleeps — giving worker the chance to run, set the result, and `notify_one()` — then reporter wakes up, re-acquires the lock, verifies the predicate, and safely reads the result. Zero CPU wasted while waiting, zero race conditions on the shared data.