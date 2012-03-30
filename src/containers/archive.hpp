#ifndef CONTAINERS_ARCHIVE_HPP_
#define CONTAINERS_ARCHIVE_HPP_

#include "errors.hpp"
#include "arch/address.hpp"
#include "arch/types.hpp"
#include "containers/intrusive_list.hpp"

class read_stream_t {
public:
    read_stream_t() { }
    // Returns number of bytes read or 0 upon EOF, -1 upon error.
    virtual int64_t read(void *p, int64_t n) = 0;
protected:
    virtual ~read_stream_t() { }
private:
    DISABLE_COPYING(read_stream_t);
};

class write_stream_t {
public:
    write_stream_t() { }
    // Returns n, or -1 upon error.  Blocks until all bytes are
    // written.
    virtual int64_t write(const void *p, int64_t n) = 0;
protected:
    virtual ~write_stream_t() { }
private:
    DISABLE_COPYING(write_stream_t);
};

class write_buffer_t : public intrusive_list_node_t<write_buffer_t> {
public:
    write_buffer_t() : size(0) { }

    static const int DATA_SIZE = 4096;
    int size;
    char data[DATA_SIZE];

private:
    DISABLE_COPYING(write_buffer_t);
};

// A set of buffers in which an atomic message to be sent on a stream
// gets built up.  (This way we don't flush after the first four bytes
// sent to a stream, or buffer things and then forget to manually
// flush.  This type can be extended to support holding references to
// large buffers, to save copying.)  Generally speaking, you serialize
// to a write_message_t, and then flush that to a write_stream_t.
class write_message_t {
public:
    write_message_t() { }

    void append(const void *p, int64_t n);

private:
    intrusive_list_t<write_buffer_t> buffers_;

    DISABLE_COPYING(write_message_t);
};

// TODO: Move this and "arch/..." inclusions to a separate header.
class tcp_conn_stream_t : public read_stream_t, public write_stream_t {
public:
    tcp_conn_stream_t(const char *host, int port, int local_port = 0);
    tcp_conn_stream_t(const ip_address_t &host, int port, int local_port = 0);
    virtual ~tcp_conn_stream_t();

    virtual int64_t read(void *p, int64_t n);
    virtual int64_t write(const void *p, int64_t n);

private:
    tcp_conn_t *conn_;

    DISABLE_COPYING(tcp_conn_stream_t);
};



#endif  // CONTAINERS_ARCHIVE_HPP_
