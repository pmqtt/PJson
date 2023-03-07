#include <chrono>
#include <iostream>
#include <numeric>
#include "JsonLexer.h"

double average(const std::vector<unsigned long int> & v){
    if(v.empty()){
        return 0;
    }

    const auto count = static_cast<double>(v.size());
    return std::reduce(v.begin(), v.end()) / count;
}

int main() {
   // std::string x = "  -1234.1234   1234 -1234.5678E+12234 \"HALLO \\\"x\\\" WELT\" true \"x\" false 123  ";
   // std::string z = "false 123  ";
    std::string testJson = "{\"menu\": {\n"
                           "  \"id\": \"file\",\n"
                           "  \"value\": \"File\",\n"
                           "  \"popup\": {\n"
                           "    \"menuitem\": [\n"
                           "      {\"value\": \"New\", \"onclick\": \"CreateNewDoc()\"},\n"
                           "      {\"value\": \"Open\", \"onclick\": \"OpenDoc()\"},\n"
                           "      {\"value\": \"Close\", \"onclick\": \"CloseDoc()\"}\n"
                           "    ]\n"
                           "  }\n"
                           "}}";
    //std::cout<<"LEX STRING:"<<testJson<<"\n";
    //JsonLexer lexer(testJson);
    //auto token = lexer.next_token();
    //do{
    //    std::cout<<token<<"\n";
    //}while((token = lexer.next_token()).kind != JsonTokenKind::END);
    //std::cout<<token<<"\n";
    std::vector<unsigned long int> execution_times;
    for(std::size_t i = 0; i < 1000000; ++i) {
        auto begin = std::chrono::high_resolution_clock::now();
        JsonParser parser(testJson);
        if (!parser.parse()) {
            std::cout << "NOT VALID" ;
            return -1;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
        //std::cout<<" : time : "<< elapsed.count()<<"\n";
        execution_times.push_back(elapsed.count());
    }
    std::cout<<"Execution Time is: "<<average(execution_times)<<" microseconds\n";
    execution_times.clear();
    for(std::size_t i = 0; i < 1000000; ++i) {
        auto begin = std::chrono::high_resolution_clock::now();
        unsigned long sum = 0;
        std::vector<std::size_t> positions;
        for(std::size_t i = 0; i < testJson.length(); ++i){
            sum += testJson[i];
            if(testJson[i] == '"'){
                positions.push_back(i);
            }
        }
        if( sum < 10 && positions.size() <= 0){
            std::cout << "NOT VALID" ;
            return -1;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
        //std::cout<<" : time : "<< elapsed.count()<<"\n";
        execution_times.push_back(elapsed.count());
    }
    std::cout<<"Execution Time is: "<<average(execution_times)<<" microseconds\n";

    return 0;
}
