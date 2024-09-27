#pragma once

#include <variant>

#include "tokenizer.h"

#include "arena.h"

namespace Node {

    struct Scope;
    struct Statement;
    struct Expression;

    namespace ExpressionVariant {

        struct Identifier {
            Token value;
        };

        struct Integer {
            Token value;
        };

        struct RoundBrackets {
            Expression* expression;
        };

        namespace TermVariant {

            struct Addition {
                Expression* left;
                Expression* right;
            };

            struct Subtraction {
                Expression* left;
                Expression* right;
            };

            struct Multiplication {
                Expression* left;
                Expression* right;
            };

            struct Division {
                Expression* left;
                Expression* right;
            };

        }

        struct Term {
            variant<TermVariant::Multiplication*, TermVariant::Division*, TermVariant::Addition*, TermVariant::Subtraction*> variant;
        };

    }

    struct Expression {
        variant<Node::ExpressionVariant::Identifier*, Node::ExpressionVariant::Integer*, Node::ExpressionVariant::RoundBrackets*, Node::ExpressionVariant::Term*> variant;
    };

    namespace StatementVariant {

        struct Exit {
            Expression* expression;
        };

        struct Let {
            Token identifierToken;
            Expression* expression {};
        };

        struct Assign {
            Token identifierToken;
            Expression* expression {};
        };

        struct If {
            Expression* condition{};
            Statement* statement{};
            optional<Statement*> elseStatement;
        };

    }

    // TODO Refactor code to use "using" instead of "struct"
    struct Statement {
        variant<StatementVariant::Exit*, StatementVariant::Let*, StatementVariant::Assign*, StatementVariant::If*, Scope*> variant;
    };

    struct Scope {
        vector<Statement*> statements;
    };

    struct Program {
        Scope* scope;
    };

}

class Parser {

public:
    inline explicit Parser (vector<Token> tokens):
        tokens(std::move(tokens)),
        allocator(1024 * 1024 * 4) // 4 MB
    {}

    inline Node::Program parse() {
        return Node::Program { .scope = parseScope() };
    }

private:

    inline Node::Scope* parseScope() {

        auto scope = allocator.allocate<Node::Scope>();

        while (hasNext()) {
            auto statement = parseStatement();
            if (!statement.has_value()) break;
            scope->statements.push_back(statement.value());
        }

        return scope;

    }

    inline optional<Node::Statement*> parseStatement() {

        auto statement = allocator.allocate<Node::Statement>();

        if (!hasNext()) return {};

        switch (get().type) {

            case TokenType::EXIT: {

                auto exitStatement = allocator.allocate<Node::StatementVariant::Exit>();

                next();

                exitStatement->expression = parseExpression();

                statement->variant = exitStatement;

                break;

            }

            case TokenType::LET: {

                auto letStatement = allocator.allocate<Node::StatementVariant::Let>();

                next();

                if (get().type != TokenType::IDENTIFIER) raise("Failed to parse Expression! Identifier expected", get().line);

                letStatement->identifierToken = get();
                next();

                if (get().type == TokenType::EQUALS) {
                    next();
                } else raise("Failed to parse Expression! '=' expected", get().line);

                letStatement->expression = parseExpression();

                statement->variant = letStatement;

                break;

            }

            case TokenType::IDENTIFIER: {

                auto assignmentStatement = allocator.allocate<Node::StatementVariant::Assign>();

                assignmentStatement->identifierToken = get();
                next();

                if (get().type == TokenType::EQUALS) {
                    next();
                } else raise("Failed to parse Expression! '=' expected", get().line);

                assignmentStatement->expression = parseExpression();

                statement->variant = assignmentStatement;

                break;

            }

            case TokenType::IF: {

                auto ifStatement = allocator.allocate<Node::StatementVariant::If>();

                next();

                ifStatement->condition = parseExpression();
                ifStatement->statement = parseStatement().value();

                if (hasNext() && get().type == TokenType::ELSE) {
                    next();
                    ifStatement->elseStatement = parseStatement().value();
                }

                statement->variant = ifStatement;

                return statement;

            }

            case TokenType::OPEN_CURLY_BRACKET: {
                next();

                auto scope = parseScope();

                if (get().type == TokenType::CLOSED_CURLY_BRACKET) next();
                else raise("Failed to parse Expression! '}' expected", get().line);

                statement->variant = scope;

                return statement;

            }

            case TokenType::CLOSED_CURLY_BRACKET: {

                return {};

            }

            default: raise("Failed to parse Expression! Unknown token", get().line, get().column);

        }

        if (get().type == TokenType::SEMICOLON) next();
        else raise("Failed to parse Expression! ';' expected", get().line);

        return statement;

    }

