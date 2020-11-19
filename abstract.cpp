#include <iostream>
#include <iterator>
//надеюсь вы вы не хейтеры using namespace std;
using namespace std;

template<class T>
class Node {
public:
    //вообще возможно лучше засунуть все это в private, и во всех дальнейших классах прописать
    // что node дружественный, над этим еще подумаем
    T value;
    Node *right;
    Node *left;
    Node *father;
    Node(Node<T> *node = NULL):father(node){
        right = NULL;
        left = NULL;
    }
    Node(T val,Node<T> *node = NULL):father(node),value(val){
        right = NULL;
        left = NULL;
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
        node->value = p;
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
private:
    Node<T> *root;
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
    // это нужно будет наследовать и реализовать в порядке убывания приоретета(по моему личному мнению)
    virtual void insert(T val) = 0;

    virtual void erase(T val) = 0;

    virtual OwnIterator<T> find(T val) = 0;

    virtual OwnIterator<T> begin() = 0;

    virtual OwnIterator<T> end() = 0;

    virtual int size() = 0;

    virtual int empty() = 0;
};

int main() {
    return 0;
}
