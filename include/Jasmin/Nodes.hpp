#pragma once

#include <memory>

namespace Jasmin
{

struct Node
{
};

using NodePtr = std::unique_ptr<Node>;

} //namespace: Jasmin

