#ifndef PJSON_JSONLEXER_H
#define PJSON_JSONLEXER_H
#include <memory>
#include <optional>
#include <string>
#include <vector>
enum class JsonTokenKind{
    START,
    INT,
    FRAC,
    NEG,
    EXP,
    END,
    STRING,
    CURLY_OPEN,
    CURLY_CLOSE,
    TRUE,
    FALSE,
    EDGE_OPEN,
    EDGE_CLOSE,
    DOUPLEPOINT,
    COMMA,
    NULL_J,
    UNKNOWN
};

struct JsonToken{
    JsonToken(JsonTokenKind k, std::string && v): kind(k),value(v) {}
    JsonToken(JsonTokenKind k): kind(k){}
    JsonTokenKind kind;
    std::string value;
};

auto operator<<(std::ostream & out,const JsonTokenKind & token) -> std::ostream&{
    switch(token){
        case JsonTokenKind::START:
            out << "START";
            break;
        case JsonTokenKind::INT:
            out << "INT";
            break;
        case JsonTokenKind::FRAC:
            out << "FRAC";
            break;
        case JsonTokenKind::END:
            out << "END";
            break;
        case JsonTokenKind::NEG:
            out << "NEG";
            break;
        case JsonTokenKind::EXP:
            out << "EXP";
            break;
        case JsonTokenKind::STRING:
            out << "STRING";
            break;
        case JsonTokenKind::CURLY_OPEN:
            out << "CURLY_OPEN";
            break;
        case JsonTokenKind::CURLY_CLOSE:
            out << "CURLY_CLOSE";
            break;
        case JsonTokenKind::TRUE:
            out << "TRUE";
            break;
        case JsonTokenKind::FALSE:
            out << "FALSE";
            break;
        case JsonTokenKind::EDGE_OPEN:
            out << "EDGE_OPEN";
            break;
        case JsonTokenKind::EDGE_CLOSE:
            out << "EDGE_CLOSE";
            break;
        case JsonTokenKind::DOUPLEPOINT:
            out << "DOUPLEPOINT";
            break;
        case JsonTokenKind::COMMA:
            out << "COMMA";
            break;
        case JsonTokenKind::NULL_J:
            out << "NULL";
            break;
        default:
            out << "UNKNOWN";
            break;
    }
    return out;
}
auto operator<<(std::ostream & out,const JsonToken & token) -> std::ostream&{
    out << "Token.Kind: "<< token.kind <<" Token.Value:"<<token.value;
    return out;
}

class JsonLexer{
public:
    explicit JsonLexer(const std::string & str) : toLex_(str), status_(JsonTokenKind::START), current_pos_(0){}

    auto next_token() -> JsonToken{
        std::string value;
        while(status_ != JsonTokenKind::END) {
            if (current_pos_ >= toLex_.length()) {
                return {JsonTokenKind::END};
            }
            if (isCharacterA('-') || ::isdigit(toLex_[current_pos_])) {
                value += toLex_[current_pos_];
                current_pos_++;
                if (value == "0") {
                    return {JsonTokenKind::INT,std::move(value)};
                }
                if (current_pos_ < toLex_.length()) {
                    value += read_int();
                    return {JsonTokenKind::INT,std::move(value)};
                } else {
                    if (value == "-") {
                        return {JsonTokenKind::NEG,"-"};
                    }
                    return {JsonTokenKind::INT,std::move(value)};
                }
            }
            else if (isCharacterA('.')) {
                value += ".";
                current_pos_++;
                value += read_int_allow_zero_first();
                return {JsonTokenKind::FRAC,std::move(value)};
            }
            else if (::isspace(toLex_[current_pos_])) {
                current_pos_++;
            }
            else if(isCharacterA('E') || isCharacterA('e')){
                value += toLex_[current_pos_];
                current_pos_++;
                if(isCharacterA('+') || isCharacterA('-')){
                    value += toLex_[current_pos_];
                    current_pos_++;
                    value += read_int_allow_zero_first();
                    return {JsonTokenKind::EXP,std::move(value)};
                }
                if(current_pos_ < toLex_.length() && ::isdigit(toLex_[current_pos_])){
                    value += toLex_[current_pos_];
                    current_pos_++;
                    value += read_int_allow_zero_first();
                    return {JsonTokenKind::EXP,std::move(value)};
                }
                return {JsonTokenKind::UNKNOWN,std::move(value)};
            }
            else if(isCharacterA('"')){
                current_pos_++;
                return {JsonTokenKind::STRING,std::move(read_until('"'))};
            }
            else if(isCharacterA('{')){
                current_pos_++;
                return {JsonTokenKind::CURLY_OPEN};
            }
            else if(isCharacterA('}')){
                current_pos_++;
                return {JsonTokenKind::CURLY_CLOSE};
            }
            else if(isCharacterA('[')){
                current_pos_++;
                return {JsonTokenKind::EDGE_OPEN};
            }else if(isCharacterA(']')){
                current_pos_++;
                return {JsonTokenKind::EDGE_CLOSE};
            }
            else if(isCharacterA(',')){
                current_pos_++;
                return {JsonTokenKind::COMMA};
            }
            else if(isCharacterA(':')){
                current_pos_++;
                return {JsonTokenKind::DOUPLEPOINT};
            }
            else if(isCharacterSequence('t','r','u','e')){
                return {JsonTokenKind::TRUE,"true"};
            }
            else if(isCharacterSequence('f','a','l','s','e')) {
                return {JsonTokenKind::FALSE, "false"};
            }
            else if(isCharacterSequence('n','u','l','l')){
                return {JsonTokenKind::NULL_J,std::move(value)};
            }else {
                current_pos_++;
                return {JsonTokenKind::UNKNOWN, std::move(value)};
            }
        }
        return {JsonTokenKind::END};
    }

