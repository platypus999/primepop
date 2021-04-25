#pragma once

#include <Siv3D.hpp>

class OptionSwitch
{
private:
	const bool& isActive;
	bool isOn;
	Point position;
	Image source, hatena;
	Texture graph;
	String displayString;//改行無しでいいよ
	String descriptString;
	String unlockString;
	static Rect strWindow;
	static String showString;
	void updateGraph()
	{
		Image img = source;
		if (!isActive)img = hatena;
		for (auto&& c : img)
		{
			HSV y(isOn ? Palette::Yellow : Palette::White);
			y.v = c.r / 255.0;
			c = Color(y, c.a);
		}
		graph = Texture(img);
	}
public:
	OptionSwitch(const bool& activeBase,Image tex,Image hat, const String&display, Point pos, const String&discript, const String&unlock):
		isActive(activeBase),
		source(tex),
		hatena(hat),
		isOn(false),
		displayString(display),
		descriptString(discript),
		unlockString(unlock),
		position(pos)
	{
		strWindow = Rect(Window::Width() + 10, Window::Height() - 140 - 10, Window::Width() - 20, 140);
		updateGraph();
	}
	//戻り値:マウスがhoverしているか
	bool update()
	{
		bool res = false;
		if (Rect(position, Point(graph.size)).leftClicked)
		{
			if (isActive)isOn = !isOn;
			updateGraph();
		}
		if (Rect(position, Point(graph.size)).mouseOver)
		{
			showString = isActive ? descriptString : unlockString;
			res = true;
		}
		return res;
	}
	void draw()const
	{
		graph.draw(position);//draw?
		FontAsset(L"Descript")(isActive ? displayString : L"？？？").drawAt(250 + Window::Width(), position.y + 25);
	}
	static void resetString()
	{
		showString = GameInfo::defaultString;
	}
	bool isSwitchOn()
	{
		return isOn;
	}
	//下のウィンドウを描画
	static void draw_stat()
	{
		strWindow.draw(Palette::Gray);
		strWindow.drawFrame();
		//FontAsset(L"Descript")(showString).draw(strWindow.tl + Point(5, 5));


		Array<String> texts;

		size_t pos = 0;

		while (pos < showString.length)
		{
			const size_t n = Max<size_t>(FontAsset(L"Descript").drawableCharacters(showString, strWindow.w - 20), 1);

			texts.push_back(showString.substr(pos, n));

			pos += n;
		}

		for (size_t i = 0; i < texts.size(); ++i)
		{
			const int32 y = static_cast<int32>(strWindow.y + 10 + i * FontAsset(L"Descript").height);

			const bool overflow = (i + 1 < texts.size())
				&& (y + FontAsset(L"Descript").height * 2 + 10 > strWindow.y + strWindow.h);

			if (overflow)
			{
				if (texts[i].length > 2)
				{
					texts[i].resize(texts[i].length - (texts[i].length > 2 ? 2 : 1));
				}

				texts[i].append(L"...");
			}

			FontAsset(L"Descript")(texts[i]).draw(strWindow.x + 10, y, Palette::White);

			if (overflow)
			{
				break;
			}
		}
		showString = GameInfo::defaultString;
	}
};

String OptionSwitch::showString = GameInfo::defaultString;
Rect OptionSwitch::strWindow = Rect();