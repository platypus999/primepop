#pragma once
#include <Siv3D.hpp>
#include "GameData.h"
#include "Question.h"

class Achievement : public MyApp::Scene
{
private:
	double screenScroll;//画面のスクロール位置
	const double scrollSpeed = -18.0;
	Array<int>trophyList;//取れた実績のリスト
	int collectY;//素数コレクションのY座標
	const int trophySize = 60;
	Texture trophyTex;
	Array<Question>trophyQuestion;
	Array<int>primeList;
	const int primeCircleRadius = Window::Width() / 8;
public:
	~Achievement()
	{
		Cursor::SetStyle(CursorStyle::Default);
	}
	void init() override
	{
		screenScroll = 0.;
		for (int i = 0; i < m_data->trophy.size(); ++i)
		{
			if (m_data->trophy[i])
			{
				int width = /*FontAsset(L"Trophy")(GameInfo::TrophyHintString[i]).region().w*/ +trophySize + 10;
				trophyQuestion.push_back(Question(Circle(width, trophySize * (trophyList.size() + 1) - 10, 10), GameInfo::TrophyString[i]));
				trophyList.push_back(i);
			}
		}
		collectY = (trophyList.size() + 1) * trophySize;
		trophyTex = Texture(L"Graph\\trophy.png");
	}
	void update() override
	{
		const Transformer2D transformer(Mat3x2::Translate(0, screenScroll), true);
		if (Input::MouseL.clicked)
		{
			changeScene(L"Title");
		}
		screenScroll += scrollSpeed * Mouse::Wheel();
		screenScroll = Min(screenScroll, 0.);
		for (auto& q : trophyQuestion)
		{
			q.update();
		}
	}
	void draw() const override
	{
		const Transformer2D transformer(Mat3x2::Translate(0, screenScroll), true);
		const int ascent = FontAsset(L"Trophy").height;
		for (int i : step(trophyList.size()))
		{
			trophyTex.drawAt({ trophySize / 2,trophySize*i + trophySize / 2 });
			FontAsset(L"Trophy")(GameInfo::TrophyHintString[trophyList[i]]).draw({ trophySize,trophySize*i + trophySize / 2 - ascent / 2 });
		}
		for (auto& q : trophyQuestion)
		{
			q.draw();
		}
		int cnt = 0;
		int one = 0, two = 0, three = 0, other = 0;
		for (int p : m_data->primeSet.getAllPrimes())
		{
			if (p < 10)
			{
				++one;
			}
			else if (p >= 100 && p < 1000)
			{
				++three;
			}
			else if (p >= 1000)
			{
				++other;
			}
			else
			{
				++two;
			}
		}
		String progressOne = Format(L"[一桁：", one, L"/4]");
		String progressTwo = Format(L"[二桁：", two, L"/21]");
		String progressThree = Format(L"[三桁：", three, L"/143]");
		String progressOther = Format(L"[その他：", other, L"]");
		int height = FontAsset(L"Equation").height;
		int collectY = this->collectY;
		if (one)FontAsset(L"Equation")(progressOne).draw({ 10,collectY }, Palette::Skyblue), collectY += height;
		if (two)FontAsset(L"Equation")(progressTwo).draw({ 10,collectY }, Palette::White), collectY += height;
		if (three)FontAsset(L"Equation")(progressThree).draw({ 10,collectY }, Palette::Red), collectY += height;
		if (other)FontAsset(L"Equation")(progressOther).draw({ 10,collectY }, Palette::Lightgrey), collectY += height;
		for (int p : m_data->primeSet.getAllPrimes())
		{
			Color color = Palette::White;
			Color letcol = Palette::Black;
			if (p < 10)
			{
				color = Palette::Skyblue;
			}
			else if (p >= 100 && p < 1000)
			{
				color = Palette::Red;
			}
			else if (p >= 1000)
			{
				color = Palette::Black;
				letcol = Palette::White;
			}
			int xx = cnt % 4;
			int yy = cnt / 4;
			auto circle = Circle(primeCircleRadius * (xx * 2 + 1), primeCircleRadius * (yy * 2 + 1) + collectY, 50);
			circle.draw(color);
			FontAsset(L"Bubble")(ToString(p)).drawAt(circle.center, letcol);
			++cnt;
		}
	}
};