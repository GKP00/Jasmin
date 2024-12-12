#pragma once

#include "./Common.hpp"

#include <memory>
#include <tuple>
#include <optional>

namespace Jasmin
{

struct Node
{
  virtual ~Node(){}
  protected:
  Node(){}
};
using NodePtr = std::unique_ptr<Node>;

struct InstructionNode : public Node
{
  std::string Mnemonic;
  std::vector<std::string> Args;
};

struct LabelNode : public Node
{
  std::string LabelName;
};

struct DirectiveNode : public Node 
{
  virtual ~DirectiveNode(){}
  protected:
  DirectiveNode(){}
};

struct DUnimplemented : public DirectiveNode
{
  std::string DirectiveName;
  std::vector<std::string> Args;
};

struct DBytecode : public DirectiveNode
{
  U16 Major, Minor;
};

struct DSource : public DirectiveNode
{
  std::string Source;
};

struct DClass : public DirectiveNode
{
  std::string ClassName;
  AccessSpec Access;
};

struct DSuper : public DirectiveNode
{
  std::string SuperName;
};

} //namespace: Jasmin

