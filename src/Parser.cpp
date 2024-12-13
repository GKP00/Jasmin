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
  while( (token = peekNextToken()).Type == TT::Newline) 
  { 
    consumeNextToken(); }

  if(token.IsDirective())
    return parseDirective();

  if(token.Type == TT::Instruction)
    return parseInstruction();

  if(token.Type == TT::Symbol)
    return parseLabel();

  throw error(fmt::format(
        "unexpected top level token: {}=\"{}\"", ToString(token.Type), token.Value));
}

Token Parser::peekNextToken() const
{
  return tokens.size() >= currentToken ? 
         tokens[currentToken] :
         throw error("ran out of tokens");
}

Token Parser::consumeNextToken()
{
  return tokens.size() > currentToken ? 
         tokens[currentToken++] :
         throw error("ran out of tokens");
}

std::string Parser::consumeExpected(TT expectedType)
{
  Token token = peekNextToken();

  if(token.Type != expectedType)
    throw error(fmt::format("unexpected token (expected {})", ToString(expectedType)));

  consumeNextToken();
  return token.Value;
}

Token Parser::consumeDirective()
{
  Token token = peekNextToken();

  if(!token.IsDirective())
    throw error(fmt::format("unexpected token (expected any directive)"));

  consumeNextToken();
  return token;
}

NodePtr Parser::parseDirective()
{ 
  NodePtr pDir = nullptr;
  Token directiveToken = consumeDirective();

  switch(directiveToken.Type)
  {
    default:
    {
      auto pUnimplemented = std::make_unique<DUnimplemented>();
      pUnimplemented->DirectiveName = directiveToken.Value;

      Token arg;
      while( (arg = peekNextToken()).Type != TT::Newline ) 
        pUnimplemented->Args.emplace_back( consumeNextToken().Value );

      pDir = std::move(pUnimplemented);
    }
  }

  consumeExpected(TT::Newline);
  return pDir;
}

NodePtr Parser::parseInstruction()
{
  std::string mnemonic = consumeExpected(TT::Instruction);

  auto pINode = std::make_unique<InstructionNode>();
  if(!pINode)
    throw error("parseInstruction failed to allocate InstructionNode");

  pINode->Mnemonic = std::move(mnemonic);

  Token arg;
  while( (arg = consumeNextToken()).Type != TT::Newline ) 
    pINode->Args.emplace_back( std::move(arg.Value) );

  return pINode;
}

NodePtr Parser::parseLabel()
{
  std::string label = consumeExpected(TT::Symbol);
  consumeExpected(TT::Colon);

  auto pLNode = std::make_unique<LabelNode>();
  if(!pLNode)
    throw error("parseLabel failed to allocate LabelNode");

  pLNode->LabelName = std::move(label);
  return pLNode;
}

std::runtime_error Parser::error(std::string_view message) const
{
  return std::runtime_error{
      fmt::format("Parser error: {} on line {} col {}",
      message,
      peekNextToken().Info.LineNumber,
      peekNextToken().Info.LineOffset - peekNextToken().Value.length())};
}

} //namespace: Jasmin
