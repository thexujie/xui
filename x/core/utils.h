#pragma once

namespace core
{
    inline constexpr uint16_t make_twocc(uint8_t ch0, uint8_t ch1)
    {
        return ch0 | (ch1 << 8);
    }

    inline constexpr uint32_t make_fourcc(uint8_t ch0, uint8_t ch1, uint8_t ch2, uint8_t ch3)
    {
        return ch0 | (ch1 << 8) | (ch2 << 16) | (ch3 << 24);
    }


    /*!@brief 触发一个调试断点。*/
    void breakpoint();

    /*!@brief 断言，如果失败，会触发一个调试断点。非Debug版该函数不起任何作用。*/
    template<typename ValT>
    void verify(const ValT & val)
    {
#ifdef _DEBUG
        if (val == 0)
            breakpoint();
#endif
    }

    /*!@brief 断言，如果失败，会触发一个调试断点。非Debug版该函数也会起作用。*/
    template<typename ValT>
    void ensure(const ValT & val)
    {
        if (val == 0)
            breakpoint();
    }

    void dbg_output(std::string str);

    /**
    * @} // VenusCore-Base-Fun
    */

    /**
    * @brief 安全地 delete 一个指针，并将指针赋 nullptr。
    删除方式为 delete ptr
    * @param ptr 需要被 delete 的指针的引用。
    */
    template<typename ValT>
    void SafeDelete(ValT *& ptr)
    {
        if (ptr)
        {
            delete ptr;
            ptr = nullptr;
        }
    }

    /**
    *@brief 安全地 delete 一个数组指针，并将指针赋 nullptr。
    * 删除方式为 delete[] ptr
    *@param ptr 需要被 delete 的数组指针的引用。
    */
    template<typename ValT>
    void SafeDeleteArray(ValT *& ptr)
    {
        if (ptr)
        {
            delete[] ptr;
            ptr = nullptr;
        }
    }

    /**
    *@brief 安全地 delete 一个数组里的每一个指针，并将指针赋 nullptr。
    * 删除方式为 delete ptr[n]
    *@param ptr 需要被 delete 的数组指针的引用。
    */
    template<typename ValT>
    void SafeDeleteArray(ValT * ptr[], int32_t iLength)
    {
        if (ptr)
        {
            for (int32_t cnt = 0; cnt < iLength; ++cnt)
                delete ptr[cnt];
        }
    }

    /**
    * @brief 安全地 调用 Release 指针，并将指针赋 nullptr。
    * @param ptr 需要被 Release 的指针的引用。
    */
    template<typename ValT>
    void SafeRelease(ValT *& ptr)
    {
        if (ptr)
        {
            ptr->Release();
            ptr = nullptr;
        }
    }

    template<typename ValT>
    void SafeReleaseP(ValT ** ptr)
    {
        if (ptr && *ptr)
        {
            (*ptr)->Release();
            (*ptr) = nullptr;
        }
    }

    /**
    * @brief 安全地将指针赋 nullptr。
    * @param ptr 需要被赋 nullptr 的指针的引用。
    */
    template<typename ValT>
    void SafeNull(ValT *& ptr)
    {
        ptr = nullptr;
    }

    /**
    * @brief 安全地 调用 Release 一个数组，并将数组中每个指针赋 nullptr。
    * @param ptr 需要被 Release 的指针的引用。
    */
    template<typename ValT>
    void SafeReleaseArray(ValT ** pPtr, int32_t iLength)
    {
        for (int32_t cnt = 0; cnt < iLength; ++cnt)
        {
            if (pPtr[cnt])
            {
                (pPtr[cnt])->Release();
                pPtr[cnt] = nullptr;
            }
        }
    }

    /**
    * @brief 安全地 调用 AddRef。
    * @param ptr 需要被 AddRef 的指针的引用。
    */
    template<typename ValT>
    void SafeAddRef(ValT * const & ptr)
    {
        if (ptr)
        {
            ptr->AddRef();
        }
    }

    /**
    * @brief 不调用 AddRef，部分情况下，防止造成循环引用，
    *        在确信子对象生命周期短与父对象时，不增加父对象
    *        的引用计数
    * @param ptr 目标指针
    */
    template<typename ValT>
    void FakeAddRef(ValT *& ptr) { }

