#include "stdafx.h"
#include "TitleBar.h"
#include "ui/Form.h"

namespace ui::controlsex
{
	TitleButton::TitleButton()
	{
		_border = core::vec4<core::dimenf>{ 1_px };
		_padding = { 1_em, 0.5_em };
		_size = { 1.5_em, 1.5_em };
		_fixed_aspect = core::vec2{ 1.0f, 1.0f };
	}

	TitleButton::TitleButton(const std::u8string & text)
	{
		_border = core::vec4<core::dimenf>{ 1_px };
		_padding = { 1_em, 0.5_em };
        _size = { 1.5_em, 1.5_em };
		_fixed_aspect = core::vec2{ 1.0f, 1.0f };
	}

	TitleButton::~TitleButton()
	{

	}

	void TitleButton::propertyTableCallback(core::property_table & properties)
	{
		Control::propertyTableCallback(properties);
		properties[u8"shape-color"] = core::make_accessor(&TitleButton::_setShapeColor, &TitleButton::_shape_color);
	}

	void TitleButton::propertyTable(core::property_table & properties)
	{
		TitleButton::propertyTableCallback(properties);
	}

	void TitleButton::setPath(drawing::path_source & path)
	{
		_path = path;
		refresh();
	}

	std::u8string TitleButton::styleName() const
	{
		if(_actived)
			return u8"titlebutton:active";
		else if(_hovered)
			return u8"titlebutton:hover";
		else
			return u8"titlebutton";
	}

	void TitleButton::onEnterScene()
	{
		_path_object = scene()->createPath();
		ui::base::Button::onEnterScene();
	}
	
	void TitleButton::onLeaveScene()
	{
		_path_object.reset();
		ui::base::Button::onLeaveScene();
	}

	void TitleButton::update()
	{
		_path_object->update(_path);
	}
	
	void TitleButton::paint(drawing::Graphics & graphics, const core::rectf & clip) const
	{
		auto b = box();
		if(_path_object)
			graphics.drawPath(*_path_object, drawing::PathFormat()._stoke(_shape_color, calc(1.5_px)));
	}

	void TitleButton::_setShapeColor(core::color color)
	{
		if(_shape_color != color)
		{
			_shape_color = color;
			repaint();
		}
	}



	//---------------------------------------------------

	void TitleBar::onEnter(std::shared_ptr<ui::Form> & form)
	{
        form->stylesChanged += std::bind(&TitleBar::onFormStylesChanged, this, std::placeholders::_1, std::placeholders::_2);
        form->stateChanged += std::bind(&TitleBar::onFormStateChanged, this, std::placeholders::_1, std::placeholders::_2);
        form->titleChanged += std::bind(&TitleBar::onFormTitleChanged, this, std::placeholders::_1);
        refresh();
        ui::Container::onEnter(form);
	}

	void TitleBar::onLeave()
	{
		ui::Container::onLeave();
		if(auto f = form())
		{
			f->stylesChanged -= std::bind(&TitleBar::onFormStylesChanged, this, std::placeholders::_1, std::placeholders::_2);
			f->stateChanged -= std::bind(&TitleBar::onFormStateChanged, this, std::placeholders::_1, std::placeholders::_2);
            f->titleChanged += std::bind(&TitleBar::onFormTitleChanged, this, std::placeholders::_1);
		}
	}

	void TitleBar::onAction(action_t action)
	{
		if(auto f = form())
			f->onAction(action);
	}

	void TitleBar::update()
	{
		auto f = form();
		if(!f)
			return;

		auto styles = f->styles();

		if(!_close)
		{
			_close = std::make_shared<TitleButton>();
            _close->setSize({ 1.5_em });
            _close->setMargin({ 0.2_em });
			auto b = _close->box();
			core::float3x2 matrix = core::float3x2::scale(b.cx, b.cy) * core::float3x2::translate(b.x, b.y);
			drawing::path_source path;
			path.move({ 0.3f, 0.3f });
			path.line({ 0.7f, 0.7f });
			path.move({ 0.7f, 0.3f });
			path.line({ 0.3f, 0.7f });
			path.transform(matrix);
			
			_close->setPath(path);
			_close->setActionT(ui::system_action::close);
            _close->active += std::bind(&TitleBar::onAction, this, std::placeholders::_1);
			addControl(_close);
		}

		if(!_maximize)
		{
			_maximize = std::make_shared<TitleButton>();
            _maximize->setSize({ 1.5_em });
            _maximize->setMargin({ 0.2_em });
			_maximize->setActionT(ui::system_action::maximize);
            _maximize->active += std::bind(&TitleBar::onAction, this, std::placeholders::_1);
			addControl(_maximize);
		}

		if(!_minimize)
		{
			_minimize = std::make_shared<TitleButton>();
            _minimize->setSize({ 1.5_em });
            _minimize->setMargin({ 0.2_em });
			auto b = _close->box();
			core::float3x2 matrix = core::float3x2::scale(b.cx, b.cy) * core::float3x2::translate(b.x, b.y);
			drawing::path_source path;
			path.move({ 0.3f, 0.7f });
			path.line({ 0.7f, 0.7f });
			path.close();
			path.transform(matrix);
			_minimize->setPath(path);
			_minimize->setActionT(ui::system_action::minimize);
			_minimize->active += std::bind(&TitleBar::onAction, this, std::placeholders::_1);
			addControl(_minimize);
		}

        if (!_title)
        {
            _title = std::make_shared<controls::Lable>();
            _title->setMargin({ 0.2_em });
            _title->setPlaceAlignment(core::align::leftCenterY);
            _title->setText(f->title());
            auto font = _title->font();
            font.style.weight = drawing::font_weight::bold;
            _title->setFont(font);
            addSpacer(100_per);
            addControl(_title);
        }

		{
			auto state = f->formState();
			drawing::path_source path;
			if(state == form_state::maximize)
			{
				path.move({ 0.4f, 0.4f });
				path.line({ 0.4f, 0.3f });
				path.line({ 0.7f, 0.3f });
				path.line({ 0.7f, 0.6f });
				path.line({ 0.6f, 0.6f });

				path.move({ 0.3f, 0.4f });
				path.line({ 0.6f, 0.4f });
				path.line({ 0.6f, 0.7f });
				path.line({ 0.3f, 0.7f });
				path.close();
			}
			else
			{
				path.move({ 0.3f, 0.3f });
				path.line({ 0.7f, 0.3f });
				path.line({ 0.7f, 0.7f });
				path.line({ 0.3f, 0.7f });
				path.close();
			}
			auto b = _close->box();
			core::float3x2 matrix = core::float3x2::scale(b.cx, b.cy) * core::float3x2::translate(b.x, b.y);
			path.transform(matrix);
			_maximize->setPath(path);
		}
		setAviliable(styles.any(ui::form_style::frameless));
		_maximize->setAviliable(!styles.any(ui::form_style::nomax));
		_minimize->setAviliable(!styles.any(ui::form_style::nomin));
        relayout();
	}
}

