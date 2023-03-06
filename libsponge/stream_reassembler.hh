#ifndef SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
#define SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH

#include "byte_stream.hh"

#include <cstdint>
#include <map>
#include <string>

//! \brief A class that assembles a series of excerpts from a byte stream (possibly out of order,
//! possibly overlapping) into an in-order byte stream.
class StreamReassembler {
  private:
    // Your code here -- add private members as necessary.

    ByteStream _output;  //!< The reassembled in-order byte stream
    size_t _capacity;    //!< The maximum number of bytes

    size_t used_auxiliary_storage;
    size_t expected_id;
    struct message {
        size_t idx_;
        size_t expected_next_id_;  // make sure that, messageA -> messageB => messageA.idx_ < messageB.expected_id
        std::string context_;
        message(const size_t idx, const size_t expected_next_id, const std::string context)
            : idx_(idx), expected_next_id_(expected_next_id), context_(context) {}
    };

    // key(start id): value()
    std::map<int, message> auxiliary_map;

  public:
    //! \brief Construct a `StreamReassembler` that will store up to `capacity` bytes.
    //! \note This capacity limits both the bytes that have been reassembled,
    //! and those that have not yet been reassembled.
    StreamReassembler(const size_t capacity);

    //! \brief Receive a substring and write any newly contiguous bytes into the stream.
    //!
    //! The StreamReassembler will stay within the memory limits of the `capacity`.
    //! Bytes that would exceed the capacity are silently discarded.
    //!
    //! \param data the substring
    //! \param index indicates the index (place in sequence) of the first byte in `data`
    //! \param eof the last byte of `data` will be the last byte in the entire stream
    void push_substring(const std::string &data, const uint64_t index, const bool eof);

    //! \name Access the reassembled byte stream
    //!@{
    const ByteStream &stream_out() const { return _output; }
    ByteStream &stream_out() { return _output; }
    //!@}

    //! The number of bytes in the substrings stored but not yet reassembled
    //!
    //! \note If the byte at a particular index has been pushed more than once, it
    //! should only be counted once for the purpose of this function.
    size_t unassembled_bytes() const;

    //! \brief Is the internal state empty (other than the output stream)?
    //! \returns `true` if no substrings are waiting to be assembled
    bool empty() const;

    void cache_string(const string &context, int index) {
        size_t msg_expected_id = index + context.size();
        auto new_context = const_cast<string &>(context);
        auto vaild_space = _capacity / 2 + _capacity % 2 - used_auxiliary_storage;
        msg_expected_id = index + vaild_space;
        while (context.size() > vaild_space) {
            new_context.pop_back();
            msg_expected_id -= 1;
        }
        std::cout << msg_expected_id;
        // construct message
        message value(index, msg_expected_id, new_context);
        auxiliary_map.insert({index, value});
    }
};

#endif  // SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