    /**
    * @brief 不调用 Release，仅赋空，部分情况下，防止造成循环引用，
    *        在确信子对象生命周期短与父对象时，不减少父对象
    *        的引用计数
    * @param ptr 目标指针
    */
    template<typename ValT>
    void FakeRelease(ValT *& ptr)
    {
        if (ptr)
            ptr = nullptr;
    }

    template<typename ValT>
    void SafeAddRefPtr(ValT ** ptr)
    {
        if (ptr && *ptr)
        {
            (*ptr)->AddRef();
        }
    }

    /**
    * @brief 求两个数种较小的一个。
    * @param a,b 要比较的两个数的 const 引用。
    * @return 返回较小的一个数的 const 引用。
    */
    template<typename ValT>
    constexpr const ValT & minof(const ValT & a, const ValT & b)
    {
        return a > b ? b : a;
    }

    /**
    * @brief 求若干个数中较小的一个。
    * @param params,v 要比较的数
    * @return 返回较小的一个数的 const 引用。
    */
    template<typename ...ParamsT, typename ValT>
    constexpr const ValT & minof(const ValT & a, const ValT & b, const ParamsT & ...params)
    {
        return minof(minof(a, b), params...);
    }

    /**
    * @brief 求两个数种较大的一个。
    * @param a,b 要比较的两个数的 const 引用。
    * @return 返回较大的一个数的 const 引用。
    */
    template<typename ValT>
    constexpr const ValT & maxof(const ValT & a, const ValT & b)
    {
        return a > b ? a : b;
    }

    /**
    * @brief 求若干个数中较大的一个。
    * @param params,v 要比较的数
    * @return 返回较大的一个数的 const 引用。
    */
    template<typename ...ParamsT, typename ValT>
    constexpr const ValT & maxof(const ValT & a, const ValT & b, const ParamsT & ...params)
    {
        return maxof(maxof(a, b), params...);
    }


    template<typename ValT>
    constexpr const ValT & saturate(const ValT & value, const ValT & min, const ValT & max)
    {
        return value < min ? value : (value > max ? max : value);
    }

    template<typename T>
    constexpr T floor(float32_t val)
    {
        return static_cast<T>(std::floorf(val));
    }

    template<typename T>
    constexpr T ceil(float32_t val)
    {
        return static_cast<T>(std::ceilf(val));
    }

    template<typename T>
    constexpr T floor(float64_t val)
    {
        return static_cast<T>(std::floor(val));
    }

    template<typename T>
    constexpr T ceil(float64_t val)
    {
        return static_cast<T>(std::ceil(val));
    }

    template<typename T>
    constexpr T abs(const T & val)
    {
        return val > 0 ? val : -val;
    }

    constexpr bool equal(float32_t a, float32_t b)
    {
        return abs(a - b) <= std::numeric_limits<float32_t>::epsilon();
    }

    constexpr bool equal(float64_t a, float64_t b)
    {
        return abs(a - b) <= std::numeric_limits<float64_t>::epsilon();
    }

    constexpr bool less_equal(float32_t a, float32_t b)
    {
        return a < b || equal(a, b);
    }

    constexpr bool less_equal(float64_t a, float64_t b)
    {
        return a < b || equal(a, b);
    }

    template<typename T>
    constexpr bool is_zero(T a)
    {
        return -std::numeric_limits<T>::epsilon() <= a && a <= std::numeric_limits<T>::epsilon();
    }

    template<typename T>
    constexpr bool testbit(const T & val, const T & test)
    {
        return (val & test) != static_cast<T>(0);
    }

    template<class T, size_t SIZE>
    constexpr size_t size(const T(&)[SIZE])
    {
        return SIZE;
    }

    template <typename T>
    struct reversion_wrapper { T& iterable; };

    template <typename T>
    auto begin(reversion_wrapper<T> w) { return std::rbegin(w.iterable); }

    template <typename T>
    auto end(reversion_wrapper<T> w) { return std::rend(w.iterable); }

    template <typename T>
    reversion_wrapper<T> reverse(T&& iterable) { return { iterable }; }

#pragma region lowhigh

    /*! @brief 获取 int32_t 类型值的高16位。*/
    inline int16_t i32li16(int32_t val)
    {
        return ((int16_t *)&val)[0];
    }

    /*! @brief 获取 int32_t 类型值的低16位。*/
    inline int16_t i32hi16(int32_t val)
    {
        return ((int16_t *)&val)[1];
    }

    /*! @brief 获取 int32_t 类型值的高16位。*/
    inline uint16_t i32lu16(int32_t val)
    {
        return ((uint16_t *)&val)[0];
    }

