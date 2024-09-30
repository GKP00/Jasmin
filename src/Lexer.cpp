#include "Jasmin/Lexer.hpp"

#include <fmt/core.h>

namespace Jasmin
{

using TT = Token::TokenType;

std::ostream& operator<<(std::ostream& out, const Token::TokenType& type)
{
  switch(type)
  {
    case TT::Symbol:      out << "Symbol"       ; break;
    case TT::Label:       out << "Label"        ; break;
    case TT::Integer:     out << "Integer"      ; break;
    case TT::Decimal:     out << "Decimal"      ; break;
    case TT::String:      out << "String"       ; break;
    case TT::Colon:       out << "Colon"        ; break;

    case TT::DCatch:      out << "DCatch"       ; break;
    case TT::DClass:      out << "DClass"       ; break;
    case TT::DEnd:        out << "DEnd"         ; break;
    case TT::DField:      out << "DField"       ; break;
    case TT::DImplements: out << "DImplements"  ; break;
    case TT::DInterface:  out << "DInterface"   ; break;
    case TT::DLimit:      out << "DLimit"       ; break;
    case TT::DLine:       out << "DLine"        ; break;
    case TT::DMethod:     out << "DMethod"      ; break;
    case TT::DSource:     out << "DSource"      ; break;
    case TT::DSuper:      out << "DSuper"       ; break;
    case TT::DThrows:     out << "DThrows"      ; break;
    case TT::DVar:        out << "DVar"         ; break;

    case TT::Minus:       out << "Minus"        ; break;

    case TT::Public:       out << "Public"      ; break;
    case TT::Private:      out << "Private"     ; break;
    case TT::Protected:    out << "Protected"   ; break;
    case TT::Static:       out << "Static"      ; break;
    case TT::Final:        out << "Final"       ; break;
    case TT::Synchronized: out << "Synchronized"; break;
    case TT::Native:       out << "Native"      ; break;
    case TT::Abstract:     out << "Abstract"    ; break;
    case TT::Volatile:     out << "Volatile"    ; break;
    case TT::Transient:    out << "Transient"   ; break;
    case TT::Default:      out << "Default"     ; break;

    case TT::Newline:     out << "Newline"      ; break;
    case TT::END:         out << "End"          ; break;
  }

  return out;
}

bool Lexer::HasMore() const
{
  return !isEOF(peek());
}

Token Lexer::LexNext()
{
  consumeWhitespaceAndComments();

  if(consumeNextCharIf('\n'))
    return makeToken(TT::Newline);

  if(!HasMore())
    return makeToken(TT::END);

  if(consumeNextCharIf('.'))
  {
    if( isDigit(peek()) )
      return lexDecimal("0");
    else
      return lexDirective();
  }

  if(consumeNextCharIf('"'))
    return lexString();

  if(consumeNextCharIf('-'))
    return makeToken(TT::Minus);

  if(isDigit(peek()))
    return lexNumber();

  std::string tokenStr;

  while( !isWhitespace(peek()) && !isEOF(peek()) )
    tokenStr += get();

  if(tokenStr == ":")
    return makeToken(TT::Colon);

  if(tokenStr.back() == ':')
    return makeToken(TT::Label, std::move(tokenStr));

  std::optional<Token> token = isKeywordToken(tokenStr);
  if(token)
    return *token;

  return makeToken(TT::Symbol, std::move(tokenStr));
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

unsigned int Lexer::CurrentLineNumber() const
{
  return inputStream.CurrentLineNumber();
}

unsigned short Lexer::CurrentLineOffset() const
{
  return inputStream.CurrentLineOffset();
}

size_t Lexer::CurrentFileOffset() const
{
  return inputStream.CurrentFileOffset();
}

Token Lexer::lexDirective()
{
  std::string dirName;

  while( !isWhitespace(peek()) )
  {
    ensureNextChar(isAlpha, "non alpha characters are invalid in directives");
    dirName += get();
  }

  if(dirName.empty())
    throw error("invalid directive name of length 0");

  static const std::map<std::string_view, TT> directiveTokenMap =
  {
    {"catch",      TT::DCatch     },
    {"class",      TT::DClass     },
    {"end",        TT::DEnd       },
    {"field",      TT::DField     },
    {"implements", TT::DImplements},
    {"interface",  TT::DInterface },
    {"limit",      TT::DLimit     },
    {"line",       TT::DLine      },
    {"method",     TT::DMethod    },
    {"source",     TT::DSource    },
    {"super",      TT::DSuper     },
    {"throws",     TT::DThrows    },
    {"var",        TT::DVar       },
  };

  auto it = directiveTokenMap.find(dirName);

  if(it == directiveTokenMap.end())
    throw error(fmt::format("invalid directive name \"{}\"", dirName));

  return makeToken(it->second);
}

Token Lexer::lexString()
{
  std::string str;

  while(peek() != '"')
  {
    if(isEOF(peek()) || isNewline(peek()))
        throw error("invalid string with no end");

    if(consumeNextCharIf('\\'))
    {
      if(consumeNextCharIf('"'))
        str += '"';
      else if(consumeNextCharIf('n'))
        str += '\n';
      else
        throw error("invalid escape character");

      continue;
    }

    str += get();
  }

  get(); 

  return makeToken(TT::String, std::move(str));
}

Token Lexer::lexDecimal( std::string integerPart )
{
  if(integerPart.empty())
    throw logicError("lexDecimal() called with empty integer part");

  std::string fractionPart;
  while( isDigit(peek()) )
    fractionPart += get();

  if(fractionPart.empty())
    throw error("invalid decimal with no fraction part");

  std::string decimalStr;
  decimalStr += std::move(integerPart);
  decimalStr += '.';
  decimalStr += std::move(fractionPart);

  return makeToken(TT::Decimal, std::move(decimalStr));
}

Token Lexer::lexNumber()
{
  std::string integerStr;

  if(consumeNextCharIf('0'))
  {
    integerStr += '0';

    if(consumeNextCharIf('x'))
      integerStr += 'x';
    else if(consumeNextCharIf('.'))
      return lexDecimal("0");

    if(peek() == '0')
      throw error("double zero encountered in integer");
  }

  while( isDigit(peek()) )
    integerStr += get();

  if(integerStr.empty())
    throw logicError("lexNumber() called but no digits consumed");

  return makeToken(TT::Integer, std::move(integerStr));
}

std::optional<Token> Lexer::isKeywordToken(std::string_view keywordStr)
{
  static const std::map<std::string_view, TT> keywordTokenMap = 
  {
    {"public",       TT::Public      },
    {"private",      TT::Private     },
    {"protected",    TT::Protected   },
    {"static",       TT::Static      },
    {"final",        TT::Final       },
    {"synchronized", TT::Synchronized},
    {"native",       TT::Native      },
    {"abstract",     TT::Abstract    },
    {"volatile",     TT::Volatile    },
    {"transient",    TT::Transient   },
    {"default",      TT::Default     },
  };

  auto it = keywordTokenMap.find(keywordStr);
  if(it != keywordTokenMap.end())
    return makeToken(it->second);

  return std::nullopt;
}

void Lexer::consumeToEndOfLine()
{
  while(!isNewline(peek()) && !isEOF(peek()))
    get();
}

void Lexer::consumeWhitespaceAndComments()
{
  //NOTE: technically the spec (https://jasmin.sourceforge.net/guide.html) 
  //says comments have to be preceded by any whitespace which includes 
  //newlines, but thats stupid and im not doing that.
  while(consumeNextCharIf(isSpace))
  {
    if(consumeNextCharIf(';'))
      consumeToEndOfLine();
  }
}


Token Lexer::makeToken(TT type, std::string val) const
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
  return peek() == c;
}

void Lexer::ensureNextChar(char next, std::string_view msg) const
{
  if(peek() != next)
    throw error(fmt::format("encountered '{}' when '{}' was expected", peek(), next));
}

void Lexer::ensureNextChar(std::function<bool(char)> isWhatsExpected, std::string_view msg) const
{
  if(!isWhatsExpected(peek()))
    throw error(fmt::format("unexpected character '{}' ({})", peek(), msg));
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

std::runtime_error Lexer::logicError(std::string_view message) const
{
  return std::runtime_error{fmt::format(
      "Program error: {} (occured while lexing line {} col {})", 
      message, this->CurrentLineNumber(), this->CurrentLineOffset())};
}

} //namespace: Jasmin
