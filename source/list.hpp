/**
 * \file
 * \brief Contains list class and his functions
*/


#define ASSERT(condition, message)                                          \
do {                                                                        \
    if (!(condition)) {                                                     \
        printf("%s(%i) in %s:\n", __FILE__, __LINE__, __PRETTY_FUNCTION__); \
        printf(message);                                                    \
        exit(1);                                                            \
    }                                                                       \
} while(0)


/// Array of objects with type T
template<class T>
class List {
private:
    T *buffer;          ///< List buffer
    size_t size;        ///< Current amount of elements in list
    size_t capacity;    ///< Current buffer size


    /**
     * \brief Calls destructors for buffer elements then free buffer
    */
    void free_buffer() {
        ASSERT(buffer, "Buffer is nullptr!\n");

        for(size_t i = 0; i < size; i++) 
            buffer[i].~T();

        free(buffer);
        buffer = nullptr;
    }


    /**
     * \brief Copies n elements from one buffer to another
    */
    void copy_buffer(T *dst, const T *src, size_t n) {
        for (size_t i = 0; i < n; i++)
            dst[i] = src[i];
    }


    /**
     * \brief Allocates buffer at size of new_capacity and sets capacity
    */
    void allocate_buffer(size_t new_capacity) {
        ASSERT(buffer == nullptr, "Buffer is not nullptr!\n");

        buffer = (T*) calloc(new_capacity, sizeof(T));
        ASSERT(buffer, "Failed to allocate buffer!\n");

        capacity = new_capacity;
    }


    /**
     * \brief Resizes list buffer to new capacity and sets capacity
    */
    void resize_capacity(size_t new_capacity) {
        ASSERT(buffer, "Buffer is nullptr!\n");
        capacity = new_capacity;

        T *new_buffer = (T*) calloc(capacity, sizeof(T));
        ASSERT(buffer, "Failed to allocate buffer!\n");

        copy_buffer(new_buffer, buffer, size);

        free_buffer();
        buffer = new_buffer;
    }

public:
    /**
     * \brief Creates empty list
    */
    List() :
        buffer(nullptr), size(0), capacity(1)
    {
        allocate_buffer(capacity);
    }


    /**
     * \brief Creates list with init_size copies of init_value
    */
    List(size_t init_size, const T &init_value) :
        buffer(nullptr), size(init_size), capacity(init_size + 1)
    {
        allocate_buffer(capacity);

        for (size_t i = 0; i < init_size; i++)
            buffer[i] = init_value;
    }


    /**
     * \brief Copies values from other list to this
    */
    List(const List &list):
        buffer(nullptr), size(list.size), capacity(list.capacity)
    {
        allocate_buffer(capacity);
        
        copy_buffer(buffer, list.buffer, size);
    }


    /**
     * \brief Copies values from other list to this
    */
    List &operator = (const List &list) {
        if (this != &list) {
            free_buffer();

            size = list.size;

            allocate_buffer(list.capacity);
            copy_buffer(buffer, list.buffer, size);
        }

        return *this;
    }


    /**
     * \brief Returns current size of the list
    */
    size_t getSize() const {
        return size;
    }


    /**
     * \brief Resizes list if needed
    */
    void resize(size_t new_size, const T &init_value) {
        ASSERT(buffer, "Buffer is nullptr!\n");

        if (new_size == size) return;
        
        if (new_size < size) {
            for(size_t i = new_size; i < size; i++) 
                buffer[i].~T();
            
            size = new_size;
            return;
        }
        
        if (new_size >= capacity)
            resize_capacity(new_size + 1);

        for (size_t i = size; i < new_size; i++)
            buffer[i] = init_value;

        size = new_size;
    }


    /**
     * \brief Inserts new element at index position 
    */
    void insert(size_t index, T new_value) {
        ASSERT(index < size, "Index is out of range!\n");

        size++;
        for(size_t i = size - 1; i > index; i--)
            buffer[i] = buffer[i - 1];
        
        buffer[index] = new_value;
    
        if (size == capacity) resize_capacity(capacity * 2);
    }


    /**
     * \brief Removes element at index position
    */
    void remove(size_t index) {
        ASSERT(index < size, "Index is out of range!\n");

        size--;
        for(size_t i = index; i < size; i++)
            buffer[i] = buffer[i + 1];
        
        // Last element duplicate should be destroyed manually
        buffer[size].~T();
        
        if (size * 4 < capacity) resize_capacity(capacity / 2);
    }


    /**
     * \brief Pushes value to the end of the list
    */
    void push_back(const T &new_value) {
        ASSERT(buffer, "Buffer is nullptr!\n");

        buffer[size++] = new_value;

        if (size == capacity) resize_capacity(capacity * 2);
    }


    /**
     * \brief Pops last value from list
    */
    void pop_back() {
        ASSERT(buffer, "Buffer is nullptr!\n");

        size--;

        if (size * 4 < capacity) resize_capacity(capacity / 2);
    }


    /**
     * \brief Returns first element
     * \warning If list is empty, an abort() will be called
    */
    T &front() { return (*this)[0]; }


    /**
     * \brief Returns last element
     * \warning If list is empty, an abort() will be called
    */
    T &back() { return (*this)[size - 1]; }


    /**
     * \brief Returns const reference to the first element
     * \warning If list is empty, an abort() will be called
    */
    const T &front() const { return front(); }


    /**
     * \brief Returns const reference to the last element
     * \warning If list is empty, an abort() will be called
    */
    const T &back() const { return back(); }


    /**
     * \brief Gives access to value at the given index
     * \warning If index is out range, an abort() will be called
    */
    T &operator [] (size_t i) {
        ASSERT(buffer, "Buffer is nullptr!\n");
        ASSERT(i < size, "Index is out of range!\n");
        return buffer[i];
    }


    /**
     * \brief Gives read access to value at the given index
     * \warning If index is out range, an abort() will be called
    */
    const T &operator [] (size_t i) const {
        ASSERT(buffer, "Buffer is nullptr!\n");
        ASSERT(i < size, "Index is out of range!\n");
        return buffer[i];
    }


    /**
     * \brief Free list buffer and set fields with zeros
    */
    ~List() {
        free_buffer();

        size = 0;
        capacity = 0;
    }
};
