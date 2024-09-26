#pragma once

enum class TokenType {
    EXIT,

    IF,

    LET,
    IDENTIFIER,
    EQUALS,

    INTEGER,

    PLUS,
    MINUS,
    ASTERISK,
    SLASH,

    OPEN_ROUND_BRACKET,
    CLOSED_ROUND_BRACKET,

    OPEN_CURLY_BRACKET,
    CLOSED_CURLY_BRACKET,

    SEMICOLON
};

optional<int> getBinaryPrecedence (TokenType type) {

    switch (type) {
        case TokenType::PLUS: return 1;
        case TokenType::MINUS: return 1;
        case TokenType::ASTERISK: return 2;
        case TokenType::SLASH: return 2;
        default: return {};
    }

}

struct Token {
    TokenType type;
    optional<string> value;
};

class Tokenizer {

public:

    inline explicit Tokenizer(string source)
        : source(std::move(source)) {}

    inline vector<Token> tokenize() {

        vector<Token> tokens;
        string buffer;

        while (hasNext()) {

            if (isspace(get())) {
                next();
            }

            else if (isalpha(get())) {

                while (hasNext() && isalnum(get())) {
                    buffer.push_back(get());
                    next();
                }

                if (buffer == "exit") tokens.push_back({ .type = TokenType::EXIT });
                else if (buffer == "let") tokens.push_back({ .type = TokenType::LET });
                else if (buffer == "if") tokens.push_back({ .type = TokenType::IF });
                else tokens.push_back({ .type = TokenType::IDENTIFIER, .value = buffer });

            }

            else if (isdigit(get())) {

                while (hasNext() && isdigit(get())) {
                    buffer.push_back(get());
                    next();
                }

                tokens.push_back({.type = TokenType::INTEGER, .value = buffer});

            }

            else if (get() == '/' && peak(1) == '/') {
                next(); next();
                while (hasNext() && get() != '\n') {
                    next();
                }
            }

            else if (get() == '/' && peak(1) == '*') {
                next(2);
                while (hasNext() && !(get() == '*' && peak(1) == '/')) {
                    next();
                }
                next(2);
            }

            else if (get() == ';') {
                tokens.push_back({.type = TokenType::SEMICOLON});
                next();
            }

            else if (get() == '=') {
                tokens.push_back({.type = TokenType::EQUALS});
                next();
            }

            else if (get() == '+') {
                tokens.push_back({.type = TokenType::PLUS});
                next();
            }

            else if (get() == '-') {
                tokens.push_back({.type = TokenType::MINUS});
                next();
            }

            else if (get() == '*') {
                tokens.push_back({.type = TokenType::ASTERISK});
                next();
            }

            else if (get() == '/') {
                tokens.push_back({.type = TokenType::SLASH});
                next();
            }

            else if (get() == '(') {
                tokens.push_back({.type = TokenType::OPEN_ROUND_BRACKET});
                next();
            }

            else if (get() == ')') {
                tokens.push_back({.type = TokenType::CLOSED_ROUND_BRACKET});
                next();
            }

            else if (get() == '{') {
                tokens.push_back({.type = TokenType::OPEN_CURLY_BRACKET});
                next();
            }

            else if (get() == '}') {
                tokens.push_back({.type = TokenType::CLOSED_CURLY_BRACKET});
                next();
            }

            buffer.clear();

        }

        return tokens;

    }

private:

    const string source;
    size_t pointer = 0;

    [[nodiscard]] char peak(int count) const {
        return source.at(pointer + count);
    }

    [[nodiscard]] char get() const {
        return source.at(pointer);
    }

    void next(int count = 1) {
        pointer += 1;
    }

    [[nodiscard]] bool hasNext() const {
        return pointer < source.length();
    }

};