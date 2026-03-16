#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

void flip_random_bit_in_range(uint8_t* new_buf,
                              std::uint64_t buf_size,
                              std::uint64_t i,
                              std::uint64_t j) {
    if (!new_buf) {
        throw std::invalid_argument("new_buf is null");
    }

    if (j <= i) {
        throw std::invalid_argument("Need 0 <= i < j");
    }

    if (j > buf_size) {
        throw std::out_of_range("Range end j is past buffer size");
    }

    static thread_local std::mt19937_64 gen(std::random_device{}());

    std::uniform_int_distribution<std::uint64_t> off_dist(i, j - 1);
    std::uniform_int_distribution<int> bit_dist(0, 7);

    std::uint64_t off = off_dist(gen);
    int bit = bit_dist(gen);
    uint8_t mask = static_cast<uint8_t>(1u << bit);

    uint8_t old_byte = new_buf[off];
    uint8_t new_byte = static_cast<uint8_t>(old_byte ^ mask);
    new_buf[off] = new_byte;

    std::cout << "Flipped bit " << bit
              << " at byte offset " << off
              << ": 0x" << std::hex
              << static_cast<int>(old_byte)
              << " -> 0x" << static_cast<int>(new_byte)
              << std::dec << '\n';
}

int main() {
    try {
        std::string path = "/Users/karine/Downloads/F-Droid.apk";
        std::uint64_t i = 7401341;
        std::uint64_t j = i + 8921;

        std::ifstream in(path, std::ios::binary | std::ios::ate);
        if (!in) {
            throw std::runtime_error("Failed to open file: " + path);
        }

        std::uint64_t size = static_cast<std::uint64_t>(in.tellg());
        in.seekg(0, std::ios::beg);

        std::vector<uint8_t> buf(size);
        if (!in.read(reinterpret_cast<char*>(buf.data()),
                     static_cast<std::streamsize>(buf.size()))) {
            throw std::runtime_error("Failed to read file");
        }

        flip_random_bit_in_range(buf.data(), buf.size(), i, j);

        // Optional: write mutated buffer back out
        std::ofstream out("mutated.apk", std::ios::binary);
        if (!out) {
            throw std::runtime_error("Failed to create output file");
        }

        out.write(reinterpret_cast<const char*>(buf.data()),
                  static_cast<std::streamsize>(buf.size()));
        if (!out) {
            throw std::runtime_error("Failed to write output file");
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
