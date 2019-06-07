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
    vector<Header<T>*> vector_row;
    vector<Header<T>*> vector_col;
    unsigned rows, columns;

public:
    Matrix(unsigned rows, unsigned columns);

    void set(unsigned, unsigned, T);
    T operator()(unsigned, unsigned) const;
    Matrix<T> operator*(T scalar) const;
    Matrix<T> operator*(Matrix<T> other) const;
    Matrix<T> operator+(Matrix<T> other) const;
    Matrix<T> operator-(Matrix<T> other) const;
    Matrix<T> transpose() const;
    void print() const;

    ~Matrix();

protected:
    bool find_by_rows(unsigned, unsigned, Element<T>**&) const;
    bool find_by_cols(unsigned, unsigned, Element<T>**&) const;
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
bool Matrix<T>::find_by_rows(unsigned x, unsigned y, Element<T>** &data) const {
    data = (Element<T>**)&(vector_row[x]->down);
    
    while (*data != nullptr) {
        if ((*data)->y >= y) break;
        data = (Element<T>**)&((*data)->down);
    }   return *data != nullptr;
}

template <typename T>
bool Matrix<T>::find_by_cols(unsigned x, unsigned y, Element<T>** &data) const {
    data = (Element<T>**)&(vector_col[y]->next);

    while (*data != nullptr) {
        if ((*data)->x >= x) break;
        data = (Element<T>**)&((*data)->next);
    }   return *data != nullptr;
}

template <typename T>
void Matrix<T>::set(unsigned x, unsigned y, T value) {
    if (x >= rows || y >= columns)
        exit(1);
    
    Element<T> **ptr_row;
    Element<T> **ptr_col;
    find_by_rows(x, y, ptr_row);
    /*
    if () {
        if ((*ptr_row)->y == y) {
            // Crear funcion delete si value es 0
            //if (value == 0) ;
            //else (*ptr_row)->value = value;
            return;
        }
    }*/

    find_by_cols(x, y, ptr_col);

    Element<T> *temp = new Element<T>(x, y, value);
    temp->down = (Node<T>*)*ptr_col;
    temp->next = (Node<T>*)*ptr_row;
    
    *ptr_col = temp;
    *ptr_row = temp;
}

template <typename T>
void Matrix<T>::print() const{
    Element<T> *ptr;
    for (Header<T>* row : vector_row) {
        ptr = (Element<T>*)row->down;
        for (unsigned i = 0; i < columns; ++i) { 
            if (ptr && ptr->y == i) {
                cout << ptr->value << " ";
                ptr = (Element<T>*)ptr->down;
            } else
                cout << 0 << " ";
        }
        cout << endl;
    }
}

template <typename T>
T Matrix<T>::operator()(unsigned x, unsigned y) const {
    if (x >= rows || y >= columns)
        exit(1);
    
    Element<T> **ptr;

    if (!find_by_rows(x, y, ptr)) exit(1);

    return (*ptr)->value;
}

template <typename T>
Matrix<T> Matrix<T>::operator*(T scalar) const {//Se puede mejorar
    Element<T> *ptr;
    Matrix<T> temp(rows, columns);

    for (Header<T>* row : vector_row) {
        ptr = (Element<T>*)row->down;
        while (ptr != nullptr) {
            temp.set(ptr->x, ptr->y, ptr->value * scalar);
            ptr = (Element<T>*)ptr->down;
        }
    }
    return temp;
}

template <typename T>
Matrix<T> Matrix<T>::operator+(Matrix<T> other) const {
    if (rows != other.rows || columns != other.columns) exit(1);

    Matrix<T> temp(rows, columns);
    Element<T> *ptr_int, *ptr_ext;
    
    auto matrix_op = [&](Element<T> *&ptr, T val) {
        temp.set(ptr->x, ptr->y, ptr->value + val);
        ptr = (Element<T>*)ptr->down;
    };

    for (int i = 0; i < rows; ++i) {
        ptr_int = (Element<T>*)vector_row[i]->down;
        ptr_ext = (Element<T>*)other.vector_row[i]->down;
        
        while (ptr_int && ptr_ext) {
            if (ptr_ext == nullptr) {
                matrix_op(ptr_int, 0);
                continue;
            }
            if (ptr_int == nullptr) {
                matrix_op(ptr_ext, 0);
                continue;
            }

            if (ptr_ext->y == ptr_int->y) {
                matrix_op(ptr_int, ptr_ext->value);
                ptr_ext = (Element<T>*)ptr_ext->down;
            } else {
                if (ptr_ext->y < ptr_int->y)
                    matrix_op(ptr_ext, 0);
                else
                    matrix_op(ptr_int, 0);
            }
        }
    }   return temp;
}

template <typename T>
Matrix<T> Matrix<T>::operator-(Matrix<T> other) const {
    if (rows != other.rows || columns != other.columns) exit(1);

    Matrix<T> temp(rows, columns);
    Element<T> *ptr_int, *ptr_ext;
    
    auto matrix_op = [&](Element<T> *&ptr, T val) {
        temp.set(ptr->x, ptr->y, ptr->value - val);
        ptr = (Element<T>*)ptr->down;
    };

    for (int i = 0; i < rows; ++i) {
        ptr_int = (Element<T>*)vector_row[i]->down;
        ptr_ext = (Element<T>*)other.vector_row[i]->down;
        
        while (ptr_int && ptr_ext) {
            if (ptr_ext == nullptr) {
                matrix_op(ptr_int, 0);
                continue;
            }
            if (ptr_int == nullptr) {
                matrix_op(ptr_ext, -2 * ptr_ext->value);
                continue;
            }

            if (ptr_ext->y == ptr_int->y) {
                matrix_op(ptr_int, ptr_ext->value);
                ptr_ext = (Element<T>*)ptr_ext->down;
            } else {
                if (ptr_ext->y < ptr_int->y)
                    matrix_op(ptr_ext, -2 * ptr_ext->value);
                else
                    matrix_op(ptr_int, 0);
            }
        }
    }   return temp;
}

template <typename T>
Matrix<T>::~Matrix() {
    vector_col.clear();
    vector_row.clear();
}