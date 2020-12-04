#ifndef ABSTRACT_TREE_H
#define ABSTRACT_TREE_H

#include "node.h"


#include <memory>
#include <iostream>
#include "tree_iterator.h"


template<class T>
class Tree {
protected:
	std::shared_ptr<Node<T>> root;
public:
    //в с++ можно красиво передовать переменные из нового конструктора
    // в конструктор наследуемого класса, потом чекни как
    Tree() {
        root = nullptr;
    }

    Tree(T val) {
        root = std::make_shared<Node<T>>(val, nullptr);
    }

    ~Tree() {
    	root = nullptr;
    }

    // это нужно будет наследовать и реализовать в порядке убывания приоретета(по моему личному мнению)
    virtual void insert(T key) = 0;

    virtual void erase(T key) = 0;

    virtual OwnIterator<T> find(T key) = 0;

    virtual OwnIterator<T> begin() = 0;

    virtual OwnIterator<T> end() = 0;

    virtual int size() = 0;

    virtual bool empty() = 0;
};


#endif
