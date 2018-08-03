#include "stdafx.h"
#include "Component.h"
#include "Scene.h"
#include "View.h"

namespace controls::component
{
    Scene::Scene() { }

    Scene::~Scene() { }

    void Scene::invalid(const core::rc32f & rect)
    {
        _invalid = true;
        _invalid_rect = _invalid_rect.unite(rect);
        invalidated(_invalid_rect);
        invoke([this]() { flush(); });
    }

    void Scene::flush()
    {
        if (_invalid_rect.empty())
            return;

        core::si32i size = _invalid_rect.size.to<int32_t>();
        if (!_renderBuffer || _renderBuffer->size().cx < size.cx || _renderBuffer->size().cy < size.cy)
            _renderBuffer = std::make_shared<graphics::Bitmap>(size);

        graphics::Graphics graphics(_renderBuffer);
        graphics.clear(_color_default);
        render(graphics);
        rendered(_invalid_rect);
        _invalid_rect.clear();
    }

    core::error Scene::insert(std::shared_ptr<View> view)
    {
        if (!view)
            return core::error_args;

        view->enteringScene(share_ref<Scene>());
        _views.push_back(view);
        view->enterScene(share_ref<Scene>());
        return core::error_ok;
    }

    core::error Scene::remove(std::shared_ptr<View> view)
    {
        if (!view)
            return core::error_args;

        view->leavingScene(share_ref<Scene>());
        _views.remove(view);
        view->leaveScene(share_ref<Scene>());
        return core::error_ok;
    }

    void Scene::render(graphics::Graphics & graphics) const
    {
        for (auto & view : _views)
            view->render(graphics);
        const_cast<Scene *>(this)->_invalid = false;
    }

    std::shared_ptr<MouseArea> Scene::findMouseArea(const core::pt32f & pos) const
    {
        for (auto & view : _views)
        {
            auto ma = view->findMouseArea(pos);
            if (ma)
                return ma;
        }
        return nullptr;
    }

    void Scene::onMouseEnter(const mosue_state & state)
    {
        auto ma = findMouseArea(state.pos());
        if (_mousearea_hoving == ma)
            return;

        if (_mousearea_hoving)
            _mousearea_hoving->onMouseLeave(state);

        _mousearea_hoving = ma;

        if (_mousearea_hoving)
            _mousearea_hoving->onMouseEnter(state);
    }

    void Scene::onMouseMove(const mosue_state & state)
    {
        auto ma = findMouseArea(state.pos());
        if (_mousearea_hoving != ma)
        {
            if (_mousearea_hoving)
                _mousearea_hoving->onMouseLeave(state);

            _mousearea_hoving = ma;

            if (_mousearea_hoving)
                _mousearea_hoving->onMouseEnter(state);
        }

        if (_mousearea_hoving)
            _mousearea_hoving->onMouseMove(state);
    }

    void Scene::onMouseLeave(const mosue_state & state)
    {
        if (_mousearea_hoving)
            _mousearea_hoving->onMouseLeave(state);

        _mousearea_hoving = nullptr;
    }

    void Scene::onMouseDown(const mosue_state & state)
    {
        if (_mousearea_hoving)
            _mousearea_hoving->onMouseDown(state);
    }

   void Scene::onMouseUp(const mosue_state & state)
   {
       if (_mousearea_hoving)
           _mousearea_hoving->onMouseUp(state);
   }

   void Scene::onMouseClick(const mosue_state & state)
   {
       if (_mousearea_hoving)
           _mousearea_hoving->onMouseClick(state);
   }

   void Scene::onMouseDBClick(const mosue_state & state)
   {
       if (_mousearea_hoving)
           _mousearea_hoving->onMouseDBClick(state);
   }
}
