#ifndef SPARSE_MATRIX_MATRIX_H
#define SPARSE_MATRIX_MATRIX_H

#include <stdexcept>
#include <iostream>
#include <vector>
#include "node.h"

using namespace std;

template <typename T>
class Matrix {
private:
    vector<Node<T>*> vector_row;
    vector<Node<T>*> vector_col;
    unsigned rows, columns;

public:
    Matrix(unsigned rows, unsigned columns);

    void set(unsigned, unsigned, T);
    T operator()(unsigned, unsigned) const;
    Matrix<T> operator*(T scalar) const;
    Matrix<T> operator*(Matrix<T> other) const;
    Matrix<T> operator+(Matrix<T> other) const;
    Matrix<T> operator-(Matrix<T> other) const;
    Matrix<T> operator=(Matrix<T> other) const;
    Matrix<T> transpose() const;
    void print() const;

    ~Matrix();
};

#endif //SPARSE_MATRIX_MATRIX_H


template <typename T>
Matrix<T>::Matrix(unsigned rows, unsigned columns) {    
    this->rows = rows;
    this->columns = columns;

    for (unsigned i = 0; i < rows; ++i)
        vector_row.push_back(new Header<T> (i));

    for (unsigned i = 0; i < columns; ++i)
        vector_col.push_back(new Header<T> (i));
}

template <typename T>
void Matrix<T>::set(unsigned x, unsigned y, T value) {
    if (x >= rows || y >= columns)
        exit(1);
    Element<T> **ptr_row = (Element<T>**)&(vector_row[x]->down);
    Element<T> **ptr_col = (Element<T>**)&(vector_col[y]->next);

    while (*ptr_row != nullptr) {
        if ((*ptr_row)->y >= y) break;
        ptr_row = (Element<T>**)&((*ptr_row)->down);
    }

    if ((*ptr_row)->y == y) {
        // Crear funcion delete si value es 0
        //if (value == 0) ;
        //else (*ptr_row)->value = value;
        return;
    }

    while (*ptr_col != nullptr) {
        if ((*ptr_col)->x > x) break;
        ptr_col = (Element<T>**)&((*ptr_col)->down);
    }

    Element<T> *temp = new Element<T>(x, y, value);
    temp->down = (Node<T>*)*ptr_col;
    temp->next = (Node<T>*)*ptr_row;

    *ptr_col = temp;
    *ptr_row = temp;
}

template <typename T>
void Matrix<T>::print() const{
    Element<T> ptr;
    for (Node<T> row : vector_row) {
        ptr = (Element<T>*)row->next;
        for (unsigned i = 0; i < rows; ++i) {
            if (ptr->value == i) {
                cout << ptr->value << " ";
                ptr = (Element<T>*)ptr->next;
            } else
                cout << 0 << " ";
        }
        cout << endl;
    }
}

template <typename T>
Matrix<T>::~Matrix() {
    vector_col.clear();
    vector_row.clear();
}