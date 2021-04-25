#pragma once
#include <Siv3D.hpp>
//�I����ʂ̖��̓_���G�t�F�N�g
struct ArrowEffect : IEffect
{
	Triangle triangle;
	ArrowEffect(const Triangle&t) :triangle(t) {}
	bool update(double timeSec)
	{
		timeSec *= 2;
		if (timeSec >= 1)return false;
		//�����x���C�[�W���O
		const double e = EaseOut<Easing::Quad>(1 - timeSec);
		triangle.draw(Color(Palette::Orange, e * 255));
		return true;
	}
};