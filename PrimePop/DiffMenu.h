#pragma once
#include <Siv3D.hpp>
#include "GameData.h"
#include "GameInfo.h"
#include "MenuEffect.h"
#include "ArrowEffect.h"
//#include "OnOffSwitch.h"
#include "OptionSwitch.h"
#include "Question.h"

//難易度選択
class DiffMenu : public MyApp::Scene
{
private:
	Array<Rect>leverBox;
	Array<Triangle>leverDecrease, leverIncrease;
	Array<Question>question;
	Rect optionButton;
	String optionString = L"オプション";
	Rect backButton;
	String backString = L"もどる";
	Circle goButton;
	String goString = L"ゲーム開始！";
	Array<int32>choice;
	double screenScroll = 0.;
	int tilt = 0;
	Effect effectArrow, effectMenu;
	Array<OptionSwitch>switches;
	Stopwatch menuItemStopwatch{ true };
	double goParameter;
public:
	~DiffMenu()
	{
		Cursor::SetStyle(CursorStyle::Default);
	}
	void init() override
	{
		leverBox.resize(2);
		leverDecrease.resize(leverBox.size());
		leverIncrease.resize(leverBox.size());
		choice.resize(leverBox.size());
		const int32 leverThickness = 50, leverWidth = 250;
		for (auto i : step(leverBox.size()))
		{
			leverBox[i] = Rect(leverWidth, leverThickness).setCenter(Window::Center().x, 100 + i * 75);
			leverDecrease[i] = Triangle(0, 0, -leverThickness / 2, -leverThickness / 2, 0, -leverThickness);
			leverIncrease[i] = Triangle(0, 0, leverThickness / 2, -leverThickness / 2, 0, -leverThickness);
			leverDecrease[i].setCentroid((Window::Width() - leverWidth) / 2 - 20, leverBox[i].center.y);
			leverIncrease[i].setCentroid(Window::Width() - (Window::Width() - leverWidth) / 2 + 20, leverBox[i].center.y);
			question.emplace_back(Circle(leverBox[i].pos + Point(leverWidth - 10, leverThickness - 10), 10), GameInfo::QuestionString[i]);
		}
		Image moto(L"Graph\\options.png");
		for (auto i : step(GameInfo::SwitchString.size()))
		{
			switches.push_back(OptionSwitch(m_data->optionActive[i], moto.clipped(50 * (i + 1), 0, 50, 50), moto.clipped(0, 0, 50, 50), GameInfo::SwitchString[i], Point(50 + Window::Width(), 120 + 70 * i), GameInfo::DescriptString[i], GameInfo::UnlockString[i]));
		}
		optionButton = Rect(leverWidth, leverThickness).setCenter(Window::Center().x, 300);
		question.emplace_back(Circle(optionButton.center + Point(leverWidth / 2 - 10, leverThickness / 2 - 10), 10), GameInfo::QuestionString[2]);
		backButton = Rect(leverWidth, leverThickness).setCenter(Window::Center().x + Window::Width(), 60);
		const int32 rad = 150;
		goButton = Circle(Window::Width() / 2, Window::Height() - rad - 40, rad);
		goParameter = 0.0;
	}
	void update() override
	{
		//画面を適宜ずらす
		//Graphics2D::PushTransform(Mat3x2::Translate(-EaseInOut<Easing::Quad>(screenScroll) * Window::Width(), 0));
		const Transformer2D transformer(Mat3x2::Translate(-EaseInOut<Easing::Quad>(screenScroll) * Window::Width(), 0), true);
		bool handCursor = false;
		for (auto i : step(leverBox.size()))
		{
			//カーソルの形を更新
			if (choice[i] < GameInfo::SelectNum[i] - 1)handCursor |= leverIncrease[i].mouseOver;
			if (choice[i] > 0)handCursor |= leverDecrease[i].mouseOver;
			//クリックされたらエフェクトを追加する
			if (leverIncrease[i].leftClicked)
			{
				if (choice[i] < GameInfo::SelectNum[i] - 1)effectArrow.add<ArrowEffect>(leverIncrease[i]);
				choice[i]++;
				if (choice[i] >= GameInfo::SelectNum[i])
				{
					choice[i]--;
				}
			}
			if (leverDecrease[i].leftClicked)
			{
				if (choice[i] > 0)effectArrow.add<ArrowEffect>(leverDecrease[i]);
				choice[i]--;
				if (choice[i] < 0)
				{
					choice[i]++;
				}
			}
		}
		handCursor |= optionButton.mouseOver;
		handCursor |= backButton.mouseOver;
		handCursor |= goButton.mouseOver;
		//MenuEffectを追加
		if (optionButton.mouseOver && menuItemStopwatch.elapsed() > 300ms)
		{
			effectMenu.add<MenuEffect>(optionButton);
			menuItemStopwatch.restart();
		}
		if (backButton.mouseOver && menuItemStopwatch.elapsed() > 300ms)
		{
			effectMenu.add<MenuEffect>(backButton);
			menuItemStopwatch.restart();
		}
		//オプションをクリックされたらスクロールを開始する
		if (optionButton.leftClicked)
		{
			tilt = 1;
			OptionSwitch::resetString();
		}
		//もどるをクリックされたら戻る
		if (backButton.leftClicked)
		{
			tilt = -1;
		}
		//tiltに応じて補間する
		screenScroll += tilt * (0.05);
		if (screenScroll < 0)
		{
			screenScroll = 0;
			tilt = 0;
		}
		if (screenScroll > 1)
		{
			screenScroll = 1;
			tilt = 0;
		}
		//switchを更新
		for (auto& sw : switches)
		{
			handCursor |= sw.update();
		}
		if (goButton.leftClicked)
		{
			//ゲーム開始をする
			m_data->difficulity = choice[0];
			m_data->endless = choice[1];
			//オプションデータをGameDataにコピー
			m_data->options.noMiss = switches[0].isSwitchOn();
			m_data->options.noItems = switches[1].isSwitchOn();
			m_data->options.freeFall = switches[2].isSwitchOn();
			m_data->options.hardCore = switches[3].isSwitchOn();
			m_data->options.fiftySeven = switches[4].isSwitchOn();
			m_data->options.hexaDecimal = switches[5].isSwitchOn();
			changeScene(L"Game");
		}
		//?ボタンの更新
		for (auto&q : question)q.update();
		//ボタンのパラメーターを更新
		if (goButton.mouseOver)
		{
			goParameter += 0.05;
		}
		else
		{
			if (goParameter > 1)goParameter = 1;
			goParameter -= 0.05;
			if (goParameter < 0)goParameter = 0;
		}
		//カーソルを変える
		Cursor::SetStyle(handCursor ? CursorStyle::Hand : CursorStyle::Default);

		//Graphics2D::PopTransform();
	}
	void draw() const override
	{
		//Graphics2D::PushTransform(Mat3x2::Translate(-EaseInOut<Easing::Quad>(screenScroll) * Window::Width(), 0));
		const Transformer2D transformer(Mat3x2::Translate(-EaseInOut<Easing::Quad>(screenScroll) * Window::Width(), 0), true);
		//GoEffectを追加
		//if (goButton.mouseOver)
		{
			//goButton.drawShadow({ 0,0 }, 50, 2.0, Palette::Green);
			double s = Min(goParameter, 1.0);
			goButton.stretched(s * 1).drawShadow({ 0, 0 }, 28 * s, 6 * s, Palette::Green);
		}
		Graphics2D::SetBlendState(BlendState::Additive);
		effectMenu.update();
		Graphics2D::SetBlendState(BlendState::Default);
		for (auto i : step(leverBox.size()))
		{
			leverBox[i].draw();
			if (choice[i] < GameInfo::SelectNum[i] - 1)leverIncrease[i].draw();
			if (choice[i] > 0)leverDecrease[i].draw();
		}
		optionButton.draw();
		backButton.draw();
		FontAsset(L"Lever")(optionString).drawAt(optionButton.center, Color(40));
		FontAsset(L"Lever")(backString).drawAt(backButton.center, Color(40));
		FontAsset(L"Lever")(GameInfo::DifficulityLevel[choice[0]]).drawAt(leverBox[0].center, Color(40));
		FontAsset(L"Lever")(GameInfo::ModeSet[choice[1]]).drawAt(leverBox[1].center, Color(40));
		effectArrow.update();
		for (auto& sw : switches)sw.draw();
		OptionSwitch::draw_stat();
		goButton.draw(Palette::Green);
		FontAsset(L"Lever")(goString).drawAt(goButton.center, Palette::Black);
		//?ボタンの更新
		for (auto&q : question)q.draw();
		//Graphics2D::PopTransform();
	}
};
