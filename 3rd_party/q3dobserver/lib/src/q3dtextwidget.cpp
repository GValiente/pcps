/*
 * Copyright (c) 2018 Gustavo Valiente gustavo.valiente@protonmail.com
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "q3dtextwidget.h"

#include <cmath>
#include <QPainter>
#include "Q3DObserver"

Q3DTextWidget::Q3DTextWidget(Q3DObserver& parent) :
    QWidget(&parent)
{
}

bool Q3DTextWidget::empty() const noexcept
{
    return _texts.empty();
}

void Q3DTextWidget::viewport(QVector3D& minPos, QVector3D& maxPos) const noexcept
{
    QVector3D min = minPos;
    QVector3D max = maxPos;

    for(const auto& text : _texts)
    {
        const auto& pos = text.vertex.position();
        min[0] = std::min(min[0], pos[0]);
        min[1] = std::min(min[1], pos[1]);
        min[2] = std::min(min[2], pos[2]);
        max[0] = std::max(max[0], pos[0]);
        max[1] = std::max(max[1], pos[1]);
        max[2] = std::max(max[2], pos[2]);
    }

    minPos = min;
    maxPos = max;
}

void Q3DTextWidget::clear() noexcept
{
    _texts.clear();
}

void Q3DTextWidget::reserve(int texts)
{
    _texts.reserve(texts);
}

void Q3DTextWidget::add(const Q3DVertex& vertex, QString&& text)
{
    _texts.append(Text{ vertex, std::move(text) });
}

void Q3DTextWidget::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);

    if(empty())
    {
        return;
    }

    auto parent = dynamic_cast<Q3DObserver*>(parentWidget());

    if(! parent)
    {
        return;
    }

    const auto& projection = parent->projection();
    const auto& modelView = parent->modelView();
    auto viewport = rect();
    QPainter painter(this);
    painter.setFont(parent->font());
    painter.setRenderHint(QPainter::TextAntialiasing);

    QFontMetrics fontMetrics = painter.fontMetrics();

    for(const Text& text : _texts)
    {
        QVector3D screenPos = text.vertex.position().project(modelView, projection, viewport);

        if(std::abs(screenPos[2]) < 1)
        {
            int width = fontMetrics.width(text.content);
            int x = int(std::round(screenPos[0])) - (width / 2);
            int y = viewport.height() - int(std::round(screenPos[1]));
            painter.setPen(text.vertex.color().color());
            painter.drawText(x, y, text.content);
        }
    }
}
