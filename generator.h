#pragma once

#include <map>
#include <cassert>
#include <algorithm>
#include "parser.h"

class Generator {

public:
    inline explicit Generator(Node::Program program):
            program(program)
    {}

    [[nodiscard]] string generate () {

        assembly << "global _start\n"
                 << "_start:\n";

        generateScope(program.scope);

        assembly << "    mov rax, 60" << endl
                 << "    mov rdi, 0" << endl
                 << "    syscall";

        return assembly.str();

    }

private:

    void generateScope(const Node::Scope* scope) {
        for (const Node::Statement* statement : scope->statements) {
            generateStatement(statement);
        }
    }

    void generateStatement(const Node::Statement* statement) {

        struct statementVisitor {

            Generator* generator;

            void operator()(const Node::StatementVariant::Exit* returnStatement) const {

                generator->generateExpression(returnStatement->expression);

                generator->assembly << "    mov rax, 60" << endl;
                generator->pop("rdi");
                generator->assembly << "    syscall" << endl;

            }

            void operator()(const Node::StatementVariant::Let* letStatement) const {

                auto it = find_if(
                    generator->variables.cbegin(),
                    generator->variables.cend(),
                    [&](const Variable& variable){
                        return variable.name == letStatement->identifierToken.value.value();
                    }
                );

                if (it != generator->variables.cend()) {
                    cerr << "Double Declaration of Variable '" << letStatement->identifierToken.value.value() << "'!" << endl;
                    exit(EXIT_FAILURE);
                }

                generator->variables.push_back({ .name = letStatement->identifierToken.value.value(), .location = generator->stack_size });
                generator->generateExpression(letStatement->expression);

            }

            void operator()(const Node::StatementVariant::If* ifStatement) const {

                generator->generateExpression(ifStatement->condition);
                generator->pop("rax");

                string label = generator->createLabel();

                generator->assembly << "    test rax, rax" << endl // If rax == 2
                                    << "    jz " << label << endl;
                generator->generateStatement(ifStatement->statement);
                generator->assembly << label << ":" << endl;

            }

            void operator()(const Node::Scope* scope) const {
                generator->startScope();
                generator->generateScope(scope);
                generator->endScope();
            }

        };

        statementVisitor visitor { .generator = this };
        visit(visitor, statement->variant);

    }

    void generateExpression(const Node::Expression* expression) {

        struct expressionVisitor {

            Generator* generator;

            void operator()(const Node::ExpressionVariant::Integer* integerExpression) const {

                generator->assembly << "    mov rax, " << integerExpression->value.value.value() << endl;
                generator->push("rax");

            }

            void operator()(const Node::ExpressionVariant::Identifier* identifierExpression) const {


                auto it = find_if(
                    generator->variables.cbegin(),
                    generator->variables.cend(),
                    [&](const Variable& variable){
                        return variable.name == identifierExpression->value.value.value();
                    }
                );

                if (it == generator->variables.cend()) {
                    cerr << "Undeclared Variable '" << identifierExpression->value.value.value() << "'!" << endl;
                    exit(EXIT_FAILURE);
                }

                generator->push("QWORD [rsp+" + to_string((generator->stack_size - (*it).location - 1) * 8) + "]");

            }

            void operator()(const Node::ExpressionVariant::RoundBrackets* roundBracketExpression) const {

                generator->generateExpression(roundBracketExpression->expression);

            }

            void operator()(const Node::ExpressionVariant::Term* thermExpression) const {

                generator->generateTerm(thermExpression);

            }

        };

        expressionVisitor visitor { .generator = this };
        visit(visitor, expression->variant);

    };

    void generateTerm(const Node::ExpressionVariant::Term* term) {

        struct termVisitor {

            Generator* generator;

            void operator()(const Node::ExpressionVariant::TermVariant::Addition* additionTerm) const {

                generator->generateExpression(additionTerm->left);
                generator->generateExpression(additionTerm->right);

                generator->pop("rbx");
                generator->pop("rax");

                generator->assembly << "    add rax, rbx" << endl;

                generator->push("rax");

            }

            void operator()(const Node::ExpressionVariant::TermVariant::Subtraction* subtractionTerm) const {

                generator->generateExpression(subtractionTerm->left);
                generator->generateExpression(subtractionTerm->right);

                generator->pop("rbx");
                generator->pop("rax");

                generator->assembly << "    sub rax, rbx" << endl;

                generator->push("rax");

            }

            void operator()(const Node::ExpressionVariant::TermVariant::Multiplication* multiplicationTerm) const {

                generator->generateExpression(multiplicationTerm->left);
                generator->generateExpression(multiplicationTerm->right);

                generator->pop("rbx");
                generator->pop("rax");

                generator->assembly << "    mul rbx" << endl;

                generator->push("rax");

            }

            void operator()(const Node::ExpressionVariant::TermVariant::Division* divisionTerm) const {

                generator->generateExpression(divisionTerm->left);
                generator->generateExpression(divisionTerm->right);

                generator->pop("rbx");
                generator->pop("rax");

                generator->assembly << "    div rbx" << endl;

                generator->push("rax");

            }

        };

        termVisitor visitor { .generator = this };
        visit(visitor, term->variant);

    }

    // Scopes
    void startScope() {
        scopes.push_back(variables.size());
    }

    void endScope() {
        size_t popCount = variables.size() - scopes.back();
        assembly << "    add rsp, " << popCount * 8 << endl;
        stack_size -= popCount;
        for (int i = 0; i < popCount; i++) {
            variables.pop_back();
        }
        scopes.pop_back();
    }

    // Stack
    size_t stack_size = 0;
    void push (const string& reg) {
        assembly << "    push " << reg << endl;
        stack_size++;
    }
    void pop (const string& reg) {
        assembly << "    pop " << reg << endl;
        stack_size--;
    }

    // Variables
    struct Variable {
        string name;
        size_t location;
    };
    vector<Variable> variables {};
    vector<size_t> scopes {};

    // Labels
    string createLabel () {
        return "label" + to_string(++labelCount);
    }
    size_t labelCount = 0;

    const Node::Program program; // Input
    stringstream assembly; // Output
};