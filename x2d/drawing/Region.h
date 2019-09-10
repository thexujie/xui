#pragma once

#include "drawing.h"

class SkRegion;

namespace drawing
{
	class Region final
	{
	public:
		Region() = default;
		Region(const Region & another);
		Region(Region && another) noexcept : _native(std::move(another._native)) {}
		~Region();

		void clear();
		bool empty() const;

		void setRect(const core::recti & rect);
		void addRect(const core::recti & rect);

		bool intersects(const core::recti & rect) const;
		bool intersects(const Region & region) const;

		bool contains(const core::pointi & point) const;
		bool contains(const core::recti & rect) const;
		bool contains(const Region & region) const;

		core::recti bounds() const;

		Region & operator =(Region && another) noexcept
		{
			_native = std::move(another._native);
			return *this;
		}

		Region & operator =(const Region & another)
		{
			_native = another._native;
			return *this;
		}

	private:
		void _confirmNative();

	public:
		operator bool() const { return !!_native; }
		std::shared_ptr<SkRegion> native_shared() { return _native; }
		SkRegion & native() { return *_native; }
		const SkRegion & native() const { return *_native; }
		SkRegion * native_ptr() { return _native.get(); }
		const SkRegion * native_ptr() const { return _native.get(); }
	private:
		std::shared_ptr<SkRegion> _native;
	};

	class RegionIterator
	{
	public:
		RegionIterator(const Region & region);
		bool done();
		void next();
		core::recti rect() const;

		std::shared_ptr<void> _native;
	};
}
