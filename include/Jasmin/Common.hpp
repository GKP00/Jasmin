#pragma once

#include <cstdint>
namespace Jasmin
{
  using U16 = std::uint16_t;

  enum AccessFlag : U16
  {
    PUBLIC       = 0x0001, 
    PRIVATE      = 0x0002, 
    PROTECTED    = 0x0004, 
    STATIC       = 0x0008, 
    FINAL        = 0x0010, 
    SYNCHRONIZED = 0x0020,
    NATIVE       = 0x0040,
    SUPER        = 0x0080, 
    INTERFACE    = 0x0100, 
    ABSTRACT     = 0x0200,
    ANNOTATION   = 0x0400,
    ENUM         = 0x0800,
    VOLATILE     = 0x1000,
    TRANSIENT    = 0x2000,
  };
  using AccessSpec = U16;

} //namespace: Jasmin
