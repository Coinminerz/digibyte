// Copyright (c) 2021 The DigiByte Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <interfaces/init.h>
#include <node/context.h>
#include <util/system.h>

#include <memory>

namespace init {
namespace {
class DigiBytedInit : public interfaces::Init
{
public:
    DigiBytedInit(NodeContext& node) : m_node(node)
    {
        m_node.args = &gArgs;
        m_node.init = this;
    }
    NodeContext& m_node;
};
} // namespace
} // namespace init

namespace interfaces {
std::unique_ptr<Init> MakeNodeInit(NodeContext& node, int argc, char* argv[], int& exit_status)
{
    return std::make_unique<init::DigiBytedInit>(node);
}
} // namespace interfaces
