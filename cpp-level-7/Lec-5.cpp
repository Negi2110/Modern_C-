// ============================================================
// COMMAND LINE ARGUMENTS AND ENVIRONMENT VARIABLES - INTERVIEW NOTES
// ============================================================

#include <iostream>
#include <cstdlib>  // for std::getenv

// ============================================================
// SECTION 1: main() SIGNATURES - THREE VERSIONS
// ============================================================

// VERSION 1: No arguments (simplest)
// int main() { }

// VERSION 2: Standard command line args (most common)
// int main(int argc, char* argv[]) { }
//           ^^^^     ^^^^
//           count    values array

// VERSION 3: With environment variables (POSIX extension)
// int main(int argc, char* argv[], char* env[]) { }
//                                  ^^^^
//                                  environment array
// NOTE: char* env[] is NOT standard C++ (POSIX/platform specific)
// -> works on Linux/Mac, might not work on all platforms
// -> std::getenv() from <cstdlib> is the STANDARD way to access env vars

// ============================================================
// SECTION 2: argc AND argv EXPLAINED
// ============================================================

// argc = "argument count"
// -> number of arguments passed INCLUDING the program name itself
// -> always >= 1 (program name is always argv[0])
// -> ./prog hello world  -> argc = 3
//    [0]   [1]    [2]
//
// argv = "argument values"
// -> array of C-style strings (char*)
// -> argv[0] = program name/path ("./prog" or full path)
// -> argv[1] = first user argument
// -> argv[argc-1] = last argument
// -> argv[argc] = nullptr (null terminator of the array)
//
// EXAMPLE: g++ main.cpp -o prog
// -> argc = 4
// -> argv[0] = "g++"
// -> argv[1] = "main.cpp"
// -> argv[2] = "-o"
// -> argv[3] = "prog"
// -> argv[4] = nullptr
//
// YOUR COMMENT IS EXACT:
// "argc = count, argv = values as c-strings, env = environmental"

// ============================================================
// SECTION 3: TYPES - argv IS char** or char*[]
// ============================================================

// char* argv[]  and  char** argv  are IDENTICAL in function parameters
// (array decays to pointer - same rule as int arr[] = int* arr)
//
// argv is a POINTER TO POINTER:
// argv ──→ [ptr0][ptr1][ptr2][ptr3][null]
//            │     │     │     │
//            ↓     ↓     ↓     ↓
//         "./prog" "hello" "world" nullptr
//
// argv[0] = "ptr to first string"  = "./prog\0"
// argv[1] = "ptr to second string" = "hello\0"
// *argv   = argv[0] = same as above
// **argv  = argv[0][0] = '.' (first char of program name)

// ============================================================
// SECTION 4: main() TRACE
// ============================================================

int main(int argc, char* argv[], char* env[])
{
    std::cout << "argc: " << argc << std::endl;
    // ./prog          -> argc=1, prints "argc: 1"
    // ./prog hello    -> argc=2, prints "argc: 2"
    // ./prog a b c   -> argc=4, prints "argc: 4"

    if (argc < 2)
    {
        std::cout << "Usage: ./main some_string" << std::endl;
        return 0;
        // guard: if no args provided, show usage and exit
        // argc < 2 means only argv[0] (program name) exists
    }

    for (int i = 0; i < argc; i++)
    {
        std::cout << "[" << i << "]" << argv[i] << std::endl;
        // prints each argument with its index
        // RUN: ./prog hello world
        // OUTPUT:
        // [0]./prog
        // [1]hello
        // [2]world
    }

    std::cout << std::getenv("PATH") << std::endl;
    // std::getenv("PATH") = get environment variable named "PATH"
    // returns const char* to the value string
    // returns nullptr if variable doesn't exist!
    // PATH on Linux: "/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin"
    //
    // DANGEROUS if PATH not set: getenv returns nullptr -> cout << nullptr = UB!
    // SAFE VERSION:
    // if (const char* path = std::getenv("PATH")) {
    //     std::cout << path << std::endl;
    // }

    return 0;
}

// ============================================================
// SECTION 5: YOUR COMMENTED env[] LOOP EXPLAINED
// ============================================================

// char* env[] = array of ALL environment variables as strings
// format: "VARIABLE_NAME=value"
// terminated by nullptr (like argv)
//
// env[0] = "HOME=/home/aman"
// env[1] = "PATH=/usr/bin:/bin..."
// env[2] = "SHELL=/bin/bash"
// env[n] = nullptr  <- end sentinel
//
// YOUR LOOP:
// int i = 0;
// for (;;) {
//     std::cout << env[i++];    // print env[i], then increment
//     if (env[i] == nullptr) {  // check if NEXT is null (after increment!)
//         break;
//     }
//     break;  // BUG! this break exits after first iteration always
// }
//
// CORRECT LOOP to print ALL env vars:
// for (int i = 0; env[i] != nullptr; i++) {
//     std::cout << env[i] << '\n';
// }
//
// OR with pointer arithmetic:
// for (char** e = env; *e != nullptr; e++) {
//     std::cout << *e << '\n';
// }

// ============================================================
// SECTION 6: std::getenv - STANDARD WAY TO ACCESS ENV VARS
// ============================================================

// #include <cstdlib>
// const char* std::getenv(const char* name)
// -> looks up environment variable by name
// -> returns const char* to value (do NOT free it!)
// -> returns nullptr if variable not found
//
// COMMON ENVIRONMENT VARIABLES:
// std::getenv("HOME")     // /home/aman
// std::getenv("PATH")     // /usr/local/bin:/usr/bin:/bin...
// std::getenv("USER")     // aman
// std::getenv("SHELL")    // /bin/bash
// std::getenv("PWD")      // /home/aman/projects
// std::getenv("TERM")     // xterm-256color
//
// ALWAYS CHECK FOR nullptr:
// const char* home = std::getenv("HOME");
// if (home) {
//     std::cout << home;
// } else {
//     std::cout << "HOME not set";
// }

