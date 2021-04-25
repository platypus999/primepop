#pragma once
#include <Siv3D.hpp>

struct EquationEffect : IEffect
{
	String str;
	Color color;
	Vec2 place;
	Font font;
	double limT;
	EquationEffect(String s, Color c, Vec2 p, double lim = 1.0, Font f = Font()) :str(s), color(c), place(p), font(f), limT(lim) {}
	bool update(double t)
	{
		// 1.0 •b‚ÅÁ‚¦‚é
		if (t >= limT)
		{
			return false;
		}
		const double alpha = (limT - t) / limT;
		color.setAlpha(alpha * 255);
		if (font == Font())
		{
			FontAsset(L"Equation")(str).drawCenter(place + Vec2(0, -40 * t), color);
		}
		else
		{
			font(str).drawCenter(place + Vec2(0, -40 * t), color);
		}
		return true;
	}
};