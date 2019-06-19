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

    void delete_recursive() {
        if (down)
            down->delete_recursive();
        delete this;
    }

    ~Node(){}

    void transpose_nodes() { swap(next, down); }

    friend class Matrix<T>;
};

template <typename T>
class Header : public Node<T> {
protected:
    unsigned index;

public:
    Header(unsigned __index__)
    : index(__index__) { }

    friend class Matrix<T>;
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

    void transpose_pos() { swap(x, y); }

    Element<T>* self_copy() {
        return new Element<T> (y, x, value);
    }

    friend class Matrix<T>;
};

#endif //SPARSE_MATRIX_NODE_H
