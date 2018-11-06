#include "stdafx.h"
#include "TitleBar.h"
#include "ui/Form.h"

namespace ui::controlsex
{
	TitleButton::TitleButton()
	{
		_border = core::vec4<core::dimensionf>{ 1_px };
		_padding = { 1_em, 0.5_em };
		_size = { 100_per, 100_per };
		_fixed_aspect = core::vec2{ 1.0f, 1.0f };
	}

	TitleButton::TitleButton(std::string text)
	{
		_border = core::vec4<core::dimensionf>{ 1_px };
		_padding = { 1_em, 0.5_em };
		_size = { 100_per, 100_per };
		_fixed_aspect = core::vec2{ 1.0f, 1.0f };
	}

	TitleButton::~TitleButton()
	{

	}

	void TitleButton::propertyTableCallback(core::property_table & properties)
	{
		Control::propertyTableCallback(properties);
		properties["shape-color"] = core::make_accessor(&TitleButton::_setShapeColor, &TitleButton::_shape_color, core::parseColor, nullptr);
	}

	void TitleButton::propertyTable(core::property_table & properties)
	{
		TitleButton::propertyTableCallback(properties);
	}

	void TitleButton::setPath(std::shared_ptr<drawing::Path> path)
	{
		_path = path;
	}

	std::string TitleButton::styleName() const
	{
		if(_pressed)
			return "titlebutton:active";
		else if(_mousein)
			return "titlebutton:hover";
		else
			return "titlebutton";
	}

	void TitleButton::draw(drawing::Graphics & graphics, const core::rectf & clip) const
	{
		auto b = box();
		if(_path)
		{
			core::float3x2 matrix;
			//matrix *= core::float3x2::translate(-0.5f, -0.5f);
			matrix *= core::float3x2::scale(b.cx, b.cy);
			matrix *= core::float3x2::translate(b.x, b.y);
			drawing::Path path;
			_path->transform(matrix, path);
			//matrix *= core::float3x2::translate(box.x + box.cx * 0.5f, box.y + box.cy * 0.5f);
			//matrix *= core::float3x2::scale(box.cx, box.cy);
			//matrix *= core::float3x2::translate(box.x, box.y);
			graphics.drawPath(path, drawing::PathStyle().stoke(_shape_color, calc(1.5_px)));
		}
	}

	void TitleButton::onMouseClick(const ui::input_state & state, ui::mouse_button button)
	{
		action(_action);
		ui::base::Button::onMouseClick(state, button);
	}

	void TitleButton::_setShapeColor(core::color color)
	{
		if(_shape_color != color)
		{
			_shape_color = color;
			refresh();
		}
	}



	//---------------------------------------------------

	void TitleBar::onEnterScene(std::shared_ptr<ui::Scene> & scene)
	{
		if(auto f = form())
		{
			f->stylesChanged += std::weak_bind(&TitleBar::onFormStylesChanged, share_ref<TitleBar>(), std::placeholders::_1, std::placeholders::_2);
			f->stateChanged += std::weak_bind(&TitleBar::onFormStateChanged, share_ref<TitleBar>(), std::placeholders::_1, std::placeholders::_2);
			_updateButtons();
		}
		ui::Container::onEnterScene(scene);
	}

	void TitleBar::onLeaveScene()
	{
		ui::Container::onLeaveScene();
		if(auto f = form())
		{
			f->stylesChanged -= std::weak_bind(&TitleBar::onFormStylesChanged, share_ref<TitleBar>(), std::placeholders::_1, std::placeholders::_2);
			f->stateChanged -= std::weak_bind(&TitleBar::onFormStateChanged, share_ref<TitleBar>(), std::placeholders::_1, std::placeholders::_2);
		}
	}

	void TitleBar::onAction(ui::title_action action)
	{
		if(auto f = form())
			f->onTitleAction(action);
	}

	void TitleBar::_updateButtons()
	{

		auto f = form();
		if(!f)
			return;

		auto styles = f->styles();

		if(!_close)
		{
			_close = std::make_shared<TitleButton>();
			auto path = std::make_shared<drawing::Path>();
			path->moveTo({ 0.3f, 0.3f });
			path->lineTo({ 0.7f, 0.7f });
			path->moveTo({ 0.7f, 0.3f });
			path->lineTo({ 0.3f, 0.7f });
			_close->setPath(path);
			_close->setTitleAction(ui::title_action::close);
			_close->action += std::weak_bind(&TitleBar::onAction, share_ref<TitleBar>(), std::placeholders::_1);
			addControl(_close);
		}

		if(!_maximize)
		{
			_maximize = std::make_shared<TitleButton>();
			_maximize->setTitleAction(ui::title_action::maximize);
			_maximize->action += std::weak_bind(&TitleBar::onAction, share_ref<TitleBar>(), std::placeholders::_1);
			addControl(_maximize);
		}

		if(!_minimize)
		{
			_minimize = std::make_shared<TitleButton>();
			auto path = std::make_shared<drawing::Path>();
			path->moveTo({ 0.3f, 0.7f });
			path->lineTo({ 0.7f, 0.7f });
			path->close();
			_minimize->setPath(path);
			_minimize->setTitleAction(ui::title_action::minimize);
			_minimize->action += std::weak_bind(&TitleBar::onAction, share_ref<TitleBar>(), std::placeholders::_1);
			addControl(_minimize);
		}

        if (!_title)
        {
            _title = std::make_shared<controls::Text>();
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
			auto path = std::make_shared<drawing::Path>();
			if(state == form_state::maximize)
			{
				path->moveTo({ 0.4f, 0.4f });
				path->lineTo({ 0.4f, 0.3f });
				path->lineTo({ 0.7f, 0.3f });
				path->lineTo({ 0.7f, 0.6f });
				path->lineTo({ 0.6f, 0.6f });

				path->moveTo({ 0.3f, 0.4f });
				path->lineTo({ 0.6f, 0.4f });
				path->lineTo({ 0.6f, 0.7f });
				path->lineTo({ 0.3f, 0.7f });
				path->close();
			}
			else
			{
				path->moveTo({ 0.3f, 0.3f });
				path->lineTo({ 0.7f, 0.3f });
				path->lineTo({ 0.7f, 0.7f });
				path->lineTo({ 0.3f, 0.7f });
				path->close();
			}
			_maximize->setPath(path);
		}
		setAviliable(styles.any(ui::form_style::frameless));
		_maximize->setAviliable(!styles.any(ui::form_style::nomax));
		_minimize->setAviliable(!styles.any(ui::form_style::nomin));
	}
}

