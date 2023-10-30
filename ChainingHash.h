#ifndef __CHAINING_HASH_H
#define __CHAINING_HASH_H

#include <vector>
#include <list>
#include <stdexcept>
#include "Hash.h"

template<typename K, typename V>
class ChainingHash : public Hash<K, V> {
private:
    std::vector<std::list<std::pair<K, V>>> table_;
    size_t size_;
    size_t bucket_count_;

public:
    ChainingHash(int n = 101) : size_(0), bucket_count_(n) {
        n = findNextPrime(n);
        table_.resize(n);
    }

    ~ChainingHash() {
        clear();
    }

    int size() {
        return size_;
    }

    V operator[](const K& key) {
        size_t index = hash(key);
        for (const auto& pair : table_[index]) {
            if (pair.first == key)
                return pair.second;
        }
        throw std::out_of_range("Key not found");
    }

    bool insert(const std::pair<K, V>& pair) {
        if (load_factor() > 0.75)
            rehash();

        size_t index = hash(pair.first);
        for (auto& p : table_[index]) {
            if (p.first == pair.first) {
                p.second = pair.second;
                return false;  // Key already exists
            }
        }
        table_[index].push_back(pair);
        size_++;
        return true;
    }

    void erase(const K& key) {
        size_t index = hash(key);
        table_[index].remove_if([key](const std::pair<K, V>& pair) {
            return pair.first == key;
            });
        size_--;
    }

    void clear() {
        for (auto& bucket : table_)
            bucket.clear();
        size_ = 0;
    }

    int bucket_count() {
        return bucket_count_;
    }

    float load_factor() {
        return static_cast<float>(size_) / static_cast<float>(bucket_count_);
    }

private:
    int findNextPrime(int n) {
        while (!isPrime(n))
            n++;
        return n;
    }

    bool isPrime(int n) {
        if (n <= 1)
            return false;
        if (n <= 3)
            return true;
        if (n % 2 == 0 || n % 3 == 0)
            return false;

        for (int i = 5; i * i <= n; i += 6) {
            if (n % i == 0 || n % (i + 2) == 0)
                return false;
        }
        return true;
    }

    size_t hash(const K& key) {
        return std::hash<K>{}(key) % bucket_count_;
    }

    void rehash() {
        int new_bucket_count = findNextPrime(2 * bucket_count_);
        std::vector<std::list<std::pair<K, V>>> new_table(new_bucket_count);
        for (auto& bucket : table_) {
            for (auto& pair : bucket) {
                size_t index = hash(pair.first, new_bucket_count);
                new_table[index].push_back(pair);
            }
        }
        table_ = std::move(new_table);
        bucket_count_ = new_bucket_count;
    }

    size_t hash(const K& key, size_t bucket_count) {
        return std::hash<K>{}(key) % bucket_count;
    }
};

#endif //__CHAINING_HASH_H
