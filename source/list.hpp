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
    T *buffer_;         ///< List buffer
    size_t size_;       ///< Current amount of elements in list
    size_t capacity_;   ///< Current buffer size


    /**
     * \brief Calls destructors for buffer elements then free buffer
    */
    void free_buffer() {
        ASSERT(buffer_, "Buffer is nullptr!\n");

        for(size_t i = 0; i < size_; i++) 
            buffer_[i].~T();

        free(buffer_);
        buffer_ = nullptr;
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
        ASSERT(buffer_ == nullptr, "Buffer is not nullptr!\n");

        buffer_ = (T*) calloc(new_capacity, sizeof(T));
        ASSERT(buffer_, "Failed to allocate buffer!\n");

        capacity_ = new_capacity;
    }


    /**
     * \brief Resizes list buffer to new capacity and sets capacity
    */
    void resize_capacity(size_t new_capacity) {
        ASSERT(buffer_, "Buffer is nullptr!\n");
        capacity_ = new_capacity;

        T *new_buffer = (T*) calloc(capacity_, sizeof(T));
        ASSERT(buffer_, "Failed to allocate buffer!\n");

        copy_buffer(new_buffer, buffer_, size_);

        free_buffer();
        buffer_ = new_buffer;
    }

public:
    /**
     * \brief Creates empty list
    */
    List() :
        buffer_(nullptr), size_(0), capacity_(1)
    {
        allocate_buffer(capacity_);
    }


    /**
     * \brief Creates list with init_size copies of init_value
    */
    List(size_t init_size, const T &init_value) :
        buffer_(nullptr), size_(init_size), capacity_(init_size + 1)
    {
        allocate_buffer(capacity_);

        for (size_t i = 0; i < init_size; i++)
            buffer_[i] = init_value;
    }


    /**
     * \brief Copies values from other list to this
    */
    List(const List &list):
        buffer_(nullptr), size_(list.size_), capacity_(list.capacity_)
    {
        allocate_buffer(capacity_);
        
        copy_buffer(buffer_, list.buffer_, size_);
    }


    /**
     * \brief Copies values from other list to this
    */
    List &operator = (const List &list) {
        if (this != &list) {
            free_buffer();

            size_ = list.size_;

            allocate_buffer(list.capacity_);
            copy_buffer(buffer_, list.buffer_, size_);
        }

        return *this;
    }


    /**
     * \brief Returns current size of the list
    */
    size_t size() const {
        return size_;
    }


    /**
     * \brief Resizes list if needed
    */
    void resize(size_t new_size, const T &init_value) {
        ASSERT(buffer_, "Buffer is nullptr!\n");

        if (new_size == size_) return;
        
        if (new_size < size_) {
            for(size_t i = new_size; i < size_; i++) 
                buffer_[i].~T();
            
            size_ = new_size;
            return;
        }
        
        if (new_size >= capacity_)
            resize_capacity(new_size + 1);

        for (size_t i = size_; i < new_size; i++)
            buffer_[i] = init_value;

        size_ = new_size;
    }


    /**
     * \brief Inserts new element at index position 
    */
    void insert(size_t index, T new_value) {
        ASSERT(index < size_, "Index is out of range!\n");

        size_++;
        for(size_t i = size_ - 1; i > index; i--)
            buffer_[i] = buffer_[i - 1];
        
        buffer_[index] = new_value;
    
        if (size_ == capacity_) resize_capacity(capacity_ * 2);
    }


    /**
     * \brief Removes element at index position
    */
    void remove(size_t index) {
        ASSERT(index < size_, "Index is out of range!\n");

        size_--;
        for(size_t i = index; i < size_; i++)
            buffer_[i] = buffer_[i + 1];
        
        // Last element duplicate should be destroyed manually
        buffer_[size_].~T();
        
        if (size_ * 4 < capacity_) resize_capacity(capacity_ / 2);
    }


    /**
     * \brief Pushes value to the end of the list
    */
    void push_back(const T &new_value) {
        ASSERT(buffer_, "Buffer is nullptr!\n");

        buffer_[size_++] = new_value;

        if (size_ == capacity_) resize_capacity(capacity_ * 2);
    }


    /**
     * \brief Pops last value from list
    */
    void pop_back() {
        ASSERT(buffer_, "Buffer is nullptr!\n");

        size_--;

        if (size_ * 4 < capacity_) resize_capacity(capacity_ / 2);
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
    T &back() { return (*this)[size_ - 1]; }


    /**
     * \brief Returns const reference to the first element
     * \warning If list is empty, an abort() will be called
    */
    const T &front() const { return (*this)[0]; }


    /**
     * \brief Returns const reference to the last element
     * \warning If list is empty, an abort() will be called
    */
    const T &back() const { return (*this)[size_ - 1]; }


    /**
     * \brief Gives access to value at the given index
     * \warning If index is out range, an abort() will be called
    */
    T &operator [] (size_t i) {
        ASSERT(buffer_, "Buffer is nullptr!\n");
        ASSERT(i < size_, "Index is out of range!\n");
        return buffer_[i];
    }


    /**
     * \brief Gives read access to value at the given index
     * \warning If index is out range, an abort() will be called
    */
    const T &operator [] (size_t i) const {
        ASSERT(buffer_, "Buffer is nullptr!\n");
        ASSERT(i < size_, "Index is out of range!\n");
        return buffer_[i];
    }


    /**
     * \brief Free list buffer and sets fields with zeros
    */
    ~List() {
        free_buffer();

        size_ = 0;
        capacity_ = 0;
    }
};
