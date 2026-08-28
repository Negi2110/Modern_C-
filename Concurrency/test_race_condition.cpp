```cpp
// ============================================================
// THREAD SANITIZER (TSan) WITH GTEST AND QTEST - NOTES
// ============================================================

// ============================================================
// SECTION 1: WHAT IS ThreadSanitizer?
// ============================================================

// ThreadSanitizer (TSan) = Google's runtime data race detector
// -> detects races WHILE YOUR PROGRAM RUNS
// -> reports: which file, which line, which threads caused the race
// -> 5-15x slower than normal (test builds only!)
// -> catches races that code review CANNOT see
//
// WORKS BY:
// -> instruments every memory access at compile time
// -> at runtime: tracks which thread accessed which memory when
// -> if two threads access same memory (one writing) without sync:
//    REPORTS the exact race with stack traces

// ============================================================
// SECTION 2: GOOGLE TEST (GTEST) WITH TSan
// ============================================================

// ---- CMakeLists.txt setup ----

// cmake_minimum_required(VERSION 3.14)
// project(MyProject)
//
// find_package(GTest REQUIRED)
//
// # Option 1: Enable TSan for ALL tests
// add_compile_options(-fsanitize=thread -g -O1)
// add_link_options(-fsanitize=thread)
//
// # Option 2: Separate TSan test target (RECOMMENDED)
// add_executable(my_tests_tsan
//     tests/test_threading.cpp
//     tests/test_shared_value.cpp
//     src/shared_value.cpp
// )
// target_link_libraries(my_tests_tsan GTest::gtest_main)
// target_compile_options(my_tests_tsan PRIVATE
//     -fsanitize=thread
//     -g          # debug symbols (exact line numbers)
//     -O1         # light optimization (TSan needs this)
// )
// target_link_options(my_tests_tsan PRIVATE -fsanitize=thread)
//
// # Normal tests WITHOUT TSan (for speed)
// add_executable(my_tests
//     tests/test_threading.cpp
//     src/shared_value.cpp
// )
// target_link_libraries(my_tests GTest::gtest_main)

// ============================================================
// SECTION 3: WRITING GTEST THREAD TESTS
// ============================================================

// test_threading.cpp

#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>

// ---- TEST 1: Detect race WITHOUT fix (TSan should catch this) ----
// NOTE: this test is INTENTIONALLY broken to show TSan output
// In real code: comment this out or use TSAN_OPTIONS to suppress
// Run with TSan to SEE the race report, then enable the fix

TEST(DataRaceTest, DISABLED_RaceConditionDetected)
{
    // DISABLED_ prefix skips by default
    // Run with: --gtest_also_run_disabled_tests to see TSan catch it
    int shared = 0;

    std::thread t1([&]() { for (int i=0; i<1000; i++) shared++; });
    std::thread t2([&]() { for (int i=0; i<1000; i++) shared++; });

    t1.join();
    t2.join();

    // TSan OUTPUT when run:
    // WARNING: ThreadSanitizer: data race
    //   Write of size 4 at 0x... by thread T2:
    //     #0 DataRaceTest_RaceConditionDetected_Test::TestBody()
    //        test_threading.cpp:40
    //   Previous write of size 4 at 0x... by thread T1:
    //     #0 DataRaceTest_RaceConditionDetected_Test::TestBody()
    //        test_threading.cpp:39
    //   SUMMARY: ThreadSanitizer: data race
}

// ---- TEST 2: Correct mutex usage (TSan should pass) ----

TEST(DataRaceTest, MutexProtectedIncrement)
{
    std::mutex m;
    int shared = 0;
    const int NUM_THREADS    = 100;
    const int INCREMENTS_EACH = 100;

    std::vector<std::thread> threads;
    threads.reserve(NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) {
        threads.emplace_back([&]() {
            for (int j = 0; j < INCREMENTS_EACH; j++) {
                std::lock_guard<std::mutex> lock(m);
                shared++;
            }
        });
    }

    for (auto& t : threads) t.join();

    // TSan: no race detected ✅
    EXPECT_EQ(shared, NUM_THREADS * INCREMENTS_EACH);
    // 100 threads * 100 increments = 10000 ALWAYS
}

// ---- TEST 3: Atomic (TSan should pass) ----

TEST(DataRaceTest, AtomicIncrement)
{
    std::atomic<int> shared = 0;

    std::vector<std::thread> threads;
    for (int i = 0; i < 1000; i++) {
        threads.emplace_back([&]() { shared++; });
    }
    for (auto& t : threads) t.join();

    EXPECT_EQ(shared.load(), 1000);
    // Atomic: always correct, TSan: no race ✅
}

// ---- TEST 4: Producer-Consumer pattern ----

#include <queue>
#include <condition_variable>

TEST(ConcurrencyTest, ProducerConsumerCorrect)
{
    std::queue<int> buffer;
    std::mutex m;
    std::condition_variable cv;
    bool done = false;
    std::vector<int> results;
    const int NUM_ITEMS = 100;

    // Producer thread:
    std::thread producer([&]() {
        for (int i = 0; i < NUM_ITEMS; i++) {
            {
                std::lock_guard<std::mutex> lock(m);
                buffer.push(i);
            }
            cv.notify_one();
        }
        {
            std::lock_guard<std::mutex> lock(m);
            done = true;
        }
        cv.notify_all();
    });

    // Consumer thread:
    std::thread consumer([&]() {
        while (true) {
            std::unique_lock<std::mutex> lock(m);
            cv.wait(lock, [&]{ return !buffer.empty() || done; });

            while (!buffer.empty()) {
                results.push_back(buffer.front());
                buffer.pop();
            }

            if (done && buffer.empty()) break;
        }
    });

    producer.join();
    consumer.join();

    EXPECT_EQ((int)results.size(), NUM_ITEMS);
    // TSan: no races ✅ (all accesses protected by mutex)
}

// ---- TEST 5: Timeout test (thread doesn't hang) ----

TEST(ConcurrencyTest, ThreadCompletesInTime)
{
    std::atomic<bool> completed = false;

    std::thread worker([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        completed.store(true);
    });

    worker.join();

    // Verify completion (not just "did it crash"):
    EXPECT_TRUE(completed.load());
}

// ---- TEST 6: Future/async test ----

#include <future>

TEST(AsyncTest, AsyncReturnsCorrectResult)
{
    auto square = [](int x) { return x * x; };

    auto f1 = std::async(std::launch::async, square, 5);
    auto f2 = std::async(std::launch::async, square, 10);
    auto f3 = std::async(std::launch::async, square, 15);

    EXPECT_EQ(f1.get(), 25);
    EXPECT_EQ(f2.get(), 100);
    EXPECT_EQ(f3.get(), 225);
    // TSan: no races ✅ (async/future handle synchronization)
}

// ============================================================
// SECTION 4: QTest WITH TSan (for your Qt/ISSST work)
// ============================================================

// ---- CMakeLists.txt for Qt ----

// find_package(Qt5 COMPONENTS Test REQUIRED)
//
// add_executable(issst_tests_tsan
//     tests/test_dis_threading.cpp
//     src/PDUSender.cpp
//     src/EntityStateManager.cpp
// )
//
// target_link_libraries(issst_tests_tsan
//     Qt5::Test
//     Qt5::Core
//     Qt5::Network
// )
//
// target_compile_options(issst_tests_tsan PRIVATE
//     -fsanitize=thread -g -O1
// )
// target_link_options(issst_tests_tsan PRIVATE
//     -fsanitize=thread
// )

// ---- test_dis_threading.cpp (QTest style) ----

// #include <QtTest/QtTest>
// #include <QThread>
// #include <QMutex>
// #include <QAtomicInt>
// #include <QFuture>
// #include <QtConcurrent>
//
// class TestDISThreading : public QObject
// {
//     Q_OBJECT
//
// private slots:
//
//     // ---- QTest: basic thread safety ----
//     void test_atomicCounter()
//     {
//         QAtomicInt counter = 0;
//         QVector<QThread*> threads;
//
//         for (int i = 0; i < 100; i++) {
//             auto* t = QThread::create([&counter]() {
//                 for (int j = 0; j < 100; j++) {
//                     counter.fetchAndAddOrdered(1);
//                 }
//             });
//             threads.append(t);
//             t->start();
//         }
//
//         for (auto* t : threads) { t->wait(); delete t; }
//
//         QCOMPARE(counter.loadAcquire(), 10000);
//         // TSan: no race (QAtomicInt = Qt's atomic type)
//     }
//
//     // ---- QTest: mutex protection ----
//     void test_mutexProtectedMap()
//     {
//         QMutex mutex;
//         QMap<int, int> entityMap;
//
//         // Writer threads:
//         QVector<QThread*> writers;
//         for (int i = 0; i < 10; i++) {
//             auto* t = QThread::create([&mutex, &entityMap, i]() {
//                 QMutexLocker locker(&mutex);
//                 entityMap.insert(i, i * i);
//             });
//             writers.append(t);
//             t->start();
//         }
//         for (auto* t : writers) { t->wait(); delete t; }
//
//         QCOMPARE(entityMap.size(), 10);
//         // TSan: no race ✅
//     }
//
//     // ---- QTest: signal/slot across threads ----
//     void test_signalSlotCrossThread()
//     {
//         QAtomicInt received = 0;
//         QTimer timer;
//
//         // signals/slots with Qt::QueuedConnection are thread-safe
//         // (Qt's event loop handles the synchronization)
//         // Test that signals from worker thread are received:
//
//         // ... (requires QApplication/QCoreApplication event loop)
//         // In practice: use QSignalSpy for this
//     }
//
//     // ---- QTest: QtConcurrent (thread pool) ----
//     void test_qtConcurrentMap()
//     {
//         QVector<int> input{1, 2, 3, 4, 5, 6, 7, 8};
//
//         // map: apply function to each element in parallel
//         QFuture<int> future = QtConcurrent::mapped(input,
//             [](int x) { return x * x; });
//
//         future.waitForFinished();
//
//         QVector<int> results = future.results();
//         QCOMPARE(results.size(), 8);
//         // TSan: no race (QtConcurrent handles synchronization)
//     }
// };
//
// QTEST_MAIN(TestDISThreading)
// #include "test_dis_threading.moc"

// ============================================================
// SECTION 5: RUNNING TESTS WITH TSan
// ============================================================

// BUILD:
// mkdir build_tsan && cd build_tsan
// cmake .. -DCMAKE_BUILD_TYPE=Debug \
//          -DCMAKE_CXX_FLAGS="-fsanitize=thread -g -O1" \
//          -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=thread"
// make -j4
//
// RUN ALL TESTS:
// ./my_tests_tsan                          # run all
// ./my_tests_tsan --gtest_filter="*Race*" # specific tests
//
// RUN WITH TSAN OPTIONS:
// TSAN_OPTIONS="halt_on_error=1 log_path=tsan_report" ./my_tests_tsan
// -> halt_on_error=1: stop on FIRST race (easier to debug)
// -> log_path=tsan_report: save reports to tsan_report.PID files
//
// SUPPRESS KNOWN FALSE POSITIVES:
// Create suppression file: tsan.supp
// race:^ClassName::methodName$
//
// TSAN_OPTIONS="suppressions=tsan.supp" ./my_tests_tsan
//
// FOR QTEST:
// ./issst_tests_tsan -v2              # verbose output
// ./issst_tests_tsan -functions test_atomicCounter  # specific test

// ============================================================
// SECTION 6: INTERPRETING TSan OUTPUT
// ============================================================

// TYPICAL TSan RACE REPORT:
// ==================
// WARNING: ThreadSanitizer: data race (pid=1234)
//   Write of size 4 at 0x7f8b2c001a40 by thread T3 (mutexes held: {}):
//     #0 shared_value_increment() datarace.cpp:15
//     #1 std::thread::_Invoker...
//
//   Previous read of size 4 at 0x7f8b2c001a40 by thread T1:
//     #0 shared_value_increment() datarace.cpp:15
//     #1 std::thread::_Invoker...
//
//   Thread T3 (tid=1237, running) created at:
//     #0 pthread_create (/usr/lib/libasan.so)
//     #1 std::thread::thread() main.cpp:45
//
// READING THE REPORT:
// "Write of size 4" = writing an int (4 bytes)
// "at 0x7f8b..." = memory address of shared_value
// "by thread T3" = which thread wrote
// "#0 function() file:line" = exact location of the race
// "(mutexes held: {})" = no mutex held = UNPROTECTED!
//
// FIX: add mutex around that file:line
// Re-run: no more race report = fixed!

// ============================================================
// SECTION 7: ISSST-SPECIFIC TEST EXAMPLES
// ============================================================

// For your DIS/ISSST simulation - what to test with TSan:

// TEST: Entity state thread safety
// TEST(DISTest, EntityStateThreadSafe) {
//     EntityStateManager manager;
//     std::atomic<bool> done = false;
//
//     // Simulate network thread writing:
//     std::thread networkThread([&]() {
//         for (int i = 0; i < 1000; i++) {
//             EntityStatePDU pdu;
//             pdu.entityID = i % 10;
//             pdu.x = i * 1.0f;
//             manager.updateEntity(pdu);  // should be mutex-protected
//         }
//         done = true;
//     });
//
//     // Simulate render thread reading:
//     std::thread renderThread([&]() {
//         while (!done) {
//             auto entities = manager.getAllEntities(); // should be safe
//             // if manager.updateEntity not protected: TSan catches it!
//         }
//     });
//
//     networkThread.join();
//     renderThread.join();
//
//     EXPECT_EQ(manager.count(), 10);  // 10 unique entity IDs
// }

// ============================================================
// SECTION 8: INTERVIEW QUESTIONS
// ============================================================

// Q1: What is ThreadSanitizer and how do you enable it?
//     -> Google's runtime data race detector. Instruments all memory
//        accesses at compile time. Enable with -fsanitize=thread flag.
//        Reports exact file/line/thread of every data race at runtime.

// Q2: Can you write a GTest that specifically tests for thread safety?
//     -> YES. Create multiple threads, have them access shared data,
//        join them, verify the result is correct.
//        WITH TSan enabled: any unsynchronized access causes TSan report.
//        Without TSan: result might be wrong but hard to debug why.

// Q3: What GTest filter would you use to run only thread tests?
//     -> ./test_binary --gtest_filter="*Thread*" or "*Race*" or "*Concurrent*"
//        Use descriptive test names to make filtering easy.

// Q4: What TSAN_OPTIONS are most useful?
//     -> halt_on_error=1: stop at first race (easier single-race debug).
//        log_path=report: save output to file for later analysis.
//        suppressions=file.supp: ignore known false positives.

// Q5: Why use -O1 with TSan instead of -O0 or -O2?
//     -> -O0: too much overhead, slow, harder to correlate with source.
//        -O1: good balance (TSan works well, still debuggable).
//        -O2/-O3: optimizer might eliminate races or add false positives.
//        -O1 is the recommended optimization level for TSan.

// Q6: How do you suppress a known false positive in TSan?
//     -> Create a .supp file:
//        race:^ClassName::methodName$
//        Run with: TSAN_OPTIONS="suppressions=tsan.supp" ./tests
//        Use sparingly! Most TSan reports are REAL races.

// Q7: What is the difference between TSan for GTest and QTest?
//     -> Same TSan flags! (-fsanitize=thread)
//        GTest: add to target_compile_options in CMake.
//        QTest: same CMake setup, just different test framework.
//        TSan works at the compiler/OS level, framework-agnostic.

// Q8: In CI/CD, how would you integrate TSan tests?
//     -> Separate CMake target (my_tests_tsan) built in CI.
//        Run after normal tests: if tsan_tests fail, block merge.
//        TSAN_OPTIONS="halt_on_error=1 exitcode=1" ensures CI fails.
//        Store TSan reports as CI artifacts for debugging.
```

