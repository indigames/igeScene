#pragma once

#include <cstdint>

namespace ige::scene
{
    //! MemBuffer: memory buffer
    class MemBuffer
    {
    public:
        //! Constructor
        MemBuffer(size_t size = 0);

        //! Destructor
        virtual ~MemBuffer();

        //! Size
        size_t size() const { return m_size; }

        //! Check empty
        bool empty() const { return m_size == 0; }

        //! Check end of file
        bool isEof() const { return m_position >= m_size; }

        //! Read bytes. Return number of read bytes;
        size_t read(void *dest, size_t size);

        //! Seek to position.
        size_t seek(size_t position);

        //! Write bytes to the memory area.
        size_t write(const void *data, size_t size);

        //! Read template
        template <typename T>
        size_t read(T &t)
        {
            return read(&t, sizeof(T));
        }

        //! Write template
        template <typename T>
        size_t write(const T &t)
        {
            return write(reinterpret_cast<const void *>(&t), sizeof(T));
        }

    protected:
        //! Resize
        virtual size_t resize(size_t size);

    protected:
        //! Size
        size_t m_size;

        //! Position
        size_t m_position;

        //! Internal buffer
        uint8_t *m_buffer;
    };
} // namespace ige::scene
