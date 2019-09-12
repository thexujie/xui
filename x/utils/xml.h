#pragma once

#include "core/core.h"

namespace rapidxml
{
	template<class Ch>
	class xml_node;

	template<class Ch>
	class xml_attribute;

	template<class Ch>
	class xml_document;
}

namespace core
{
	using xml_char = char8_t;
	using xml_string = std::basic_string<xml_char>;
	using xml_string_view = std::basic_string_view<xml_char>;
	
	struct xml_attribute
	{
	private:
#ifdef _DEBUG
		xml_string_view _debug_name;
#endif
	public:
		rapidxml::xml_document<xml_char> * document = nullptr;
		rapidxml::xml_node<xml_char> * node = nullptr;
		rapidxml::xml_attribute<xml_char> * attribute = nullptr;

		xml_attribute() = default;
		xml_attribute(rapidxml::xml_document<xml_char> * document_, rapidxml::xml_node<xml_char> * node_, rapidxml::xml_attribute<xml_char> * attribute_);

		xml_string_view name() const;
		xml_string_view value() const;
	};
	
	struct xml_node
	{
	private:
#ifdef _DEBUG
		xml_string_view _debug_name;
#endif
	public:
		rapidxml::xml_document<xml_char> * document= nullptr;
		rapidxml::xml_node<xml_char> * node = nullptr;

		xml_node() = default;
		xml_node(rapidxml::xml_document<xml_char> * document_, rapidxml::xml_node<xml_char> * node_);

		bool valid() const { return !!node; }
		
		xml_string_view name() const;
		size_t num_attributes() const;
		std::vector<xml_attribute> attributes() const;
		size_t num_children() const;
		std::vector<xml_node> children() const;

		std::generator<xml_attribute> attributes_generator() const;
		std::generator<xml_node> children_generator() const;

		xml_attribute attribute(xml_string_view name) const;
		std::generator<xml_attribute> attributes(xml_string_view name) const;
		
		xml_node child(xml_string_view name) const;
		std::generator<xml_node> children(xml_string_view name) const;

		xml_node find(xml_string_view path) const;

		xml_attribute operator[](xml_string_view name) const { return attribute(name); }
	};
	
	class xml
	{
	public:
		xml() = default;
		xml(std::u8string_view path);
		~xml() = default;

		error load(std::u8string_view path);
		error parse(xml_string_view text);

		xml_node root() const;
		
	private:
		std::shared_ptr<rapidxml::xml_document<xml_char>> _document;
		xml_string _path;
#ifdef _DEBUG
		xml_string _debug_text;
#endif
	};
}
