#include "common/common.hpp"
#include "buffer.hpp"
#include "stats.hpp"
#include "producer.hpp"
#include "consumer.hpp"
#include <fstream>
#include <filesystem>

// ============================================================================
// TeeBuf — writes every byte to two streambufs simultaneously.
// Redirecting std::cout through this lets all existing print calls go to both
// the terminal and the report file without touching any other source files.
// ============================================================================
class TeeBuf : public std::streambuf {
public:
    TeeBuf(std::streambuf* sb1, std::streambuf* sb2) : sb1_(sb1), sb2_(sb2) {}

    int overflow(int c) override {
        if (c == EOF) return !EOF;
        if (sb1_->sputc(static_cast<char>(c)) == EOF) return EOF;
        if (sb2_->sputc(static_cast<char>(c)) == EOF) return EOF;
        return c;
    }

    int sync() override {
        if (sb1_->pubsync() == -1) return -1;
        if (sb2_->pubsync() == -1) return -1;
        return 0;
    }

private:
    std::streambuf* sb1_; ///< primary (terminal stdout)
    std::streambuf* sb2_; ///< secondary (report file)
};

// ============================================================================
// run_simulation
// Returns elapsed wall-clock time in seconds.
// All std::cout output is automatically teed to the open report file via the
// caller's TeeBuf redirect — no other files need to change.
// ============================================================================
static double run_simulation(int p, int c, int b) {
    std::cout << "\n========================================\n"
              << "producers=" << p
              << "  consumers=" << c
              << "  buffer=" << b
              << "\n========================================\n";

    SharedBuffer     buffer(b);
    GlobalStats      global(p);
    std::atomic<int> total_produced{0};

    auto start = std::chrono::steady_clock::now();

    // Launch consumers first so they're ready when producers start
    std::vector<std::unique_ptr<Consumer>> consumers;
    std::vector<std::thread> consumer_threads;
    consumers.reserve(c);
    consumer_threads.reserve(c);
    for (int i = 1; i <= c; ++i) {
        consumers.push_back(std::make_unique<Consumer>(i, p, buffer, global));
        consumer_threads.emplace_back(&Consumer::run, consumers.back().get());
    }

    // Launch producers
    std::vector<std::unique_ptr<Producer>> producers;
    std::vector<std::thread> producer_threads;
    producers.reserve(p);
    producer_threads.reserve(p);
    for (int i = 1; i <= p; ++i) {
        producers.push_back(std::make_unique<Producer>(i, buffer, total_produced));
        producer_threads.emplace_back(&Producer::run, producers.back().get());
    }

    for (auto& t : producer_threads) t.join();

    // Send one sentinel per consumer to signal shutdown
    for (int i = 0; i < c; ++i) {
        SalesRecord s{};
        s.is_sentinel = true;
        buffer.produce(s);
    }

    for (auto& t : consumer_threads) t.join();

    auto end = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(end - start).count();

    global.print();
    std::cout << "\nTotal simulation time: "
              << std::fixed << std::setprecision(3) << elapsed << "s\n";

    return elapsed;
}

