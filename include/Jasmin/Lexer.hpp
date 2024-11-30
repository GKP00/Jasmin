#pragma once

#include "Stream.hpp"

#include <functional>
#include <string>
#include <string_view>
#include <queue>
#include <optional>
#include <map>

namespace Jasmin
{

class Token
{
  public:
    enum class TokenType
    {
      Instruction,
      Symbol,
      Label,
      Integer,
      Decimal,
      String,
      Colon,

      //DIRECTIVES (GROUPING MATTERS)
      Catch,
      Class,
      End,
      Field,
      Implements,
      Interface,
      Limit,
      Line,
      Method,
      Source,
      Super,
      Throws,
      Var,
      //END OF DIRECTIVES

      Minus,

      Public,
      Private,
      Protected,
      Static,
      Final,
      Synchronized,
      Native,
      Abstract,
      Volatile,
      Transient,
      Default,

      Newline,
    };

    bool IsDirective();

    struct MetaInfo
    {
      unsigned int LineNumber;
      unsigned short LineOffset;
      size_t FileOffset;
    };

    TokenType Type;
    std::string Value;
    MetaInfo Info;
};

using TT = Token::TokenType;

std::string ToString(const Token::TokenType&);
std::ostream& operator<<(std::ostream&, const Token::TokenType&);

class Lexer 
{
  public:
    Lexer(InStream& in);
    static std::vector<Token> LexAll(InStream& in);
    static std::vector<Token> LexAll(InStream&& in);

    bool  HasMore() const;
    Token LexNext();

    unsigned int   CurrentLineNumber() const;
    unsigned short CurrentLineOffset() const;
    size_t         CurrentFileOffset() const;

  private:
    //NOTE: below functions assume the first char has already been consumed
    //('.' for directives, ';' for comments, etc.)
    Token lexDirective();
    Token lexString();

    //NOTE: assumes '.' has already been consumed after the integer part
    Token lexDecimal(std::string integerPart);

    Token lexNumber();

    std::optional<Token> isKeywordToken(std::string_view);

    char get();
    char get(char);
    char peek() const;
    bool peek(char) const;

    void ensureNextChar(char, std::string_view msg="") const;
    void ensureNextChar(std::function<bool(char)>, std::string_view msg="") const;

    bool consumeNextCharIf(char);
    bool consumeNextCharIf(std::function<bool(char)>);

    void consumeToEndOfLine();
    void consumeWhitespaceAndComments();

    Token makeToken(Token::TokenType, std::string="") const;

    //NOTE: these are defined statically to be more easily passed as functors
    //(apparently std functions are special and cant be passed directly)
    static bool isAlpha(char c) { return std::isalpha(c); } 
    static bool isDigit(char c) { return std::isdigit(c); } 
    static bool isWhitespace(char c) { return std::isspace(c); }
    static bool isNewline(char c) { return c == '\n'; }
    static bool isSpace(char c) { return c == ' ' || c == '\t'; }
    static bool isEOF(char c) { return c == EOF; }

    std::runtime_error error(std::string_view) const;
    std::runtime_error logicError(std::string_view) const;

  private:
    InStream& inputStream;
};

} //namespace: Jasmin
