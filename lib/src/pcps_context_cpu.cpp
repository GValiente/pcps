/*
 * (c) 2019 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * MIT License, see LICENSE file.
 */

#include "pcps_context.h"

namespace pcps
{

std::unique_ptr<Context> Context::buildDefault()
{
    return std::unique_ptr<Context>(new Context());
}

}
