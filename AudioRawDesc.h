/*****************************************************
Copyright © DogmaNet Team, 2020
Site: dogmanet.ru
See the license in LICENSE
*****************************************************/

#ifndef __AUDIORAWDESC_H
#define __AUDIORAWDESC_H

#ifdef _MSC_VER
#pragma warning(disable : 4800)
#endif

#include <assert.h>

//! тип аудио семпла
enum AUDIO_SAMPLE_TYPE
{
	AUDIO_SAMPLE_TYPE_INT,
	AUDIO_SAMPLE_TYPE_FLOAT
};

//! формат аудио семпла 
enum AUDIO_SAMPLE_FMT
{
	AUDIO_SAMPLE_FMT_SINT8 = 0x1,
	AUDIO_SAMPLE_FMT_SINT16 = 0x2,
	AUDIO_SAMPLE_FMT_SINT24 = 0x4,
	AUDIO_SAMPLE_FMT_SINT32 = 0x8,

	AUDIO_SAMPLE_FMT_F32 = 0x10,
	AUDIO_SAMPLE_FMT_F64 = 0x20,

	//! ошибочный формат 
	AUDIO_SAMPLE_FMT_NULL = 0,
};

//##########################################################################

/*! описание сырых аудио данных
	создавать новый обьект можно:
	* через конструкторы с передачей параметров (предпочтительно)
	* через инциализацию u8Channels, uSampleRate, (fmtSample или u8BitsPerSample) и вызов calc метода
	@note если перед вызовом calc метода будут инициализированы и fmtSample и u8BitsPerSample, приоритет будет у fmtSample
	@note uBytesPerSec и u8BlockAlign вычисляемые, их инициализация необязательна, метод calc их пересчитает 
*/
struct AudioRawDesc
{
	AudioRawDesc(){}

	AudioRawDesc(uint8_t _u8Channels, uint32_t _uSampleRate, AUDIO_SAMPLE_FMT _fmtSample)
	{
		u8Channels = _u8Channels;
		uSampleRate = _uSampleRate;
		fmtSample = _fmtSample;
		calc();
	}

	AudioRawDesc(uint8_t _u8Channels, uint32_t _uSampleRate, uint8_t _u8BytesPerSample)
	{
		u8Channels = _u8Channels;
		uSampleRate = _uSampleRate;
		u8BytesPerSample = _u8BytesPerSample;
		calc();
	}

	//************************************************************************

	//! количество каналов
	uint8_t u8Channels = 0;

	//! частота дискретизации
	uint32_t uSampleRate = 0;

	//! формат семпла
	AUDIO_SAMPLE_FMT fmtSample = AUDIO_SAMPLE_FMT_SINT16;

	//! количество байт на семпл
	uint8_t u8BytesPerSample = 0;

	//! (ВЫЧИСЛЯЕМО) количество байт в секунду (u8BlockAlign * uSampleRate)
	uint32_t uBytesPerSec = 0;

	//! (ВЫЧИСЛЯЕМО) размер блока данных (u8BytesPerSample * u8Channels)
	uint8_t u8BlockAlign = 0;

	//! размер аудиобуфера в байтах
	uint32_t uSize = 0;

	//************************************************************************

	//! расчет считаемых свойств
	void calc()
	{
		assert(fmtSample != AUDIO_SAMPLE_FMT_NULL || (u8BytesPerSample > 0 && u8BytesPerSample <= 64));

		if(fmtSample != AUDIO_SAMPLE_FMT_NULL)
		{
			switch (fmtSample)
			{
			case AUDIO_SAMPLE_FMT_SINT8:
				u8BytesPerSample = 1;
				break;
			case AUDIO_SAMPLE_FMT_SINT16:
				u8BytesPerSample = 2;
				break;
			case AUDIO_SAMPLE_FMT_SINT24:
				u8BytesPerSample = 3;
				break;
			case AUDIO_SAMPLE_FMT_SINT32:
			case AUDIO_SAMPLE_FMT_F32:
				u8BytesPerSample = 4;
				break;
			case AUDIO_SAMPLE_FMT_F64:
				u8BytesPerSample = 8;
				break;
			default:
				assert(AUDIO_SAMPLE_FMT_NULL);
				break;
			}
		}
		else if(u8BytesPerSample > 0 && u8BytesPerSample <= 64)
		{
			switch (u8BytesPerSample)
			{
			case 1:
				fmtSample = AUDIO_SAMPLE_FMT_SINT8;
				break;
			case 2:
				fmtSample = AUDIO_SAMPLE_FMT_SINT16;
				break;
			case 3:
				fmtSample = AUDIO_SAMPLE_FMT_SINT24;
				break;
			case 4:
				fmtSample = AUDIO_SAMPLE_FMT_SINT32;
				break;
			case 8:
				fmtSample = AUDIO_SAMPLE_FMT_F64;
				break;
			default:
				assert(!bool(u8BytesPerSample));
				break;
			}
		}

		u8BlockAlign = u8BytesPerSample * u8Channels;
		uBytesPerSec = uSampleRate * u8BlockAlign;
	}
};

//##########################################################################

/*! возвращает формат семпла по количеству бит
@param type тип семпла (int/float)
@param iCountBits количество бит в семпле
@return если удалось найти подходящий тип тогда вернет значение отличное от SAMPLE_FMT_NULL, иначе SAMPLE_FMT_NULL
*/
inline AUDIO_SAMPLE_FMT AudioGetSampleFmt(AUDIO_SAMPLE_TYPE type, int iCountBits)
{
	if (type == AUDIO_SAMPLE_TYPE_INT)
	{
		switch (iCountBits)
		{
		case 8:
			return AUDIO_SAMPLE_FMT_SINT8;
		case 16:
			return AUDIO_SAMPLE_FMT_SINT16;
		case 24:
			return AUDIO_SAMPLE_FMT_SINT24;
		case 32:
			return AUDIO_SAMPLE_FMT_SINT32;
		default:
			return AUDIO_SAMPLE_FMT_NULL;
		}
	}
	else if (type == AUDIO_SAMPLE_TYPE_FLOAT)
	{
		switch (iCountBits)
		{
		case 32:
			return AUDIO_SAMPLE_FMT_F32;
		case 64:
			return AUDIO_SAMPLE_FMT_F64;
		default:
			return AUDIO_SAMPLE_FMT_NULL;
		}
	}
	return AUDIO_SAMPLE_FMT_NULL;
}

//! возвращает размер семпла в байтах, на основании формата
inline uint32_t AudioGetFormatBytes(AUDIO_SAMPLE_FMT fmtSample)
{
	switch (fmtSample)
	{
	case AUDIO_SAMPLE_FMT_SINT8:
		return 1;
	case AUDIO_SAMPLE_FMT_SINT16:
		return 2;
	case AUDIO_SAMPLE_FMT_SINT24:
		return 3;
	case AUDIO_SAMPLE_FMT_SINT32:
		return 4;
	case AUDIO_SAMPLE_FMT_F32:
		return 4;
	case AUDIO_SAMPLE_FMT_F64:
		return 8;
	default:
		return 0;
	}
}

#endif
