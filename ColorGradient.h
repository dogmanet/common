#ifndef __COLOR_GRADIENT_H
#define __COLOR_GRADIENT_H

#include <xcommon/util/IXColorGradient.h>

class CColorGradient final: public IXColorGradient
{
public:
	CColorGradient();

	float4_t XMETHODCALLTYPE evaluate(float fTime) const override;

	UINT XMETHODCALLTYPE addColorKey(float fTime, const float3_t &vValue) override;
	UINT XMETHODCALLTYPE addAlphaKey(float fTime, float fValue) override;

	int XMETHODCALLTYPE moveColorKey(UINT uIdx, float fNewTime) override;
	int XMETHODCALLTYPE moveAlphaKey(UINT uIdx, float fNewTime) override;

	void XMETHODCALLTYPE setColorKey(UINT uIndex, const XColorKey &key) override;
	void XMETHODCALLTYPE setAlphaKey(UINT uIndex, const XAlphaKey &key) override;

	UINT XMETHODCALLTYPE getColorKeyCount() const override;
	UINT XMETHODCALLTYPE getAlphaKeyCount() const override;

	void XMETHODCALLTYPE setColorKeyCount(UINT uKeys) override;
	void XMETHODCALLTYPE setAlphaKeyCount(UINT uKeys) override;

	const XColorKey* XMETHODCALLTYPE getColorKeyAt(UINT uIdx) const override;
	const XAlphaKey* XMETHODCALLTYPE getAlphaKeyAt(UINT uIdx) const override;
			
	void XMETHODCALLTYPE removeColorKey(UINT uIdx) override;
	void XMETHODCALLTYPE removeAlphaKey(UINT uIdx) override;

	void XMETHODCALLTYPE setFrom(const IXColorGradient *pOther) override;

	void XMETHODCALLTYPE sortKeys() override;

private:
	Array<XColorKey> m_aColorKeys;
	Array<XAlphaKey> m_aAlphaKeys;
};

inline CColorGradient::CColorGradient()
{
	addColorKey(0.0f, float3_t(1.0f, 1.0f, 1.0f));
	addColorKey(1.0f, float3_t(1.0f, 1.0f, 1.0f));

	addAlphaKey(0.0f, 1.0f);
	addAlphaKey(1.0f, 1.0f);
}

inline float4_t XMETHODCALLTYPE CColorGradient::evaluate(float fTime) const
{
	float4_t vResult;

	fTime = clampf(fTime, 0.0f, 1.0f);

	if(m_aColorKeys.size() == 0)
	{
		vResult = float4_t(1.0f, 1.0f, 1.0f, 1.0f);
	}
	else if(m_aColorKeys.size() == 1)
	{
		vResult = float4(m_aColorKeys[0].vColor, 1.0f);
	}
	else
	{
		UINT uLength = m_aColorKeys.size();
		int idx = m_aColorKeys.indexOf(fTime, [](const XColorKey &kf, float fTime){
			return(kf.fTime >= fTime);
		});

		if(idx < 0)
		{
			vResult = float4(m_aColorKeys[uLength - 1].vColor, 1.0f);
		}
		else if(idx == 0)
		{
			vResult = float4(m_aColorKeys[0].vColor, 1.0f);
		}
		else
		{
			const XColorKey &keyColor0 = m_aColorKeys[idx - 1];
			const XColorKey &keyColor1 = m_aColorKeys[idx];

			float fT = (fTime - keyColor0.fTime) / (keyColor1.fTime - keyColor0.fTime);
			vResult = vlerp(keyColor0.vColor, keyColor1.vColor, fT);
		}
	}

	if(m_aAlphaKeys.size() == 0)
	{
		vResult.w = 1.0f;
	}
	else if(m_aAlphaKeys.size() == 1)
	{
		vResult.w = m_aAlphaKeys[0].fAlpha;
	}
	else
	{
		UINT uLength = m_aAlphaKeys.size();

		int idx = m_aAlphaKeys.indexOf(fTime, [](const XAlphaKey &kf, float fTime){
			return(kf.fTime >= fTime);
		});

		if(idx < 0)
		{
			vResult.w = m_aAlphaKeys[uLength - 1].fAlpha;
		}
		else if(idx == 0)
		{
			vResult.w = m_aAlphaKeys[0].fAlpha;
		}
		else
		{
			const XAlphaKey &keyAlpha0 = m_aAlphaKeys[idx - 1];
			const XAlphaKey &keyAlpha1 = m_aAlphaKeys[idx];

			float fT = (fTime - keyAlpha0.fTime) / (keyAlpha1.fTime - keyAlpha0.fTime);

			vResult.w = lerpf(keyAlpha0.fAlpha, keyAlpha1.fAlpha, fT);
		}
	}
	return(vResult);
}