**The CMake setup - the most important part:**

```cmake
# Normal tests (fast):
add_executable(my_tests tests/test_threading.cpp)
target_link_libraries(my_tests GTest::gtest_main)

# TSan tests (slow but catches races):
add_executable(my_tests_tsan tests/test_threading.cpp)
target_link_libraries(my_tests_tsan GTest::gtest_main)
target_compile_options(my_tests_tsan PRIVATE -fsanitize=thread -g -O1)
target_link_options(my_tests_tsan PRIVATE -fsanitize=thread)

# Run both in CI:
# ctest -R "my_tests$"      ← fast, run always
# ctest -R "my_tests_tsan"  ← slow, run on PR/nightly
```

**Reading a TSan report:**

```
WARNING: ThreadSanitizer: data race
  Write at datarace.cpp:15  ← WHERE the race is
    by thread T3             ← WHICH thread wrote
    (mutexes held: {})       ← NO MUTEX HELD = BUG!

  Previous read at datarace.cpp:15
    by thread T1             ← another thread read simultaneously

FIX: add mutex around datarace.cpp:15
Re-run with TSan → no report = fixed ✅
```

**Test naming convention:**

```cpp
TEST(DataRaceTest, DISABLED_RaceWithoutFix)    // show what a race looks like
TEST(DataRaceTest, MutexProtectedIncrement)    // correct mutex usage
TEST(DataRaceTest, AtomicIncrement)            // correct atomic usage
TEST(ConcurrencyTest, ProducerConsumerCorrect) // correct pattern
TEST(ConcurrencyTest, ThreadCompletesInTime)   // no deadlock/hang
```

> **The workflow**: write your concurrent code → add a GTest that exercises it with multiple threads → build with `-fsanitize=thread` → run → TSan either passes silently (no races) or prints the exact file and line of the race. Fix the race → re-run → TSan passes. Add this as a separate CI step so every PR is checked for new races before merging.