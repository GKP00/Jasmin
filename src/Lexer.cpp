#include "Jasmin/Lexer.hpp"

#include <fmt/core.h>


namespace Jasmin
{

using TT = Token::TokenType;

std::ostream& operator<<(std::ostream& out, const Token::TokenType& type)
{
  switch(type)
  {
    case TT::Symbol: out << "Symbol"     ; break;

    case TT::DCatch:      out << "DCatch"     ; break;
    case TT::DClass:      out << "DClass"     ; break;
    case TT::DEnd:        out << "DEnd"       ; break;
    case TT::DField:      out << "DField"     ; break;
    case TT::DImplements: out << "DImplements"; break;
    case TT::DInterface:  out << "DInterface" ; break;
    case TT::DLimit:      out << "DLimit"     ; break;
    case TT::DLine:       out << "DLine"      ; break;
    case TT::DMethod:     out << "DMethod"    ; break;
    case TT::DSource:     out << "DSource"    ; break;
    case TT::DSuper:      out << "DSuper"     ; break;
    case TT::DThrows:     out << "DThrows"    ; break;
    case TT::DVar:        out << "DVar"       ; break;
  }

  return out;
}

bool Lexer::HasMore()
{
  this->skipWhitespaceAndComments();
  return peek() != EOF;
}

Token Lexer::LexNext()
{
  if(!this->HasMore())
    throw error(fmt::format("LexNext called after empty input stream"));

  if(consumeNextCharIf('.'))
    return this->lexDirective();

  std::string symbol;

  while( !isSpace(peek()) && peek() != EOF )
    symbol += get();

  return this->makeToken(Token::TokenType::Symbol, std::move(symbol));
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

Token Lexer::lexDirective()
{
  std::string directiveName;

  while(!isSpace(peek()))
    directiveName += get();

  std::pair<Token::TokenType, std::string_view> validTypeNamePairs[] =
  {
    {TT::DCatch,      "catch"},
    {TT::DClass,      "class"},
    {TT::DEnd,        "end"},
    {TT::DField,      "field"},
    {TT::DImplements, "implements"},
    {TT::DInterface,  "interface"},
    {TT::DLimit,      "limit"},
    {TT::DLine,       "line"},
    {TT::DMethod,     "method"},
    {TT::DSource,     "source"},
    {TT::DSuper,      "super"},
    {TT::DThrows,     "throws"},
    {TT::DVar,        "var"},
  };

  auto it = std::find_if(
      std::begin(validTypeNamePairs), 
      std::end(validTypeNamePairs), 
      [&directiveName](auto p)
      {
        return directiveName == p.second;
      });

  if( it == std::end(validTypeNamePairs) )
    throw error(fmt::format("invalid directive \".{}\"", directiveName));

  return this->makeToken((*it).first);
}

void Lexer::skipWhitespaceAndComments()
{
  while(isSpace(peek()) || peek(';'))
  {
    this->skipComment(); 
    this->skipWhitespace();
  }
}

void Lexer::skipWhitespace()
{
  while(consumeNextCharIf( this->isSpace ));
}

void Lexer::skipComment()
{
  while(consumeNextCharIf(';'))
    while(get() != '\n' && peek() != EOF);

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

} //namespace: Jasmin
