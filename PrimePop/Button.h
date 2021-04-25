#pragma once
#include <Siv3D.hpp>

class Button
{
private:
	Circle circle;
	String string;
	Color color;
	int32 spread;
	const int32 spreadMax = 60;
public:
	Button(Circle c, String s, Color cl) :circle(c), string(s), color(cl) 
	{
		spread = 0;
	}
	bool update(bool& mouse)
	{
		//çLÇ™ÇËÇï\åª
		if (circle.mouseOver)
		{
			spread++;
			mouse = true;
		}
		else
		{
			spread -= 5;
		}
		spread = Min(spread, spreadMax);
		spread = Max(spread, 0);
		return circle.leftClicked;
	}
	void draw()const
	{
		//ï`âÊ
		double offset = spread / (double)spreadMax;
		circle.stretched(offset).drawShadow({ 0,0 }, offset * 28, offset * 6, color);
		circle.draw(color);
		FontAsset(L"Button")(string).drawCenter(circle.center);
	}
	void changeText(String s)
	{
		string = s;
	}
	void changeColor(Color c)
	{
		color = c;
	}
};