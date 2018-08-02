#pragma once
#include "component/Scene.h"
#include "component/View.h"
#include "renderables/Text.h"
#include "renderables/Image.h"
#include "renderables/Line.h"
#include "renderables/Rectangle.h"

namespace controls
{
    enum class control_box
    {
        // �߿򷽿�
        border_box = 0,
        // �ڿ򷽿�
        padding_box,
        // ���ݷ���
        content_box,
    };

    enum class layout_origin
    {
        // Ĭ��λ��
        layout = 0,
        // ����� parent ��λ��
        parent,
        // ����� scene ��λ��
        scene,
        // ����� view ��λ��
        view,
        // ͬ layout��������� scene ������� view
        sticky,
    };

    class Control : public core::invokable<Control>
    {
    public:
        Control();
        virtual ~Control();

        void setLayoutOrigin(layout_origin origin) { _layout_origin = origin; }
        layout_origin layoutOrigin() const { return _layout_origin; }

        void setPos(const core::vec2<core::dimensionf> & pos) { _pos = pos; }
        const core::vec2<core::dimensionf> & pos() const { return _pos; }
        void setSize(const core::vec2<core::dimensionf> & size) { _size = size; }
        const core::vec2<core::dimensionf> & size() const { return _size; }
        void setMinSize(const core::vec2<core::dimensionf> & minSize) { _minSize = minSize; }
        const core::vec2<core::dimensionf> & minSize() const { return _minSize; }
        void setMaxSize(const core::vec2<core::dimensionf> & maxSize) { _maxSize = maxSize; }
        const core::vec2<core::dimensionf> & maxSize() const { return _maxSize; }

        void setPos(const core::vec2f & pos);
        void setSize(const core::vec2f & size);

        // prefferSize ��һ�����������ؼ���С�ġ�������С�����ɿؼ��������
        core::si32f prefferSize() const;
        virtual core::si32f contentSize() const { return core::si32f(); }

        std::shared_ptr<component::View> view() const;
        std::shared_ptr<component::Scene> scene() const { return _scene.lock(); }
        void setParent(std::shared_ptr<Control> parent) { _parent = parent; }
        std::shared_ptr<Control> parent() const { return _parent.lock(); }

        const core::color32 & color() const;
        const graphics::font & font() const;

        core::si32f realPos() const { return _rect.pos; }
        core::si32f realSize() const { return _rect.size; }
        core::rc32f realRect() const { return _rect; }
        float32_t width() const { return _rect.cx; }
        float32_t height() const { return _rect.cy; }
        core::vec4f border() const { return calc(_border); }
        core::vec4f margin() const { return calc(_margin); }


        float32_t calc(const core::dimensionf & value) const;
        core::vec2f calc(const core::vec2<core::dimensionf> & value) const;
        core::vec4f calc(const core::vec4<core::dimensionf> & value) const;

       core::rc32f box() const;
       core::rc32f borderBox() const;
       core::rc32f paddingBox() const;
       core::rc32f contentBox() const;
       core::rc32f box(control_box box) const;

        void invalid();

        void setBackgroundColor(core::color32 color);
        core::color32 backgroundColor() const;
        void setBackgroundImage(std::shared_ptr<graphics::Image> image);
        std::shared_ptr<graphics::Image> backgroundImage() const;
        void setMargin(const core::vec4<core::dimensionf> & margin) { _margin = margin; }
        void setBorder(const core::vec4<core::dimensionf> & border) { _border = border; }
        void setPadding(const core::vec4<core::dimensionf> & padding) { _padding = padding; }
        void setBorderColors(const core::vec4<core::color32> & boderColors) { _border_colors = boderColors; }
        void setBorderStyles(const core::vec4<graphics::stroke_style> & boderStyles) { _border_styles = boderStyles; }
        const core::vec4<core::color32> & boderColors() const { return _border_colors; }
        std::array<core::pt32f, 4> boderPoints(core::align edge) const;
        std::array<core::pt32f, 2> boderLine(core::align edge) const;

        virtual void enteringScene(std::shared_ptr<component::Scene> & scene);
        virtual void enterScene(std::shared_ptr<component::Scene> & scene);
        virtual void leavingScene(std::shared_ptr<component::Scene> & scene);
        virtual void leaveScene(std::shared_ptr<component::Scene> & scene);

        // rect �ؼ�Ӧ�ö�λ�ķ�Χ
        // size �ؼ���Ԥ�Ƴߴ�
        virtual void layout() {}
        virtual void layout(const core::rc32f & rect, const core::si32f & size);

        virtual void update();
        virtual void updateContent(std::shared_ptr<component::View> & view) {}
        virtual void onPosChanged(const core::pt32f & from, const core::pt32f & to);
        virtual void onSizeChanged(const core::si32f & from, const core::si32f & to);
        virtual void onRectChanged(const core::rc32f & from, const core::rc32f & to);
    public:
        core::event<void(const core::pt32f & from, const core::pt32f & to)> posChanged;
        core::event<void(const core::si32f & from, const core::si32f & to)> sizeChanged;
        core::event<void(const core::rc32f & from, const core::rc32f & to)> rectChanged;

    protected:
        void _updateBackground(std::shared_ptr<component::View> & view);
        void _updateBorder(std::shared_ptr<component::View> & view);
        void _adjustSizeMinMax(core::si32f & size) const;

    protected:
        std::weak_ptr<component::Scene> _scene;
        std::weak_ptr<Control> _parent;
        std::shared_ptr<component::View> _view;

        layout_origin _layout_origin = layout_origin::layout;

        core::attribute<core::color32> _color;
        // �ؼ���С������ padding�������� margin��
        core::attribute<core::vec2<core::dimensionf>> _pos;
        core::attribute<core::vec2<core::dimensionf>> _size;
        core::attribute<core::vec4<core::dimensionf>> _padding;
        core::attribute<core::vec4<core::dimensionf>> _border;
        core::attribute<core::vec4<core::color32>> _border_colors;
        core::attribute<core::vec4<graphics::stroke_style>> _border_styles;
        core::attribute<core::vec4<core::dimensionf>> _margin;
        core::attribute<core::vec2<core::dimensionf>> _minSize;
        core::attribute<core::vec2<core::dimensionf>> _maxSize;

        core::attribute<graphics::font> _font;

        // background
        core::color32 _background_color = core::colors::Auto;
        std::shared_ptr<graphics::Image> _background_image;
        layout_origin _background_position;
        core::attribute<core::vec2<core::dimensionf>> _background_size;
        core::vec2<renderables::image_fitting> _background_fitting = core::vec2<renderables::image_fitting>(renderables::image_fitting::none, renderables::image_fitting::none);
        control_box _background_box = control_box::border_box;

        // ����֮��
        core::rc32f _rect;

        std::shared_ptr<renderables::Image> _background_imgage_obj;
        std::shared_ptr<renderables::Rectangle> _background_rect_obj;
        std::shared_ptr<renderables::Rectangle> _border_obj;
        std::array<std::shared_ptr<renderables::Line>, 4> _border_objs;
    };
}
