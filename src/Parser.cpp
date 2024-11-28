#include "Jasmin/Parser.hpp"

namespace Jasmin
{

Parser::Parser(const std::vector<Token>& ts): tokens{ts} {}

std::vector<NodePtr> Parser::ParseAll(const std::vector<Token>& tokens)
{
  Parser parser{tokens};

  std::vector<NodePtr> nodes;
  while(parser.HasMore())
    nodes.emplace_back(parser.ParseNext());

  return nodes;
}

std::vector<NodePtr> Parser::ParseAll(const std::vector<Token>&& tokens)
{
  return ParseAll(tokens);
}

bool Parser::HasMore() const
{
  return currentToken < tokens.size();
}

NodePtr Parser::ParseNext()
{
  consumeNextToken();
  return std::make_unique<Node>();
}

Token Parser::consumeNextToken()
{
  return tokens[currentToken++];
}

} //namespace: Jasmin
