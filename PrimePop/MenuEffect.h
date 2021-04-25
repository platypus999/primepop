#pragma once
#include <Siv3D.hpp>
//�^�C�g����ʂ̃��j���[�I�����̃G�t�F�N�g
struct MenuEffect : IEffect
{
	Rect rect;
	MenuEffect(const Rect&r) :rect(r) {}
	bool update(double timeSec)
	{
		const double e = EaseOut<Easing::Quad>(timeSec);
		RectF(rect).stretched(e * 20).draw(AlphaF((1.0 - e)*0.4));
		return timeSec < 1.0;
	}
};