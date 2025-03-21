#pragma once

#include "search_server.h"
#include <deque>
#include <vector>
#include <string>

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);
    
    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
        std::vector<Document> results = search_server_.FindTopDocuments(raw_query, document_predicate);
        QueryResult query_result;
        query_result.number_of_results = results.size();
        if (query_result.number_of_results == 0){
            no_result_requests_ += 1;
        }
        for(int i = 0; i < query_result.number_of_results; ++i){
            current_time_ += 1;
        }
        query_result.current_time = current_time_;
        requests_.push_back(query_result);
        if (requests_.size() == min_in_day_ + 1){
            if (requests_.front().number_of_results == 0){
                no_result_requests_ -= 1;
            }
            requests_.pop_front();
        }
        return results;
    }
    
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);
    
    std::vector<Document> AddFindRequest(const std::string& raw_query);
    
    int GetNoResultRequests() const;
private:
    int no_result_requests_ = 0;
    int current_time_ = 0;
    struct QueryResult {
        int current_time = 0;
        int number_of_results = 0;
    };
    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    const SearchServer& search_server_;
};