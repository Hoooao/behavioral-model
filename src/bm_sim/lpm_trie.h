/* Copyright 2013-present Barefoot Networks, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Antonin Bas (antonin@barefootnetworks.com)
 *
 */

#ifndef BM_SIM_LPM_TRIE_H_
#define BM_SIM_LPM_TRIE_H_

#include <bf_lpm_trie/bf_lpm_trie.h>

#include <algorithm>  // for std::swap
#include <utility>  // for std::swap

namespace bm {

static_assert(sizeof(value_t) == sizeof(uintptr_t),
              "Invalid type sizes");

class LPMTrie {
 public:
  explicit LPMTrie(size_t key_width_bytes)
    : key_width_bytes(key_width_bytes) {
    trie = std::make_unique<BfLpmTrie>(key_width_bytes, true);
  }

  /* Copy constructor */
  LPMTrie(const LPMTrie& other) = delete;

  /* Move constructor */
  LPMTrie(LPMTrie&& other) noexcept
  : key_width_bytes(other.key_width_bytes) {trie.swap(other.trie);}

  ~LPMTrie() {
    // Hao: may need to clearn up memory for trie
  }

  /* Copy assignment operator */
  LPMTrie &operator=(const LPMTrie &other) = delete;

  /* Move assignment operator */
  LPMTrie &operator=(LPMTrie &&other) noexcept {
    key_width_bytes = other.key_width_bytes;
    std::swap(trie, other.trie);
    return *this;
  }

  void insert_prefix(const ByteContainer &prefix, int prefix_length,
                     uintptr_t value) {
    printf("LPMTrie::insert_prefix prefix: %s\n", prefix.to_hex().c_str());
    std::string prefix_str(prefix.data(), prefix.size());
    trie->insert(prefix_str, prefix_length, static_cast<value_t>(value));
  }

  bool delete_prefix(const ByteContainer &prefix, int prefix_length) {
    std::string prefix_str(prefix.data(), prefix.size());
    return trie->remove(prefix_str, prefix_length);
  }

  bool has_prefix(const ByteContainer &prefix, int prefix_length) const {
    std::string prefix_str(prefix.data(), prefix.size());
    return trie->hasPrefix(prefix_str, prefix_length);
  }

  bool retrieve_value(const ByteContainer &prefix, int prefix_length,
                      uintptr_t *value) const {               
    std::string prefix_str(prefix.data(), prefix.size());
    return trie->retrieveValue(prefix_str, prefix_length,
                                      *reinterpret_cast<value_t *>(value));
  }

  bool lookup(const ByteContainer &key, uintptr_t *value) const {
    printf("LPMTrie::lookup key: %s\n", key.to_hex().c_str());
    std::string key_str(key.data(), key.size());
    return trie->lookup(key_str, *reinterpret_cast<value_t *>(value));
  }

  void clear() {
    // Hao: make sure cleans up memory
    trie.reset(new BfLpmTrie(key_width_bytes, true));
  }

 private:
  size_t key_width_bytes{0};
  std::unique_ptr<BfLpmTrie> trie{nullptr};
};

}  // namespace bm

#endif  // BM_SIM_LPM_TRIE_H_
