#pragma once

namespace ige::scene
{
    //! MemBuffer: memory buffer
    class MemBuffer
    {
    public:
        //! Constructor
        MemBuffer(unsigned long size = 0);

        //! Destructor
        virtual ~MemBuffer();

        //! Size
        unsigned long size() const { return m_size; }

        //! Check empty
        bool empty() const { return m_size == 0; }

        //! Check end of file
        bool isEof() const { return m_position >= m_size; }

        //! Read bytes. Return number of read bytes;
        unsigned long read(void *dest, unsigned long size);

        //! Seek to position.
        unsigned long seek(unsigned long position);

        //! Write bytes to the memory area.
        unsigned long write(const void *data, unsigned long size);

        //! Read template
        template <typename T>
        unsigned long read(T &t)
        {
            return read(&t, sizeof(T));
        }

        //! Write template
        template <typename T>
        unsigned long write(const T &t)
        {
            return write(reinterpret_cast<const void *>(&t), sizeof(T));
        }

    protected:
        //! Resize
        virtual unsigned long resize(unsigned long size);

    protected:
        //! Size
        unsigned long m_size;

        //! Position
        unsigned long m_position;

        //! Internal buffer
        unsigned char *m_buffer;
    };
} // namespace ige::scene
