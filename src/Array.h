#ifndef PROJET_POO_ARRAY_H
#define PROJET_POO_ARRAY_H

#include <cstddef>
#include <type_traits>
#include <algorithm>

struct IndexOutOfBounds : std::exception
{
    const std::size_t index;

    explicit IndexOutOfBounds(std::size_t index) : index(index) {}
};

template<class T>
class Array
{
private:
    T* data;
    std::size_t capacity;
    std::size_t size;

    void checkIndex(std::size_t index) const
    {
        if (index >= size)
        {
            throw IndexOutOfBounds(index);
        }
    }

    void ensureCapacity(std::size_t minCapacity)
    {
        if (capacity >= minCapacity)
        {
            return;
        }

        std::size_t newCapacity = std::max(capacity * 2, minCapacity);
        T* newData = new T[newCapacity];
        for (size_t i = 0; i < size; i++)
        {
            if constexpr (std::is_trivially_copyable<T>())
            {
                newData[i] = data[i];
            }
            else
            {
                new(&newData[i]) T(data[i]);
            }

            if constexpr (!std::is_trivially_destructible<T>())
            {
                data[i].~T();
            }
        }

        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

public:
    Array() : data(new T[0]), capacity(0), size(0) {}
    ~Array()
    {
        empty();
        delete[] data;
        capacity = 0;
    }

    std::size_t getSize() const
    {
        return size;
    }

    T& operator[](size_t index)
    {
        checkIndex(index);
        return data[index];
    }

    void empty()
    {
        if constexpr (!std::is_trivially_destructible<T>())
        {
            for (std::size_t i = 0; i < size; i++)
            {
                data[i].~T();
            }
        }

        size = 0;
    }

    T& add(const T& e)
    {
        ensureCapacity(size + 1);

        if constexpr (std::is_trivially_copyable<T>())
        {
            data[size] = e;
        }
        else
        {
            new(&data[size]) T(e);
        }

        return data[size++];
    }

    void remove(std::size_t index)
    {
        checkIndex(index);
        if (index < --size)
        {
            data[index] = data[size];
        }

        if constexpr (!std::is_trivially_destructible<T>())
        {
            data[size].~T();
        }
    }
};

#endif
