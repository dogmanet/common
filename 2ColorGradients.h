#ifndef __2_COLOR_GRADIENTS_H
#define __2_COLOR_GRADIENTS_H

#include <xcommon/util/IX2ColorGradients.h>
#include "ColorGradient.h"

class C2ColorGradients: public IX2ColorGradients
{
public:
	C2ColorGradients() = default;
	C2ColorGradients(const float4_t &vVal):
		m_mode(X2CGM_COLOR),
		m_vColor0(vVal)
	{
	}
	C2ColorGradients(X2COLOR_GRADIENT_MODE mode):
		m_mode(mode)
	{
	}

	XMETHOD_GETSET_IMPL(Mode, X2COLOR_GRADIENT_MODE, mode, m_mode);

	XMETHOD_GETSET_REF_IMPL(Color, float4_t, vValue, m_vColor0);

	XMETHOD_GETSET_REF_IMPL(Color0, float4_t, vValue, m_vColor0);
	XMETHOD_GETSET_REF_IMPL(Color1, float4_t, vValue, m_vColor1);

	XMETHOD_2CONST_IMPL(IXColorGradient*, getGradient0, &m_gradient0);
	XMETHOD_2CONST_IMPL(IXColorGradient*, getGradient1, &m_gradient1);

	float4_t XMETHODCALLTYPE evaluate(float fTime, float fLerpFacton = 1.0f) const override
	{
		fTime = clampf(fTime, 0.0f, 1.0f);
		fLerpFacton = clampf(fLerpFacton, 0.0f, 1.0f);

		switch(m_mode)
		{
		case X2CGM_TWO_COLORS:
			return(vlerp(m_vColor0, m_vColor1, fLerpFacton));

		case X2CGM_GRADIENT:
			return(m_gradient0.evaluate(fTime));

		case X2CGM_TWO_GRADIENTS:
			{
				float4_t v0 = m_gradient0.evaluate(fTime);
				float4_t v1 = m_gradient1.evaluate(fTime);
				return(vlerp(v0, v1, fLerpFacton));
			}

		case X2CGM_COLOR:;
		}

		return(m_vColor0);
	}

	void XMETHODCALLTYPE setFrom(const IX2ColorGradients *pOther) override
	{
		setMode(pOther->getMode());
		setColor0(pOther->getColor0());
		setColor1(pOther->getColor1());
		m_gradient0.setFrom(pOther->getGradient0());
		m_gradient1.setFrom(pOther->getGradient1());
	}

private:
	X2COLOR_GRADIENT_MODE m_mode = X2CGM_COLOR;
	CColorGradient m_gradient0;
	CColorGradient m_gradient1;
	float4_t m_vColor0 = float4_t(1.0f, 1.0f, 1.0f, 1.0f);
	float4_t m_vColor1 = float4_t(1.0f, 1.0f, 1.0f, 1.0f);
};

#endif
