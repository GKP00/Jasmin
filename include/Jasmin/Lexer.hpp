#include "Stream.hpp"

#include<functional>
#include<string>
#include<string_view>
#include<queue>

namespace Jasmin
{

class Token
{
  public:
    enum class TokenType
    {
      Symbol,
    };

    struct MetaInfo
    {
      unsigned int LineNumber;
      unsigned short LineOffset;
      size_t FileOffset;
    };

    const TokenType Type;
    const std::string Value;
    const MetaInfo Info;
};


class Lexer 
{
  public:
    Lexer(InStream& in);
    Lexer(InStream&& in);
    static std::queue<Token> LexAll(InStream& in);
    static std::queue<Token> LexAll(InStream&& in);

    bool  HasMore(); 
    Token LexNext();

    unsigned int   CurrentLineNumber() const;
    unsigned short CurrentLineOffset() const;
    size_t         CurrentFileOffset() const;

  private:
    char get();
    char get(char);
    char peek() const;
    bool peek(char) const;

    void ensureNextChar(char) const;
    void ensureNextChar(std::function<bool(char)>) const;

    bool consumeNextCharIf(char);
    bool consumeNextCharIf(std::function<bool(char)>);

    void skipWhitespaceAndComments();
    void skipWhitespace();
    void skipComments();

    Token makeToken(Token::TokenType, std::string) const;
    Token makeToken(Token::TokenType, char) const;

    static bool isSpace(char c) { return std::isspace(c); }

    std::runtime_error error(std::string_view) const;

  private:
    InStream& inputStream;
};

} //namespace: Jasmin