inline UINT XMETHODCALLTYPE CColorGradient::addColorKey(float fTime, const float3_t &vValue)
{
	assert(fTime >= 0.0f && fTime <= 1.0f);

	fTime = clampf(fTime, 0.0f, 1.0f);

	XColorKey newKey;
	newKey.fTime = fTime;
	newKey.vColor = vValue;

	return(m_aColorKeys.insert(newKey, [](const XColorKey &a, const XColorKey &b){
		return(a.fTime > b.fTime);
	}));
}
inline UINT XMETHODCALLTYPE CColorGradient::addAlphaKey(float fTime, float fValue)
{
	assert(fTime >= 0.0f && fTime <= 1.0f);

	fTime = clampf(fTime, 0.0f, 1.0f);

	XAlphaKey newKey;
	newKey.fTime = fTime;
	newKey.fAlpha = fValue;

	return(m_aAlphaKeys.insert(newKey, [](const XAlphaKey &a, const XAlphaKey &b){
		return(a.fTime > b.fTime);
	}));
}

inline int XMETHODCALLTYPE CColorGradient::moveColorKey(UINT uIdx, float fNewTime)
{
	assert(m_aColorKeys.size() > uIdx);

	if(m_aColorKeys.size() > uIdx)
	{
		float3_t vColor = m_aColorKeys[uIdx].vColor;
		m_aColorKeys.erase(uIdx);

		return(addColorKey(fNewTime, vColor));
	}

	return(-1);
}
inline int XMETHODCALLTYPE CColorGradient::moveAlphaKey(UINT uIdx, float fNewTime)
{
	assert(m_aAlphaKeys.size() > uIdx);

	if(m_aAlphaKeys.size() > uIdx)
	{
		float fAlpha = m_aAlphaKeys[uIdx].fAlpha;
		m_aAlphaKeys.erase(uIdx);

		return(addAlphaKey(fNewTime, fAlpha));
	}

	return(-1);
}

inline void XMETHODCALLTYPE CColorGradient::setColorKey(UINT uIndex, const XColorKey &key)
{
	assert(m_aColorKeys.size() > uIndex);

	if(m_aColorKeys.size() > uIndex)
	{
		m_aColorKeys[uIndex] = key;
	}
}
inline void XMETHODCALLTYPE CColorGradient::setAlphaKey(UINT uIndex, const XAlphaKey &key)
{
	assert(m_aAlphaKeys.size() > uIndex);

	if(m_aAlphaKeys.size() > uIndex)
	{
		m_aAlphaKeys[uIndex] = key;
	}
}

inline UINT XMETHODCALLTYPE CColorGradient::getColorKeyCount() const
{
	return(m_aColorKeys.size());
}
inline UINT XMETHODCALLTYPE CColorGradient::getAlphaKeyCount() const
{
	return(m_aAlphaKeys.size());
}

inline void XMETHODCALLTYPE CColorGradient::setColorKeyCount(UINT uKeys)
{
	m_aColorKeys.resize(uKeys);
}
inline void XMETHODCALLTYPE CColorGradient::setAlphaKeyCount(UINT uKeys)
{
	m_aAlphaKeys.resize(uKeys);
}

inline const XColorKey* XMETHODCALLTYPE CColorGradient::getColorKeyAt(UINT uIdx) const
{
	assert(m_aColorKeys.size() > uIdx);

	if(m_aColorKeys.size() > uIdx)
	{
		return(&m_aColorKeys[uIdx]);
	}

	return(NULL);
}
inline const XAlphaKey* XMETHODCALLTYPE CColorGradient::getAlphaKeyAt(UINT uIdx) const
{
	assert(m_aAlphaKeys.size() > uIdx);

	if(m_aAlphaKeys.size() > uIdx)
	{
		return(&m_aAlphaKeys[uIdx]);
	}

	return(NULL);
}

inline void XMETHODCALLTYPE CColorGradient::removeColorKey(UINT uIdx)
{
	assert(m_aColorKeys.size() > uIdx);

	if(m_aColorKeys.size() > uIdx)
	{
		m_aColorKeys.erase(uIdx);
	}
}
inline void XMETHODCALLTYPE CColorGradient::removeAlphaKey(UINT uIdx)
{
	assert(m_aAlphaKeys.size() > uIdx);

	if(m_aAlphaKeys.size() > uIdx)
	{
		m_aAlphaKeys.erase(uIdx);
	}
}

inline void XMETHODCALLTYPE CColorGradient::setFrom(const IXColorGradient *pOther)
{
	UINT uKeys = pOther->getColorKeyCount();
	setColorKeyCount(uKeys);

	for(UINT i = 0; i < uKeys; ++i)
	{
		setColorKey(i, *pOther->getColorKeyAt(i));
	}

	uKeys = pOther->getAlphaKeyCount();
	setAlphaKeyCount(uKeys);

	for(UINT i = 0; i < uKeys; ++i)
	{
		setAlphaKey(i, *pOther->getAlphaKeyAt(i));
	}
}

inline void XMETHODCALLTYPE CColorGradient::sortKeys()
{
	m_aColorKeys.quickSort([](const XColorKey &a, const XColorKey &b){
		return(a.fTime < b.fTime);
	});

	m_aAlphaKeys.quickSort([](const XAlphaKey &a, const XAlphaKey &b){
		return(a.fTime < b.fTime);
	});
}

#endif
