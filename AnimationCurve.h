#ifndef __ANIMATION_CURVE_H
#define __ANIMATION_CURVE_H

#include <xcommon/util/IXAnimationCurve.h>

class CAnimationCurve final: public IXAnimationCurve
{
public:
	CAnimationCurve();

	float XMETHODCALLTYPE evaluate(float fTime) const override;

	UINT XMETHODCALLTYPE addKey(float fTime, float fValue) override;

	int XMETHODCALLTYPE moveKey(UINT uIdx, const XKeyframe &key) override;

	void XMETHODCALLTYPE setKey(UINT uIndex, const XKeyframe &key) override;

	const XKeyframe* XMETHODCALLTYPE getKeyAt(UINT uIdx) const override;

	void XMETHODCALLTYPE setKeyCount(UINT uKeys) override;

	UINT XMETHODCALLTYPE getKeyframeCount() const override;

	void XMETHODCALLTYPE smoothTangents(UINT uIdx, float fWeight) override;

	void XMETHODCALLTYPE removeKey(UINT uIdx) override;

	XCURVE_WRAP_MODE XMETHODCALLTYPE getPreWrapMode() const override;
	void XMETHODCALLTYPE setPreWrapMode(XCURVE_WRAP_MODE mode) override;
	XCURVE_WRAP_MODE XMETHODCALLTYPE getPostWrapMode() const override;
	void XMETHODCALLTYPE setPostWrapMode(XCURVE_WRAP_MODE mode) override;

	void XMETHODCALLTYPE setFrom(const IXAnimationCurve *pOther) override;

private:
	float wrapTime(float fTime) const;

	float evaluate(float fTime, const XKeyframe &keyframe, const XKeyframe &nextKeyframe) const;

	void getTWithNewtonMethod(float fTime, const float4 &xCoords, const float4 &curveXCoords, float *t, float *tBottom, float *tTop, float *diff) const;
	void getTWithBisectionMethod(float time, const float4 &curveXCoords, float *t, float *tBottom, float *tTop, float *diff) const;
	float useNewtonMethod(const float4 &curveCoords, float coord, float t, const float3 &curvePrimeCoords, const float4 &primePrimeCoords, float *coordAtT) const;

	float cubicBezier(const float4 &curveMatrix, float t) const;
	float cubicBezier(const float3 &curveMatrix, float t) const;

	float deCasteljauBezier(int degree, const float4 &coords, float t) const;

	void updateTLimits(float x, float time, float t, float *tBottom, float *tTop) const;

private:
	Array<XKeyframe> m_aKeyFrames;
	XCURVE_WRAP_MODE m_preWrapMode = XCWM_DEFAULT;
	XCURVE_WRAP_MODE m_postWrapMode = XCWM_DEFAULT;
};

inline CAnimationCurve::CAnimationCurve()
{
	setKeyCount(2);
	XKeyframe kf0;
	kf0.fOutTangent = 1.0f;
	setKey(0, kf0);

	XKeyframe kf1;
	kf1.fInTangent = 1.0f;
	kf1.fTime = 1.0f;
	kf1.fValue = 1.0f;
	setKey(1, kf1);
}

inline float XMETHODCALLTYPE CAnimationCurve::evaluate(float fTime) const
{
	fTime = wrapTime(fTime);
	UINT uLength = m_aKeyFrames.size();

	int idx = m_aKeyFrames.indexOf(fTime, [](const XKeyframe &kf, float fTime){
		return(kf.fTime >= fTime);
	});

	if(idx < 0)
	{
		idx = uLength - 1;
	}
	else if(idx == 0)
	{
		idx = 1;
	}

	const XKeyframe &keyframe = m_aKeyFrames[idx - 1];
	const XKeyframe &nextKeyframe = m_aKeyFrames[idx];
	return(evaluate(fTime, keyframe, nextKeyframe));
}

inline UINT XMETHODCALLTYPE CAnimationCurve::addKey(float fTime, float fValue)
{
	assert(fTime >= 0.0f && fTime <= 1.0f);

	fTime = clampf(fTime, 0.0f, 1.0f);

	XKeyframe newKF;
	newKF.fTime = fTime;
	newKF.fValue = fValue;

	return(m_aKeyFrames.insert(newKF, [](const XKeyframe &a, const XKeyframe &b){
		return(a.fTime > b.fTime);
	}));
}

