#pragma once

#include "Lexer.hpp"
#include "Nodes.hpp"

#include <vector>
#include <string_view>

namespace Jasmin
{

class Parser
{
  public:
    Parser(const std::vector<Token>& tokens);
    Parser(Lexer lexer);
    std::vector<NodePtr> ParseAll();
    static std::vector<NodePtr> ParseAll(const std::vector<Token>& tokens);
    static std::vector<NodePtr> ParseAll(const std::vector<Token>&& tokens);

    bool HasMore() const;
    NodePtr ParseNext();

  private:
    Token consumeNextToken();
    Token peekNextToken() const;
    std::string consumeExpected(TT);
    Token consumeDirective();

    NodePtr parseDirective();
    NodePtr parseInstruction();
    NodePtr parseLabel();

    std::runtime_error error(std::string_view) const;

    const std::vector<Token>& tokens;
    size_t currentToken = 0;
};

} //namespace: Jasmin
