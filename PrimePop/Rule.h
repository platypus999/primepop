#pragma once
#include <Siv3D.hpp>
#include "GameData.h"
#include "Button.h"
#include <memory>

//ゲーム
class Rule : public MyApp::Scene
{
private:
	int statementNum;
	int index;
	const int firstSplit = Window::Height() * 2 / 5;
	const int secondSplit = Window::Height() * 4 / 5;
	std::unique_ptr<Button> prev, next;
public:
	~Rule()
	{
		Cursor::SetStyle(CursorStyle::Default);
	}
	void init()override
	{
		prev.reset(new Button(Circle(Window::Width() * 1 / 5, (secondSplit + Window::Height()) / 2, 50), L"戻る", Palette::Red));
		next.reset(new Button(Circle(Window::Width() * 4 / 5, (secondSplit + Window::Height()) / 2, 50), L"次へ", Palette::Green));
		index = 0;
		statementNum = GameInfo::RuleString.size();
	}
	void update() override
	{
		bool tmp = false;
		if (index == 0)
		{
			prev->changeColor(Palette::Red);
			prev->changeText(L"戻る");
		}
		else
		{
			prev->changeColor(Palette::Green);
			prev->changeText(L"前へ");
		}
		if (index == statementNum - 1)
		{
			next->changeColor(Palette::Red);
			next->changeText(L"はい！");
		}
		else
		{
			next->changeColor(Palette::Green);
			next->changeText(L"次へ");
		}
		if (next->update(tmp))
		{
			++index;
			if (index == statementNum)
			{
				changeScene(L"Title");
			}
		}
		if (prev->update(tmp))
		{
			--index; 
			if (index == -1)
			{
				changeScene(L"Title");
			}
		}
		index = Clamp(index, 0, statementNum - 1);
	}
	void draw() const override
	{
		Array<String> texts_;

		size_t pos = 0;

		String showString = GameInfo::RuleString[index];

		showString.push_back(L'\n');

		while (showString.length > 0)
		{
			const size_t n = Max<size_t>((Window::Width() - 20) / (FontAsset(L"Descript")(L"　").region().w), 1);

			texts_.push_back(showString.substr(0, Min(n,showString.indexOf(L"\n"))));

			showString.erase(0, Min(Min(n, showString.length), showString.indexOf(L"\n") + 1));

			pos += n;
		}

		int32 y = 10;
		for (size_t i = 0; i < texts_.size(); ++i)
		{
			//const int32 y = static_cast<int32>(0 + 10 + i * FontAsset(L"Descript").height);

			const bool overflow = (i + 1 < texts_.size())
				&& (y + FontAsset(L"Descript").height * 2 + 10 > 0 + firstSplit);

			if (overflow)
			{
				if (texts_[i].length > 2)
				{
					texts_[i].resize(texts_[i].length - (texts_[i].length > 2 ? 2 : 1));
				}

				texts_[i].append(L"...");
			}

			FontAsset(L"Descript")(texts_[i]).draw(0 + 10, y, Palette::White);

			int ans = 0;
			for (int j : step(texts_[i].length))ans += (texts_[i][j] == L'\n');

			//y += (ans + 1) * FontAsset(L"Descript").height;
			y += FontAsset(L"Descript").height;

			if (overflow)
			{
				break;
			}
		}
		prev->draw();
		next->draw();


	}
};