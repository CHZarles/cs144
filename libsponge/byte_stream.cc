#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;




ByteStream::ByteStream(const size_t capacity) :buffer_(vector<char>(capacity + 1)),
capacity_(capacity + 1),first_data_index_(0), available_data_index_(0),
bytes_written_(0), bytes_read_(0), end_input_(false) { 

}

size_t ByteStream::write(const string &data) {
    
    size_t avild_input_size = 0;
    std::cout << "call write :" << data << "\n";
    for(auto ch : data){
        if(push(ch))
            avild_input_size += 1;
        else
            break;
    }
    return avild_input_size;
    
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    string content;
    size_t vaild_length = std::min(buffer_size() , len);
    size_t st = first_data_index_;
    while(vaild_length > 0){
        vaild_length -= 1;
        content.push_back(buffer_[st]);
        st = (st + 1) % capacity_;
    }
    return content;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) { 
    char ch = '/';
    std::cout << "call pop_output: " << len << "\n";
    for (size_t i = 0 ; i < len; ++ i){
        auto vaild = pop(ch);
        std::cout << "pop : " << ch << endl;
        if ( !vaild ) 
            break;
    }

}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    char ch;
    string content;
    for (size_t i = 0 ; i < len; ++ i){
        auto vaild = pop(ch);
        if ( !vaild ) 
            break;
        content.push_back(ch);
    }
    return content;
    
}

void ByteStream::end_input() { end_input_ = true; }

bool ByteStream::input_ended() const { return end_input_; }


size_t ByteStream::buffer_size() const { 
    if ( buffer_empty() )
        return 0;
    else {
        return (available_data_index_ + capacity_ - first_data_index_) % capacity_; 
    }
        
}

bool ByteStream::buffer_empty() const { return available_data_index_ == first_data_index_ ; }

bool ByteStream::eof() const { return input_ended() && buffer_empty() ; }

size_t ByteStream::bytes_written() const { return bytes_written_; }

size_t ByteStream::bytes_read() const { return bytes_read_; }

size_t ByteStream::remaining_capacity() const { return capacity_ - buffer_size() - 1; }
