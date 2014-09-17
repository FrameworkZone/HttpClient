/****************************************************************************
  Copyright (c) 2013-2014 libo.
 
  losemymind.libo@gmail.com

****************************************************************************/

#ifndef Foundation_Buffer_h
#define Foundation_Buffer_h

#include <cstring>
#include <cstddef>
#include <memory>

namespace network {

/** 
 * A buffer class that allocates a buffer of a given type and size 
 * in the constructor and deallocates the buffer in the destructor.
 *
 * This class is useful everywhere where a temporary buffer
 * is needed.
 */
template <class T>
class Buffer
{
 /** Creates and allocates the Buffer. */
public:
    Buffer(std::size_t capacity):
        m_capacity(capacity),
        m_used(0),
        m_ptr(new T[capacity]),
        m_ownMem(true)
    {
    }

    /** 
     * Creates the Buffer. Length argument specifies the length
     * of the supplied memory pointed to by pMem in the number
     * of elements of type T. Supplied pointer is considered
     * blank and not owned by Buffer, so in this case Buffer 
     * only acts as a wrapper around externally supplied 
     * (and lifetime-managed) memory.
     */
    explicit Buffer(T* pMem, std::size_t length):
        m_capacity(length),
        m_used(length),
        m_ptr(pMem),
        m_ownMem(false)
    {
    }

    /** 
     * Creates and allocates the Buffer; copies the contents of
     * the supplied memory into the buffer. Length argument specifies
     * the length of the supplied memory pointed to by pMem in the
     * number of elements of type T.
     */
    explicit Buffer(const T* pMem, std::size_t length):
        m_capacity(length),
        m_used(length),
        m_ptr(new T[length]),
        m_ownMem(true)
    {
        if (m_used)
            std::memcpy(m_ptr, pMem, m_used * sizeof(T));
    }

    /**  Copy constructor. */
    Buffer(const Buffer& other):
        m_capacity(other.m_used),
        m_used(other.m_used),
        m_ptr(new T[other.m_used]),
        m_ownMem(true)
    {
        if (m_used)
            std::memcpy(m_ptr, other.m_ptr, m_used * sizeof(T));
    }

    /** Assignment operator. */
    Buffer& operator =(const Buffer& other)
    {
        if (this != &other)
        {
            Buffer tmp(other);
            swap(tmp);
            m_ownMem = true;
        }
        return *this;
    }

    ~Buffer()
    {
        if (m_ownMem) delete [] m_ptr;
    }

    /** 
     * Resizes the buffer capacity and size. If preserveContent is true,
     * the content of the old buffer is copied over to the
     * new buffer. The new capacity can be larger or smaller than
     * the current one; if it is smaller, capacity will remain intact.
     * Size will always be set to the new capacity.
     *  
     * Buffers only wrapping externally owned storage can not be 
     * resized. If resize is attempted on those, IllegalAccessException
     * is thrown.
     */
    void resize(std::size_t newCapacity, bool preserveContent = true)
    {
        if (!m_ownMem)
            throw std::exception("Cannot resize buffer which does not own its storage.");

        if (newCapacity > m_capacity)
        {
            T* ptr = new T[newCapacity];
            if (preserveContent)
                std::memcpy(ptr, m_ptr, m_used * sizeof(T));

            delete [] m_ptr;
            m_ptr = ptr;
            m_capacity = newCapacity;
        }

        m_used = newCapacity;
    }

