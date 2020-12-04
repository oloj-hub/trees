#include <iostream>
#include <iterator>
#include <set>

#include "node.h"

#include "tree_iterator.h"

#include "abstract_tree.h"


template<class T>
class RedBlackTree : public Tree<T> {
    using Tree<T>::root;

public:
    typedef std::shared_ptr<Node<T>> node_ptr_c;

private:
    int scale;

    void  delete_node(node_ptr_c node) {
        if (node->right == nullptr && node->left == nullptr) {
            if (node->color == BLACK) {
                fix_deleting(node);
            }
            if (node->father.lock() != nullptr) {
                if (node == node->father.lock()->right)
                    node->father.lock()->right = nullptr;
                else node->father.lock()->left = nullptr;
                node = nullptr;
            }
        } else if (node->right == nullptr || node->left == nullptr) {
            node_ptr_c son;
            colors col = node->color;
            if (node->right != nullptr && node->left == nullptr)
                son = node->right;
            else son = node->left;
            son->father.lock() = node->father.lock();
            if (node->father.lock() != nullptr) {
                if (node == node->father.lock()->left)
                    node->father.lock()->left = son;
                else node->father.lock()->right = son;
            } else root = son;
            node = nullptr;
            if (son->color == BLACK && col == BLACK)
                fix_deleting(son);
            else son->color = BLACK;
        } else {
            node_ptr_c next = node->right;
            while (next->left != nullptr) {
                next = next->left;
            }
            node->key = next->key;
            delete_node(next);
        }
    }

    void rotate_left(node_ptr_c node) {
        node_ptr_c pivot = node->right;
        pivot->father.lock() = node->father.lock();
        if (node->father.lock() != nullptr) {
            if (node->father.lock()->left == node)
                node->father.lock()->left = pivot;
            else {
                node->father.lock()->right = pivot;
            }
        } else root = pivot;
        node->right = pivot->left;
        if (pivot->left != nullptr)
            pivot->left->father.lock() = node;
        node->father.lock() = pivot;
        pivot->left = node;
        node = pivot;
    }

    void rotate_right(node_ptr_c node) {
        node_ptr_c pivot = node->left;
        pivot->father.lock() = node->father.lock();
        if (node->father.lock() != nullptr) {
            if (node->father.lock()->left == node)
                node->father.lock()->left = pivot;
            else
                node->father.lock()->right = pivot;
        } else root = pivot;
        node->left = pivot->right;
        if (pivot->right != nullptr)
            pivot->right->father.lock() = node;
        node->father.lock() = pivot;
        pivot->right = node;
        node = pivot;
    }

    void balance_tree(node_ptr_c node) {
        if (node->father.lock() == nullptr) {
            node->color = BLACK;
            return;
        }
        while (node->father.lock() != nullptr && node->father.lock()->color == RED) {
            if (node->father.lock() == node->father.lock()->father.lock()->left) {
                if (node->uncle() != nullptr && node->uncle()->color == RED) {
                    node->father.lock()->color = BLACK;
                    node->uncle()->color = BLACK;
                    node->father.lock()->father.lock()->color = RED;
                    node = node->father.lock()->father.lock();
                } else {
                    if (node == node->father.lock()->right) {
                        node = node->father.lock();
                        rotate_left(node);
                    }
                    node->father.lock()->color = BLACK;
                    node->father.lock()->father.lock()->color = RED;
                    rotate_right(node->father.lock()->father.lock());
                }
            } else {
                if (node->uncle() != nullptr && node->uncle()->color == RED) {
                    node->father.lock()->color = BLACK;
                    node->uncle()->color = BLACK;
                    node->father.lock()->father.lock()->color = RED;
                    node = node->father.lock()->father.lock();
                } else {
                    if (node == node->father.lock()->left) {
                        node = node->father.lock();
                        rotate_right(node);
                    }
                    node->father.lock()->color = BLACK;
                    node->father.lock()->father.lock()->color = RED;
                    rotate_left(node->father.lock()->father.lock());
                }
            }
        }
        root->color = BLACK;
    }

    bool black_or_null(node_ptr_c node) {
        if (node == nullptr || node->color == BLACK)
            return true;
        else return false;
    }

