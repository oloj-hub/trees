#ifndef NODE_H
#define NODE_H
#include<memory>
#include<iostream>



enum colors {
    BLACK,
    RED
};

template<class T>
class Node {
public:
    typedef std::shared_ptr<Node<T>> node_ptr_c;
    T key;
    unsigned int height;
    node_ptr_c right;
    node_ptr_c left;
    std::weak_ptr<Node<T>>  father;
    colors color;

    explicit Node(node_ptr_c node = nullptr, colors col = BLACK) : father(node), color(col) {
        right = nullptr;
	height = 1;
        left = nullptr;
    }

    explicit Node(T val, node_ptr_c node = nullptr, colors col = BLACK) : father(node), key(val), color(col) {
        right = nullptr;
        left = nullptr;
    }

    node_ptr_c brother() {
        if (father.lock() != nullptr) {
            if (this == father.lock()->left.get())
                return father.lock()->right;
            else
                return father.lock()->left;
        } else return nullptr;
    }

    node_ptr_c uncle() {
        if (father.lock() != nullptr)
            return father.lock()->brother();
        else return nullptr;
    }
};

#endif