    auto read_until(char x) -> std::string{
        std::string res;
        while(!isCharacterA(x)){
            res += toLex_[current_pos_];
            current_pos_++;
            if(isCharacterA('\\')){
                res += toLex_[current_pos_];
                current_pos_++;
                if(current_pos_ < toLex_.size()) {
                    res += toLex_[current_pos_];
                    current_pos_++;
                }
            }
        }
        current_pos_++;
        return res;
    }

    auto read_int() -> std::string{
        std::string res;
        if(isCharacterA('0')){
            current_pos_++;
            return "0";
        }
        while(current_pos_ < toLex_.length() && ::isdigit(toLex_[current_pos_])){
            res += toLex_[current_pos_];
            current_pos_++;
        }
        return res;
    }

    auto read_int_allow_zero_first() -> std::string{
        std::string res;
        while(current_pos_ < toLex_.length() &&  ::isdigit(toLex_[current_pos_])){
            res += toLex_[current_pos_];
            current_pos_++;
        }
        return res;
    }

    auto isCharacterSequence(char c) ->bool{
        if(current_pos_ < toLex_.length() && toLex_[current_pos_] == c){
            current_pos_++;
            return true;
        }
        return false;
    }

    template<class T, class... ARGS>
    auto isCharacterSequence(T arg,ARGS... args) ->bool{
        if(!isCharacterSequence(arg)){
            return false;
        }
        return isCharacterSequence(args...);
    }
    [[nodiscard]] auto isCharacterA(char x)const ->bool{
        return current_pos_ < toLex_.length() && x == toLex_[current_pos_];
    }

private:
    std::string toLex_;
    JsonTokenKind status_;
    std::size_t current_pos_;
};

struct JsonElement;
struct JsonObject;
struct JsonArray;
struct JsonString;
struct JsonNumber;
struct JsonBool;
struct JsonNull;
struct JsonMember;

struct JsonAst{
    std::unique_ptr<JsonElement>  element_;
};

struct JsonElement: public JsonAst{
    std::unique_ptr<JsonObject>  object_;
    std::unique_ptr<JsonArray>   array_;
    std::unique_ptr<JsonString>  string_;
    std::unique_ptr<JsonNumber>  number_;
    std::unique_ptr<JsonBool>    bool_;
    std::unique_ptr<JsonNull>    null_;
};

struct JsonObject: public JsonAst{
    std::vector<std::unique_ptr<JsonMember>> members_;
};

struct JsonMember: public JsonAst{
    std::unique_ptr<JsonString>  string_;
    std::unique_ptr<JsonElement> element;
};

struct JsonArray : public JsonAst{
    std::vector<std::unique_ptr<JsonElement>> elements_;
};

struct JsonString : public JsonArray{
    std::string string_;
};

struct JsonBool: public JsonAst{
    bool bool_;
};

struct JsonNumber: public JsonAst{
    double number_;
};

struct JsonNull: public JsonAst{

};

class JsonParser{
public:
    explicit JsonParser(const std::string & str) :
        lexer_(str),
        current_token_(JsonTokenKind::UNKNOWN,""),
        last_token_(JsonTokenKind::UNKNOWN,""){ }

    auto accept(JsonTokenKind kind) -> bool{
        if(current_token_.kind == kind){
            last_token_ = current_token_;
            current_token_ = lexer_.next_token();
            return true;
        }
        return false;
    }