    /*! @brief 获取 int32_t 类型值的低16位。*/
    inline uint16_t i32hu16(int32_t val)
    {
        return ((uint16_t *)&val)[1];
    }

    /*! @brief 使用指定的低位和高位生成一个 int32_t 类型的值*/
    inline int32_t i32fromi16(int16_t lval, int16_t hval)
    {
        int32_t val;
        ((int16_t *)&val)[0] = lval;
        ((int16_t *)&val)[1] = hval;
        return val;
    }

    /*! @brief 使用指定的低位和高位生成一个 int32_t 类型的值*/
    inline int32_t i32fromu16(uint16_t lval, uint16_t hval)
    {
        int32_t val;
        ((uint16_t *)&val)[0] = lval;
        ((uint16_t *)&val)[1] = hval;
        return val;
    }

    /*! @brief 获取 uint32_t 类型值的低16位。*/
    inline uint16_t u32lu16(uint32_t val)
    {
        return ((uint16_t *)&val)[0];
    }

    /*! @brief 获取 uint32_t 类型值的高16位。*/
    inline uint16_t u32hu16(uint32_t val)
    {
        return ((uint16_t *)&val)[1];
    }

    /*! @brief 获取 uint32_t 类型值的低16位。*/
    inline int16_t u32li16(uint32_t val)
    {
        return ((int16_t *)&val)[0];
    }

    /*! @brief 获取 uint32_t 类型值的高16位。*/
    inline int16_t u32hi16(uint32_t val)
    {
        return ((int16_t *)&val)[1];
    }

    /*! @brief 使用指定的低位和高位生成一个 uint32_t 类型的值*/
    inline uint32_t u32fromu16(uint16_t lval, uint16_t hval)
    {
        uint32_t val;
        ((uint16_t *)&val)[0] = lval;
        ((uint16_t *)&val)[1] = hval;
        return val;
    }

    /*! @brief 使用指定的低位和高位生成一个 uint32_t 类型的值*/
    inline uint32_t u32fromi16(int16_t lval, int16_t hval)
    {
        uint32_t val;
        ((int16_t *)&val)[0] = lval;
        ((int16_t *)&val)[1] = hval;
        return val;
    }

    /*! @brief 获取 int64_t 类型值的低32位。*/
    inline int32_t i64li32(int64_t val)
    {
        return ((int32_t *)&val)[0];
    }

    /*! @brief 获取 int64_t 类型值的高32位。*/
    inline int32_t i64hi32(int64_t val)
    {
        return ((int32_t *)&val)[1];
    }

    /*! @brief 获取 int64_t 类型值的低32位。*/
    inline uint32_t i64lu32(int64_t val)
    {
        return ((uint32_t *)&val)[0];
    }

    /*! @brief 获取 int64_t 类型值的高32位。*/
    inline uint32_t i64hu32(int64_t val)
    {
        return ((uint32_t *)&val)[1];
    }

    /*! @brief 使用指定的低位和高位生成一个 int64_t 类型的值*/
    inline int64_t i64fromi32(int32_t lval, int32_t hval)
    {
        int64_t val;
        ((int32_t *)&val)[0] = lval;
        ((int32_t *)&val)[1] = hval;
        return val;
    }

    /*! @brief 使用指定的低位和高位生成一个 int64_t 类型的值*/
    inline int64_t i64fromu32(uint32_t lval, uint32_t hval)
    {
        int64_t val;
        ((uint32_t *)&val)[0] = lval;
        ((uint32_t *)&val)[1] = hval;
        return val;
    }

    /*! @brief 获取 uint64_t 类型值的低32位。*/
    inline uint32_t u64lu32(uint64_t val)
    {
        return ((uint32_t *)&val)[0];
    }

    /*! @brief 获取 uint64_t 类型值的高32位。*/
    inline uint32_t u64hu32(uint64_t val)
    {
        return ((uint32_t *)&val)[1];
    }

    /*! @brief 获取 uint64_t 类型值的低32位。*/
    inline int32_t u64li32(uint64_t val)
    {
        return ((int32_t *)&val)[0];
    }

    /*! @brief 获取 uint64_t 类型值的高32位。*/
    inline int32_t u64hi32(uint64_t val)
    {
        return ((int32_t *)&val)[1];
    }