    inline Node::Expression* parseExpression(int minPrecedence = 1) {

        Node::Expression* expression;

        if (get().type == TokenType::INTEGER) {

            auto integerExpression = allocator.allocate<Node::ExpressionVariant::Integer>();
            integerExpression->value = get();

            next();

            expression = allocator.allocate<Node::Expression>();
            expression->variant = integerExpression;

        }
        else if (get().type == TokenType::IDENTIFIER) {

            auto identifierExpression = allocator.allocate<Node::ExpressionVariant::Identifier>();
            identifierExpression->value = get();

            next();

            expression = allocator.allocate<Node::Expression>();
            expression->variant = identifierExpression;

        } else if (get().type == TokenType::OPEN_ROUND_BRACKET) {

            auto roundBracketExpression = allocator.allocate<Node::ExpressionVariant::RoundBrackets>();

            next();

            roundBracketExpression->expression = parseExpression();

            if (get().type == TokenType::CLOSED_ROUND_BRACKET) {
                next();
            } else {
                raise("Failed to parse Expression! ')' expected", get().line);
            }

            expression = allocator.allocate<Node::Expression>();
            expression->variant = roundBracketExpression;


        } else raise("Failed to parse Expression! Unexpected Token", get().line, get().column);

        while (true) {

            TokenType operatorType = get().type;

            if (auto precedence = getBinaryPrecedence(operatorType)) {

                if (precedence.value() < minPrecedence) break;

                next();

                int nextMinPrecedence = precedence.value() + 1;

                auto term = allocator.allocate<Node::ExpressionVariant::Term>();

                switch (operatorType) {
                    case TokenType::PLUS: {
                        auto additionTerm = allocator.allocate<Node::ExpressionVariant::TermVariant::Addition>();
                        additionTerm->left = expression;
                        additionTerm->right = parseExpression(nextMinPrecedence);
                        term->variant = additionTerm;
                        break;
                    }
                    case TokenType::MINUS: {
                        auto subtractionTerm = allocator.allocate<Node::ExpressionVariant::TermVariant::Subtraction>();
                        subtractionTerm->left = expression;
                        subtractionTerm->right = parseExpression(nextMinPrecedence);
                        term->variant = subtractionTerm;
                        break;
                    }
                    case TokenType::ASTERISK: {
                        auto multiplicationTerm = allocator.allocate<Node::ExpressionVariant::TermVariant::Multiplication>();
                        multiplicationTerm->left = expression;
                        multiplicationTerm->right = parseExpression(nextMinPrecedence);
                        term->variant = multiplicationTerm;
                        break;
                    }
                    case TokenType::SLASH: {
                        auto divisionTerm = allocator.allocate<Node::ExpressionVariant::TermVariant::Division>();
                        divisionTerm->left = expression;
                        divisionTerm->right = parseExpression(nextMinPrecedence);
                        term->variant = divisionTerm;
                        break;
                    }
                    default: raise("Failed to parse Therm! Unexpected Operator Type", get().line, get().column);
                }

                expression = allocator.allocate<Node::Expression>();
                expression->variant = term;

            } else break;

        }

        return expression;

    }
    // Allocation
    ArenaAllocator allocator;

    // Tokens
    const vector<Token> tokens;
    size_t pointer = 0;

    inline Token peek(int ahead = 1) {
        return tokens.at(pointer + ahead);
    }

    inline Token get() {

        if (!hasNext()) {
            cerr << "Unexpected End Exception! Tried to access value out of bounds!" << endl;
            exit(EXIT_FAILURE);
        }

        return tokens.at(pointer);
    }

    inline void next() {
        pointer++;
    }

    [[nodiscard]] inline bool hasNext() const {
        return pointer < tokens.size();
    }

    // Errors
    static void raise(const string& message, size_t line, size_t column) {
        cerr << message << " at " << line << ":" << column << "!" << endl;
        exit(EXIT_FAILURE);
    }

    static void raise(const string& message, size_t line) {
        cerr << message << " at line " << line << "!" << endl;
        exit(EXIT_FAILURE);
    }

};