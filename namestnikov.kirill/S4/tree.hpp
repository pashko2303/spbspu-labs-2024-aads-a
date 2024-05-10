#ifndef TREE_HPP
#define TREE_HPP

#include <functional>
#include "tree_node.hpp"
#include "tree_iterator.hpp"
#include "const_tree_iterator.hpp"

namespace namestnikov
{
  template< class Key, class Value, class Compare = std::less< Key > >
  class Tree
  {
  public:
    using pair_key_t = std::pair< const Key, Value >;
    using node_t = detail::TreeNode< Key, Value >;
    using iterator = IteratorTree< Key, Value, Compare >;
    using const_iterator = ConstIteratorTree< Key, Value, Compare >;
    Tree():
      root_(nullptr),
      size_(0),
      compare_(Compare())
    {}

    Tree(const Tree & other):
      root_(nullptr),
      size_(0),
      compare_(other.compare_)
    {
      try
      {
        const_iterator begin = other.cbegin();
        const_iterator end = other.cend();
        for (; begin != end; ++begin)
        {
          insert(begin->first, begin->second);
        }
      }
      catch (...)
      {
        clear();
        throw;
      }
    }

    Tree(Tree && other) noexcept:
      root_(other.root_),
      size_(other.size_),
      compare_(other.compare_)
    {
      other.root_ = nullptr;
      other.size_ = 0;
    }

    Tree & operator=(const Tree & other)
    {
      Tree< Key, Value > temp(other);
      if (this != std::addressof(other))
      {
        swap(temp);
      }
      return *this;
    }

    Tree & operator=(Tree && other) noexcept
    {
      Tree< Key, Value > temp(std::move(other));
      if (this != std::addressof(other))
      {
        swap(temp);
      }
      return *this;
    }

    void swap(Tree< Key, Value, Compare > & other) noexcept
    {
      std::swap(root_, other.root_);
      std::swap(size_, other.size_);
      std::swap(compare_, other.compare_);
    }

    void insert(const Key & key, const Value & val)
    {
      try
      {
        if (root_)
        {
          insert_impl(key, val, root_);
          ++size_;
        }
        else
        {
          root_ = new node_t(key, val);
          ++size_;
        }
      }
      catch (...)
      {
        clear();
        throw;
      }
    }
    void balance(node_t * node)
    {
      if (node->height < 0)
      {
        if (node->right->height > 0)
        {
          rotateRight(node->right);
        }
        rotateLeft(node);
      }
      else
      {
        if (node->left->height < 0)
        {
          rotateLeft(node->left);
        }
        rotateRight(node);
      }
    }

    void getNewBalance(node_t * node)
    {
      if ((node->height > 1) || (node->height < -1))
      {
        balance(node);
      }
      else
      {
        if (node->parent)
        {
          if (node->left)
          {
            ++node->parent->height;
          }
          else if (node->right)
          {
            --node->parent->height;
          }
          if (node->parent->height != 0)
          {
            getNewBalance(node->parent);
          }
        }
      }
    }
    node_t * search(const Key & key)
    {
      node_t * res = search_impl(root_, key);
      return res;
    }
    const_iterator cbegin() const noexcept
    {
      return const_iterator(get_min(root_));
    }
    iterator begin() const noexcept
    {
      return iterator(get_min(root_));
    }
    const_iterator cend() const noexcept
    {
      return const_iterator();
    }
    iterator end() const noexcept
    {
      return iterator();
    }