    /*! @brief 使用指定的低位和高位生成一个 int64_t 类型的值*/
    inline uint64_t u64fromu32(uint32_t lval, uint32_t hval)
    {
        uint64_t val;
        ((uint32_t *)&val)[0] = lval;
        ((uint32_t *)&val)[1] = hval;
        return val;
    }

    /*! @brief 使用指定的低位和高位生成一个 int64_t 类型的值*/
    inline uint64_t u64fromi32(int32_t lval, int32_t hval)
    {
        uint64_t val;
        ((int32_t *)&val)[0] = lval;
        ((int32_t *)&val)[1] = hval;
        return val;
    }

    /*! @brief 获取 int32_t 类型值的低16/32位。*/
    inline intx_h ixlih(intx_t iVal)
    {
#ifdef BIT64
        return i64li32(iVal);
#else
        return i32li16(iVal);
#endif
    }

    /*! @brief 获取 int32_t 类型值的高16/32位。*/
    inline intx_h ixhih(intx_t val)
    {
#ifdef BIT64
        return i64hi32(val);
#else
        return i32hi16(val);
#endif
    }

    /*! @brief 获取 intx_t 类型值的低16/32位。*/
    inline uintx_h ixluh(intx_t iVal)
    {
#ifdef BIT64
        return i64lu32(iVal);
#else
        return i32lu16(iVal);
#endif
    }

    /*! @brief 获取 int32_t 类型值的高16/32位。*/
    inline intx_h ixhuh(intx_t val)
    {
#ifdef BIT64
        return i64hu32(val);
#else
        return i32hu16(val);
#endif
    }

    /*! @brief 使用指定的低位和高位生成一个 int32_t 类型的值*/
    inline intx_t ixfromih(intx_h lval, intx_h hval)
    {
        intx_t val;
        ((intx_h *)&val)[0] = lval;
        ((intx_h *)&val)[1] = hval;
        return val;
    }

    /*! @brief 使用指定的低位和高位生成一个 int32_t 类型的值*/
    inline intx_t ixfromuh(uintx_h lval, uintx_h hval)
    {
        intx_t val;
        ((uintx_h *)&val)[0] = lval;
        ((uintx_h *)&val)[1] = hval;
        return val;
    }

    /*! @brief 获取 uint32_t 类型值的低16/32位。*/
    inline uintx_h uxluh(uintx_t val)
    {
#ifdef BIT64
        return u64lu32(val);
#else
        return u32lu16(val);
#endif
    }

    /*! @brief 获取 uint32_t 类型值的高16/32位。*/
    inline uintx_h uxhuh(uintx_t val)
    {
#ifdef BIT64
        return u64hu32(val);
#else
        return u32hu16(val);
#endif
    }

    /*! @brief 获取 uint32_t 类型值的高16/32位。*/
    inline intx_h uxlih(uintx_t val)
    {
#ifdef BIT64
        return u64li32(val);
#else
        return u32li16(val);
#endif
    }

    /*! @brief 获取 uint32_t 类型值的低16/32位。*/
    inline intx_h uxhih(uintx_t val)
    {
#ifdef BIT64
        return u64hi32(val);
#else
        return u32hi16(val);
#endif
    }

    /*! @brief 使用指定的低位和高位生成一个 uint32_t 类型的值*/
    inline uintx_t uxfromuh(uintx_h lval, uintx_h hval)
    {
        uintx_t val;
        ((uintx_h *)&val)[0] = lval;
        ((uintx_h *)&val)[1] = hval;
        return val;
    }

    /*! @brief 使用指定的低位和高位生成一个 uint32_t 类型的值*/
    inline uintx_t uxfromih(intx_h lval, intx_h hval)
    {
        uintx_t val;
        ((intx_h *)&val)[0] = lval;
        ((intx_h *)&val)[1] = hval;
        return val;
    }


#pragma endregion

#pragma region buff
    inline void buffcpy8(void * dst, const void * src, size_t count)
    {
        if (count > 0)
            memcpy_s(dst, count, src, count);
    }

    inline void buffcpy8(void * dst, size_t size, const void * src, size_t count)
    {
        buffcpy8(dst, src, minof(size, count));
    }

    inline void buffcpy16(void * dst, const void * src, size_t count)
    {
        buffcpy8(dst, src, count * 2);
    }

    inline void buffcpy16(void * dst, size_t size, const void * src, size_t count)
    {
        buffcpy8(dst, src, minof(size, count));
    }

    inline void buffcpy32(void * dst, const void * src, size_t count)
    {
        buffcpy8(dst, src, count * 4);
    }

