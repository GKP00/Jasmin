#pragma once

#include <ClassFile/ClassFile.hpp>

#include "Parser.hpp"

namespace Jasmin
{

class Assembler
{
  public:
    static ClassFile::ClassFile Assemble(Parser);
    static ClassFile::ClassFile Assemble(InStream);
  private:
    ClassFile::ClassFile cf;
};

} //namespace: Jasmin
