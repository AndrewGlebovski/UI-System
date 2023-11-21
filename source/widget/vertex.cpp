/**
 * \file
 * \brief Contains implementation of vertex and vertex array
*/


#include "common/assert.hpp"
#include "widget/vertex.hpp"


VertexArray::VertexArray(PrimitiveType type_, size_t size_ = 0) :
    type(type_), data(nullptr), size(size_), capacity(std::max(size_, 1ul))
{
    data = new Vertex[capacity];
}


VertexArray::VertexArray(const VertexArray& other) :
    type(other.type), data(nullptr), size(other.size), capacity(other.capacity)
{
    data = new Vertex[capacity];
    for (size_t ind = 0; ind < size; ++ind)
        data[ind] = other.data[ind];
}


VertexArray& VertexArray::operator = (const VertexArray& other) {
    if (&other != this) {
        delete[] data;

        type = other.type;
        size = other.size;
        capacity = other.capacity;

        data = new Vertex[capacity];
        for (size_t ind = 0; ind < size; ++ind)
            data[ind] = other.data[ind];
    }

    return *this;
}


VertexArray::~VertexArray() {
    size = capacity = -1;
    delete[] data;
}


PrimitiveType VertexArray::getPrimitive() const {
    return type;
}


void VertexArray::setPrimitive(PrimitiveType type_) {
    type = type_;
}


size_t VertexArray::getSize() const {
    return size;
}


void VertexArray::resize(size_t new_size) {
    if (new_size <= capacity) return;

    Vertex* new_data = new Vertex[new_size];
    for (size_t ind = 0; ind < size; ++ind)
        new_data[ind] = data[ind];
    
    delete[] data;

    data = new_data;
    capacity = new_size;
}


void VertexArray::appendVertex(const Vertex& vertex) {
    if (size == capacity) resize(capacity * 2);
    ASSERT(size < capacity, "Buffer is full!\n");

    data[size++] = vertex;
}


Vertex& VertexArray::operator[](size_t index) {
    ASSERT(index < size, "Index is out of range!\n");
    return data[index];
}


const Vertex& VertexArray::operator[](size_t index) const {
    ASSERT(index < size, "Index is out of range!\n");
    return data[index];
}
