#ifndef __MIN_MAX_CURVE_H
#define __MIN_MAX_CURVE_H

#include <xcommon/util/IXMinMaxCurve.h>
#include "AnimationCurve.h"

class CMinMaxCurve: public IXMinMaxCurve
{
public:
	CMinMaxCurve() = default;
	CMinMaxCurve(float fVal):
		m_mode(XMCM_CONSTANT),
		m_fMax(fVal)
	{
	}
	CMinMaxCurve(float fMin, float fMax):
		m_mode(XMCM_TWO_CONSTANTS),
		m_fMin(fMin),
		m_fMax(fMax)
	{}
	CMinMaxCurve(XMINMAX_CURVE_MODE mode):
		m_mode(mode)
	{
	}

	XMETHOD_GETSET_IMPL(Mode, XMINMAX_CURVE_MODE, mode, m_mode);

	XMETHOD_GETSET_IMPL(Min, float, fValue, m_fMin);
	XMETHOD_GETSET_IMPL(Max, float, fValue, m_fMax);

	XMETHOD_2CONST_IMPL(IXAnimationCurve*, getMinCurve, &m_curveMin);
	XMETHOD_2CONST_IMPL(IXAnimationCurve*, getMaxCurve, &m_curveMax);

	float XMETHODCALLTYPE evaluate(float fTime, float fLerpFacton = 1.0f) const override
	{
		fTime = clampf(fTime, 0.0f, 1.0f);
		fLerpFacton = clampf(fLerpFacton, 0.0f, 1.0f);

		switch(m_mode)
		{
		case XMCM_TWO_CONSTANTS:
			return(lerpf(m_fMin, m_fMax, fLerpFacton));

		case XMCM_CURVE:
			return(m_curveMax.evaluate(fTime) * (m_fMax - m_fMin) + m_fMin);

		case XMCM_TWO_CURVES:
			return(lerpf(m_curveMin.evaluate(fTime), m_curveMax.evaluate(fTime), fLerpFacton) * (m_fMax - m_fMin) + m_fMin);

		case XMCM_CONSTANT:;
		}

		return(m_fMax);
	}

	void XMETHODCALLTYPE setFrom(const IXMinMaxCurve *pOther) override
	{
		setMode(pOther->getMode());
		setMax(pOther->getMax());
		setMin(pOther->getMin());
		m_curveMin.setFrom(pOther->getMinCurve());
		m_curveMax.setFrom(pOther->getMaxCurve());
	}

private:
	XMINMAX_CURVE_MODE m_mode = XMCM_CONSTANT;
	CAnimationCurve m_curveMin;
	CAnimationCurve m_curveMax;
	float m_fMin = 0.0f;
	float m_fMax = 1.0f;
};

#endif
