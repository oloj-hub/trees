#include <iostream>
#include <iterator>
#include <set>


//прошу не называйте переменные BLACK и RED
enum colors {
    BLACK,
    RED
};

template<class T>
class Node {
public:
    //вообще возможно лучше засунуть все это в private, и во всех дальнейших классах прописать
    // что node дружественный, над этим еще подумаем
    T value;
    Node *right;
    Node *left;
    Node *father;
    colors color;

    explicit Node(Node<T> *node = NULL, colors col = BLACK) : father(node), color(col) {
        right = NULL;
        left = NULL;
    }

    explicit Node(T val, Node<T> *node = NULL, colors col = BLACK) : father(node), value(val), color(col) {
        right = NULL;
        left = NULL;
    }

    Node *brother() {
        if (father != NULL) {
            if (this == father->left)
                return father->right;
            else
                return father->left;
        } else return NULL;
    }

    Node *uncle() {
        if (father != NULL)
            return father->brother();
        else return NULL;
    }
};

template<class T>
//хуй знает как работают кастомные итераторы, я что-то почитал,
// но пока неоч разобрался https://habr.com/ru/post/265491/
class OwnIterator : std::iterator<std::input_iterator_tag, T> {
private:
    //можно конечно просто указатель на переменную хранить,
    // но мне кажется так будет лучше
    Node<T> *node;
public:
    OwnIterator(Node<T> *p) {
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
        return node->value;
    };

    OwnIterator operator++() {
        if (node != NULL) {
            if (node->right != NULL) {
                node = node->right;
                while (node->left != NULL) {
                    node = node->left;
                }
            } else {
                while (node->father != NULL && node != node->father->left) {
                    node = node->father;
                }
                node = node->father;
            }
        };
        return *this;
    };

    OwnIterator operator++(int) {
        if (node != NULL) {
            if (node->right != NULL) {
                node = node->right;
                while (node->left != NULL) {
                    node = node->left;
                }
            } else {
                while (node->father != NULL && node != node->father->left) {
                    node = node->father;
                }
                node = node->father;
            }
        };
        return *this;
    }
};

template<class T>
class Tree {
protected:
    Node<T> *root;
private:
    void delete_tree(Node<T> *node) {
        if (node->right != NULL)
            delete_tree(node->right);
        if (node->left != NULL)
            delete_tree(node->left);
        delete node;
    }

public:
    //в с++ можно красиво передовать переменные из нового конструктора
    // в конструктор наследуемого класса, потом чекни как
    Tree() {
        root = new Node<T>;
    }

    Tree(T val) {
        root = new Node<T>;
        root->value = val;
    }

    ~Tree() {
        delete_tree(root);
    }

    // это нужно будет наследовать и реализовать в порядке убывания приоретета(по моему личному мнению)
    virtual void insert(T val) = 0;

    virtual void erase(T val) = 0;

    virtual OwnIterator<T> find(T val) = 0;

    virtual OwnIterator<T> begin() = 0;

    virtual OwnIterator<T> end() = 0;

    virtual int size() = 0;

    virtual bool empty() = 0;
};


template<class T>
class RedBlackTree : public Tree<T> {
    using Tree<T>::root;
private:
    int scale;

    void *delete_node(Node<T> *node) {
        if (node->right == NULL && node->left == NULL) {
            if (node->color == BLACK) {
                fix_deleting(node);
            }
            if (node->father != NULL) {
                if (node == node->father->right)
                    node->father->right = NULL;
                else node->father->left = NULL;
                delete node;
            }
        } else if (node->right == NULL || node->left == NULL) {
            Node<T> *son;
            colors col = node->color;
            if (node->right != NULL && node->left == NULL)
                son = node->right;
            else son = node->left;
            son->father = node->father;
            if (node->father != NULL) {
                if (node == node->father->left)
                    node->father->left = son;
                else node->father->right = son;
            } else root = son;
            delete node;
            if (son->color == BLACK && col == BLACK)
                fix_deleting(son);
            else son->color = BLACK;
        } else {
            Node<T> *next = node->right;
            while (next->left != NULL) {
                next = next->left;
            }
            node->value = next->value;
            delete_node(next);
        }
    }

    void rotate_left(Node<T> *node) {
        Node<T> *pivot = node->right;
        pivot->father = node->father;
        if (node->father != NULL) {
            if (node->father->left == node)
                node->father->left = pivot;
            else {
                node->father->right = pivot;
            }
        } else root = pivot;
        node->right = pivot->left;
        if (pivot->left != NULL)
            pivot->left->father = node;
        node->father = pivot;
        pivot->left = node;
        node = pivot;
    }

    void rotate_right(Node<T> *node) {
        Node<T> *pivot = node->left;
        pivot->father = node->father;
        if (node->father != NULL) {
            if (node->father->left == node)
                node->father->left = pivot;
            else
                node->father->right = pivot;
        } else root = pivot;
        node->left = pivot->right;
        if (pivot->right != NULL)
            pivot->right->father = node;
        node->father = pivot;
        pivot->right = node;
        node = pivot;
    }

