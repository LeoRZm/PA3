#ifndef __PROBING_HASH_H
#define __PROBING_HASH_H

#include <vector>
#include <stdexcept>
#include "Hash.h"

template<typename K, typename V>
class ProbingHash : public Hash<K, V> {
private:
    std::vector<std::pair<K, V>> table_;
    std::vector<EntryState> entry_states_;
    size_t total_size_;

public:
    ProbingHash(int n = 101) : total_size_(0) {
        n = findNextPrime(n);
        table_.resize(n);
        entry_states_.resize(n, EntryState::EMPTY);
    }

    ~ProbingHash() {
        clear();
    }

    int size() {
        return total_size_;
    }

    V operator[](const K& key) {
        size_t index = hash(key);
        for (size_t i = 0; i < table_.size(); ++i) {
            size_t probe_index = (index + i) % table_.size();
            if (entry_states_[probe_index] == EntryState::VALID && table_[probe_index].first == key)
                return table_[probe_index].second;
        }
        throw std::out_of_range("Key not found");
    }

    bool insert(const std::pair<K, V>& pair) {
        if (load_factor() > 0.75)
            rehash();

        size_t index = hash(pair.first);
        size_t probe_index = index;
        size_t deleted_index = -1;
        for (size_t i = 0; i < table_.size(); ++i) {
            if (entry_states_[probe_index] == EntryState::DELETED && deleted_index == -1)
                deleted_index = probe_index;

            if (entry_states_[probe_index] == EntryState::EMPTY || entry_states_[probe_index] == EntryState::DELETED) {
                if (deleted_index != -1)
                    probe_index = deleted_index;
                table_[probe_index] = pair;
                entry_states_[probe_index] = Entry
