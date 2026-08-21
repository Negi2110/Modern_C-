
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <chrono>

// ============================================================
// std::thread WITH LAMBDA - NOTES
// ============================================================

// ============================================================
// SECTION 1: WHY LAMBDA OVER FUNCTION POINTER?
// ============================================================

// PREVIOUS SESSION: function pointer
// void test(int x) { ... }
// std::thread t(&test, 100);
//
// THIS SESSION: lambda
// auto lambda = [](int x) { ... };
// std::thread t(lambda, 100);
//
// ADVANTAGES OF LAMBDA FOR THREADS:
// 1. DEFINED INLINE - no need for separate function definition
// 2. CAPTURES - can access local variables (the real power!)
// 3. READABLE - thread logic sits next to thread creation
// 4. FLEXIBLE - different captures per thread (no global state)
//
// ALL THREE ARE EQUIVALENT PERFORMANCE-WISE:
// std::thread t(freeFunction, arg);   // function pointer
// std::thread t(lambda, arg);          // named lambda
// std::thread t([](int x){...}, arg); // anonymous lambda (inline)

// ============================================================
// SECTION 2: YOUR CODE - BASIC LAMBDA THREAD
// ============================================================

void basicExample()
{
    auto lambda = [](int x)
    {
        std::cout << "hello from thread\n";
        std::cout << "Argument passed in: " << x << '\n';
    };
    // lambda = a callable (functor) stored in 'auto' variable
    // []: captures nothing from surrounding scope
    // (int x): takes one parameter (passed when thread starts)

    std::thread myThread(lambda, 100);
    // creates thread, immediately starts running lambda(100)
    // main thread continues past this line concurrently

    myThread.join();
    // main WAITS here until lambda(100) finishes
    // guarantees thread output appears before main output

    std::cout << "hello from my main thread\n";
    // always prints AFTER lambda finishes (join ensures this)
}

// ============================================================
// SECTION 3: THE REAL POWER - LAMBDA CAPTURES WITH THREADS
// ============================================================

void captureExamples()
{
    int sharedValue = 42;
    std::string name = "Aman";

    // CAPTURE BY VALUE (COPY into lambda/thread):
    std::thread t1([sharedValue, name]() {
        // sharedValue and name are COPIES
        // thread has its OWN copies (safe - no sharing)
        std::cout << "t1 sees: " << sharedValue << " " << name << '\n';
    });
    sharedValue = 99;  // changing original doesn't affect t1's copy!
    t1.join();
    // prints: "t1 sees: 42 Aman" (captures the value at creation time)

    // CAPTURE BY REFERENCE (SHARE - DANGEROUS!):
    int counter = 0;
    std::thread t2([&counter]() {
        counter++;  // modifies the ORIGINAL counter
        // DANGER: if main() also modifies counter simultaneously
        // = RACE CONDITION (undefined behavior!)
    });
    t2.join();  // join BEFORE reading counter (ensures thread finished)
    std::cout << "counter: " << counter << '\n';  // safe to read now

    // CAPTURE ALL BY VALUE [=]:
    int a = 1, b = 2;
    std::thread t3([=]() {
        std::cout << a + b << '\n';  // uses copies of a and b
    });
    t3.join();

    // CAPTURE ALL BY REFERENCE [&] (MOST DANGEROUS):
    std::thread t4([&]() {
        std::cout << a << " " << b << '\n';
        // references to a and b - must ensure they stay alive!
        // if main() returns before t4 finishes: DANGLING REFERENCES
    });
    t4.join();  // join ensures a and b still alive
}

// ============================================================
// SECTION 4: PASSING MULTIPLE THREADS
// ============================================================

void multipleThreads()
{
    std::vector<std::thread> threads;

    for (int i = 0; i < 5; i++) {
        threads.emplace_back([i]() {
            // capture i BY VALUE (each thread gets its own copy of i)
            // if [&i]: all threads might see same final value of i (race!)
            std::cout << "Thread " << i << " running\n";
        });
    }

    // join all threads:
    for (auto& t : threads) {
        t.join();
    }
    // output order is UNPREDICTABLE (all threads run concurrently)
    // but all 5 will print before this function returns
}

