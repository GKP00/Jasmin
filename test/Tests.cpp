#include <gtest/gtest.h>

#include <Jasmin/Lexer.hpp>

#include <iostream>

#include <string>
#include <sstream>
#include <vector>
#include <queue>

#ifndef RES_DIR
#define RES_DIR "res"
#endif

using T = Jasmin::Token;
using TT = T::TokenType;

static void expectTokens(std::vector<T> tokens, 
                         const std::vector<TT>& expectedTTs)
{
  EXPECT_TRUE(tokens.size() == expectedTTs.size() );

  for(unsigned i = 0; i < expectedTTs.size(); ++i)
    EXPECT_EQ(tokens[i].Type, expectedTTs[i]);
}

TEST(LexerTests, SampleClassAndSuperStatement)
{
  auto tokens = Jasmin::Lexer::LexAll( std::stringstream{
      R"(.class public HelloWorld
         .super java/lang/Object)"});

    expectTokens(tokens,
    {
      TT::Class,   TT::Public, TT::Symbol,
      TT::Newline, TT::Super,  TT::Symbol,
    });
}

TEST(LexerTests, SampleMethodDefinition)
{
  auto tokens = Jasmin::Lexer::LexAll( std::stringstream{
      R"(.method public static main([Ljava/lang/String;)V
           .limit stack 2
           getstatic java/lang/System/out Ljava/io/PrintStream;
           ldc "Hello World!"
           invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V
           return
         .end method)"});

  expectTokens(tokens, 
  {
    TT::Method, TT::Public, TT::Static,  TT::Symbol, TT::Newline,
    TT::Limit,  TT::Symbol, TT::Integer, TT::Newline,
    TT::Instruction, TT::Symbol, TT::Symbol,  TT::Newline,
    TT::Instruction, TT::String, TT::Newline,
    TT::Instruction, TT::Symbol, TT::Newline,
    TT::Instruction, TT::Newline,
    TT::End,    TT::Symbol,
  });

}