    void balance_tree(Node<T> *node) {
        if (node->father == NULL) {
            node->color = BLACK;
            return;
        }
        while (node->father != NULL && node->father->color == RED) {
            if (node->father == node->father->father->left) {
                if (node->uncle() != NULL && node->uncle()->color == RED) {
                    node->father->color = BLACK;
                    node->uncle()->color = BLACK;
                    node->father->father->color = RED;
                    node = node->father->father;
                } else {
                    if (node == node->father->right) {
                        node = node->father;
                        rotate_left(node);
                    }
                    node->father->color = BLACK;
                    node->father->father->color = RED;
                    rotate_right(node->father->father);
                }
            } else {
                if (node->uncle() != NULL && node->uncle()->color == RED) {
                    node->father->color = BLACK;
                    node->uncle()->color = BLACK;
                    node->father->father->color = RED;
                    node = node->father->father;
                } else {
                    if (node == node->father->left) {
                        node = node->father;
                        rotate_right(node);
                    }
                    node->father->color = BLACK;
                    node->father->father->color = RED;
                    rotate_left(node->father->father);
                }
            }
        }
        root->color = BLACK;
    }

    bool black_or_null(Node<T> *node) {
        if (node == NULL || node->color == BLACK)
            return true;
        else return false;
    }

    void fix_deleting(Node<T> *node) {
        if (node->father != NULL) {
            if (node == node->father->left) {
                if (node->brother()->color == RED) {
                    node->brother()->color = BLACK;
                    node->father->color = RED;
                    rotate_left(node->father);
                }
                if (black_or_null(node->brother()->left) && black_or_null(node->brother()->right)) {
                    node->brother()->color = RED;
                    if (node->father->color == BLACK)
                        fix_deleting(node->father);
                    else node->father->color = BLACK;
                } else {
                    if (black_or_null(node->brother()->right)) {
                        node->brother()->left->color = BLACK;
                        node->brother()->color = RED;
                        rotate_right(node->brother());
                    }
                    node->brother()->color = node->father->color;
                    node->father->color = BLACK;
                    node->brother()->right->color = BLACK;
                    rotate_left(node->father);
                }
            } else {
                if (node->brother()->color == RED) {
                    node->brother()->color = BLACK;
                    node->father->color = RED;
                    rotate_right(node->father);
                }
                if (black_or_null(node->brother()->left) && black_or_null(node->brother()->right)) {
                    node->brother()->color = RED;
                    if (node->father->color == BLACK)
                        fix_deleting(node->father);
                    else node->father->color = BLACK;
                } else {
                    if (black_or_null(node->brother()->left)) {
                        node->brother()->right->color = BLACK;
                        node->brother()->color = RED;
                        rotate_left(node->brother());
                    }
                    node->brother()->color = node->father->color;
                    node->father->color = BLACK;
                    node->brother()->left->color = BLACK;
                    rotate_right(node->father);
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
            root->
                    value = val;
        } else {
            Node<T> *p;
            Node<T> *father;
            p = root;
            while (p != NULL && p->value != val) {
                father = p;
                if (val > p->value)
                    p = p->right;
                else
                    p = p->left;
            }
            if (p != NULL)
                return;
            if (val > father->value) {
                father->
                        right = new Node<T>(val, father, RED);
                balance_tree(father
                                     ->right);
            } else {
                father->
                        left = new Node<T>(val, father, RED);
                balance_tree(father
                                     ->left);
            }
        }
        scale++;
    };

    void erase(T val) override {
        if (scale != 0) {
            Node<T> *node;
            node = root;
            while (node != NULL && node->value != val) {
                if (val > node->value)
                    node = node->right;
                else
                    node = node->left;
            }
            if (node != NULL) {
                delete_node(node);
                scale--;
            }
        } else return;
    }

    OwnIterator<T> find(T val) override {
        if (scale != 0) {
            Node<T> *node = root;
            while (node != NULL && node->value != val) {
                if (val < node->value)
                    node = node->left;
                else
                    node = node->right;
            }
            return OwnIterator<T>(node);
        } else return NULL;
// по хорошему нужно конечно не NULL возвращать, а
// специальный итератор tree.end()
    };

    OwnIterator<T> begin() override {
        if (scale != 0) {
            Node<T> *node = root;
            while (node->left != NULL) {
                node = node->left;
            }
            return OwnIterator<T>(node);
        } else return NULL;
    };

    OwnIterator<T> end() override {
        if (scale != 0) {
            Node<T> *node = root;
            while (node->right != NULL) {
                node = node->right;
            }
            return OwnIterator<T>(node);
        } else return NULL;
    };

    int size() override {
        return scale;
    };

    bool empty() override {
        return scale == 0;
    };
};

int main() {

    return 0;
}


