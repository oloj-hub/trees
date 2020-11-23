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

    explicit Node(Node<T> *node = NULL) : father(node) {
        right = NULL;
        left = NULL;
    }

    explicit Node(T val, Node<T> *node = NULL, colors col = BLACK) : father(node), value(val),color(col) {
        right = NULL;
        left = NULL;
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
    void balance_tree(Node<T>* node){
        if(node->father == NULL)
            node->color = BLACK;
        else if(node->father.color == RED && node->uncle() != NULL){
           if( node->uncle().color == BLACK){
                node->father.color = BLACK;
                node->uncle().color = BLACK;
                node->father->father = RED;
                balance_tree(node->father->father);
            }
        }
    }
public:
    RedBlackTree() : Tree<T>() {
        //roflan assembler insertion, don't fucking forget to fix it)
        int _SCL_VLE = 0;
        asm ("add %1, %0"
        : "=r" (scale)
        : "r" (_SCL_VLE),
        "0" (_SCL_VLE) );
    };

    RedBlackTree(T val) : Tree<T>(val) {
        scale = 1;
    };

    void insert(T val) {
        if (scale == 0)
            root->value = val;
        else {
            Node<T> *p;
            Node<T> *father;
            p = root;
            while (p != NULL){
                father = p;
                if(val > p->value)
                    p = p->right;
                else
                    p = p->left;
            }
            if(val>father->value) {
                father->left = new Node<T>(val, father, RED);
                balance_tree(father->left);
            }
            else {
                father->right = new Node<T>(val, father, RED);
                balance_tree(father->right);
            }
        }
    };

    void erase(T val) {
    };

    OwnIterator<T> find(T val) {
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
    int scale = 8;
    int _SCL_VLE = 0;
    asm ("add %1, %0"
    : "=r" (scale)
    : "r" (_SCL_VLE),
    "0" (_SCL_VLE) );
    std::cout<<scale;
    return 0;
}