// ============================================================
// SECTION 5: INLINE LAMBDA (anonymous, most concise)
// ============================================================

void inlineLambda()
{
    // No named variable needed - lambda defined directly in constructor:
    std::thread myThread([](int x) {
        std::cout << "inline lambda thread: " << x << '\n';
    }, 100);

    myThread.join();

    // MOST COMMON PATTERN in real code:
    // thread created and lambda defined at same line
}

// ============================================================
// SECTION 6: LAMBDA CAPTURING SHARED RESULT (common pattern)
// ============================================================

void resultCapture()
{
    int result = 0;

    std::thread t([&result]() {
        // compute something expensive
        for (int i = 1; i <= 100; i++) {
            result += i;  // SAFE: only this thread writes result
        }                 // main is blocked on join() below
    });

    t.join();  // WAIT for computation to finish
    // result is now safe to read (thread finished)
    std::cout << "Sum 1..100 = " << result << '\n';  // 5050
}

// ============================================================
// SECTION 7: WRONG CAPTURE - LIFETIME DANGER
// ============================================================

// DANGER: capturing reference to local variable that goes out of scope
//
// void DANGEROUS()
// {
//     std::thread t;
//     {
//         int local = 42;
//         t = std::thread([&local]() {
//             std::this_thread::sleep_for(std::chrono::seconds(1));
//             std::cout << local;  // DANGLING! local is destroyed!
//         });
//     }  // local goes out of scope HERE - destroyed!
//     t.join();  // thread still running but local is GONE = UB
// }
//
// FIX: capture by value
// t = std::thread([local]() { cout << local; }); // own copy, safe!
// OR ensure local outlives the thread (join before scope exit)

// ============================================================
// SECTION 8: LAMBDA vs FUNCTION vs FUNCTOR FOR THREADS
// ============================================================

// FUNCTION POINTER:
// void work(int x) { ... }
// std::thread t(work, 42);
// WHEN: reusable work across many call sites

// FUNCTOR:
// struct Worker {
//     int multiplier;
//     void operator()(int x) { cout << x * multiplier; }
// };
// std::thread t(Worker{3}, 42);  // CTAD: Worker with multiplier=3
// WHEN: complex state, multiple methods, testable unit

// NAMED LAMBDA (your code):
// auto lambda = [](int x) { ... };
// std::thread t(lambda, 42);
// WHEN: local work, simple captures, readable code nearby

// INLINE LAMBDA (most common):
// std::thread t([](int x) { ... }, 42);
// WHEN: simple one-shot work defined at point of use

// INTERVIEW QUESTION: "Which is best?"
// -> lambda for most cases (concise, captures local state easily)
// -> functor when thread work is complex enough to test separately
// -> function pointer for backward compatibility or C APIs

// ============================================================
// SECTION 9: DSA PATTERNS WITH LAMBDA THREADS
// ============================================================

// PATTERN 1: Parallel work on array chunks
// vector<int> data(1000000);
// int mid = data.size() / 2;
// int sum1 = 0, sum2 = 0;
// thread t1([&]() { for (int i=0; i<mid; i++) sum1 += data[i]; });
// thread t2([&]() { for (int i=mid; i<data.size(); i++) sum2 += data[i]; });
// t1.join(); t2.join();
// int total = sum1 + sum2;  // combine results

// PATTERN 2: Background loading
// thread loader([&resourceMap]() {
//     resourceMap["texture"] = loadTexture("sprite.png");
//     resourceMap["audio"]   = loadAudio("bgm.mp3");
// });
// while (!loader.joinable()) showLoadingScreen();
// loader.join();