    Value & operator[](const Key & key)
    {
      node_t * traverser = search(key);
      if (traverser)
      {
        return traverser->data.second;
      }
      else
      {
        insert(key, Value());
        node_t * result = search(key);
        return result->data.second;
      }
    }
    const Value & operator[](const Key & key) const
    {
      node_t * traverser = search(key);
      if (traverser)
      {
        return traverser->data.second;
      }
      else
      {
        insert(key, Value());
        node_t * result = search(key);
        return result->data.second;
      }
    }
    Value & at(const Key & key)
    {
      node_t * traverser = search(key);
      if (traverser)
      {
        return traverser->data.second;
      }
      throw std::out_of_range("No such element");
    }
    const Value & at(const Key & key) const
    {
      node_t * traverser = search(key);
      if (traverser)
      {
        return traverser->data.second;
      }
      throw std::out_of_range("No such element");
    }
    const_iterator find(const Key & key) const
    {
      node_t * wanted = root_;
      while ((wanted) && (wanted->data.first != key))
      {
        if (compare_(wanted->data.first, key))
        {
          wanted = wanted->right;
        }
        else
        {
          wanted = wanted->left;
        }
      }
      return const_iterator(wanted);
    }
    size_t size() const noexcept
    {
      return size_;
    }
    bool empty() const noexcept
    {
      return (size_ == 0);
    }
    void clear()
    {
      clear_impl(root_);
      root_ = nullptr;
    }
    ~Tree()
    {
      clear();
    }
  private:
    node_t * root_;
    size_t size_;
    Compare compare_;
    void clear_impl(node_t * node)
    {
      if (node)
      {
        --size_;
        clear_impl(node->left);
        clear_impl(node->right);
        delete node;
      }
    }
    node_t * get_min(node_t * node) const
    {
      node_t * result = node;
      if (!result)
      {
        return nullptr;
      }
      while (result->left)
      {
        result = result->left;
      }
      return result;
    }
    void rotateLeft(node_t * node)
    {
      node_t * newRoot = node->right;
      node->right = newRoot->left;
      if (newRoot->left)
      {
        newRoot->left->parent = node;
      }
      newRoot->parent = node->parent;
      if (node->isRoot())
      {
        root_ = newRoot;
      }
      else
      {
        if (node->isLeftChild())
        {
          node->parent->left = newRoot;
        }
        else
        {
          node->parent->right = newRoot;
        }
      }
      newRoot->left = node;
      node->parent = newRoot;
      node->height = node->height + 1 - std::min(newRoot->height, 0);
      newRoot->height = newRoot->height + 1 + std::max(node->height, 0);
    }
    void rotateRight(node_t * node)
    {
      node_t * newRoot = node->left;
      node->left = newRoot->right;
      if (newRoot->right)
      {
        newRoot->right->parent = node;
      }
      newRoot->parent = node->parent;
      if (node->isRoot())
      {
        root_ = newRoot;
      }
      else
      {
        if (node->isLeftChild())
        {
          node->parent->left = newRoot;
        }
        else
        {
          node->parent->right = newRoot;
        }
      }
      newRoot->right = node;
      node->parent = newRoot;
      node->height = node->height - 1 - std::max(0, newRoot->height);
      newRoot->height = newRoot->height - 1 + std::min(0, node->height);
    }
    node_t * search_impl(node_t * node, const Key & key)
    {
      if (!node)
      {
        return nullptr;
      }
      else if (node->data.first == key)
      {
        return node;
      }
      else if (compare_(key, node->data.first))
      {
        return search_impl(node->left, key);
      }
      else
      {
        return search_impl(node->right, key);
      }
    }
    void insert_impl(const Key & key, const Value & val, node_t * currentNode)
    {
      try
      {
        if (compare_(key, currentNode->data.first))
        {
          if (currentNode->left)
          {
            insert_impl(key, val, currentNode->left);
          }
          else
          {
            currentNode->left = new node_t(key, val, currentNode);
            getNewBalance(currentNode->left);
          }
        }
        else
        {
          if (currentNode->right)
          {
            insert_impl(key, val, currentNode->right);
          }
          else
          {
            currentNode->right = new node_t(key, val, currentNode);
            getNewBalance(currentNode->right);
          }
        }
      }
      catch (...)
      {
        clear();
        throw;
      }
    }
  };
}

#endif