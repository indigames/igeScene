#include "core/MemBuffer.h"

#include <cstdarg>
#include <cstring>
#include <cctype>

namespace ige::scene
{
    MemBuffer::MemBuffer(unsigned long size)
        : m_buffer(nullptr), m_size(size), m_position(0)
    {
        if(m_size > 0)
        {
            m_buffer = new unsigned char[m_size];
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

    unsigned long MemBuffer::resize(unsigned long newSize)
    {
        if(newSize > m_size)
        {
            auto newBuffer = new unsigned char[newSize];
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
    unsigned long MemBuffer::read(void* dest, unsigned long size)
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
    unsigned long MemBuffer::seek(unsigned long position)
    {
        if (position > m_size)
            position = m_size;
        m_position = position;
        return m_position;
    }

    /// Write bytes
    unsigned long MemBuffer::write(const void* data, unsigned long size)
    {
        if (!size)
            return 0;

        if (size + m_position > m_size)
            resize(size + m_position);

        auto* srcPtr = (unsigned char*)data;
        auto* destPtr = &m_buffer[m_position];
        memcpy(destPtr, srcPtr, size);
        m_position += size;
        return size;
    }
}