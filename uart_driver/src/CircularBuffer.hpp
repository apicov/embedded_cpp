#pragma once

//for std c++
#include <cstring> // For memcpy
#include <cstdint>
#include <cstddef>

template <typename T, size_t N>
class CircularBuffer {
public:
    CircularBuffer();
    ~CircularBuffer();

    int push(const T* data, size_t length);
    size_t pop(T* output, size_t length);
    size_t element_count() const;
    size_t capacity() const;
    bool is_full() const;
    bool is_empty() const;
    void clear();
    T peek_last();

private:
    T buffer_[N];
    size_t head_;
    size_t tail_;
    bool full_;
};


template <typename T, size_t N>
CircularBuffer<T, N>::CircularBuffer() 
    : head_(0), tail_(0), full_(false) {}

template <typename T, size_t N>
CircularBuffer<T, N>::~CircularBuffer()
{
  return;
}

template <typename T, size_t N>
int CircularBuffer<T, N>::push(const T* data, size_t length) {
    if (length > N) {
        return -1; // Data length exceeds buffer size
    }

    // Check if the buffer is full before pushing
    bool was_full = full_;

    // Copy data into the buffer
    for (size_t i = 0; i < length; ++i) {
        buffer_[head_] = data[i];
        head_ = (head_ + 1) % N;

        // If we are about to overwrite the tail, move the tail forward
        if (head_ == tail_) {
            tail_ = (tail_ + 1) % N; // Move tail to overwrite the oldest data
            full_ = true; // Set full_ to true since we are overwriting
        }
    }

    // Return 1 if data was overridden, otherwise return 0
    return was_full ? 1 : 0;
}

template <typename T, size_t N>
size_t CircularBuffer<T, N>::pop(T* output, size_t length)
{
    size_t available_elements = element_count();
    size_t elements_to_pop = (length > available_elements) ? available_elements : length;

    for (size_t i = 0; i < elements_to_pop; ++i) {
        output[i] = buffer_[tail_]; // Copy the oldest data to output
        tail_ = (tail_ + 1) % N; // Move tail forward
    }

    full_ = false; // After popping, the buffer cannot be full
    return elements_to_pop; // Return the number of elements popped
}

template <typename T, size_t N>
T CircularBuffer<T, N>::peek_last(){
  return buffer_[head_];  
}


template <typename T, size_t N>
size_t CircularBuffer<T, N>::element_count() const
{
    if (full_) {
        return N; // Buffer is full
    }
    return (head_ >= tail_) ? (head_ - tail_) : (N + head_ - tail_);
}

template <typename T, size_t N>
size_t CircularBuffer<T, N>::capacity() const
{
    return N;
}

template <typename T, size_t N>
bool CircularBuffer<T, N>::is_full() const
{
    return full_;
}

template <typename T, size_t N>
bool CircularBuffer<T, N>::is_empty() const
{
    return (!full_ && (head_ == tail_));
}

template <typename T, size_t N>
void CircularBuffer<T, N>::clear()
{
    head_ = tail_;
    full_ = false;
}
