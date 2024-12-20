#include "Jasmin/Lexer.hpp"

#include <ClassFile/OpCodes.hpp>

#include <fmt/core.h>

namespace Jasmin
{

using TT = Token::TokenType;

std::string ToString(const Token::TokenType& type)
{
  switch(type)
  {
    case TT::Instruction: return "Instruction"  ; break;
    case TT::Symbol:      return "Symbol"       ; break;
    case TT::Label:       return "Label"        ; break;
    case TT::Integer:     return "Integer"      ; break;
    case TT::Decimal:     return "Decimal"      ; break;
    case TT::String:      return "String"       ; break;
    case TT::Colon:       return "Colon"        ; break;

    case TT::Catch:      return "Catch"       ; break;
    case TT::Class:      return "Class"       ; break;
    case TT::End:        return "End"         ; break;
    case TT::Field:      return "Field"       ; break;
    case TT::Implements: return "Implements"  ; break;
    case TT::Interface:  return "Interface"   ; break;
    case TT::Limit:      return "Limit"       ; break;
    case TT::Line:       return "Line"        ; break;
    case TT::Method:     return "Method"      ; break;
    case TT::Source:     return "Source"      ; break;
    case TT::Super:      return "Super"       ; break;
    case TT::Throws:     return "Throws"      ; break;
    case TT::Var:        return "Var"         ; break;

    case TT::Minus:       return "Minus"        ; break;

    case TT::Public:       return "Public"      ; break;
    case TT::Private:      return "Private"     ; break;
    case TT::Protected:    return "Protected"   ; break;
    case TT::Static:       return "Static"      ; break;
    case TT::Final:        return "Final"       ; break;
    case TT::Synchronized: return "Synchronized"; break;
    case TT::Native:       return "Native"      ; break;
    case TT::Abstract:     return "Abstract"    ; break;
    case TT::Volatile:     return "Volatile"    ; break;
    case TT::Transient:    return "Transient"   ; break;
    case TT::Default:      return "Default"     ; break;

    case TT::Newline:     return "Newline"      ; break;
  }

  return "UNKNOWN";
}

std::ostream& operator<<(std::ostream& out, const Token::TokenType& type)
{
  return out << ToString(type);
}

bool Token::IsDirective() const
{
  return this->Type >= TT::Catch && this->Type <= TT::Var;
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
    return makeToken(TT::Newline);

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

  //check if token is a valid instr mnemonic, according to libClassFile, by
  //getting its opcode
  auto errOrOp = ClassFile::GetOpCode(tokenStr);
  if(!errOrOp.IsError())
    return makeToken(TT::Instruction, std::move(tokenStr));

  return makeToken(TT::Symbol, std::move(tokenStr));
}

std::vector<Token> Lexer::LexAll()
{
  std::vector<Token> tokens;

  while(HasMore())
    tokens.emplace_back(LexNext());

  if(tokens.back().Type != TT::Newline)
    tokens.emplace_back(makeToken(TT::Newline));

  return tokens;
}

std::vector<Token> Lexer::LexAll(InStream& in)
{
  Lexer lexer{in};
  return lexer.LexAll();
}

std::vector<Token> Lexer::LexAll(InStream&& in)
{
  return Lexer::LexAll(in);
}

Lexer::Lexer(InStream in)
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
    {"catch",      TT::Catch     },
    {"class",      TT::Class     },
    {"end",        TT::End       },
    {"field",      TT::Field     },
    {"implements", TT::Implements},
    {"interface",  TT::Interface },
    {"limit",      TT::Limit     },
    {"line",       TT::Line      },
    {"method",     TT::Method    },
    {"source",     TT::Source    },
    {"super",      TT::Super     },
    {"throws",     TT::Throws    },
    {"var",        TT::Var       },
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
