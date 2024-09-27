#pragma once

enum class TokenType {
    EXIT,

    IF,
    ELSE,

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
    size_t line;
    size_t column;
    optional<string> value;
};

class Tokenizer {

public:

    inline explicit Tokenizer(string source)
        : source(std::move(source)) {}

    inline vector<Token> tokenize() {

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

                if (buffer == "exit") append( TokenType::EXIT );
                else if (buffer == "let") append( TokenType::LET );
                else if (buffer == "if") append( TokenType::IF );
                else if (buffer == "else") append( TokenType::ELSE );
                else append(TokenType::IDENTIFIER, buffer );

            }

            else if (isdigit(get())) {

                while (hasNext() && isdigit(get())) {
                    buffer.push_back(get());
                    next();
                }

                append(TokenType::INTEGER, buffer);

            }

            else if (get() == '/' && peak(1) == '/') {
                next(); next();
                while (hasNext() && get() != '\n') {
                    next();
                }
            }

            else if (get() == '/' && peak(1) == '*') {
                next();
                next();
                while (hasNext() && !(get() == '*' && peak(1) == '/')) {
                    next();
                }
                if (hasNext()) next();
                if (hasNext()) next();
            }

            else if (get() == ';') {
                append(TokenType::SEMICOLON);
                next();
            }

            else if (get() == '=') {
                append(TokenType::EQUALS);
                next();
            }

            else if (get() == '+') {
                append(TokenType::PLUS);
                next();
            }

            else if (get() == '-') {
                append(TokenType::MINUS);
                next();
            }

            else if (get() == '*') {
                append(TokenType::ASTERISK);
                next();
            }

            else if (get() == '/') {
                append(TokenType::SLASH);
                next();
            }

            else if (get() == '(') {
                append(TokenType::OPEN_ROUND_BRACKET);
                next();
            }

            else if (get() == ')') {
                append(TokenType::CLOSED_ROUND_BRACKET);
                next();
            }

            else if (get() == '{') {
                append(TokenType::OPEN_CURLY_BRACKET);
                next();
            }

            else if (get() == '}') {
                append(TokenType::CLOSED_CURLY_BRACKET);
                next();
            }

            buffer.clear();

        }

        return tokens;

    }

private:

    const string source;
    size_t pointer = 0;

    size_t line = 1;
    size_t column = 1;

    vector<Token> tokens;

    void append(TokenType type) {
        Token token { type, line, column };
        tokens.push_back(token);
    }
    
    void append(TokenType type, string value) {
        Token token { type, line, column, value };
        tokens.push_back(token);
    }

    [[nodiscard]] char peak(int count) const {
        return source.at(pointer + count);
    }

    [[nodiscard]] char get() const {
        return source.at(pointer);
    }

    void next() {

        pointer++;

        if (hasNext() && get() == '\n') {
            line++;
            column = 0;
        }

        column++;
    }

    [[nodiscard]] bool hasNext() const {
        return pointer < source.length();
    }

};