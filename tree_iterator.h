#ifndef AVL_TREE_ITERATOR_H
#define AVL_TREE_ITERATOR_H

#include "node.h"




template<class T>
class OwnIterator : std::iterator<std::input_iterator_tag, T> {
private:
    std::shared_ptr<Node<T>> node;

public:

   typedef std::shared_ptr<Node<T>> node_ptr_c;

   OwnIterator(node_ptr_c p) {
        node = p;
    };

    OwnIterator(const OwnIterator &it) : node(it.node) {
    };

    bool operator!=(OwnIterator const &other) {
        return node != other.node;
    };

    bool operator==(OwnIterator const &other) {
        return node == other.node;
    };

    typename OwnIterator::reference operator*() {
        return node->key;
    };

    OwnIterator operator++() {
        if (node != nullptr) {
            if (node->right != nullptr) {
                node = node->right;
                while (node->left != nullptr) {
                    node = node->left;
                }
            } else {
                while (node->father != nullptr && node != node->father.lock()->left) {
                    node = node->father.lock();
                }
                node = node->father.lock();
            }
        };
        return *this;
    };

    OwnIterator operator++(int) {
        if (node != nullptr) {
            if (node->right != nullptr) {
                node = node->right;
                while (node->left != nullptr) {
                    node = node->left;
                }
            } else {
                while (node->father != nullptr && node != node->father.lock()->left) {
                    node = node->father.lock();
                }
                node = node->father.lock();
            }
        };
        return *this;
    }
};

#endif
