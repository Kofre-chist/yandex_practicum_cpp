#pragma once

#include <iostream>
#include <iterator>

struct Document {
    Document() = default;

    Document(int id, double relevance, int rating)
        : id(id)
        , relevance(relevance)
        , rating(rating) {
    }

    int id = 0;
    double relevance = 0.0;
    int rating = 0;
};

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED,
};

std::ostream& operator<<(std::ostream& output, const Document& document);

// По сути это одна страница. В ней должны храниться несколько документов и количетсво этих докумнетов.
template<typename Doc>
class IteratorRange {
public:
    explicit IteratorRange(Doc& begin, Doc& end){
        begin_ = begin;
        end_ =  end;
    }
    
    Doc begin() const {
        return begin_;
    }
    
    Doc end() const {
        return end_;
    }
    
    size_t size() {
        return std::distance(begin_, end_);
    }
    
private:
    Doc begin_;
    Doc end_;
};

template<typename Doc>
std::ostream& operator<<(std::ostream& output, const IteratorRange<Doc>& range) {
    for (auto it = range.begin(); it != range.end(); ++it) {
        output << *it;
    }
    return output;
}