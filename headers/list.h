#pragma once

#include "error.h"

template <class T>
class List{
    template <class X>
    struct list_node{
        X value;
        list_node *next;
    };

    list_node<T> *head;
    list_node<T> *tail; 
    int size_;
public:

    List(): head(nullptr), tail(nullptr), size_(0){};
    List(const List&) = delete;
    List(List&& other): head(other.head), tail(other.tail), size_(other.size_){
        other.head = nullptr;
        other.tail = nullptr;
        other.size_ = 0;
    }
    List& operator=(const List&) = delete;

    void push(T);  
    void Add(T);   
    T pop();
    
    void clear() { while(size_) pop(); }
    int  size() const{ return size_; }
    ~List(){ clear(); }
    T& Get(int index);
    const T& Get(int index) const;
    template <class X>
    class Iterator{
        list_node<X>* cur_node;
        
    public:
        Iterator(const List<X> *lst){
            cur_node = lst->head;
        }
        X& next(){
            if (!cur_node)
                throw EndOfIterator();
            X& val = cur_node->value;
            cur_node = cur_node->next;
            return val;
        }
        bool isEnd(){ return !cur_node;}
    };

    Iterator<T> iter() const{
        return Iterator<T>(this);
    }

    bool in_list(T comp_obj);
    T* find_in_list(T comp_obj);
};

template <class T>
inline void List<T>::push(T value)
{
    list_node<T>* new_elem = new list_node<T>;
    new_elem->value = value;
    new_elem->next = head;
    head = new_elem;
    if (size_ == 0) {
        tail = new_elem; // если список был пуст, tail указывает на новый элемент
    }
    size_++;
}

template <class T>
inline void List<T>::Add(T value)
{
    list_node<T>* new_elem = new list_node<T>;
    new_elem->value = value;
    new_elem->next = nullptr;
    if (size_ == 0) {
        head = tail = new_elem;
    } else {
        tail->next = new_elem;
        tail = new_elem;
    }
    size_++;
}

template <class T>
inline T List<T>::pop()
{
    if (!size_)
        throw EmptyListError();
    T return_value = head->value;
    list_node<T>* tmp = head;
    head = head->next;
    size_--;
    if (size_ == 0) {
        tail = nullptr;  // если список стал пустым, сбросить tail
    }
    delete tmp;    
    return return_value;
}

// Остальные методы не требуют изменений для работы с tail
// (но если будут операции удаления из середины, нужно будет следить за tail)

template <class T>
inline T &List<T>::Get(int index)
{
    if (index < 0 || index >= size_) 
        throw Error();
    list_node<T>* current = head;
    for (int i = 0; i < index; ++i) {
        current = current->next;
    }
    return current->value;
}

template <class T>
inline const T &List<T>::Get(int index) const
{
    if (index < 0 || index >= size_) 
        throw Error();
    list_node<T>* current = head;
    for (int i = 0; i < index; ++i) {
        current = current->next;
    }
    return current->value;
}

template <class T>
inline bool List<T>::in_list(T comp_obj)
{
    bool flag = false;
    Iterator<T> it = this->iter();
    while(!it.isEnd() && !flag)
        flag = (comp_obj == it.next());
    return flag;
}

template <class T>
inline T *List<T>::find_in_list(T comp_obj)
{
    bool flag = false;
    T* tmp = nullptr;
    Iterator<T> it = this->iter();
    while(!it.isEnd() && !flag){
        flag = (comp_obj == it.next());
        tmp = it.next();
    }
    if(!flag) tmp = nullptr;
    return tmp;
}