    void fix_deleting(node_ptr_c node) {
        if (node->father.lock() != nullptr) {
            if (node == node->father.lock()->left) {
                if (node->brother()->color == RED) {
                    node->brother()->color = BLACK;
                    node->father.lock()->color = RED;
                    rotate_left(node->father.lock());
                }
                if (black_or_null(node->brother()->left) && black_or_null(node->brother()->right)) {
                    node->brother()->color = RED;
                    if (node->father.lock()->color == BLACK)
                        fix_deleting(node->father.lock());
                    else node->father.lock()->color = BLACK;
                } else {
                    if (black_or_null(node->brother()->right)) {
                        node->brother()->left->color = BLACK;
                        node->brother()->color = RED;
                        rotate_right(node->brother());
                    }
                    node->brother()->color = node->father.lock()->color;
                    node->father.lock()->color = BLACK;
                    node->brother()->right->color = BLACK;
                    rotate_left(node->father.lock());
                }
            } else {
                if (node->brother()->color == RED) {
                    node->brother()->color = BLACK;
                    node->father.lock()->color = RED;
                    rotate_right(node->father.lock());
                }
                if (black_or_null(node->brother()->left) && black_or_null(node->brother()->right)) {
                    node->brother()->color = RED;
                    if (node->father.lock()->color == BLACK)
                        fix_deleting(node->father.lock());
                    else node->father.lock()->color = BLACK;
                } else {
                    if (black_or_null(node->brother()->left)) {
                        node->brother()->right->color = BLACK;
                        node->brother()->color = RED;
                        rotate_left(node->brother());
                    }
                    node->brother()->color = node->father.lock()->color;
                    node->father.lock()->color = BLACK;
                    node->brother()->left->color = BLACK;
                    rotate_right(node->father.lock());
                }
            }

        }
    }

public:

    RedBlackTree() : Tree<T>() {
        scale = 0;
    };

    RedBlackTree(T val) : Tree<T>(val) {
        scale = 1;
    };

    void insert(T val) override {
        if (scale == 0) {
            root = std::make_shared<Node<T>>(val);
        } else {
            node_ptr_c p;
            node_ptr_c father;
            p = root;
            while (p != nullptr && p->key != val) {
                father = p;
                if (val > p->key)
                    p = p->right;
                else
                    p = p->left;
            }
            if (p != nullptr)
                return;
            if (val > father->key) {
                father->
                        right = std::make_shared<Node<T>>(val, father, RED);
                balance_tree(father
                                     ->right);
            } else {
                father->
                        left = std::make_shared<Node<T>>(val, father, RED);
                balance_tree(father
                                     ->left);
            }
        }
        scale++;
    };

    void erase(T val) override {
        if (scale != 0) {
            node_ptr_c node;
            node = root;
            while (node != nullptr && node->key != val) {
                if (val > node->key)
                    node = node->right;
                else
                    node = node->left;
            }
            if (node != nullptr) {
                delete_node(node);
                scale--;
            }
        } else return;
    }

    OwnIterator<T> find(T val) override {
        if (scale != 0) {
            node_ptr_c node = root;
            while (node != nullptr && node->key != val) {
                if (val < node->key)
                    node = node->left;
                else
                    node = node->right;
            }
            return OwnIterator<T>(node);
        } else return OwnIterator<T>(nullptr);
// по хорошему нужно конечно не NULL возвращать, а
// специальный итератор tree.end()
    };

    OwnIterator<T> begin() override {
        if (scale != 0) {
            node_ptr_c node = root;
            while (node->left != nullptr) {
                node = node->left;
            }
            return OwnIterator<T>(node);
        } else return OwnIterator<T>(nullptr);
    };

    OwnIterator<T> end() override {
        if (scale != 0) {
            node_ptr_c node = root;
            while (node->right != nullptr) {
                node = node->right;
            }
            return OwnIterator<T>(node);
        } else return OwnIterator<T>(nullptr);
    };

    int size() override {
        return scale;
    };

    bool empty() override {
        return scale == 0;
    };
};

int main() {

	using namespace std;
    RedBlackTree<int> k;
    k.insert(5);
    cout << k.size() << endl;
    k.erase(5);
    for (int i = 0; i < 80; i ++) {
	k.insert(i);
    }
    k.insert(13);
    cout << k.size() << endl;
    return 0;
}


