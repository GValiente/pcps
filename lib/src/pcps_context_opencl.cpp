/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_context.h"

#include "pcps_boost_compute.h"

namespace bpc = boost::compute;

namespace pcps
{

namespace
{
    class DefaultContext : public Context
    {

    public:
        bpc::context defaultContext;
        bpc::command_queue defaultQueue;

        explicit DefaultContext(const bpc::device& device) :
            defaultContext(device),
            defaultQueue(defaultContext, device)
        {
            context = &defaultContext;
            queue = &defaultQueue;
        }
    };
}

std::unique_ptr<Context> Context::buildDefault()
{
    return buildDefault(bpc::system::default_device());
}

std::unique_ptr<Context> Context::buildDefault(const bpc::device& device)
{
    return std::unique_ptr<Context>(new DefaultContext(device));
}

Context::Context(const bpc::context& _context, bpc::command_queue& _queue) noexcept :
    context(&_context),
    queue(&_queue)
{
}

}
