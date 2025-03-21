#pragma once

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <stdexcept>
#include <iostream>
#include <utility>

#include "array_ptr.h"

using namespace std;

class ReserveProxyObj {
public:
    ReserveProxyObj(size_t value)
        : value_(value) {
    }

    size_t GetValue() {
        return value_;
    }

private:
    size_t value_;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
};

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) 
    : SimpleVector(size, Type()) {
    }

    explicit SimpleVector(ReserveProxyObj new_capacity)
        : items_(static_cast<size_t>(0)), size_(static_cast<size_t>(0)), capacity_(new_capacity.GetValue()) {
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value)
        : items_(size), size_(size), capacity_(size) {
        fill(items_.Get(), items_.Get() + size, value);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init)
        : items_(init.size()), size_(init.size()), capacity_(init.size()) {
        copy(init.begin(), init.end(), items_.Get());
    }

    // Конструктор копирования
    SimpleVector(const SimpleVector& other)
        : items_(other.GetCapacity()), size_(other.GetSize()), capacity_(other.GetCapacity()) {
        copy(other.begin(), other.begin() + other.GetSize(), items_.Get());
    }

    // Конструктор перемещения
    SimpleVector(SimpleVector&& other)
        : items_(std::move(other.items_)), size_(other.size_), capacity_(other.GetCapacity()) {
        other.size_ = 0;
        other.capacity_ = 0;
    }

    // Перемещающий оператор присваивания
    SimpleVector& operator=(SimpleVector&& other) {
        if (this != &other) {
            items_ = std::move(other.items_);
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    void Reserve(size_t new_capacity) {
        if (capacity_ < new_capacity) {
            ArrayPtr<Type> new_items_(new_capacity);
            move(items_.Get(), items_.Get() + size_, new_items_.Get());
            items_.swap(new_items_);
            capacity_ = new_capacity;
        }
    };

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            SimpleVector tmp(rhs);
            swap(tmp);
        }
        return *this;
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        if (size_ == capacity_) {
            size_t new_capacity = std::max(capacity_ * 2, size_t(1)); // capacity_ == 0 ? 1 : capacity_ * 2
            ArrayPtr<Type> new_items_(new_capacity);
            copy(items_.Get(), items_.Get() + size_, new_items_.Get());
            items_.swap(new_items_);
            capacity_ = new_capacity;
        }
        items_[size_] = item;
        size_ += 1;
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(Type&& item) {
        if (size_ == capacity_) {
            Reserve(std::max(capacity_ * 2, size_t(1))); // capacity_ == 0 ? 1 : capacity_ * 2
        }
        items_[size_] = std::move(item);
        size_ += 1;
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        size_t index = pos - items_.Get();
        if (size_ == capacity_) {
            size_t new_capacity = std::max(capacity_ * 2, size_t(1)); // capacity_ == 0 ? 1 : capacity_ * 2
            ArrayPtr<Type> new_items_(new_capacity);
            copy(items_.Get(), items_.Get() + size_, new_items_.Get());
            items_.swap(new_items_);
            capacity_ = new_capacity;
        }
        copy_backward(items_.Get() + index, items_.Get() + size_, items_.Get() + size_ + 1);
        items_[index] = value;
        size_ += 1;
        return items_.Get() + index;
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, Type&& value) {
        size_t index = pos - items_.Get();
        if (size_ == capacity_) {
            Reserve(std::max(capacity_ * 2, size_t(1))); // capacity_ == 0 ? 1 : capacity_ * 2
        }
        move_backward(items_.Get() + index, items_.Get() + size_, items_.Get() + size_ + 1);
        items_[index] = std::move(value);
        size_ += 1;
        return items_.Get() + index;
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        if (size_ != 0) {
            size_ -= 1;
        }
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        size_t index = pos - items_.Get();
        move(items_.Get() + index + 1, items_.Get() + size_, items_.Get() + index);
        size_ -= 1;
        return &items_[index];
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        items_.swap(other.items_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_) {
            throw out_of_range("index >= size");
        }
        else {
            return items_[index];
        }
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_) {
            throw out_of_range("index >= size");
        }
        else {
            return items_[index];
        }
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size < size_) {
            size_ = new_size;
        }
        else if (new_size <= capacity_) {
            for (size_t i = size_; i < new_size; ++i) {
                items_[i] = Type{};
            }
            size_ = new_size;
        }
        else {
            size_t new_capacity = std::max(new_size, capacity_ * 2);
            ArrayPtr<Type> new_items_(new_capacity);
            move(items_.Get(), items_.Get() + size_, new_items_.Get());
            for (size_t i = size_; i < new_size; ++i) {
                new_items_[i] = Type{};
            }
            items_.swap(new_items_);
            size_ = new_size;
            capacity_ = new_capacity;
        }
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return items_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return items_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return items_.Get() + size_;
    }
private:
    ArrayPtr<Type> items_;

    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return lhs.GetSize() == rhs.GetSize() && equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}