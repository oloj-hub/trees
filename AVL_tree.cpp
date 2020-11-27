#include <iostream>
#include <iterator>
#include <memory>


template<class T>
class Node {
public:
    //вообще возможно лучше засунуть все это в private, и во всех дальнейших классах прописать
    // что node дружественный, над этим еще подумаем
    	typedef std::shared_ptr<Node<T>> node_ptr_c;
	T key;
	unsigned int height;
	node_ptr_c right;
    	node_ptr_c left;
   	std::weak_ptr<Node<T>> father;

    explicit Node(node_ptr_c p) : father(p) {
        right = nullptr;
        height = 1;
	left = nullptr;
    }

    explicit Node(T& val, node_ptr_c node) : father(node), key{val} {
        right = nullptr;
        left = nullptr;
    }
    Node* brother(){
        if(this == father->left)
            return father->right;
        else
            return father->left;
    }
    Node* uncle(){
        return father->brother();
    }
};

template<class T>
//хуй знает как работают кастомные итераторы, я что-то почитал,
// но пока неоч разобрался https://habr.com/ru/post/265491/
class OwnIterator : std::iterator<std::input_iterator_tag, T> {
private:
    //можно конечно просто указатель на переменную хранить,
    // но мне кажется так будет лучше
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

template<class T>
class Tree {
protected:
	std::shared_ptr<Node<T>> root;
public:
    //в с++ можно красиво передовать переменные из нового конструктора
    // в конструктор наследуемого класса, потом чекни как
    Tree() {
        root = std::make_shared<Node<T>>(nullptr);
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
	AVL_tree<int> k(6);
	std::cout << k.size() << std::endl;
	k.insert(5);
	std::cout << k.size() << std::endl;
	k.erase(6);
	std::cout << k.size() << std::endl;
	std::cout << *(k.find(5)) << std::endl;
	for (int i = 0; i < 80; i++) {
		i++;
	}
    return 0;
}
