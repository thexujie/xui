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
        auto rc = rect.ceil<int32_t>();
        _invalid_rect.unite(rc);
        _invalid_region.addRect(rc);
        invalidated(rc);
        invoke([this]() { flush(); });
    }

    void Scene::flush()
    {
        if (_invalid_rect.empty())
            return;

        if (!_renderBuffer || _renderBuffer->size().cx < _invalid_rect.right() || _renderBuffer->size().cy < _invalid_rect.bottom())
            _renderBuffer = std::make_shared<graphics::Bitmap>(core::si32i{ _invalid_rect.right(), _invalid_rect.bottom() });

        auto rect = _invalid_region.bounds();
        graphics::Graphics graphics(_renderBuffer);
        graphics.setClipRect(rect.to<float32_t>(), false);
        graphics.clear(_color_default);
        render(graphics, _invalid_region);
        rendered(rect);
        //rendered(core::rc32i{{}, _renderBuffer ->size()});
        _invalid_rect.clear();
        _invalid_region.clear();
        static bool save = false;
        if (save)
            _renderBuffer->Save("scene.png");
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

    void Scene::render(graphics::Graphics & graphics, const graphics::Region & region) const
    {
        for (auto & view : _views)
            view->render(graphics, region);
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
