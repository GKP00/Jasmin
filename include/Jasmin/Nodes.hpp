#pragma once

#include <memory>

namespace Jasmin
{

enum AccessSpec
{
  PUBLIC, FINAL, SUPER, INTERFACE, ABSTRACT,
};

struct Node
{
  public:
    enum Type
    {
      Directive,
      Code,
      Label,
      Method,
    };

    Type GetType()
    {
      return type;
    }

    virtual ~Node() {}

  protected:
    Node(Type t) : type{t} {}

  private:
    Type type;
};

using NT = Node::Type;
using NodePtr = std::unique_ptr<Node>;

struct DirectiveNode : public Node
{
  DirectiveNode() : Node(NT::Directive) {}

  std::string Directive;
  std::vector<std::string> Args;
};

struct CodeNode : public Node
{
  CodeNode() : Node(NT::Code) {}

  struct Instr
  {
    std::string Name;
    std::vector<std::string> Args;
  };

  std::vector<Instr> Instructions;
};

struct LabelNode : public Node
{
  LabelNode() : Node(NT::Label) {}

  std::string Name;
  CodeNode Code;
};

struct MethodNode : public Node
{
  MethodNode() : Node(NT::Method) {}

  AccessSpec Access;
  std::string Name;
  std::vector<DirectiveNode> Directives;
  std::vector<CodeNode> Code;
};

} //namespace: Jasmin