// ============================================================================
// write_report_header
// Written once at the top of a full 9-run report. Documents every shared
// variable and synchronization primitive used by the program, satisfying the
// spec requirement: "clearly indicate the places where shared variables and
// semaphores were employed."
// ============================================================================
static void write_report_header() {
    std::cout <<
        "Producer-Consumer Problem — CS471 Report\n"
        "=========================================\n\n"

        "--- Shared Variables & Synchronization Primitives ---\n\n"

        "1. SharedBuffer::buf_  (vector<SalesRecord>)\n"
        "   Shared between ALL producer and consumer threads.\n"
        "   Protected by: SharedBuffer::mutex_ (std::mutex)\n"
        "   File: src/library/buffer.hpp, buffer.cpp\n\n"

        "2. SharedBuffer::empty_slots_  (std::counting_semaphore, init = b)\n"
        "   Producers acquire() before writing — blocks when buffer is full.\n"
        "   Consumers release() after reading — signals a slot is free.\n"
        "   File: src/library/buffer.hpp, buffer.cpp\n\n"

        "3. SharedBuffer::filled_slots_  (std::counting_semaphore, init = 0)\n"
        "   Consumers acquire() before reading — blocks when buffer is empty.\n"
        "   Producers release() after writing — signals a record is ready.\n"
        "   File: src/library/buffer.hpp, buffer.cpp\n\n"

        "4. SharedBuffer::mutex_  (std::mutex)\n"
        "   Guards the critical section (head_, tail_, buf_) during each\n"
        "   individual produce() and consume() call.\n"
        "   File: src/library/buffer.hpp, buffer.cpp\n\n"

        "5. total_produced_  (std::atomic<int>, shared across all producers)\n"
        "   Each producer atomically claims the next slot via fetch_add().\n"
        "   When the claimed slot >= 1000 the producer exits.\n"
        "   No mutex needed — std::atomic guarantees sequential consistency.\n"
        "   File: src/library/producer.hpp, producer.cpp\n\n"

        "6. GlobalStats::mutex_  (std::mutex)\n"
        "   Held by each consumer when it finishes: serialises the local-stats\n"
        "   print AND the merge into global totals so output never interleaves\n"
        "   and the aggregate is updated atomically.\n"
        "   File: src/library/stats.hpp\n\n"

        "Records use pseudo-random number generators (std::mt19937) seeded\n"
        "independently per producer thread via std::random_device.\n\n"

        "--- Run Results ---\n";
}

// ============================================================================
// main
// ============================================================================
int main(int argc, char* argv[]) {
    // Locate the report file next to the executable (PRODUCER-CONSUMER/)
    std::filesystem::path exe    = std::filesystem::weakly_canonical(argv[0]);
    std::filesystem::path report = exe.parent_path() / "report.txt";

    if (argc == 1) {
        // No arguments: run all 9 required p/c combinations.
        // Overwrite the report so each full run produces a clean document.
        std::ofstream ofs(report, std::ios::trunc);
        if (!ofs)
            std::cerr << "Warning: could not open report file " << report << "\n";

        TeeBuf tee(std::cout.rdbuf(), ofs.rdbuf());
        std::streambuf* old_buf = std::cout.rdbuf(&tee);

        write_report_header();

        constexpr int DEFAULT_BUFFER = 10;
        constexpr int ps[] = {2, 5, 10};
        constexpr int cs[] = {2, 5, 10};

        double times[3][3] = {};
        for (int pi = 0; pi < 3; ++pi)
            for (int ci = 0; ci < 3; ++ci)
                times[pi][ci] = run_simulation(ps[pi], cs[ci], DEFAULT_BUFFER);

        // Timing comparison table
        std::cout << "\n\n--- Timing Comparison (seconds) ---\n"
                  << std::setw(12) << "p \\ c"
                  << std::setw(10) << "c=2"
                  << std::setw(10) << "c=5"
                  << std::setw(10) << "c=10" << "\n"
                  << std::string(42, '-') << "\n";
        for (int pi = 0; pi < 3; ++pi) {
            std::cout << std::setw(12) << ("p=" + std::to_string(ps[pi]));
            for (int ci = 0; ci < 3; ++ci)
                std::cout << std::setw(10) << std::fixed
                           << std::setprecision(3) << times[pi][ci];
            std::cout << "\n";
        }
        std::cout << "\n";

        std::cout.rdbuf(old_buf);

    } else if (argc == 4) {
        int p = std::stoi(argv[1]);
        int c = std::stoi(argv[2]);
        int b = std::stoi(argv[3]);

        if (p <= 0 || c <= 0 || b <= 0) {
            std::cerr << "All arguments must be positive integers\n";
            return 1;
        }

        // Single run: overwrite report_single.txt
        std::filesystem::path report_single = exe.parent_path() / "report_single.txt";
        std::ofstream ofs(report_single, std::ios::trunc);
        if (!ofs)
            std::cerr << "Warning: could not open report file " << report << "\n";

        TeeBuf tee(std::cout.rdbuf(), ofs.rdbuf());
        std::streambuf* old_buf = std::cout.rdbuf(&tee);

        run_simulation(p, c, b);

        std::cout.rdbuf(old_buf);

    } else {
        std::cerr << "Usage: " << argv[0] << " <producers> <consumers> <buffer_size>\n"
                  << "       " << argv[0] << "   (no args — runs all 9 required combinations)\n";
        return 1;
    }

    return 0;
}