// ============================================================
// SECTION 7: COMMAND LINE FLAGS - HOW PROGRAMS USE THEM
// ============================================================

// YOUR EXAMPLE: g++ main.cpp -o prog
// -> argc = 4
// -> argv = ["g++", "main.cpp", "-o", "prog"]
// -> "-o" is a FLAG (options start with - or --)
//
// HOW to parse flags in your own program:
// ./myapp --verbose --output=result.txt input.dat
//
// for (int i = 1; i < argc; i++) {
//     std::string arg = argv[i];
//
//     if (arg == "--verbose") {
//         verbose = true;
//     }
//     else if (arg.substr(0, 9) == "--output=") {
//         outputFile = arg.substr(9);  // get "result.txt"
//     }
//     else {
//         inputFile = arg;  // non-flag argument
//     }
// }
//
// OR use a library: getopt (POSIX), cxxopts, CLI11, boost::program_options

// ============================================================
// SECTION 8: argc/argv IN YOUR CONTEXT
// ============================================================

// ISSST DIS Plugin likely uses command line args:
// ./issst_sim --exercise-id=1 --multicast=239.255.0.1 --port=3000
//
// Qt applications also use them:
// QCoreApplication app(argc, argv);  // Qt needs argc/argv
// QApplication app(argc, argv);      // same for GUI apps
// -> Qt parses Qt-specific args (-platform, -style etc.)
// -> you pass argc/argv through so Qt can process its own flags
// -> THIS is why Qt main() always takes argc/argv even if you don't use them

// ============================================================
// SECTION 9: CONVERTING argv TO std::string/vector (modern C++)
// ============================================================

// C++: argv is char**, not std::string
// To use with modern C++ code:
//
// // Convert to vector of strings:
// std::vector<std::string> args(argv, argv + argc);
// // argv to argv+argc = range of char* pointers
// // vector constructor: each char* -> std::string (automatic conversion)
//
// args[0] = "./prog"   (std::string)
// args[1] = "hello"    (std::string)
//
// // Now you can use:
// if (std::find(args.begin(), args.end(), "--verbose") != args.end()) {
//     verbose = true;
// }

// ============================================================
// SECTION 10: INTERVIEW QUESTIONS
// ============================================================

// Q1: What are argc and argv?
//     -> argc (argument count): number of command-line arguments
//        including the program name. Always >= 1.
//        argv (argument values): array of C-style strings (char*).
//        argv[0] = program name, argv[1..argc-1] = user arguments.
//        argv[argc] = nullptr (null terminator).

// Q2: What is argv[0]?
//     -> The name/path of the program being executed.
//        Could be "./prog", "/usr/bin/prog", or just "prog"
//        depending on how the program was invoked.

// Q3: Why is argc always at least 1?
//     -> argv[0] is always the program name.
//        Even with no arguments: ./prog -> argc=1, argv=["./prog", nullptr]

// Q4: What is the type of argv?
//     -> char** (pointer to pointer to char) or equivalently char*[]
//        (array of char pointers). Both are identical in function params.
//        Modern C++: often converted to std::vector<std::string> for ease.

// Q5: What does std::getenv() do?
//     -> Returns the value of an environment variable as const char*.
//        Returns nullptr if the variable doesn't exist.
//        From <cstdlib>. ALWAYS check for nullptr before using result.

// Q6: What is the difference between argv and env?
//     -> argv: command-line arguments passed when program is invoked.
//        env: environment variables (HOME, PATH, USER etc.) from the OS.
//        argv is standard C++. char* env[] as third main param is POSIX.
//        Use std::getenv() for portable env var access.

// Q7: Why does Qt main() always take argc and argv?
//     -> QApplication/QCoreApplication needs them to process Qt-specific
//        command-line options (-platform, -style, -geometry etc.).
//        Must pass them through even if your own code doesn't use them.

// Q8: How would you safely print all environment variables?
//     -> Use the env[] parameter of main (POSIX):
//        for (int i = 0; env[i] != nullptr; i++) cout << env[i];
//        Or use std::getenv() for specific variables with null check.
//        environ (global) is another POSIX option.
// ```

// **argv memory layout - the visual that explains it:**

// ```
// ./prog hello world

// argc = 3

// argv:
// [0] ──→ "./prog\0"
// [1] ──→ "hello\0"
// [2] ──→ "world\0"
// [3] ──→ nullptr   ← array always null-terminated

// for (int i = 0; i < argc; i++)  // safe: i < argc
// for (int i = 0; argv[i]; i++)   // also safe: stops at nullptr
// ```

// **The Qt connection:**

// ```cpp
// int main(int argc, char* argv[]) {   // MUST take these
//     QApplication app(argc, argv);    // Qt processes its own flags
//     // your code here
//     return app.exec();
// }
// // Qt looks for: -platform, -style, -geometry etc. in argv
// // If you pass (0, nullptr) → Qt crashes looking for its flags
// ```

// > **The getenv nullptr trap** is the most common bug: `std::cout << std::getenv("NONEXISTENT")` → if the variable doesn't exist, `getenv` returns `nullptr`, and `cout << nullptr` is **undefined behavior**. Always: `if (const char* val = std::getenv("NAME")) { cout << val; }`.