    /** 
     * Sets the buffer capacity. If preserveContent is true,
     * the content of the old buffer is copied over to the
     * new buffer. The new capacity can be larger or smaller than
     * the current one; size will be set to the new capacity only if 
     * new capacity is smaller than the current size, otherwise it will
     * remain intact.
     * 
     * Buffers only wrapping externally owned storage can not be 
     * resized. If resize is attempted on those, IllegalAccessException
     * is thrown.
     */
    void setCapacity(std::size_t newCapacity, bool preserveContent = true)
    {
        if (!m_ownMem)
            throw std::exception("Cannot resize buffer which does not own its storage.");

        if (newCapacity != m_capacity)
        {
            T* ptr = new T[newCapacity];
            if (preserveContent)
            {
                std::size_t newSz = m_used < newCapacity ? m_used : newCapacity;
                std::memcpy(ptr, m_ptr, newSz * sizeof(T));
            }

            delete [] m_ptr;
            m_ptr = ptr;
            m_capacity = newCapacity;

            if (newCapacity < m_used) m_used = newCapacity;
        }
    }
    /** 
     * Assigns the argument buffer to this buffer.
     * If necessary, resizes the buffer.
     */
    void assign(const T* buf, std::size_t sz)
    {
        if (0 == sz) return;
        if (sz > m_capacity) resize(sz, false);
        std::memcpy(m_ptr, buf, sz * sizeof(T));
        m_used = sz;
    }

    /** Resizes this buffer and appends the argument buffer.*/
    void append(const T* buf, std::size_t sz)
    {
        if (0 == sz) return;
        resize(m_used + sz, true);
        std::memcpy(m_ptr + m_used - sz, buf, sz * sizeof(T));
    }

    /** Resizes this buffer by one element and appends the argument value. */
    void append(T val)
        
    {
        resize(m_used + 1, true);
        m_ptr[m_used - 1] = val;
    }

    // Resizes this buffer and appends the argument buffer.
    void append(const Buffer& buf)
    {
        append(buf.begin(), buf.size());
    }

    // Returns the allocated memory size in elements.
    std::size_t capacity() const 
    {
        return m_capacity;
    }

    // Returns the allocated memory size in bytes.
    std::size_t capacityBytes() const 
    {
        return m_capacity * sizeof(T);
    }

    // Swaps the buffer with another one.
    void swap(Buffer& other) 
    {
        using std::swap;

        swap(m_ptr, other.m_ptr);
        swap(m_capacity, other.m_capacity);
        swap(m_used, other.m_used);
    }

    // Compare operator.
    bool operator ==(const Buffer& other) const
    {
        if (this != &other)
        {
            if (m_used == other.m_used)
            {
                if (std::memcmp(m_ptr, other.m_ptr, m_used * sizeof(T)) == 0)
                {
                    return true;
                }
            }
            return false;
        }

        return true;
    }

    // Compare operator.
    bool operator !=(const Buffer& other) const
    {
        return !(*this == other);
    }

    // Sets the contents of the bufer to zero.
    void clear()
    {
        std::memset(m_ptr, 0, m_used * sizeof(T));
    }

    // Returns the used size of the buffer in elements.
    std::size_t size() const
    {
        return m_used;
    }

    // Returns the used size of the buffer in bytes.
    std::size_t sizeBytes() const  
    {
        return m_used * sizeof(T);
    }

    // Returns a pointer to the beginning of the buffer.
    T* begin()
    {
        return m_ptr;
    }

    // Returns a pointer to the beginning of the buffer.
    const T* begin() const
    {
        return m_ptr;
    }

    // Returns a pointer to end of the buffer.
    T* end()
    {
        return m_ptr + m_used;
    }

    // Returns a pointer to the end of the buffer.
    const T* end() const
    {
        return m_ptr + m_used;
    }

    // Return true if buffer is empty.
    bool empty() const
    {
        return 0 == m_used;
    }

    T& operator [] (std::size_t index)
    {
        assert (index < m_used);
        return m_ptr[index];
    }

    const T& operator [] (std::size_t index) const
    {
        assert (index < m_used);
        return m_ptr[index];
    }

private:
    Buffer();
    std::size_t m_capacity;
    std::size_t m_used;
    T*          m_ptr;
    bool        m_ownMem;
};

} // namespace Foundation
#endif // Foundation_Buffer_h

