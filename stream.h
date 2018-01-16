#pragma once

namespace fbh {
    /**
     * Based on stream_reader_limited_ref in foobar2000 SDK.
     */
    class StreamReaderLimiter : public stream_reader {
    public:
        StreamReaderLimiter(stream_reader * p_reader,t_filesize p_limit) : m_reader(p_reader), m_remaining(p_limit), m_limit(p_limit) {}
        void set_data(stream_reader * p_reader,t_filesize p_limit) 
        {
            m_reader = p_reader;
            m_remaining = p_limit;
            m_limit = p_limit;
        }
    
        t_size read(void * p_buffer,t_size p_bytes,abort_callback & p_abort) override {
            if (p_bytes > m_remaining) p_bytes = (t_size)m_remaining;

            t_size done = m_reader->read(p_buffer,p_bytes,p_abort);
            m_remaining -= done;
            return done;
        }

        t_filesize get_remaining() const {return m_remaining;}

        t_filesize get_position() const {return m_limit-m_remaining;}

        t_filesize skip(t_filesize p_bytes,abort_callback & p_abort) override {
            if (p_bytes > m_remaining) p_bytes = m_remaining;
            t_filesize done = m_reader->skip(p_bytes,p_abort);
            m_remaining -= done;
            return done;
        }

        void flush_remaining(abort_callback & p_abort) {
            if (m_remaining > 0) skip_object(m_remaining,p_abort);
        }

    private:
        stream_reader * m_reader;
        t_filesize m_remaining, m_limit;
    };

    /**
    * Based on stream_reader_memblock_ref in foobar2000 SDK. 
    * 
    * Not intended for general use.
    */
    class StreamReaderMemblock : public stream_reader {
    public:
        template<typename t_array> StreamReaderMemblock(const t_array & p_array) : m_data(p_array.get_ptr()), m_data_size(p_array.get_size()), m_pointer(0) {
            pfc::assert_byte_type<typename t_array::t_item>();
        }
        StreamReaderMemblock(const void * p_data,t_size p_data_size) : m_data((const unsigned char*)p_data), m_data_size(p_data_size), m_pointer(0) {}
        StreamReaderMemblock() : m_data(nullptr), m_data_size(0), m_pointer(0) {}
    
        template<typename t_array> void set_data(const t_array & data) {
            pfc::assert_byte_type<typename t_array::t_item>();
            set_data(data.get_ptr(), data.get_size());
        }

        void set_data(const void * data, t_size dataSize) {
            m_pointer = 0;
            m_data = reinterpret_cast<const unsigned char*>(data);
            m_data_size = dataSize;
        }

        t_size read(void * p_buffer,t_size p_bytes,abort_callback & p_abort) override {
            t_size remaining = m_data_size - m_pointer;
            t_size toread = p_bytes;
            if (toread > remaining) toread = remaining;
            if (toread > 0) {
                memcpy(p_buffer,m_data+m_pointer,toread);
                m_pointer += toread;
            }

            return toread;
        }
        void read_nobuffer (t_uint8 * & p_out, t_size p_bytes,abort_callback & p_abort)
        {
            t_size remaining = m_data_size - m_pointer;
            t_size toread = p_bytes;
            if (toread > remaining) throw exception_io_data_truncation();
            p_out = const_cast<t_uint8*>(m_data)+m_pointer;
            m_pointer += p_bytes;
        }
        t_size get_remaining() const {return m_data_size - m_pointer;}
        t_size get_position(abort_callback & p_abort) const {return m_pointer;}
        void reset() {m_pointer = 0;}
        void seek_ex(file::t_seek_mode mode, t_ssize delta,abort_callback & p_abort)
        {
            t_ssize start = 0;
            if (mode == file::seek_from_current)
                start = m_pointer;
            else if (mode == file::seek_from_eof)
                start=(m_data_size);

            if (start + delta < 0 || t_size(start + delta) > m_data_size)
                throw exception_io_seek_out_of_range();

            m_pointer = start + delta;
        }
        
        void skip_object(t_filesize bytes, abort_callback & p_abort) override
        {
            seek_ex(file::seek_from_current, gsl::narrow<t_ssize>(bytes), p_abort);
        }

        t_filesize skip(t_filesize bytes, abort_callback & p_abort) override
        {
            t_size remaining = get_remaining();
            if (bytes >= remaining) {
                m_pointer = m_data_size; 
                return remaining;
            }
            m_pointer += static_cast<t_size>(bytes); 
            return bytes;
        }

        t_int64 read_sized_int_bendian(t_uint8 size,abort_callback & p_abort)
        {
            if (size > sizeof(t_uint64))
                throw exception_io_data();
            t_uint64 ret = 0;
            t_uint8 temp;
            while (size)
            {
                read_lendian_t(temp, p_abort);
                ret = (ret<<8)|temp;
                size--;
            }
            return (t_int64)ret;
        }
        bool is_eof(abort_callback & p_abort) {return m_pointer==m_data_size;}
    private:
        const unsigned char * m_data;
        t_size m_data_size,m_pointer;
    };

}
