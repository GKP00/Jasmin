#pragma once

#include "Lexer.hpp"
#include "Nodes.hpp"

#include <vector>

namespace Jasmin
{

class Parser
{
  public:
    Parser(const std::vector<Token>& tokens);
    static std::vector<NodePtr> ParseAll(const std::vector<Token>& tokens);
    static std::vector<NodePtr> ParseAll(const std::vector<Token>&& tokens);

    bool  HasMore() const;
    NodePtr ParseNext();

  private:
    Token consumeNextToken();

    const std::vector<Token>& tokens;
    size_t currentToken = 0;
};

} //namespace: Jasmin
