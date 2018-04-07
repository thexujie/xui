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
    *  @brief �������в��ԡ�����ָʾ�ȡ�
    /********************************************************************************************************************/
    // ֻռ�õ� 16 λ
    class align
    {
    public:
        enum
        {
            none = 0x0000,

            // �ⲿ
            left = 0x0001,
            // ��
            centerX = 0x0002,
            // ����(X)
            right = 0x0004,
            // ��

            top = 0x0008,
            // ��
            centerY = 0x0010,
            // ����(ValT)
            bottom = 0x0020,
            // ��

            front = 0x0040,
            // ǰ
            centerZ = 0x0080,
            // ����(Z)
            back = 0x0100,
            // ��

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
