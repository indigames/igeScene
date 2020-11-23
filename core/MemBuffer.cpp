#include "core/MemBuffer.h"

#include <cstdarg>
#include <cstring>
#include <cctype>

namespace ige::scene
{
    MemBuffer::MemBuffer(size_t size)
        : m_buffer(nullptr), m_size(size), m_position(0)
    {
        if(m_size > 0)
        {
            m_buffer = new uint8_t[m_size];
        }
    }

    MemBuffer::~MemBuffer()
    {
        if(m_buffer)
        {
            delete[] m_buffer;
            m_buffer = nullptr;
        }
    }

    size_t MemBuffer::resize(size_t newSize)
    {
        if(newSize > m_size)
        {
            auto newBuffer = new uint8_t[newSize];
            if(m_buffer)
            {
                memcpy(newBuffer, m_buffer, m_size);
                delete[] m_buffer;
            }
            m_size = newSize;
            m_buffer = newBuffer;
        }
        return m_size;
    }

    //! Read bytes
    size_t MemBuffer::read(void* dest, size_t size)
    {
        if (size + m_position > m_size)
            size = m_size - m_position;
        if (!size || !dest)
            return 0;
        const auto* srcPtr = &m_buffer[m_position];
        memcpy(dest, srcPtr, size);
        m_position += size;
        return size;
    }

    /// Seek
    size_t MemBuffer::seek(size_t position)
    {
        if (position > m_size)
            position = m_size;
        m_position = position;
        return m_position;
    }

    /// Write bytes
    size_t MemBuffer::write(const void* data, size_t size)
    {
        if (!size)
            return 0;

        if (size + m_position > m_size)
            resize(size + m_position);

        auto* srcPtr = (uint8_t*)data;
        auto* destPtr = &m_buffer[m_position];
        memcpy(destPtr, srcPtr, size);
        m_position += size;
        return size;
    }
}