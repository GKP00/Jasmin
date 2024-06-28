#include "Jasmin/Lexer.hpp"

#include <fmt/core.h>

using namespace Jasmin;

bool Lexer::HasMore()
{
  this->skipWhitespaceAndComments();
  return peek() != EOF;
}

Token Lexer::LexNext()
{
  if(!this->HasMore())
    throw error(fmt::format("LexNext called after empty input stream"));

  throw error("LexNext() unimplemented");

  return this->makeToken(Token::TokenType::Symbol, "unimplemented");
}

std::queue<Token> Lexer::LexAll(InStream& in)
{
  Lexer lexer{in};
  std::queue<Token> tokens;

  while(lexer.HasMore())
    tokens.emplace(lexer.LexNext());

  return tokens;
}

std::queue<Token> Lexer::LexAll(InStream&& in)
{
  return Lexer::LexAll(in);
}

Lexer::Lexer(InStream& in)
: inputStream{in}
{
}

Lexer::Lexer(InStream&& in) : Lexer(in) {}

unsigned int Lexer::CurrentLineNumber() const
{
  return this->inputStream.CurrentLineNumber();
}

unsigned short Lexer::CurrentLineOffset() const
{
  return this->inputStream.CurrentLineOffset();
}

size_t Lexer::CurrentFileOffset() const
{
  return this->inputStream.CurrentFileOffset();
}

void Lexer::skipWhitespaceAndComments()
{
  while(isSpace(peek()) || peek(';'))
  {
    this->skipWhitespace();
    this->skipComments();
  }
}

void Lexer::skipWhitespace()
{
  while(consumeNextCharIf( this->isSpace ));
}

void Lexer::skipComments()
{
  if(!consumeNextCharIf(';'))
    return;

  while(peek() != '\n' && peek() != EOF)
    get();

  if(peek() == ';')
    skipComments();
}

Token Lexer::makeToken(Token::TokenType type, std::string val) const
{
  return Token
  {
    type, 
    std::move(val), 
    {
      this->CurrentLineNumber(),
      static_cast<unsigned short>(this->CurrentLineOffset() - val.length()),
      static_cast<unsigned int>  (this->CurrentFileOffset() - val.length())
    }
  };
}

char Lexer::get(char c)
{
  ensureNextChar(c);
  return get();
}

char Lexer::get()
{
  return inputStream.get();
}

char Lexer::peek() const
{
  return inputStream.peek();
}

bool Lexer::peek(char c) const
{
  return this->peek() == c;
}

void Lexer::ensureNextChar(char next) const
{
  if(peek() != next)
    throw error(fmt::format("encountered '{}' when '{}' was expected", peek(), next));
}

void Lexer::ensureNextChar(std::function<bool(char)> isWhatsExpected) const
{
  if(!isWhatsExpected(peek()))
    throw error(fmt::format("encountered unexpected lexeme value '{}'", peek()));
}

bool Lexer::consumeNextCharIf(char c)
{
  return consumeNextCharIf([c](char next){ return c == next;});
}

bool Lexer::consumeNextCharIf(std::function<bool(char)> func)
{
  if( !func(peek()) )
    return false;

  get();
  return true;
}

std::runtime_error Lexer::error(std::string_view message) const
{
  return std::runtime_error{fmt::format("Lexer error: {} on line {} col {}", 
      message, this->CurrentLineNumber(), this->CurrentLineOffset())};
}

