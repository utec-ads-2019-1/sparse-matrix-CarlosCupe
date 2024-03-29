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
    bool resize(int rows, int columns, bool force) const;
    void print() const;

    void destroy() const;

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
    Element<T> **ptr_row, **ptr_col;
    bool exist = false;
    
    if (find_by_rows(x, y, ptr_row))
        exist = ((*ptr_row)->y == y);
    
    if (exist && (value != 0)) {
        (*ptr_row)->value = value;
        return;
    }

    find_by_cols(x, y, ptr_col);

    if (exist && (value == 0)) {
        Node<T> *temp = (Node<T>*)(*ptr_col);
        *ptr_row = (Element<T> *)(*ptr_row)->down;
        *ptr_col = (Element<T> *)(*ptr_col)->next;
        delete temp;
        return;
    }

    if (value == 0) {
        return;
    }

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
        for (unsigned i = 0; i < vector_col.size(); ++i) { 
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

    if (!find_by_rows(x, y, ptr)) return 0;
    
    return ((*ptr)->y == y) ? (*ptr)->value : 0;
}

template <typename T>
Matrix<T> Matrix<T>::operator*(T scalar) const {
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
Matrix<T> Matrix<T>::operator*(Matrix<T> other) const {
    if (other.columns != rows) exit(1);
    
    Element<T> *ptr_col, *ptr_row;
    int temp_sum;
    Matrix<T> temp(other.rows, columns);

    for (int i = 0; i < vector_col.size(); ++i) {
        for (int j = 0; j < other.vector_row.size(); ++j) {
            temp_sum = 0;
            ptr_row = (Element<T>*)other.vector_row[j]->down;
            ptr_col = (Element<T>*)vector_col[i]->next;

            while (ptr_col && ptr_row) {
                if (ptr_col->x == ptr_row->y) {
                    temp_sum += (ptr_col->value * ptr_row->value);
                    ptr_row = (Element<T>*)ptr_row->down;
                    ptr_col = (Element<T>*)ptr_col->next;
                } else {
                    if (ptr_col->x > ptr_row->y)
                        ptr_row = (Element<T>*)ptr_row->down;
                    else
                        ptr_col = (Element<T>*)ptr_col->next;
                }
            }
            temp.set(j, i, temp_sum);
        }
    }   return temp;
}

template <typename T>
Matrix<T> Matrix<T>::operator+(Matrix<T> other) const {
    if (rows != other.rows || columns != other.columns) exit(1);

    Matrix<T> temp(rows, columns);
    Element<T> *ptr_int, *ptr_ext;
    
    T ans;

    for (unsigned i = 0; i < vector_row.size(); ++i) {
        ptr_int = (Element<T>*)vector_row[i]->down;
        ptr_ext = (Element<T>*)other.vector_row[i]->down;
        for (unsigned j = 0; j < vector_col.size(); ++j) {
            if (ptr_int == nullptr && ptr_ext == nullptr) break;

            ans = 0;

            while (ptr_int && ptr_int->y < j)
                ptr_int = (Element<T>*)ptr_int->down;

            while (ptr_ext && ptr_ext->y < j)
                ptr_ext = (Element<T>*)ptr_ext->down;
            
            if (ptr_int && ptr_int->y == j) ans += ptr_int->value;
            
            if (ptr_ext && ptr_ext->y == j) ans += ptr_ext->value;

            if (ans != 0) temp.set(i, j, ans);
        }
    }   return temp;
}

template <typename T>
Matrix<T> Matrix<T>::operator-(Matrix<T> other) const {
    if (rows != other.rows || columns != other.columns) exit(1);

    Matrix<T> temp(rows, columns);
    Element<T> *ptr_int, *ptr_ext;
    
    T ans;

    for (unsigned i = 0; i < vector_row.size(); ++i) {
        ptr_int = (Element<T>*)vector_row[i]->down;
        ptr_ext = (Element<T>*)other.vector_row[i]->down;
        for (unsigned j = 0; j < vector_col.size(); ++j) {
            if (ptr_int == nullptr && ptr_ext == nullptr) break;
            ans = 0;

            while (ptr_int && ptr_int->y < j)
                ptr_int = (Element<T>*)ptr_int->down;

            while (ptr_ext && ptr_ext->y < j)
                ptr_ext = (Element<T>*)ptr_ext->down;
            
            if (ptr_int && ptr_int->y == j) ans = ptr_int->value;
            
            if (ptr_ext && ptr_ext->y == j) ans = ans - ptr_ext->value;

            if (ans != 0) temp.set(i, j, ans);
        }
    }   return temp;
}

template <typename T>
Matrix<T> Matrix<T>::transpose() const {
    Matrix<T> temp(columns, rows);
    Node<T> *ptr_last;
    Element<T> **ptr_new, **ptr_actual;

    for (int i = 0; i < vector_col.size(); ++i) {
        ptr_new     = (Element<T>**)&temp.vector_row[i]->down;
        ptr_actual  = (Element<T>**)&vector_col[i]->next;
        
        if (i) ptr_last = (Node<T>*)temp.vector_row[i-1]->down;
        
        for (; *ptr_actual != nullptr; ptr_actual = (Element<T>**)&(*ptr_actual)->next) {

            *ptr_new = (*ptr_actual)->self_copy();

            if (i == 0) {
                ptr_last = (Node<T>*)(temp.vector_col[(*ptr_new)->y]);
                ptr_last->next = *ptr_new;
            } else {
                while (ptr_last) {
                    if (((Element<T>*)ptr_last)->y < (*ptr_new)->y)
                        ptr_last = ptr_last->down;
                    else break;
                }
                if (ptr_last && ((Element<T>*)ptr_last)->y == (*ptr_new)->y)
                    ptr_last->next = *ptr_new;
            }
            ptr_new = (Element<T>**)&(*ptr_new)->down;
        }
    }   return temp;
}
template <typename T>

bool Matrix<T>::resize(int rows, int columns, bool force) const {
    if (rows < this->rows) {
        for (unsigned i = rows; i < vector_row.size(); ++i)
            vector_row[i]->delete_recursive();
        
        vector_row.resize(rows);
    } else {
        for (unsigned i = vector_row.size(); i < rows; ++i)
            vector_row.push_back(new Header<T> (i));
    }

    if (columns < this->columns) {
        for (unsigned i = columns; i < vector_col.size(); ++i) {
            delete vector_col[i];
        }
        vector_col.resize(columns);
    } else {
        for (unsigned i = vector_col.size(); i < columns; ++i)
            vector_col.push_back(new Header<T> (i));
    }

    this->rows = rows;
    this->columns = columns;
}

template <typename T>
void Matrix<T>::destroy() const {
    for (int i = 0; i < vector_row.size(); ++i)
        vector_row[i]->delete_recursive();
    
    vector_row.clear();
    
    for (int i = 0; i < vector_col.size(); ++i)
        delete vector_col[i];

    vector_col.clear();
}

template <typename T>
Matrix<T>::~Matrix() {
    vector_row.clear();
    vector_col.clear();
}