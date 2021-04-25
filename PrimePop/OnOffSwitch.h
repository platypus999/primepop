#pragma once
#if 0
#include <Siv3D.hpp>

class OnOffSwitch
{
private:
	const bool& isActive;
	bool isOn;
	double xNotchPercentage;
	String displayString;
	int32 yPosition;
	Rect rectOn, rectOff;
	const Point smallSize = Point(50, 15);
	static int32 switchLeft()
	{
		return 75 + Window::Width();
	}
	String fakeString()const
	{
		return isActive ? displayString : L"ÅHÅHÅH";
	}
	String descriptString;
	String unlockString;
	static Rect strWindow;
	static String showString;
public:
	OnOffSwitch(const bool& activeBase, const String& display, int32 yPos, const String& discript, const String& unlock) :
		isActive(activeBase),
		isOn(false),
		displayString(display),
		descriptString(discript),
		unlockString(unlock),
		yPosition(yPos),
		xNotchPercentage(0.0)
	{
		rectOn = Rect(smallSize);
		rectOn.setCenter(switchLeft(), yPosition);
		rectOff = Rect(smallSize);
		rectOff.setCenter(switchLeft() + smallSize.x, yPosition); 
		strWindow = Rect(Window::Width() + 10, Window::Height() - 140 - 10, Window::Width() - 20, 140);
	}
	bool update()
	{
		bool res = false;
		if (rectOn.leftClicked || rectOff.leftClicked)
		{
			if (isActive)isOn = !isOn;
		}
		if (rectOn.mouseOver || rectOff.mouseOver)
		{
			showString = isActive ? descriptString : unlockString;
			res = true;
		}
		xNotchPercentage += 0.1 * ((int32)isOn * 2 - 1);
		if (xNotchPercentage < 0)xNotchPercentage = 0;
		if (xNotchPercentage > 1)xNotchPercentage = 1;
		return res;
	}
	void draw()const
	{
		int32 xNotch = EaseInOut<Easing::Quad>(xNotchPercentage) * smallSize.x;
		if (isActive)
		{
			rectOn.draw(Palette::Green);
			rectOff.draw(Palette::Red);
		}
		else
		{
			rectOn.draw(Palette::Gray);
			rectOff.draw(Palette::Gray);
		}
		FontAsset(L"Switch")(L"On").drawAt(switchLeft(), yPosition, Color(40));
		FontAsset(L"Switch")(L"Off").drawAt(switchLeft() + smallSize.x, yPosition, Color(40));
		Rect(smallSize).setCenter(switchLeft() + xNotch, yPosition).draw(Palette::Silver);
		FontAsset(L"Lever")(fakeString()).drawAt(Window::Width() * 7 / 4, yPosition, Color(Palette::White));
	}
	static void draw_stat()
	{
		strWindow.draw(Palette::Gray);
		strWindow.drawFrame();
		FontAsset(L"Descript")(showString).draw(strWindow.tl + Point(5, 5));
	}
	static void resetString()
	{
		showString = GameInfo::defaultString;
	}
	bool isSwitchOn()
	{
		return isOn;
	}
};

//int32 OnOffSwitch::switchLeft = 75 + Window::Width();
String OnOffSwitch::showString = GameInfo::defaultString;
//Rect OnOffSwitch::strWindow = Rect(OnOffSwitch::switchLeft(), Window::Height() - 200 - 20, Window::Width() * 2 - switchLeft(), 200);
Rect OnOffSwitch::strWindow = Rect();
#endif