inline int XMETHODCALLTYPE CAnimationCurve::moveKey(UINT uIndex, const XKeyframe &key)
{
	assert(m_aKeyFrames.size() > uIndex);

	if(m_aKeyFrames.size() > uIndex)
	{
		m_aKeyFrames.erase(uIndex);

		int idx;// = m_aKeyFrames.indexOf(key, [](const XKeyframe &a, const XKeyframe &b){
			//return(SMIsZero(a.fTime - b.fTime));
		//});

		//if(idx < 0)
		//{
			idx = addKey(key.fTime, 0.0f);
		//}

		setKey(idx, key);

		return(idx);
	}

	return(-1);
}

inline void XMETHODCALLTYPE CAnimationCurve::setKey(UINT uIndex, const XKeyframe &key)
{
	assert(m_aKeyFrames.size() > uIndex);

	if(m_aKeyFrames.size() > uIndex)
	{
		m_aKeyFrames[uIndex] = key;
	}
}

inline const XKeyframe* XMETHODCALLTYPE CAnimationCurve::getKeyAt(UINT uIdx) const
{
	assert(m_aKeyFrames.size() > uIdx);

	if(m_aKeyFrames.size() > uIdx)
	{
		return(&m_aKeyFrames[uIdx]);
	}

	return(NULL);
}

inline void XMETHODCALLTYPE CAnimationCurve::setKeyCount(UINT uKeys)
{
	m_aKeyFrames.resize(uKeys);
}

inline UINT XMETHODCALLTYPE CAnimationCurve::getKeyframeCount() const
{
	return(m_aKeyFrames.size());
}

inline void XMETHODCALLTYPE CAnimationCurve::smoothTangents(UINT uIdx, float fWeight)
{
	if(uIdx < 0 || uIdx > m_aKeyFrames.size() - 1)
	{
		return;
	}

	XKeyframe k0;
	XKeyframe k1;
	XKeyframe k2;

	if(uIdx == 0 || uIdx == m_aKeyFrames.size() - 1)
	{
		if(m_aKeyFrames.size() < 2 || m_aKeyFrames[0].fValue != m_aKeyFrames[m_aKeyFrames.size() - 1].fValue)
		{
			m_aKeyFrames[uIdx].fInTangent = m_aKeyFrames[uIdx].fOutTangent = 0.0f;
			return;
		}

		k0 = m_aKeyFrames[m_aKeyFrames.size() - 2];
		k1 = m_aKeyFrames[uIdx];
		k2 = m_aKeyFrames[1];

		if(uIdx == 0)
		{
			k0.fTime -= m_aKeyFrames[m_aKeyFrames.size() - 1].fTime;
		}
		else
		{
			k2.fTime += m_aKeyFrames[m_aKeyFrames.size() - 1].fTime;
		}
	}
	else
	{
		k0 = m_aKeyFrames[uIdx - 1];
		k1 = m_aKeyFrames[uIdx];
		k2 = m_aKeyFrames[uIdx + 1];
	}

	float weight01 = (1.0f + fWeight) / 2.0f;
	float weight12 = (1.0f - fWeight) / 2.0f;
	float t01 = (k1.fValue - k0.fValue) / (k1.fTime - k0.fTime);
	float t12 = (k2.fValue - k1.fValue) / (k2.fTime - k1.fTime);
	k1.fInTangent = k1.fOutTangent = t01 * weight01 + t12 * weight12;
	m_aKeyFrames[uIdx] = k1;
}

inline void XMETHODCALLTYPE CAnimationCurve::removeKey(UINT uIdx)
{
	assert(m_aKeyFrames.size() > uIdx);

	if(m_aKeyFrames.size() > uIdx)
	{
		m_aKeyFrames.erase(uIdx);
	}
}