    inline void buffcpy32(void * dst, size_t size, const void * src, size_t count)
    {
        buffcpy32(dst, src, minof(size, count));
    }

    inline void buffmov8(void * dst, const void * src, size_t count)
    {
        if (dst && src && count > 0 && dst != src)
            memmove_s(dst, count, src, count);
    }

    inline void buffmov8(void * dst, size_t size, const void * src, size_t count)
    {
        buffmov8(dst, src, minof(size, count));
    }

    /*!@brief 将 src 中的 size 个 uint8_t 赋值为 val。*/
    inline void buffset8(void * dst, size_t count, uint8_t val)
    {
        if (count > 0)
            memset(dst, val, count);
    }

    /*!@brief 将 src 中的 size 个 uint16_t 赋值为 val。*/
    inline void buffset16(void * dst, size_t count, uint16_t val)
    {
        uint16_t * u16 = (uint16_t *)dst;
        for (size_t cnt = 0; cnt < count; ++cnt)
            u16[cnt] = val;
    }

    /*!@brief 将 src 中的 size 个 uint32_t 赋值为val。*/
    inline void buffset32(void * dst, size_t count, uint32_t val)
    {
        uint32_t * u32 = (uint32_t *)dst;
        for (size_t cnt = 0; cnt < count; ++cnt)
            u32[cnt] = val;
    }

    /*!@brief 将 src 中的 count 个 uint64_t 赋值为val。*/
    inline void buffset64(void * dst, size_t count, uint64_t val)
    {
        uint64_t * u64 = (uint64_t *)dst;
        for (size_t cnt = 0; cnt < count; ++cnt)
            u64[cnt] = val;
    }

    /*!@brief 比较 src 和 dst 中的前 count 个字节。*/
    inline size_t buffcmp8(void * dst, const void * src, size_t count)
    {
        if (count > 0)
            return memcmp(dst, src, count);
        else
            return 0;
    }

    /**!@brief 将 val 按字节重置为0。*/
    template<typename ValT>
    inline void buffclr(ValT & val)
    {
        buffset8(&val, sizeof(ValT), 0);
    }

    template<typename ValT>
    inline void buffset(ValT * dst, size_t size, const ValT & val)
    {
        for (size_t cnt = 0; cnt < size; ++cnt)
            dst[cnt] = val;
    }

    /*!brief 复制 将 src 按字节复制到 dst。*/
    template<typename ValT>
    inline void buffcpy(ValT & dst, const ValT & src)
    {
        buffcpy8(&dst, &src, sizeof(ValT));
    }

    template<typename ValT>
    inline void buffcpy(ValT * dst, const ValT * src, size_t count)
    {
        buffcpy8(dst, src, sizeof(ValT) * count);
    }

    /*!brief 复制 将 count 个 src<ValT> 按字节复制到 dst。*/
    template<typename ValT>
    inline void buffcpy(ValT * dst, size_t size, const ValT * src, size_t count)
    {
        buffcpy(dst, src, minof(size, count));
    }

    /*!@brief 移动 将 src 中的 count 个字节到容量为 count 字节的 count 中。*/
    template<typename ValT>
    inline void buffmov(ValT * dst, const ValT * src, size_t count)
    {
        buffmov8(dst, src, count * sizeof(ValT));
    }

    /**
    * @brief 以二进制的形式，将指定位置、指定长度的数组以移动的方式复制到目标位置。
    * @param pDst 目标位置。
    * @param iSize 目标容量。
    * @param pSrc 源数组。
    * @param iCount 要复制的数据的个数。
    */
    template<typename EleT>
    void buffmov(EleT * dst, int32_t size, const EleT * src, int32_t count)
    {
        buffmov(dst, src, minof(size, count));
    }

#pragma endregion // buff

#pragma region text
    constexpr size_t npos = size_t(-1);
    constexpr uint16_t nposu16 = uint16_t(-1);
    constexpr uint32_t nposu32 = uint32_t(-1);
    constexpr uint64_t nposu64 = uint64_t(-1);

    template<typename T>
    inline constexpr bool npos_v = T(-1);

    template<typename T>
    constexpr bool is_npos(const T & val) { return val == T(-1); }

    template<typename CharT>
    void textempty(CharT * text, size_t size = npos)
    {
        if (text && size)
            text[0] = 0;
    }

    template<typename CharT>
    bool textisempty(CharT * text, size_t size = npos)
    {
        return !size || !text || !text[0];
    }

