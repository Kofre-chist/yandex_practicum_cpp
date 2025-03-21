#pragma once

#include <vector>
#include <iterator>

template<typename Doc>
class Paginator {
public:
    explicit Paginator(Doc begin, Doc end, size_t page_size){
        int count = distance(begin, end) / page_size;
        Doc begin_ = begin;
        Doc end_ = end;
        if (distance(begin, end) / page_size == 1){
            for (int i = 0; i < count; ++i){
                Doc start = begin_;
                advance(begin_, page_size);
                Doc finish = begin_;
                pages_.push_back(IteratorRange<Doc>(start, finish));
            }
            pages_.push_back(IteratorRange<Doc>(begin_, end_));
        } else{
            for (int i = 0; i < count; ++i){
                Doc start = begin_;
                advance(begin_, page_size);
                Doc finish = begin_;
                pages_.push_back(IteratorRange<Doc>(start, finish));
            }
        }
    }
    
    auto begin() const {
        return pages_.begin();
    }
    
    auto end() const {
        return pages_.end();
    }
    
    size_t size() {
        return pages_.size();
    }
    
private:
    std::vector<IteratorRange<Doc>> pages_;
};

// Функция, которая возвращает объект класса Paginator
template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}