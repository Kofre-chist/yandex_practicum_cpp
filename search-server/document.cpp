#include "document.h"

#include <iostream>
#include <string>

std::ostream& operator<<(std::ostream& output, const Document& document) {
    output << "{ "
         << "document_id = " << document.id << ", "
         << "relevance = " << document.relevance << ", "
         << "rating = " << document.rating
         << " }";
    return output;
}