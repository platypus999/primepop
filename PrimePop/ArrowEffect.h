#pragma once
#include <Siv3D.hpp>
//選択画面の矢印の点灯エフェクト
struct ArrowEffect : IEffect
{
	Triangle triangle;
	ArrowEffect(const Triangle&t) :triangle(t) {}
	bool update(double timeSec)
	{
		timeSec *= 2;
		if (timeSec >= 1)return false;
		//透明度をイージング
		const double e = EaseOut<Easing::Quad>(1 - timeSec);
		triangle.draw(Color(Palette::Orange, e * 255));
		return true;
	}
};