    /**
    * @brief 计算文本的长度。
    * @param pText 要计算长度的文本。
    * @param iSize 容纳文本的缓冲区大小，即文本最大长度为 iSize - 1。默认为-1。
    *              如果为负数，则计算过程不考虑文本缓冲区大小。。
    * @return 返回计算的文本的长度。
    */
    template<typename CharT>
    size_t textlen(const CharT * text, size_t size = npos)
    {
        if (!text || !size)
            return 0;

        size_t len = 0;
        while (*text++ && --size)
            ++len;
        return len;
    }

    /**
    * @brief 将源字符窜的指定长度复制到目标字符串，并添加终止符(除非目标字符串大小为0)。
    * @param szText 目标字符串。
    * @param iSize 目标字符串的大小，包括终止符，它就是目标字符串的容量。
    * @param szSrc 源字符串。
    * @param iLength 要拷贝的字符的个数，不包括终止符，如果为负数，则拷贝直至终止符为止。
    *                就算 iLength 指定的长度大于 pSrc 的真实长度，那也只会拷贝直至终止符为止。
    * @return 返回拷贝的字符的个数，不包括终止符。
    */
    template<typename CharT>
    size_t textcpy(CharT * dst, size_t size, const CharT * src, size_t length = npos)
    {
        if (!size)
            return 0;
        else if (size == 1)
        {
            *dst = 0;
            return 0;
        }
        else
        {
            if (length == npos)
                length = std::min(textlen(src), size - 1);
            else
            {
                size_t lengh_max = textlen(src, length + 1);
                if (length > lengh_max)
                    length = std::min(lengh_max, size - 1);
                else
                    length = std::min(length, size - 1);
            }


            if (length)
                buffcpy(dst, src, std::min(size, length));
            dst[length] = 0;
            return length;
        }
    }

    inline size_t textcpy(char16_t * dst, size_t size, const char16_t * src, size_t length = npos)
    {
        return textcpy<char16_t>(dst, size, src, length);
    }

    inline size_t textcpy(char8_t * dst, size_t size, const char8_t * src, size_t length = npos)
    {
        return textcpy<char8_t>(dst, size, src, length);
    }


    /// 将文本转换为整数
    template<typename CharT, typename IntT>
    IntT texttoi(const CharT * text, size_t length, int32_t scale = 0)
    {
        if (length < 0)
            length = textlen(text);

        while (length && *text == ' ')
        {
            ++text;
            --length;
        }

        IntT sign = 1;
        if (text[0] == L'-')
        {
            sign = (IntT)-1;
            ++text;
        }

        if (!length || !text)
            return 0;

        const CharT * curr = text;
        if (scale <= 0)
        {
            if (curr[0] == '0' && length >= 2)
            {
                if (curr[1] == 'b' || curr[1] == 'B' || curr[1] == '0')
                {
                    scale = 2;
                    curr += 2;
                }
                else if (curr[1] == 'x' || curr[1] == 'X')
                {
                    scale = 16;
                    curr += 2;
                }
                else if (curr[1] == 'h' || curr[1] == 'H')
                {
                    scale = 8;
                    curr += 2;
                }
                else if (curr[1] == 'd' || curr[1] == 'D')
                {
                    scale = 10;
                    curr += 2;
                }
                else
                    scale = 10;
            }
            else
                scale = 10;
        }

        IntT integer = 0;
        IntT number = 0;
        CharT ch = 0;

        while (length--)
        {
            ch = *curr++;
            if (!ch)
                break;

            if (ch >= '0' && ch <= '9')
                number = ch - '0';
            else if (ch >= 'A' && ch <= 'Z')
                number = ch - 'A' + 10;
            else if (ch >= 'a' && ch <= 'z')
                number = ch - 'a' + 10;
            else
                break;

            if (number >= (IntT)scale)
                break;

            integer = integer * (IntT)scale + number;
        }
        return integer * sign;
    }

    template<typename CharT>
    int32_t texttoix(const CharT * text, size_t length, int32_t scale = 0)
    {
        return texttoi<CharT, int32_t>(text, length, scale);
    }

    template<typename CharT>
    int32_t texttoi32(const CharT * text, size_t length, int32_t scale = 0)
    {
        return texttoi<CharT, int32_t>(text, length, scale);
    }

    template<typename CharT>
    int64_t texttoi64(const CharT * text, size_t length, int32_t scale = 0)
    {
        return texttoi<CharT, int64_t>(text, length, scale);
    }

