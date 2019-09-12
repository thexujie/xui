#include "stdafx.h"
#include "xml.h"

#include "../deps/rapidxml/rapidxml.hpp"

namespace core
{
	xml_attribute::xml_attribute(rapidxml::xml_document<xml_char> * document_, rapidxml::xml_node<xml_char> * node_, rapidxml::xml_attribute<xml_char> * attribute_) : document(document_), node(node_), attribute(attribute_)
	{
#ifdef _DEBUG
		if (attribute_)
			_debug_name = xml_string_view(attribute_->name(), attribute_->name_size());
#endif
	}
	
	xml_string_view xml_attribute::name() const
	{
		if (!attribute)
			return {};
		return xml_string_view(attribute->name(), attribute->name_size());
	}

	xml_string_view xml_attribute::value() const
	{
		if (!attribute)
			return {};
		return xml_string_view(attribute->value(), attribute->value_size());
	}

	xml_node::xml_node(rapidxml::xml_document<xml_char> * document_, rapidxml::xml_node<xml_char> * node_) : document(document_), node(node_)
	{
#ifdef _DEBUG
		if (node_)
			_debug_name = std::basic_string_view<xml_char>(node_->name(), node_->name_size());
#endif
	}
	xml_string_view xml_node::name() const
	{
		if (!node)
			return {};
		return xml_string_view(node->name(), node->name_size());
	}

	size_t xml_node::num_attributes() const
	{
		size_t count = 0;
		auto attr = node->first_attribute();
		while (attr)
		{
			++count;
			attr = attr->next_attribute();
		}
		return count;
	}

	std::vector<xml_attribute> xml_node::attributes() const
	{
		if (!node || !node->first_attribute())
			return {};

		std::vector<xml_attribute> attributes;
		for(auto attribute = node->first_attribute(); attribute != nullptr; attribute = attribute->next_attribute())
			attributes.push_back(xml_attribute(document, node, attribute));
		return attributes;
	}

	size_t xml_node::num_children() const
	{
		size_t count = 0;
		auto child = node->first_node();
		while (child)
		{
			++count;
			child = child->next_sibling();
		}
		return count;
	}
	
	std::vector<xml_node> xml_node::children() const
	{
		if (!node || !node->first_node())
			return {};

		std::vector<xml_node> nodes;
		for (auto child = node->first_node(); child != nullptr; child = child->next_sibling())
			nodes.push_back(xml_node(document, child));
		return nodes;
	}

	std::generator<xml_attribute> xml_node::attributes_generator() const
	{
		for (auto attribute = node->first_attribute(); attribute != nullptr; attribute = attribute->next_attribute())
			co_yield xml_attribute(document, node, attribute);
	}
	
	std::generator<xml_node> xml_node::children_generator() const
	{
		for (auto child = node->first_node(); child != nullptr; child = child->next_sibling())
			co_yield xml_node(document, child);
	}

	xml_attribute xml_node::attribute(xml_string_view name) const
	{
		if (!node)
			return {};
		
		auto attribute = node->first_attribute(name.data(), name.length());
		return attribute ? xml_attribute(document, node, attribute) : xml_attribute();
	}

	std::generator<xml_attribute> xml_node::attributes(xml_string_view name) const
	{
		for (auto attribute = node->first_attribute(name.data(), name.length()); attribute != nullptr; attribute = attribute->next_attribute(name.data(), name.length()))
			co_yield xml_attribute(document, node, attribute);
	}
	
	xml_node xml_node::child(xml_string_view name) const
	{
		auto child = node->first_node(name.data(), name.length());
		return child ? xml_node(document, child) : xml_node();
	}
	
	std::generator<xml_node> xml_node::children(xml_string_view name) const
	{
		for (auto child = node->first_node(name.data(), name.length()); child != nullptr; child = child->next_sibling(name.data(), name.length()))
			co_yield xml_node(document, child);
	}

	xml_node xml_node::find(xml_string_view path) const
	{
		std::vector<xml_string_view> paths = core::split<char8_t>(path, u8"\\/");
		xml_node child = *this;
		size_t depth = 0;
		while(child.valid() && depth < paths.size())
			child = child.child(paths[depth++]);
		return child;
	}

	xml::xml(std::u8string_view path)
	{
		load(path);
	}

	error xml::load(std::u8string_view path)
	{
		//xml_string text = core::file::read_full_string(path);
		xml_string text = core::file::read_full_u8string(path);
		
		if (!_document)
			_document = std::make_shared<rapidxml::xml_document<xml_char>>();
		_document->parse<0>(text.data());
#ifdef _DEBUG
		_debug_text = std::move(text);
#endif
		return ok;
	}

	error xml::parse(xml_string_view text)
	{
		xml_string text_(text);
		if (!_document)
			_document = std::make_shared<rapidxml::xml_document<xml_char>>();
		_document->parse<0>(text_.data());
#ifdef _DEBUG
		_debug_text = std::move(text_);
#endif
		return ok;
	}

	xml_node xml::root() const
	{
		if (!_document)
			return {};

		auto root = _document->first_node();
		if (!root)
			return {};
		
		return xml_node(_document.get(), root);
	}
}
