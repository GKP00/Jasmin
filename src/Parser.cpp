#include "Jasmin/Parser.hpp"

#include <fmt/core.h>

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
  Token token;
  while( (token = consumeNextToken()).Type == TT::Newline);

  if(token.IsDirective())
    return parseDirective(std::move(token.Value));

  throw error(fmt::format(
        "unexpected top level token: {}=\"{}\"", ToString(token.Type), token.Value));
}

Token Parser::consumeNextToken()
{
  return tokens[currentToken++];
}

NodePtr Parser::parseDirective(std::string dName)
{ 
  auto pDNode = std::make_unique<DirectiveNode>();
  pDNode->Directive = std::move(dName);

  Token arg;
  while( (arg = consumeNextToken()).Type != TT::Newline ) 
    pDNode->Args.emplace_back( std::move(arg.Value) );

  return pDNode;
}
} //namespace: Jasmin