    /// 将文本转换为整数
    template<typename CharT, typename IntT>
    int32_t texttoiarr(const CharT * text, size_t length, IntT * arr, int32_t count, CharT split = ',', int32_t scale = 0)
    {
        if (!text || !length || !arr || count <= 0)
            return 0;

        if (length < 0)
            length = textlen(text, 64);

        int32_t index = 0;
        const CharT * beg = text;
        const CharT * end = text + length;
        for (int32_t cnt = 0; cnt < count; ++cnt)
        {
            int32_t count = textch<CharT>(beg, end - beg, split);
            if (count < 0)
            {
                if (beg < end)
                    count = end - beg;
                else
                    break;
            }

            arr[index++] = texttoi<CharT, IntT>(beg, count, scale);
            beg += count + 1;
        }
        return index;
    }

    /// 将文本转换为浮点数
    template<typename CharT, typename FloatT>
    FloatT texttof(const CharT * text, size_t length = -1)
    {
        if (length < 0)
            length = textlen<CharT>(text, 32);

        if (!length)
            return (FloatT)0;

        const CharT * end = text + length;
        bool sign_radix = true;
        bool sign_exp = true;
        // reading integer part.
        bool read_integer = true;
        // reading exp part.
        bool read_exp = false;

        // radix = integer + decimal.
        // integer part.
        FloatT integer = 0;
        // decimal part.
        FloatT decimal = 0;
        // exp part.
        int32_t exp = 0;
        // use to caculate decimal.
        FloatT decimal_index = (FloatT)1 / (FloatT)10;

        if (text[0] == '-')
        {
            sign_radix = false;
            ++text;
        }
        else {}

        while (text < end)
        {
            CharT ch = *text++;
            if (ch == '-')
            {
                if (read_exp)
                    sign_exp = false;
                else
                    break;
            }
            else if (ch == '.')
            {
                read_integer = false;
            }
            else if (ch == 'E' || ch == 'e')
            {
                read_integer = false;
                read_exp = true;
            }
            else if ('0' <= ch && ch <= '9')
            {
                int32_t number = (int32_t)(ch - '0');
                if (read_exp)
                {
                    exp *= 10;
                    exp += number;
                }
                else if (read_integer)
                {
                    integer *= 10;
                    integer += number;
                }
                else
                {
                    decimal += number * decimal_index;
                    decimal_index *= (FloatT)0.1;
                }
            }
            else
                break;
        }

        FloatT result = (FloatT)(integer + decimal);
        if (!sign_radix)
            result = -result;

        if (sign_exp)
        {
            for (int32_t cnt = 0; cnt < exp; ++cnt)
                result *= 10;
        }
        else
        {
            for (int32_t cnt = 0; cnt < exp; ++cnt)
                result *= (FloatT)0.1;
        }

        return result;
    }

    template<typename CharT>
    float32_t texttof32(const CharT * text, size_t length = -1)
    {
        return texttof<CharT, float32_t>(text, length);
    }

    template<typename CharT>
    float64_t texttof64(const CharT * text, size_t length = -1)
    {
        return texttof<CharT, float64_t>(text, length);
    }


    /**
    * @brief 计算指定的文本和参数格式化后所需的缓冲区大小(包括终止符)。
    * @param format 格式化字符串。
    * @param ... 参数。
    */
    int32_t textformat_calc(const char8_t * format, ...);

    /**
    * @copydoc textformat_calc(const char8_t * format, ...) 。
    */
    int32_t textformat_calc(const char16_t * format, ...);

    /**
    * @brief 计算指定的文本和参数列表格式化后所需的缓冲区大小(包括终止符)。
    * @param format 格式化字符串。
    * @param args 参数列表。
    */
    int32_t textformatargs_calc(const char8_t * format, void * args);

    /**
    * @copydoc TextFormatCalcArgs(const char8_t * format, void * args) 。
    */
    int32_t textformatargs_calc(const char16_t * format, void * args);

    /**
    * @brief 将格式啊字符串格式化至指定缓冲区内。
    * @param buffer 目标缓冲区。
    * @param size 目标缓冲区的大小(包括终止符)。
    * @param format 格式化字符串。
    * @param ... 参数列表。
    */
    int32_t textformat(char8_t * buffer, int32_t size, const char8_t * format, ...);

    /**
    * @copydoc TextFormatCalcArgs(const char16_t * format, void * args);
    */
    int32_t textformat(char16_t * buffer, int32_t size, const char16_t * format, ...);

