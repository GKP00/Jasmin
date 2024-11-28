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

static void expectTokens(std::queue<T> tokens, 
                         const std::vector<TT>& expectedTTs)
{
  for(TT expectedTT : expectedTTs)
  {
    EXPECT_FALSE(tokens.empty());
    EXPECT_EQ(tokens.front().Type, expectedTT);
    tokens.pop();
  }

  EXPECT_TRUE(tokens.empty());
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
    TT::Symbol, TT::Symbol, TT::Symbol,  TT::Newline, //first should be instr
    TT::Symbol, TT::String, TT::Newline, //first should be instr
    TT::Symbol, TT::Symbol, TT::Newline, //first should be instr
    TT::Symbol, TT::Newline, //first should be Return
    TT::End,    TT::Symbol,
  });

}
