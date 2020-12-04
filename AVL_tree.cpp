#include <iostream>
#include <iterator>
#include <memory>

#include "node.h"

#include "tree_iterator.h"

#include "abstract_tree.h"



template<class T>
class AVL_tree : public Tree<T> {
    using Tree<T>::root;

public:
    typedef std::shared_ptr<Node<T>> node_ptr_c;	
    

    AVL_tree() : Tree<T>()  { 
	    count = 0; 
    };

    AVL_tree(T val) : Tree<T>(val) {
	    count = 1;
    };
private:
	int count;	

	unsigned int get_height(node_ptr_c p) const {
		return p != nullptr ? p->height	 : 0;
	}

	int bfactor(node_ptr_c p) const{
		return get_height(p->right)-get_height(p->left);
	}

	//Functions for balance
	
	void fixheight(node_ptr_c p);
	
	// Rotate right
	node_ptr_c rotate_right(node_ptr_c p);

	// Rotate left
	node_ptr_c rotate_left(node_ptr_c q);

	node_ptr_c balance(node_ptr_c p);


    	node_ptr_c append(node_ptr_c p, T val);


	//Functions for remove node
	node_ptr_c find_min(node_ptr_c p) {
		return p->left ? find_min(p->left) : p;
	}

	node_ptr_c remove_min(node_ptr_c p);

	node_ptr_c remove(node_ptr_c p, T& val);


	node_ptr_c insert(node_ptr_c p, T& val);

public:
	
    void insert(T val);

    void erase(T val);

    OwnIterator<T> find(T val) {
        if (count != 0) {
            node_ptr_c node = root;
            while (node != nullptr && node->key != val) {
                if (val < node->key)
                    node = node->left;
                else
                    node = node->right;
            }
            return OwnIterator<T>(node);
        } else 
		return OwnIterator<T>(nullptr);
// по хорошему нужно конечно не NULL возвращать, а
// специальный итератор tree.end()
    };


    OwnIterator<T> begin() {
	if (count != 0) {
            node_ptr_c node = root;
            while (node->left != nullptr) {
                node = node->left;
            }
            return OwnIterator<T>(node);
        } else return OwnIterator<T>(nullptr);

    };

    OwnIterator<T> end() {
	    if (count != 0) {
            node_ptr_c node = root;
            while (node->right != nullptr) {
                node = node->right;
            }
            return OwnIterator<T>(node);
        } else return OwnIterator<T>(nullptr);
    };

    int size();

    bool empty();

};

template <typename T>
void AVL_tree<T>::fixheight(AVL_tree<T>::node_ptr_c p) {
	unsigned int hl = get_height(p->left);
	unsigned int hr = get_height(p->right);
	p->height = (hl>hr?hl:hr)+1;	
}

template <typename T>
typename AVL_tree<T>::node_ptr_c AVL_tree<T>::rotate_right(node_ptr_c p) {
	node_ptr_c q = p->left;
	p->left = q->right;
	q->right = p;
	fixheight(p);
	fixheight(q);
	return q;
}


template <typename T>
typename AVL_tree<T>::node_ptr_c AVL_tree<T>::rotate_left(node_ptr_c q) {
	node_ptr_c p = q->right;
	q->right = p->left;
	p->left = q;
	fixheight(q);
	fixheight(p);
	return p;
}


template <typename T>
typename AVL_tree<T>::node_ptr_c AVL_tree<T>::balance(node_ptr_c p) {
	fixheight(p);
	if( bfactor(p) == 2 )
	{
		if( bfactor(p->right) < 0 )
			p->right = rotate_right(p->right);
		return rotate_left(p);
	}
	if( bfactor(p)==-2 )
	{
		if( bfactor(p->left) > 0  )
			p->left = rotate_left(p->left);
		return rotate_right(p);
	}
	return p;
}


template <typename T>
typename AVL_tree<T>::node_ptr_c AVL_tree<T>::append(node_ptr_c p, T val) {
	if (!p) { 
		return std::make_shared<Node<T>>(val, p);
	}
	if (val < p->key )
		p->left = append(p->left,val);
	else
		p->right = append(p->right,val);
	std::cout <<" insert(val)" << std::endl;;
	return balance(p);
}

template <typename T>
void AVL_tree<T>::insert(T val) {
	count ++;
	root = (root == nullptr) ? std::make_shared<Node<T>>(val, nullptr) : append(root, val);
}

template <typename T>
typename AVL_tree<T>::node_ptr_c AVL_tree<T>::remove_min(node_ptr_c p) {
	if (p->left == 0) return p->right;
	p->left = remove_min(p->left);
	return balance(p);
}

template <typename T>
typename AVL_tree<T>::node_ptr_c AVL_tree<T>::remove(node_ptr_c p, T& val) {
	if (!p) {
		return nullptr;
	}
	if( val < p->key )
		p->left = remove(p->left, val);
	else if( val > p->key )
		p->right = remove(p->right, val);	
	else
	{
		count--;
		node_ptr_c q = p->left;
		node_ptr_c r = p->right;
		p = nullptr;
		if (!r) return q;
		node_ptr_c min = find_min(r);
		min->right = remove_min(r);
		min->left = q;
		return balance(min);
	}
	return balance(p);
}

template <typename T>
void AVL_tree<T>::erase(T val) {
	root = remove(root, val);	
	std::cout << "remove(val)" << std::endl;;
}

template <typename T>
bool AVL_tree<T>::empty() {
	return (root == nullptr ? 1 : 0);
}



template <typename T>
int AVL_tree<T>::size() {
	return count;
}



int main() {
	using namespace std;
	AVL_tree<int> k;
	std::cout << k.size() << std::endl;
	k.insert(5);
	std::cout << k.size() << std::endl;
	k.insert(5);
	std::cout << k.size() << std::endl;
	std::cout << *(k.find(5)) << std::endl;
	for (int i = 0; i < 80; i++) {
		i++;
	}
    return 0;
}
