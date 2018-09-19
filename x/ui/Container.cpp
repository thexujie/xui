#include "stdafx.h"
#include "Container.h"

namespace ui
{
    Container::Container()
    {
        _interactable = false;
    }

    Container::~Container()
    {
        
    }

    void Container::addControl(std::shared_ptr<Control> control)
    {
        std::lock_guard lock(*this);
        if (std::any_of(_controls.begin(), _controls.end(), [&control](const auto & pair) { return pair.second == control; }))
            return;

        auto s = scene();
        control->setParent(share_ref<Container>());
        if(s)
        {
            control->enteringScene(s);
            _controls.insert(std::make_pair(control->ZValue(), control));
            control->enterScene(s);
        }
        else
            _controls.insert(std::make_pair(control->ZValue(), control));
        relayout();
    }

    void Container::removeControl(std::shared_ptr<Control> control)
    {
        std::lock_guard lock(*this);
        control->leavingScene();
        for (auto iter = _controls.begin(); iter != _controls.end(); )
        {
            if (iter->second == control)
            {
                control->setParent(nullptr);
                _controls.erase(iter);
                break;
            }
        }
        control->leaveScene();
    }

    void Container::enteringScene(std::shared_ptr<Scene> & scene)
    {
        Control::enteringScene(scene);
        for (auto & iter : _controls)
            iter.second->enteringScene(scene);
    }

    void Container::enterScene(std::shared_ptr<Scene> & scene)
    {
        Control::enterScene(scene);
        for (auto & iter : _controls)
            iter.second->enterScene(scene);
    }

    void Container::leavingScene()
    {
        for (auto & iter : _controls)
            iter.second->leavingScene();
        Control::leavingScene();
    }

    void Container::leaveScene()
    {
        for (auto & iter : _controls)
            iter.second->leaveScene();
        Control::leaveScene();
    }

    void Container::update()
    {
        Control::update();
        for (auto & iter : _controls)
            iter.second->update();
    }

    int32_t Container::animate()
    {
        int32_t num = Control::animate();
        for (auto & iter : _controls)
            num += iter.second->animate();
        return num;
    }

    void Container::ondraw(drawing::Graphics & graphics, const drawing::Region & region) const
    {
        uint32_t a = std::clamp< uint32_t>(uint32_t(std::round(_alpha * 0xff)), 0, 0xff);
        std::lock_guard lock(*this);        
        if (a != 0xff)
            graphics.saveLayer(box(), a);
        _drawBackground(graphics);
        for (auto & iter : _controls)
        {
            auto rect = iter.second->realRect();
            if(region.intersects(rect.ceil<int32_t>()))
                iter.second->ondraw(graphics, region);
        }
        _drawBorder(graphics);
        if (a != 0xff)
            graphics.restore();
    }

    std::shared_ptr<Control> Container::findChild(const core::pt32f & pos, std::shared_ptr<Control> last) const
    {
        if (_clip_clild && !_rect.contains(pos))
            return nullptr;

        bool found = false;

        std::shared_ptr<Control> control = nullptr;
        for (auto & iter : core::reverse(_controls))
        {
            if (last && !found)
            {
                if (iter.second == last)
                    found = true;
                continue;
            }

            auto hittest = iter.second->hitTest(pos);
            if (hittest == hittest_result::nowhere)
                continue;

            if (hittest == hittest_result::client)
                return iter.second;

            auto child = iter.second->findChild(pos, last);
            if (child)
                return child;

            if (hittest == hittest_result::stable)
                return iter.second;

            control = iter.second;
        }
        return control;
    }

    void Container::onLayoutedSizeChaged(const core::si32f & from, const core::si32f & to)
    {
        layoutedSizeChaged(from, to);
    }

    void Container::setLayoutedSize(const core::si32f & layouted_size)
    {
        if (layouted_size != _layouted_size)
        {
            _layouted_size = layouted_size;
        }
    }
}

