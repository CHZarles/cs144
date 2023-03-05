#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    : _output(capacity / 2)
    , _capacity(capacity)
    , used_auxiliary_storage(capacity / 2 + capacity % 2)
    , expected_id(0)
    , auxiliary_map() {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    DUMMY_CODE(data, index, eof);

    // special case
    if (data.empty())
        return;

    // 更新 auxiliary_map
    // 把可以输入到streambuufer的先输入
    // TODO send eof
    auto item = auxiliary_map.find(expected_id);
    while (item != auxiliary_map.end()) {
        // update byte_stream
        _output.write(item->second.context_);
        // update expected_id
        expected_id = item->second.expected_next_id_;
        // update used_auxiliary_storage
        used_auxiliary_storage -= item->second.context_.size();
        // update auxiliary_map
        auxiliary_map.erase(item);

        item = auxiliary_map.find(expected_id);
    }

    // 将字符串加入stream buffer
    /* situation 1
        if index < expected_id:
            remove some characters, untill index == expected_id
            then call push_substring(data , expected_id)
    */
    if (index < expected_id) {
        std::cout << "In situation 1 , Get Index: " << index << "Get Context: " << data << std::endl;
        push_substring(data.substr(expected_id - index), index, eof);
        return;
    }

    // define messageA_iter.second.id < index
    // define messageB_iter.second.id > index
    auto upper_iter = auxiliary_map.upper_bound(index);  // 大于index的最小值
    auto lower_iter = auxiliary_map.lower_bound(index);  // 大于等于index的最小值

    if (lower_iter == auxiliary_map.end()) {        // Any index in map smaller than this index
        if (lower_iter == auxiliary_map.begin()) {  // map empty
            cache_string(data, index);
        }
        if (lower_iter != auxiliary_map.begin()) {  // there are some index ..
            lower_iter--;
            auto &messageA_iter = lower_iter;
            auto overlap_a = messageA_iter->second.expected_next_id_ - index;
            if (overlap_a > 0) {
                if (overlap_a >= data.size()) {
                    return;
                } else {
                    cache_string(data.substr(overlap_a), messageA_iter->second.expected_next_id_);
                    return;
                }
            } else {  // 与a无重叠
                cache_string(data, index);
            }
        }
    } else if (lower_iter->second.idx_ == index) {
        // data.size() <= lower_iter.seond
        if (data.size() <= lower_iter->second.context_.size())
            return;
        if (data.size() > lower_iter->second.context_.size()) {
            if (upper_iter == auxiliary_map.end()) {
                auto &message = lower_iter->second;
                cache_string(data.substr(message.context_.size()), message.expected_next_id_);
                return;
            }

            if (upper_iter != auxiliary_map.end()) {
                // exist overlap
                if (data.size() + lower_iter->second.expected_next_id_ < upper_iter->second.idx_) {
                    auto &message = lower_iter->second;
                    cache_string(data.substr(message.context_.size()), message.expected_next_id_);
                    return;
                }
                if (data.size() + lower_iter->second.expected_next_id_ >= upper_iter->second.idx_) {
                    auto &message = lower_iter->second;
                    push_substring(data.substr(message.context_.size()), message.expected_next_id_, eof);
                }
            }
        }
    } else if (lower_iter->second.idx_ > index) {
        if (lower_iter == auxiliary_map.begin()) {
            auto overlap = index + data.size() - lower_iter->second.idx_;
            if (overlap < 0) {
                cache_string(data, index);
            } else {
                cache_string(data.substr(0, lower_iter->second.idx_ - index), index);
                push_substring(data.substr(lower_iter->second.idx_ - index), lower_iter->second.idx_, eof);
            }
        }

        if (lower_iter != auxiliary_map.begin()) {
            lower_iter--;
            auto &less_index_iter = lower_iter;
            lower_iter++;
            auto overlap = index + data.size() - lower_iter->second.idx_;
            if (less_index_iter->second.expected_next_id_ <= index) {
                auto overlap = index + data.size() - lower_iter->second.idx_;
                if (overlap < 0) {
                    cache_string(data, index);
                } else {
                    cache_string(data.substr(0, lower_iter->second.idx_ - index), index);
                    push_substring(data.substr(lower_iter->second.idx_ - index), lower_iter->second.idx_, eof);
                }
            } else if (less_index_iter->second.expected_next_id_ > index) {
                auto pre_overlap = less_index_iter->second.expected_next_id_ - index;
                push_substring(data.substr(pre_overlap), less_index_iter->second.expected_next_id_, eof);
            }
        }
    }

    if (eof) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { return used_auxiliary_storage; }

bool StreamReassembler::empty() const { return used_auxiliary_storage == 0 && _output.buffer_empty(); }
