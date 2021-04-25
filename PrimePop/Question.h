#pragma once
#include <Siv3D.hpp>

class Question
{
private:
	Circle circle;
	bool isHover;
	String str;
public:
	Question(Circle c, String s)
		:circle(c), str(s), isHover(false)
	{}
	void update()
	{
		isHover = circle.mouseOver;
	}
	void draw()const
	{
		circle.draw(Palette::Gray);
		FontAsset(L"Switch")(L"?").drawCenter(circle.center);
		if (isHover)
		{
			Point p = Mouse::Pos();
			Rect r = FontAsset(L"DescriptSmall")(str).region(p);
			if (r.x + r.w > Window::Width())
			{
				r.x = Window::Width() - r.w;
			}
			r.draw(Palette::Dimgray);
			r.drawFrame(1.0, 0.0, Palette::Black);
			FontAsset(L"DescriptSmall")(str).draw(r.pos,Palette::White);
		}
	}
};