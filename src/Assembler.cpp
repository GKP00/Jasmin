#include "Jasmin/Assembler.hpp"

namespace Jasmin
{

ClassFile::ClassFile Assembler::Assemble(Parser parser)
{
  return {};
}

ClassFile::ClassFile Assembler::Assemble(InStream stream)
{
  return Assemble( Parser{ Lexer{stream} } );
}


} //namespace: Jasmin
