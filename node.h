#ifndef SPARSE_MATRIX_NODE_H
#define SPARSE_MATRIX_NODE_H

template <typename T>
class Matrix;

template <typename T>
class Node {
protected:
    Node<T> *next, *down;

public:
    explicit Node() 
    : next(nullptr), down(nullptr) {}

    friend class Matrix<T>;
};

template <typename T>
class Header : public Node<T> {
protected:
    unsigned index;

public:
    Header(unsigned __index__)
    : index(__index__) { }
};

template <typename T>
class Element : public Node<T> {
protected:
    unsigned x, y;
    T value;

public:
    Element(unsigned __x__, unsigned __y__, T __value__)
    : x(__x__), y(__y__), value(__value__) { }

    Element(){ }

    friend class Matrix<T>;
};

#endif //SPARSE_MATRIX_NODE_H