inline XCURVE_WRAP_MODE XMETHODCALLTYPE CAnimationCurve::getPreWrapMode() const
{
	return(m_preWrapMode);
}
inline void XMETHODCALLTYPE CAnimationCurve::setPreWrapMode(XCURVE_WRAP_MODE mode)
{
	m_preWrapMode = mode;
}
inline XCURVE_WRAP_MODE XMETHODCALLTYPE CAnimationCurve::getPostWrapMode() const
{
	return(m_postWrapMode);
}
inline void XMETHODCALLTYPE CAnimationCurve::setPostWrapMode(XCURVE_WRAP_MODE mode)
{
	m_postWrapMode = mode;
}

inline float CAnimationCurve::wrapTime(float fTime) const
{
	float fLastKeyTime = m_aKeyFrames[m_aKeyFrames.size() - 1].fTime;
	if(fTime < 0.0f)
	{
		switch(m_preWrapMode)
		{
		case XCWM_DEFAULT:
		case XCWM_CLAMP_FOREVER:
		case XCWM_ONCE:
			fTime = 0.0f;
			break;
		case XCWM_LOOP:
			fTime = fmodf(fTime, fLastKeyTime - m_aKeyFrames[0].fTime);
			break;
		case XCWM_PING_PONG:
			fTime = SMFloatPingPong(fTime, fLastKeyTime - m_aKeyFrames[0].fTime);
			break;
		}
	}
	else if(fTime > fLastKeyTime)
	{
		switch(m_postWrapMode)
		{
		case XCWM_DEFAULT:
		case XCWM_CLAMP_FOREVER:
			fTime = fLastKeyTime;
			break;
		case XCWM_ONCE:
			fTime = 0.0f;
			break;
		case XCWM_LOOP:
			fTime = fmodf(fTime, fLastKeyTime - m_aKeyFrames[0].fTime);
			break;
		case XCWM_PING_PONG:
			fTime = SMFloatPingPong(fTime, fLastKeyTime - m_aKeyFrames[0].fTime);
			break;
		}
	}
	return(fTime);
}

