#pragma once


namespace core { namespace math
{
    enum AxisE
    {
        AxisX,
        AxisY,
        AxisZ,
        AxisNegX,
        AxisNegY,
        AxisNegZ,
    };

    /********************************************************************************************************************
    *
    *  ---------------------------------------------------------------------------------------------------------------
    *       			|																	 |
    *     LeftTop(LT) 	|					        Top(ValT)                                   | RightTop(RT)
    *       			|																	 |
    *  ---------------------------------------------------------------------------------------------------------------
    *       			|					|					    |	 					 |
    *       			| LeftTopEdge(LTE) 	|     TopEdge(TE)       |    RightTopEdge(RTE)   |
    *       			|					|						|						 |
    *       			|-------------------|-----------------------|------------------------|
    *       			|					|                       |                        |
    *       			|					|                       |                        |
    *       			|					|                       |                        |
    *    Left(L)		|	LeftEdge(LE)	|      Inner(INNER)     |      RightEdge(RE)     |  Right(R)
    *       			|					|                       |                        |
    *       			|					|                       |                        |
    *       			|					|                       |                        |
    *       			|-------------------|-----------------------|------------------------|
    *       			|					|                       |                        |
    *       			|LeftBottomEdge(LBE)|	  BottomEdge(BE)	| RightBottomEdge(RBE)	 |
    *       			|					|						|						 |
    *  ------------------------------------------------------------------------------------------------------------
    *       			|																	 |
    *   LeftBottom(LB)	|							Bottom(B)								 |  RightBottom(RB)
    *       			|																	 |
    *  ---------------------------------------------------------------------------------------------------------------
    *  @brief 用作击中测试、方向指示等。
    /********************************************************************************************************************/
    // 只占用低 16 位
    class align
    {
    public:
        enum
        {
            none = 0x0000,

            // 外部
            left = 0x0001,
            // 左
            centerX = 0x0002,
            // 居中(X)
            right = 0x0004,
            // 右

            top = 0x0008,
            // 上
            centerY = 0x0010,
            // 居中(ValT)
            bottom = 0x0020,
            // 下

            front = 0x0040,
            // 前
            centerZ = 0x0080,
            // 居中(Z)
            back = 0x0100,
            // 后

            leftTop = left | top,
            rightTop = right | top,
            rightBottom = right | bottom,
            leftBottom = left | bottom,

            leftRight = left | right,
            topBottom = top | bottom,

            topCenterX = top | centerX,
            bottomCenterX = bottom | centerX,

            leftCenterY = left | centerY,
            rightCenterY = right | centerY,

            centerXY = centerX | centerY,

            mask = 0xfff
        };
    };
}}
