#include <iostream>
#include <iterator>

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

    //вот здесь все пизда не просто нужно будет реализовывать чтобы он шел по дереву
    OwnIterator &operator++();

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

    virtual int empty() = 0;
};


template<class T>
class RedBlackTree : public Tree<T> {
    using Tree<T>::root;
private:
    int scale;

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
        if (node->father == NULL)
            node->color = BLACK;
        else if (node->father->color == RED) {
            if (node->uncle() != NULL && node->uncle()->color == RED) {
                node->father->color = BLACK;
                node->uncle()->color = BLACK;
                node->father->father->color = RED;
                balance_tree(node->father->father);
            } else {
                Node<T> *g = node->father->father;
                if (g != NULL) {
                    if (node->father == g->left) {
                        if (node == node->father->right)
                            rotate_left(node->father);
                        node->father->color = BLACK;
                        g->color = RED;
                        rotate_right(g);
                    } else if (node->father == g->right) {
                        if (node == node->father->left)
                            rotate_right(node->father);
                        node->father->color = BLACK;
                        g->color = RED;
                        rotate_left(g);
                    }
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

    void insert(T val) {
        if (scale == 0) {
            root->value = val;
        } else {
            Node<T> *p;
            Node<T> *father;
            p = root;
            while (p != NULL) {
                father = p;
                if (val > p->value)
                    p = p->right;
                else
                    p = p->left;
            }
            if (val > father->value) {
                father->right = new Node<T>(val, father, RED);
                balance_tree(father->right);
            } else {
                father->left = new Node<T>(val, father, RED);
                balance_tree(father->left);
            }
        }
        scale++;
    };

    void erase(T val) {
    };

    OwnIterator<T> find(T val) {
        Node<T> *node = root;
        while (node != NULL && node->value != val) {
            if (val < node->value)
                node = node->left;
            else node = node->right;
        }
        return OwnIterator<T>(node);
    };

    OwnIterator<T> begin() {
    };

    OwnIterator<T> end() {
    };

    int size() {
    };

    int empty() {
    };
};


int main() {
    RedBlackTree<int> t;
    for (int i = 0; i < 100; i++) {
        t.insert(rand() % 477);
    }
    for (int i = 0; i < 477; i++) {
        if (t.find(i)!=NULL) {
            std::cout << *t.find(i) << '\n';
        }
    }
    return 0;
}

