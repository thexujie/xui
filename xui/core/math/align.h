#pragma once


namespace math
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
    enum AlignE
    {
        AlignNone = 0x0000,

        // �ⲿ
        AlignLeft = 0x0001,
        // ��
        AlignCenterX = 0x0002,
        // ����(X)
        AlignRight = 0x0004,
        // ��

        AlignTop = 0x0008,
        // ��
        AlignCenterY = 0x0010,
        // ����(ValT)
        AlignBottom = 0x0020,
        // ��

        AlignFront = 0x0040,
        // ǰ
        AlignCenterZ = 0x0080,
        // ����(Z)
        AlignBack = 0x0100,
        // ��

        AlignLeftTop = AlignLeft | AlignTop,
        AlignTopRight = AlignRight | AlignTop,
        AlignRightBottom = AlignRight | AlignBottom,
        AlignBottomLeft = AlignLeft | AlignBottom,

        AlignLeftRight = AlignLeft | AlignRight,
        AlignTopBottom = AlignTop | AlignBottom,

        AlignTopCenterX = AlignTop | AlignCenterX,
        AlignBottomCenterX = AlignBottom | AlignCenterX,

        AlignLeftCenterY = AlignLeft | AlignCenterY,
        AlignRightCenterY = AlignRight | AlignCenterY,

        AlignCenter = AlignCenterX | AlignCenterY,

        AlignLT = AlignLeft | AlignTop,
        // 1100
        AlignTR = AlignTop | AlignRight,
        // 0110
        AlignRB = AlignRight | AlignBottom,
        // 0011
        AlignBL = AlignBottom | AlignLeft,
        // 1001

        AlignLR = AlignLeft | AlignRight,
        // 0101
        AlignTB = AlignTop | AlignBottom,
        // 0101

        AlignTCx = AlignTop | AlignCenterX,
        AlignBCx = AlignBottom | AlignCenterX,

        AlignLCy = AlignLeft | AlignCenterY,
        AlignRCy = AlignRight | AlignCenterY,

        AlignLTR = AlignLeft | AlignTop | AlignRight,
        // 1110
        AlignLTRB = AlignLeft | AlignTop | AlignRight | AlignBottom,
        //1111
        AlignLTB = AlignLeft | AlignTop | AlignBottom,
        // 1101
        AlignLRB = AlignLeft | AlignRight | AlignBottom,
        // 1011

        AlignTRB = AlignTop | AlignRight | AlignBottom,
        // 0111
    };

    inline AlignE operator |(AlignE left, AlignE right)
    {
        return (AlignE)((int32_t)left | (int32_t)right);
    }

    inline AlignE operator &(AlignE left, AlignE right)
    {
        return (AlignE)((int32_t)left & (int32_t)right);
    }

    inline AlignE operator ^(AlignE left, AlignE right)
    {
        return (AlignE)((int32_t)left ^ (int32_t)right);
    }

    inline AlignE operator !(AlignE align)
    {
        return (AlignE)!((int32_t)align);
    }

    inline AlignE operator ~(AlignE align)
    {
        return (AlignE)~((int32_t)align);
    }
}