    /**
    * @brief 将格式啊字符串格式化至指定缓冲区内。
    * @param buffer 目标缓冲区。
    * @param size 目标缓冲区的大小(包括终止符)。
    * @param format 格式化字符串。
    * @param args 参数列表指针。
    */
    int32_t textformat_args(char8_t * buffer, int32_t size, const char8_t * format, void * args);

    /**
    * @copydoc TextFormatArgs(char8_t * buffer, int32_t size, const char8_t * format, void * args);
    */
    int32_t textformat_args(char16_t * buffer, int32_t size, const char16_t * format, void * args);

    /**
    * @brief 判断字符是否是可显示的。
    * 可显示字符包括标点符号、大小写字母、数字以及大于0xFF的所有字符。
    */
    bool isprintchar(char32_t ch);

    int32_t ansitounicode(const char8_t * src, size_t length, char16_t * dst, int32_t size);
    int32_t unicodetoansi(const char16_t * src, size_t length, char8_t * dst, int32_t size);

#pragma endregion

    inline size_t utf32_to_utf8(const char32_t & ch, char * text, size_t size)
    {
        if (ch <= 0x7F && size > 0)
        {
            text[0] = (char)ch;
            return 1;
        }
        if (ch <= 0x7FF && size > 2)
        {
            text[0] = 0xC0 | (ch >> 6);            /* 110xxxxx */
            text[1] = 0x80 | (ch & 0x3F);          /* 10xxxxxx */
            return 2;
        }
        if (ch <= 0xFFFF && size > 3)
        {
            text[0] = 0xE0 | (ch >> 12);           /* 1110xxxx */
            text[1] = 0x80 | ((ch >> 6) & 0x3F);   /* 10xxxxxx */
            text[2] = 0x80 | (ch & 0x3F);          /* 10xxxxxx */
            return 3;
        }
        if (ch <= 0x10FFFF && size > 4)
        {
            text[0] = 0xF0 | (ch >> 18);           /* 11110xxx */
            text[1] = 0x80 | ((ch >> 12) & 0x3F);  /* 10xxxxxx */
            text[2] = 0x80 | ((ch >> 6) & 0x3F);   /* 10xxxxxx */
            text[3] = 0x80 | (ch & 0x3F);          /* 10xxxxxx */
            return 4;
        }
        return 0;
    }

    inline size_t utf8_to_utf32(const char * text, size_t size, char32_t & ch)
    {
        if (!size)
        {
            ch = 0;
            return 0;
        }

        const uint8_t * start = reinterpret_cast<const uint8_t *>(text);
        const uint8_t * curr = reinterpret_cast<const uint8_t *>(text);
        uint32_t crtl = *start;
        ch = *curr++;

        uint32_t mask = ~0x7F;
        if (crtl & 0x80)
        {
            mask = ~0x3F;
            crtl <<= 1;
            do
            {
                if (curr - start > size)
                {
                    ch = 0;
                    return 0;
                }

                ch <<= 6;
                ch |= ((*curr++) & 0x3F);
                crtl <<= 1;

                mask <<= 5;
            } while (crtl & 0x80);
        }
        ch &= ~mask;
        return curr - start;
    }

    inline size_t utf32_to_utf16(char32_t ch, char16_t dst[2], size_t size)
    {
        if (!size)
            return 0;

        if (ch > 0xFFFF)
        {
            if (size < 2)
                return 0;
            dst[0] = uint16_t(0xD800 | ((ch - 0x10000) >> 10));
            dst[1] = uint16_t(0xD800 | ((ch >> 10) - 64));
            return 2;
        }
        else
        {
            dst[0] = uint16_t(ch);
            return 1;
        }
    }

    inline size_t utf16_to_utf32(const char16_t utf16[], size_t size, char32_t & ch)
    {
        if (!size)
            return 0;

        uint16_t ch1 = utf16[0];
        if (ch1 >= 0xD800 && ch1 <= 0xDFFF)
        {
            if (ch1 < 0xDC00 && size > 1)
            {
                uint16_t w2 = utf16[1];
                if (w2 >= 0xDC00 && w2 <= 0xDFFF)
                {
                    ch = (w2 & 0x03FF) + (((ch1 & 0x03FF) + 0x40) << 10);
                    return 2;
                }
            }
            return 0;
        }
        else
        {
            ch = ch1;
            return 1;
        }
    }
}