#include "primer/trie.h"
#include <sys/types.h>
#include <cstdint>
#include <memory>
#include <string_view>
#include <utility>
#include "common/exception.h"

namespace bustub {

auto Remove_Function(std::string_view key, int32_t index, std::shared_ptr<const TrieNode> cur_node)
    -> std::shared_ptr<TrieNode> {
  int32_t len = key.length();
  if (index >= len) {
    if (cur_node->children_.empty()) {
      return nullptr;
    } else {
      std::shared_ptr<TrieNode> new_node = std::make_shared<TrieNode>(cur_node->children_);
      return new_node;
    }
  }
  char c = key[index];
  std::shared_ptr<TrieNode> children_node = Remove_Function(key, index + 1, cur_node->children_.find(c)->second);
  if (children_node != nullptr || cur_node->is_value_node_ || cur_node->children_.size() > 1) {
    std::shared_ptr<TrieNode> new_node = cur_node->Clone();
    if (children_node != nullptr) {
      new_node->children_[c] = children_node;
    } else {
      new_node->children_.erase(c);
    }
    return new_node;
  } else {
    return nullptr;
  }
}

template <class T>
auto Trie::Get(std::string_view key) const -> const T * {
  // throw NotImplementedException("Trie::Get is not implemented.");

  // You should walk through the trie to find the node corresponding to the key. If the node doesn't exist, return
  // nullptr. After you find the node, you should use `dynamic_cast` to cast it to `const TrieNodeWithValue<T> *`. If
  // dynamic_cast returns `nullptr`, it means the type of the value is mismatched, and you should return nullptr.
  // Otherwise, return the value.
  std::shared_ptr<const TrieNode> cur_node = this->GetRoot();
  if (cur_node == nullptr) {
    return nullptr;
  }
  for (char c : key) {
    if (cur_node->children_.find(c) != cur_node->children_.end()) {
      cur_node = cur_node->children_.find(c)->second;
    } else {
      return nullptr;
    }
  }
  // auto node_with_value = std::dynamic_pointer_cast<const TrieNodeWithValue<T>>(cur_root);
  if (cur_node->is_value_node_) {
    auto node_with_value = dynamic_cast<const TrieNodeWithValue<T> *>(cur_node.get());
    if (node_with_value) {
      return node_with_value->value_.get();
    }
    return nullptr;
  }
  return nullptr;
}

template <class T>
auto Trie::Put(std::string_view key, T value) const -> Trie {
  // Note that `T` might be a non-copyable type. Always use `std::move` when creating `shared_ptr` on that value.
  // throw NotImplementedException("Trie::Put is not implemented.");

  // You should walk through the trie and create new nodes if necessary. If the node corresponding to the key already
  // exists, you should create a new `TrieNodeWithValue`.
  std::shared_ptr<TrieNode> cur_node = nullptr;
  if (this->GetRoot() == nullptr) {
    if (key.empty()) {
      cur_node = std::make_shared<TrieNodeWithValue<T>>(std::map<char, std::shared_ptr<const TrieNode>>(),
                                                        std::make_shared<T>(std::move(value)));
    } else {
      cur_node = std::make_shared<TrieNode>();
    }
  } else {
    if (key.empty()) {
      std::shared_ptr<const TrieNode> new_root = std::make_shared<TrieNodeWithValue<T>>(
          std::move(this->GetRoot()->Clone()->children_), std::make_shared<T>(std::move(value)));
      Trie new_trie = Trie(new_root);
      return new_trie;
    } else {
      cur_node = std::shared_ptr<TrieNode>(this->GetRoot()->Clone());
    }
  }
  std::shared_ptr<const TrieNode> new_root = cur_node;
  int32_t len = key.size();
  for (int32_t i = 0; i < len - 1; ++i) {
    char c = key[i];
    std::shared_ptr<TrieNode> new_node = std::make_shared<TrieNode>();
    if (cur_node->children_.find(c) != cur_node->children_.end()) {
      new_node = std::shared_ptr<TrieNode>(cur_node->children_[c]->Clone());
    }
    cur_node->children_[c] = new_node;
    cur_node = new_node;
  }
  char c = key.back();
  if (cur_node->children_.find(c) != cur_node->children_.end()) {
    std::shared_ptr<const TrieNode> children_node = cur_node->children_[c];
    std::shared_ptr<TrieNodeWithValue<T>> new_node = std::make_shared<TrieNodeWithValue<T>>(
        std::move(children_node->Clone()->children_), std::make_shared<T>(std::move(value)));
    cur_node->children_[c] = new_node;
  } else {
    std::shared_ptr<TrieNodeWithValue<T>> new_node = std::make_shared<TrieNodeWithValue<T>>(
        std::map<char, std::shared_ptr<const TrieNode>>(), std::make_shared<T>(std::move(value)));
    cur_node->children_[c] = new_node;
  }
  Trie new_trie = Trie(new_root);
  return new_trie;
}

auto Trie::Remove(std::string_view key) const -> Trie {
  // throw NotImplementedException("Trie::Remove is not implemented.");

  // You should walk through the trie and remove nodes if necessary. If the node doesn't contain a value any more,
  // you should convert it to `TrieNode`. If a node doesn't have children any more, you should remove it.
  std::shared_ptr<const TrieNode> cur_root = this->GetRoot();
  if (cur_root == nullptr) {
    return *this;
  }
  if (key.empty()) {
    if (cur_root->is_value_node_) {
      if (cur_root->children_.empty()) {
        Trie new_trie = Trie();
        return new_trie;
      } else {
        std::shared_ptr<TrieNode> new_root = std::make_shared<TrieNode>(cur_root->children_);
        Trie new_trie = Trie(new_root);
        return new_trie;
      }
    } else {
      return *this;
    }
  }
  std::shared_ptr<const TrieNode> cur_node = cur_root;
  int32_t len = key.size();
  for (int32_t i = 0; i < len; ++i) {
    char c = key[i];
    if (cur_node->children_.find(c) != cur_node->children_.end()) {
      cur_node = cur_node->children_.find(c)->second;
    } else {
      return *this;
    }
  }
  if (!cur_node->is_value_node_) {
    return *this;
  }
  std::shared_ptr<const TrieNode> new_root = Remove_Function(key, 0, cur_root);
  Trie new_trie = Trie(new_root);
  return new_trie;
}

// Below are explicit instantiation of template functions.
//
// Generally people would write the implementation of template classes and functions in the header file. However, we
// separate the implementation into a .cpp file to make things clearer. In order to make the compiler know the
// implementation of the template functions, we need to explicitly instantiate them here, so that they can be picked up
// by the linker.

template auto Trie::Put(std::string_view key, uint32_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint32_t *;

template auto Trie::Put(std::string_view key, uint64_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint64_t *;

template auto Trie::Put(std::string_view key, std::string value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const std::string *;

// If your solution cannot compile for non-copy tests, you can remove the below lines to get partial score.

using Integer = std::unique_ptr<uint32_t>;

template auto Trie::Put(std::string_view key, Integer value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const Integer *;

template auto Trie::Put(std::string_view key, MoveBlocked value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const MoveBlocked *;

}  // namespace bustub