    auto parse() -> std::optional<std::unique_ptr<JsonAst>>{
        current_token_ = lexer_.next_token();
        if( auto element = parse_element() ){
            if(accept(JsonTokenKind::END)){
                auto ast = std::make_unique< JsonAst >();
                ast->element_ = std::move(*element);
                return std::move(ast);
            }
        }
        return std::nullopt;
    }

    auto parse_element() -> std::optional<std::unique_ptr<JsonElement>>{
        if(auto obj = parse_object()){
            auto element = std::make_unique< JsonElement>();
            element->object_ = std::move(*obj);
            return {std::move(element)};
        }
        if(auto array = parse_array()){
            auto element = std::make_unique<  JsonElement>();
            element->array_ = std::move(*array);
            return {std::move(element)};
        }
        if(auto number = parse_number()){
            auto element = std::make_unique<  JsonElement>();
            element->number_ = std::move(*number);
            return {std::move(element)};
        }
        if(accept(JsonTokenKind::STRING)){
            auto element = std::make_unique<  JsonElement >();
            auto string_ = std::make_unique<  JsonString >();
            string_->string_ = last_token_.value;
            element->string_ = std::move(string_) ;
            return {std::move(element)};
        }
        if(accept(JsonTokenKind::TRUE)){
            auto element = std::make_unique<  JsonElement>();
            auto bool_ = std::make_unique<  JsonBool>();
            bool_->bool_ = true;
            element->bool_ = std::move(bool_) ;
            return {std::move(element)};
        }
        if(accept(JsonTokenKind::FALSE)){
            auto element = std::make_unique< JsonElement >();
            auto bool_ = std::make_unique< JsonBool >();
            bool_->bool_ = false;
            element->bool_ = std::move(bool_) ;
            return { std::move(element) };
        }
        if(accept(JsonTokenKind::NULL_J)){
            auto element = std::make_unique< JsonElement>();
            element->null_ = std::make_unique< JsonNull>();
            return { std::move(element) };
        }
        return std::nullopt;
    }

    auto parse_object() -> std::optional<std::unique_ptr<JsonObject>>{
        if(accept(JsonTokenKind::CURLY_OPEN)){
            if(auto member = parse_member()){
                auto object =  std::make_unique<JsonObject>();
                object->members_.emplace_back(std::move(*member));
                while(accept(JsonTokenKind::COMMA)){
                    if(auto mem = parse_member()){
                        object->members_.emplace_back(std::move(*mem));
                    }else{
                        return std::nullopt;
                    }
                }
                if(accept(JsonTokenKind::CURLY_CLOSE)){
                    return object;
                }
            }
        }
        return std::nullopt;
    }

    auto parse_member()->std::optional<std::unique_ptr<JsonMember>>{
        if(accept(JsonTokenKind::STRING)){
            auto member = std::make_unique< JsonMember>();
            auto string_ = std::make_unique< JsonString>();
            string_->string_ = last_token_.value;
            member->string_ = std::move(string_);
            if(accept(JsonTokenKind::DOUPLEPOINT)){
                if(auto element = parse_element()){
                    member->element_ = std::move(*element);
                    return member;
                }
            }
        }
        return std::nullopt;
    }

    auto parse_array() -> std::optional<std::unique_ptr<JsonArray>>{
        if(accept(JsonTokenKind::EDGE_OPEN)){
            auto array = std::make_unique<JsonArray>();
            if (auto element = parse_element()){
                array->elements_.emplace_back(std::move(*element));
                while(accept(JsonTokenKind::COMMA)){
                    if(auto ele = parse_element()){
                        array->elements_.emplace_back(std::move(*ele));
                    }else{
                        return std::nullopt;
                    }
                }
            }
            if(accept(JsonTokenKind::EDGE_CLOSE)){
                return array;
            }
        }
        return std::nullopt;
    }

    auto parse_number() -> std::optional<std::unique_ptr<JsonNumber>> {
        if(accept(JsonTokenKind::INT)){
            std::string v = last_token_.value;
            accept(JsonTokenKind::FRAC);
            v += last_token_.value;
            accept(JsonTokenKind::EXP);
            v += last_token_.value;
            auto number = std::make_unique<JsonNumber>();
            number->number_ = ::atof(v.c_str());
        }
        return std::nullopt;
    }
private:
    JsonLexer lexer_;
    JsonToken current_token_;
    JsonToken last_token_;
};

#endif //PJSON_JSONLEXER_H