inline float CAnimationCurve::evaluate(float fTime, const XKeyframe &keyframe, const XKeyframe &nextKeyframe) const
{
	if(!std::isfinite(keyframe.fOutTangent) || !std::isfinite(nextKeyframe.fInTangent))
	{
		return(keyframe.fValue);
	}

	float timeDiff = nextKeyframe.fTime - keyframe.fTime;
	float t = (fTime - keyframe.fTime) / timeDiff;
	float outWeight = (keyframe.weightedMode & XKWM_OUT) ? keyframe.fOutWeight : 1.0f / 3.0f;
	float inWeight = (nextKeyframe.weightedMode & XKWM_IN) ? nextKeyframe.fInWeight : 1.0f / 3.0f;
	float tBottom = 0, tTop = 1;
	float diff = FLT_MAX;

	SMMATRIX mCurve(
		1.0f, -3.0f, 3.0f, -1.0f,
		0.0f, 3.0f, -6.0f, 3.0f,
		0.0f, 0.0f, 3.0f, -3.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	float4 xCoords = float4(keyframe.fTime, keyframe.fTime + outWeight * timeDiff, nextKeyframe.fTime - inWeight * timeDiff, nextKeyframe.fTime);
	float4 curveXCoords = mCurve * xCoords;
	getTWithNewtonMethod(fTime, xCoords, curveXCoords, &t, &tBottom, &tTop, &diff);
	getTWithBisectionMethod(fTime, curveXCoords, &t, &tBottom, &tTop, &diff);

	float4 yCoords = float4(keyframe.fValue, keyframe.fValue + outWeight * keyframe.fOutTangent * timeDiff, nextKeyframe.fValue - inWeight * nextKeyframe.fInTangent * timeDiff, nextKeyframe.fValue);
	float4 curveYCoords = mCurve * yCoords;
	return(cubicBezier(curveYCoords, t));
}

inline void CAnimationCurve::getTWithNewtonMethod(float fTime, const float4 &xCoords, const float4 &curveXCoords, float *t, float *tBottom, float *tTop, float *diff) const
{
	const float accuracy = 0.0000001f;
	const int maxIterationCount = 20;
	int iterationCount = 0;

	float4 primeCoords;
	for(int i = 0; i < 3; i++)
	{
		primeCoords[i] = (xCoords[i + 1] - xCoords[i]) * 3;
	}
	float4 primePrimeCoords;
	for(int i = 0; i < 2; i++)
	{
		primePrimeCoords[i] = (primeCoords[i + 1] - primeCoords[i]) * 2;
	}

	SMMATRIX mCurvePrime(
		1.0f, -2.0f, 1.0f, 0.0f,
		0.0f, 2.0f, -2.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	float3 curvePrimeCoords = mCurvePrime * primeCoords;
	while(*diff > accuracy && iterationCount < maxIterationCount)
	{
		iterationCount++;
		float x;
		float newT = useNewtonMethod(curveXCoords, fTime, *t, curvePrimeCoords, primePrimeCoords, &x);
		float newDiff = fabsf(x - fTime);
		if(newT < 0 || newT > 1 || newDiff > *diff)
		{
			break;
		}
		*diff = newDiff;
		updateTLimits(x, fTime, *t, tBottom, tTop);
		*t = newT;
	}
}

inline float CAnimationCurve::useNewtonMethod(const float4 &curveCoords, float coord, float t, const float3 &curvePrimeCoords, const float4 &primePrimeCoords, float *coordAtT) const
{
	*coordAtT = cubicBezier(curveCoords, t);
	float coordPrimeAtT = cubicBezier(curvePrimeCoords, t);
	float coordPrimePrimeAtT = deCasteljauBezier(1, primePrimeCoords, t);
	float coordAtTMinusCoord = *coordAtT - coord;
	float fAtT = coordAtTMinusCoord * coordPrimeAtT;
	float fPrimeAtT = coordAtTMinusCoord * coordPrimePrimeAtT + coordPrimeAtT * coordPrimeAtT;
	return(t - (fAtT / fPrimeAtT));
}

inline float CAnimationCurve::cubicBezier(const float4 &curveMatrix, float t) const
{
	float tt = t * t;
	float4 powerSeries(1.0f, t, tt, tt * t);
	return(SMVector4Dot(powerSeries, curveMatrix));
}

inline float CAnimationCurve::cubicBezier(const float3 &curveMatrix, float t) const
{
	float3 powerSeries(1.0f, t, t * t);
	return(SMVector3Dot(powerSeries, curveMatrix));
}

inline float CAnimationCurve::deCasteljauBezier(int degree, const float4 &_coords, float t) const
{
	float4 coords = _coords;
	float one_t = 1 - t;
	for(int k = 1; k <= degree; k++)
	{
		for(int i = 0; i <= (degree - k); i++)
		{
			coords[i] = one_t * coords[i] + t * coords[i + 1];
		}
	}
	return(coords.x);
}

inline void CAnimationCurve::updateTLimits(float x, float time, float t, float *tBottom, float *tTop) const
{
	if(x > time)
	{
		*tTop = clampf(t, *tBottom, *tTop);
	}
	else
	{
		*tBottom = clampf(t, *tBottom, *tTop);
	}
}

inline void CAnimationCurve::getTWithBisectionMethod(float time, const float4 &curveXCoords, float *t, float *tBottom, float *tTop, float *diff) const
{
	const float accuracy = 0.0000001f;
	const int maxIterationCount = 20;
	int iterationCount = 0;
	while(*diff > accuracy && iterationCount < maxIterationCount)
	{
		iterationCount++;
		*t = (*tTop + *tBottom) * 0.5f;
		float x = cubicBezier(curveXCoords, *t);
		*diff = fabsf(x - time);
		updateTLimits(x, time, *t, tBottom, tTop);
	}
}

inline void XMETHODCALLTYPE CAnimationCurve::setFrom(const IXAnimationCurve *pOther)
{
	setPreWrapMode(pOther->getPreWrapMode());
	setPostWrapMode(pOther->getPostWrapMode());

	UINT uKeys = pOther->getKeyframeCount();
	setKeyCount(uKeys);

	for(UINT i = 0; i < uKeys; ++i)
	{
		setKey(i, *pOther->getKeyAt(i));
	}
}

#endif
