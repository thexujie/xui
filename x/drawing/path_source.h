#pragma once

namespace drawing
{
    //class path_source
    //{
    //public:
    //    virtual ~path_source() {}

    //    virtual void rewind(uint32_t path_id) = 0;
    //    virtual uint32_t vertex(float64_t * x, float64_t * y) = 0;
    //};

    class path_source final
    {
    public:
		path_source() = default;
        ~path_source();

        void clear();
        void close();
        void move(const core::pointf & pos);
        void line(const core::pointf & pos);
        void arc(const core::pointf & pos0, const core::pointf & pos1, float32_t radius);

        void transform(const core::float3x2 & matrix);
        void transform(const core::float3x2 & matrix, path_source & output);

        template<class Iter>
        void fromPoints(Iter iter, const Iter end, bool bclose)
        {
            if (iter == end)
                return;

            int32_t num = 0;
            move(*iter);
            iter++;
            while(iter != end)
            {
                line(*iter);
                iter++;
                ++num;
            }

            if (bclose && num > 2)
                close();
        }

		bool valid() const { return !_buffer.empty(); }
    	
    private:
		enum class command : uint32_t
		{
			none = 0,
			move,
			line,
			arc,
			close,
		};
		std::vector<byte_t> _buffer;

    private:
    	template<typename T>
    	T & _allocate()
    	{
			size_t pos = _buffer.size();
			_buffer.resize(_buffer.size() + sizeof(T));
			byte_t * pointer = _buffer.data() + pos;
			return *(T *)pointer;
    	}
    };
}
