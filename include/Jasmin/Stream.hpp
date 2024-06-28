#pragma once

#include<istream>

namespace Jasmin
{

class InStream
{
  public:
    InStream(std::istream&& in) : InStream(in) {}
    InStream(std::istream& in) : inputStream{in} 
    {
      if(!inputStream.good())
        throw std::runtime_error{"Istream given bad std::istream"};
    }

    char get()
    {
      char ch = inputStream.get();
      ++lineOffset;
      ++fileOffset;

      if(ch == '\n')
      {
        ++lineNumber;
        lineOffset = 1;
      }

      return ch;
    }

    char peek() const
    {
      return inputStream.peek();
    }

    unsigned int   CurrentLineNumber() const { return lineNumber; }
    unsigned short CurrentLineOffset() const { return lineOffset; }
    size_t         CurrentFileOffset() const { return fileOffset; }

  private:
    std::istream& inputStream;

    unsigned int   lineNumber{1};
    unsigned short lineOffset{1};
    size_t         fileOffset{0};

};

} //namespace: Jasmin