// PATTERN 3: Producer-consumer (preview of next sessions)
// queue<int> buffer;
// thread producer([&]() {
//     for (int i=0; i<10; i++) buffer.push(i);
// });
// thread consumer([&]() {
//     while (!buffer.empty()) { process(buffer.front()); buffer.pop(); }
// });
// producer.join(); consumer.join();

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: Why use a lambda instead of a function pointer for threads?
//     -> Lambdas can CAPTURE local variables (by value or reference).
//        Function pointers cannot access local state without globals.
//        Lambdas defined inline keep related code together.
//        Same performance (lambda = compiler-generated functor).

// Q2: What is the difference between [x] and [&x] capture in threads?
//     -> [x]: copies x at lambda creation. Thread has its own copy.
//              Safe even if original x changes or goes out of scope.
//        [&x]: shares x by reference. Thread modifies original.
//              DANGEROUS if: original goes out of scope, or two threads
//              write concurrently (race condition).

// Q3: Is it safe to capture a local variable by reference in a thread?
//     -> ONLY if you guarantee the variable outlives the thread.
//        Use join() before the variable goes out of scope.
//        If thread might outlive the variable: capture by value [x].

// Q4: What happens if you capture loop variable by reference in a loop?
//     -> All threads share the SAME reference to the loop variable.
//        By the time threads run, loop might have finished.
//        All threads might see the final loop value (race condition).
//        FIX: capture by value [i] - each thread gets its own copy.

// Q5: Can a lambda thread return a value?
//     -> Not directly. Use std::future/std::promise or capture
//        a reference to a result variable (safe after join).
//        Or use std::async which DOES return std::future<T>.

// Q6: What is the cleanest way to run multiple threads with lambdas?
//     -> Store in vector<thread>, use emplace_back with lambda.
//        Join with range-based for: for(auto& t: threads) t.join();

// Q7: Why is [&] capture dangerous in threads?
//     -> Captures ALL local variables by reference.
//        If ANY of those variables go out of scope while thread runs:
//        dangling references = undefined behavior (crash/corruption).
//        Thread lifetime must be carefully managed.

// Q8: What is the difference between named lambda and inline lambda?
//     -> Named: auto f = [](){}; thread t(f, arg); - can reuse f.
//        Inline: thread t([](){}, arg); - defined at point of use.
//        No performance difference. Inline preferred for one-time use.

int main()
{
    // YOUR CODE - basic lambda thread:
    auto lambda = [](int x) {
        std::cout << "hello from thread\n";
        std::cout << "Argument passed in: " << x << '\n';
    };

    std::thread myThread(lambda, 100);
    myThread.join();
    std::cout << "hello from my main thread\n";

    // Additional examples:
    resultCapture();
    multipleThreads();
    inlineLambda();

    return 0;
}
// ```

// **Lambda capture in threads - the critical rule:**

// ```cpp
// int x = 42;

// // BY VALUE [x]: SAFE (own copy, original can change/die)
// thread t1([x]() { cout << x; });  // t1 has copy of 42

// // BY REFERENCE [&x]: DANGEROUS (shared reference)
// thread t2([&x]() { cout << x; }); // t2 shares x directly
// // if x goes out of scope before t2 finishes → CRASH!
// // if another thread writes x simultaneously → RACE CONDITION!

// // RULE: join() BEFORE the captured variable goes out of scope
// t1.join();
// t2.join();  // x is still alive ← safe
// ```

// **The loop capture trap:**

// ```cpp
// // WRONG: all threads might see final value of i (race):
// for (int i = 0; i < 5; i++)
//     threads.emplace_back([&i]() { cout << i; }); // &i shared!

// // CORRECT: each thread gets its own copy of i:
// for (int i = 0; i < 5; i++)
//     threads.emplace_back([i]() { cout << i; }); // i copied
// ```

// > **The one rule for lambda captures in threads**: if the thread might run **after** the captured variable goes out of scope, capture by **value** `[x]` not by reference `[&x]`. By-reference captures are only safe when you `join()` the thread before the variable is destroyed — which means the thread's lifetime is bounded by the variable's lifetime.