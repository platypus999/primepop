#pragma once
#include <Siv3D.hpp>
#include "GameData.h"
#include "Bubble.h"
#include "ShakeEffect.h"
#include "LoadTextureRegions.h"

//ゲーム
class Game : public MyApp::Scene
{
private:
	const int32 lowerSize = 96;
	const int32 gaugeSize = 24;
	const int32 scoreCenter = 75;
	const int32 timeCenter = Window::Width() / 2;
	const int32 itemCenter = Window::Width() - scoreCenter;
	Rect lowerBackground;
	Rect timerBackground;
	Line inspectLine;
	const double squashValue = 0.5;
	const String scoreLabel = L"スコア";
	const String timerLabel = L"時間";
	const String itemsLabel = L"アイテム";
	Array<Bubble>bubbles;
	int32 numberMax;
	bool oneLife;
	bool gameOver;
	bool itemsActive;
	Array<TextureRegion>itemGraph;
	Texture mouseRight;
	Texture ballon;
	Font scoreFont;

	double yLimit;
	Milliseconds frequency;
	const Milliseconds preFrequency = 10000ms;
	const Milliseconds maxFrequency = 3200ms;
	bool isDouble;//次生成される球はダブルか
	int frequentCount;//周期の回数

	Effect shakeEffect;
	Effect boardEffect;
	Effect scoreEffect;

	int32 score; int32 scoreAdd;
	double healthGauge, itemGauge;//どちらも最大が100
	double healthAdd;
	enum ItemType { INCREMENT, SPLIT, CHECK, ITEMNUM }nowItem;

	Stopwatch onYourMark{ true };
	Stopwatch timer{ false };
	Stopwatch pulse{ false };
	Stopwatch tenSecondPulse{ false };
	Stopwatch endStopwatch{ false };
	bool handCursor;

	double lineHeight;
public:
	~Game()
	{
		Cursor::SetStyle(CursorStyle::Default);
	}
	void init() override
	{
		lowerBackground = Rect(0, Window::Height() - lowerSize, Window::Width(), lowerSize);
		timerBackground = Rect(Window::Width() - scoreCenter * 4, lowerSize).setCenter(timeCenter, Window::Height() - lowerSize / 2);
		lineHeight = Window::Height() - lowerSize - gaugeSize * 1.5;
		inspectLine = Line(0, Window::Height() - lowerSize - gaugeSize * 1.5, Window::Width(), lineHeight);
		isDouble = false;
		frequentCount = 0;

		nowItem = INCREMENT;
		healthGauge = 100.;
		itemGauge = 0.;

		oneLife = !m_data->endless;
		if (m_data->options.noMiss)
		{
			oneLife = true;
			healthGauge = 1;
		}
		gameOver = false;
		if (!m_data->endless)
		{
			frequentCount = 4;
		}
		itemsActive = m_data->endless;
		if (m_data->options.noItems)itemsActive = false;
		switch (m_data->difficulity)
		{
		case 0:numberMax = 20; break;
		case 1:numberMax = 100; break;
		case 2:numberMax = 400; break;
		case 3:numberMax = 1000; break;
		}
		if (m_data->options.hardCore)
		{
			numberMax *= 10;
		}
		frequency = preFrequency;
		yLimit = lineHeight / (60 * DurationCast<Seconds>(frequency).count());//lineHeight * DurationCast<Seconds>(frequency).count() / 2;
		scoreFont = Font(GameInfo::ScoreFontSize, L"ＭＳ Ｐゴシック");

		itemGraph = LoadTextureRegions(L"Graph/items.png", { 50,50 });

		mouseRight = Texture(L"Graph/right.png");
		ballon = Texture(L"Graph/ballon.png");

	}
	void update() override
	{
		//カーソルの変数初期化
		handCursor = false;

		//5秒たったらスタート
		if (!timer.isActive() && onYourMark.elapsed() > 5000ms)
		{
			timer.start();
			pulse.start();
			tenSecondPulse.start();
			pulse.set(frequency * (isDouble ? 2 : 1) * (frequentCount < 8 ? 2 : 1));
		}

		//10秒ごと
		if (tenSecondPulse.elapsed() >= 10000ms)
		{
			frequency -= (preFrequency - maxFrequency) / 20;//(850ms);//最大効果速度を上げる
			if (frequency < maxFrequency)
			{
				frequency = maxFrequency;
			}
			yLimit = lineHeight / (60.0 * DurationCast<Milliseconds>(frequency).count() / 1000.0);//lineHeight * DurationCast<Seconds>(frequency).count() / 2;
			tenSecondPulse.restart();
		}

		//scoreにscoreAddを足しつける
		int32 clause = scoreAdd - (int)(scoreAdd * 0.9);
		score += clause;
		scoreAdd -= clause;
		//カンストも一応
		score = Min(score, 999999);

		//ゲージ類も同じことをする
		{
			healthGauge += healthAdd * 0.1;
			healthAdd *= 0.9;
		}
		//HPが0になったら終了
		if (healthGauge + healthAdd <= 0)gameOver = true;

		//周期でバブルを発生させる
		if (!endStopwatch.isActive())
		{
			const int32 rad = 50;
			if (pulse.elapsed() > frequency * (isDouble ? 2 : 1) / 2)
			{
				isDouble = (Random(3) == 0);
				if (frequentCount < 4)isDouble = false;//4回までは無し
				for (int i = 0; i < (isDouble ? 2 : 1); ++i)
				{
					int32 xCoord = Random(Window::Width() - rad * 2) + rad;//戻して！！
					bubbles.push_back(Bubble(Random() * (numberMax - 2) + 2, Point(xCoord, -rad), rad, m_data->options.hexaDecimal));
				}
				++frequentCount;
				pulse.restart();
			}
		}

		//bubblesの当たり判定確認
		for (int32 i : step(bubbles.size()))
		{
			for (int32 j : step(bubbles.size()))
			{
				if (i == j)break;
				auto &circle1 = bubbles[i];
				auto &circle2 = bubbles[j];
				Vec2 vec = circle2.getCoordinate() - circle1.getCoordinate();
				double rad = circle1.getRadius() + circle2.getRadius();
				if (vec.dot(vec) <= rad * rad)
				{
					//x方向
					double len = vec.length();
					//重なっている場合は特殊処理
					if (len == 0)
					{
						circle1.pushHorizontal(+squashValue);
						circle2.pushHorizontal(-squashValue);
						continue;
					}
					if (circle1.getCoordinate().x <= circle2.getCoordinate().x)
					{
						circle1.pushHorizontal(-squashValue * 1/*(vec.x / len)*/);
						circle2.pushHorizontal(+squashValue * 1/*(vec.x / len)*/);
					}
					else
					{
						circle2.pushHorizontal(-squashValue * 1/*(vec.x / len)*/);
						circle1.pushHorizontal(+squashValue * 1/*(vec.x / len)*/);
					}
					//y方向
					if (circle1.getCoordinate().y <= circle2.getCoordinate().y)
					{
						circle1.pushVertical(-squashValue * (vec.y / len));
						circle2.pushVertical(+squashValue * (vec.y / len));
					}
					else
					{
						circle1.pushVertical(+squashValue * (vec.y / len));
						circle2.pushVertical(-squashValue * (vec.y / len));
					}
				}
			}
		}

		//アイテムゲージを時間で増やす
		if (timer.isActive())if (itemsActive)itemGauge += 4 / 60.;

		//ゲージ上限
		if (healthGauge > 100.)healthGauge = 100.;
		if (itemGauge > 100.)itemGauge = 100.;
		if (oneLife && m_data->options.noMiss)healthGauge = Min(healthGauge, 1.);

		//もし100秒モードだったら
		if (!m_data->endless)
		{
			if (timer.elapsed() >= GameInfo::timeLimit)
			{
				//ゲーム終了
				for (auto& b : bubbles)
				{
					b.turnYellow();
					endStopwatch.restart();
					m_data->lastResult.equation = L"クリア！";
				}
				timer.pause();
				timer.set(GameInfo::timeLimit);
			}
		}

		if (endStopwatch.elapsed() >= 2000ms)
		{
			m_data->lastResult.score = (score + scoreAdd);
			m_data->lastResult.time = timer.elapsed();
			changeScene(L"Result");
		}


		Array<Bubble>cash;//一時保管場所
		bool thirdclick = false;
		//アイテム
		for (auto itr = bubbles.begin(); itr != bubbles.end();)
		{
			auto ptr = itr->rightClick();
			bool deleted = false;
			if (ptr)
			{
				if (itemGauge >= 100.)
				{
					bool kaihi = false;
					auto place = itr->getCoordinate();
					const int rad = 50;
					switch (nowItem)
					{
					case Game::INCREMENT:
						cash.push_back(Bubble(ptr->value() + 1, Point(place.x, place.y), rad, itr->getHex()));
						cash.back().bounce(Random(-0.2, 0.2), 0.1);
						boardEffect.add<EquationEffect>(
							Format(L"{0}={1}+1"_fmt, ptr->value() + 1, ptr->value()),
							Palette::Orange, place, 2.0);
						break;
					case Game::SPLIT:
					{
						String str = itr->valueString();
						String out = Format(str, L"→");
						for (char c : str)
						{
							out.push_back(c);
							out.push_back('|');
							if (c == '1' || c == '0')continue;
							cash.push_back(Bubble(c - '0', Point(place.x, place.y), rad, itr->getHex()));
							cash.back().bounce(Random(-0.2, 0.2), 0.1);
						}
						out.pop_back();
						boardEffect.add<EquationEffect>(
							out,
							Palette::Orange, place, 2.0);
					}
					break;
					case Game::CHECK:
						thirdclick = true;
						kaihi = true;
						break;
					case Game::ITEMNUM:default:assert(false); break;
					}

					itemGauge = 0;
					nowItem = (ItemType)(nowItem + 1);
					if (nowItem == ITEMNUM)
					{
						nowItem = (ItemType)0;
					}

					if (kaihi)
					{
						++itr;
						continue;
					}

					itr = bubbles.erase(itr);
					deleted = true;


				}
			}
			if (!deleted)itr++;
		}

		//bubblesをアップデート
		int scoreTemp = 0;//今のフレームの得点増加量
		for (auto itr = bubbles.begin(); itr != bubbles.end();)
		{
			double speedLimit = yLimit;
			if (m_data->options.freeFall)speedLimit = 114514;
			bool lastpull = thirdclick && itr->rightClick();
			int wentGreen = -1;//素数が通過したかどうか
			auto result = itr->update(speedLimit, inspectLine, boardEffect, scoreTemp, handCursor, healthAdd, m_data->options.fiftySeven, wentGreen, lastpull);
			if (wentGreen > 0)
			{
				m_data->primeSet.insertPrime(wentGreen);
			}
			if (itr->isOutOfBound())
			{
				//画面外にあったら消去
				itr = bubbles.erase(itr);
				continue;
			}
			if (result.first != nullptr)
			{
				double xNotch = 0, yNotch = 0;
				if (result.second == ClickSplit)
				{
					xNotch = 2.0; yNotch = 0.4;
				}
				else if (result.second == DownSplit)
				{
					//実績解除・57
					bool fiftySeven = (itr->getValue() == 57);
					if (fiftySeven)
					{
						if (!m_data->trophy[30])
						{
							m_data->trophy[30] = true;
							//５７
							m_data->optionActive[4] = true;
						}
					}
					xNotch = 2.0; yNotch = 0.8;
					//画面を振る
					shakeEffect.add<ShakeEffect>(16, 300);
					//小さく跳ねる
					itr->bounce(0, 0.05);
					//残基１の場合、ゲームオーバー
					if (oneLife)
					{
						gameOver = true;
						healthGauge = 0;
					}
					else
					{
						//ダメージを加える
						double damage = 40. + 60. * ((double)timer.s() / 1000);
						if (damage > 99)damage = 99;
						healthAdd -= (damage);
					}
					m_data->lastResult.equation = Format(result.first->value(), L"=", (result.first->node1->value()), L"*", (result.first->node2->value()));
					if ((gameOver || healthAdd + healthGauge <= 0) && fiftySeven)
					{
						if (!m_data->trophy[31])
						{
							m_data->trophy[31] = true;
						}
					}
				}
				//分裂させる
				auto sub1 = result.first->node1;
				auto sub2 = result.first->node2;
				auto place = itr->getCoordinate();
				//貯めておく
				const int rad = 50;
				cash.push_back(Bubble(sub1, Point(place.x, place.y), rad, itr->getHex()));
				cash.back().bounce(-xNotch, yNotch);
				cash.push_back(Bubble(sub2, Point(place.x, place.y), rad, itr->getHex()));
				cash.back().bounce(+xNotch, yNotch);
				//元を消去
				itr = bubbles.erase(itr);
			}
			else
			{
				if (result.second == Shudder)
				{
					//画面を振る
					shakeEffect.add<ShakeEffect>(16, 300);
					//小さく跳ねる
					itr->bounce(0, 0.05);
					//残基１の場合、ゲームオーバー
					if (oneLife)
					{
						gameOver = true;
						healthGauge = 0;
					}
					else
					{
						//ダメージを加える
						double damage = 40. + 60. * ((double)timer.s() / 1000);
						if (damage > 99)damage = 99;
						healthAdd -= (damage);
					}
					m_data->lastResult.equation = Format(itr->getValue(), L"=prime");
				}
				++itr;
			}
		}



		//最後に統合
		while (!cash.empty())
		{
			bubbles.push_back(cash.back());
			cash.pop_back();
		}

		//ゲームオーバー
		if (gameOver && !endStopwatch.isActive())
		{
			//ゲーム終了
			for (auto& b : bubbles)
			{
				b.turnOver();
				endStopwatch.restart();
				timer.pause();
			}
		}

		//得点のエフェクトを追加
		if (scoreTemp > 0)
		{
			scoreAdd += scoreTemp;
			scoreEffect.add<EquationEffect>(Format(L"+", scoreTemp), Palette::Orange, Point(scoreCenter, Window::Height() - lowerSize / 2), 1.0, scoreFont);
		}

		//カーソル更新
		Cursor::SetStyle(handCursor ? CursorStyle::Hand : CursorStyle::Default);

	}
	void draw() const override
	{
		//画面を振る
		shakeEffect.update();

		//バブル
		for (const auto& bubble : bubbles)
		{
			bubble.draw();
		}

		//ボード上のエフェクト
		boardEffect.update();

		if (!endStopwatch.isActive())
		{
			//赤い線
			inspectLine.draw(Palette::Red);
		}
		else
		{
			//灰色線
			inspectLine.draw(Palette::Gray);
		}

		//ゲージ
		if (m_data->endless)
			Rect({ 0,Window::Height() - lowerSize - gaugeSize }, { Window::Width() * (healthGauge / 100),gaugeSize }).draw(Palette::Green);

		//画面を振り終わる
		Graphics2D::SetTransform(Mat3x2::Identity());

		//下画面の背景
		lowerBackground.draw(Palette::Gray);

		//得点
		FontAsset(L"ScoreLetter")(scoreLabel).drawCenter(scoreCenter, Window::Height() - lowerSize + FontAsset(L"ScoreLetter").ascent / 2);
		String scoreString = Format(L"{0:06d}"_fmt, score);
		FontAsset(L"Score")(scoreString).drawCenter(scoreCenter, Window::Height() - FontAsset(L"Score").ascent / 2);

		//得点のエフェクト
		scoreEffect.update();

		//時間
		timerBackground.draw(Palette::Darkgray);
		FontAsset(L"ScoreLetter")(timerLabel).drawCenter(timeCenter, Window::Height() - lowerSize + FontAsset(L"ScoreLetter").ascent);
		String timerString = Format(L"{0:02d}:{1:02d}"_fmt, timer.min(), timer.s() - timer.min() * 60);
		if (!m_data->endless)
		{
			//100秒モードは残り時間
			auto timeLeft = 100999ms - timer.elapsed();
			timeLeft = Max(timeLeft, 0ms);
			auto minute = DurationCast<Minutes>(timeLeft);
			auto second = DurationCast<Seconds>(timeLeft);
			timerString = Format(L"{0:02d}:{1:02d}"_fmt, minute.count(), (second % 60).count());
		}
		FontAsset(L"Score")(timerString).drawCenter(timeCenter, Window::Height() - FontAsset(L"Score").ascent);

		//アイテム
		FontAsset(L"ScoreLetter")(itemsLabel).drawCenter(itemCenter, Window::Height() - lowerSize + FontAsset(L"ScoreLetter").ascent / 2);
		auto region = itemGraph[nowItem];
		int offset = 50 * (itemGauge / 100.);
		int x = itemCenter;
		int y = Window::Height() - lowerSize / 5. * 2;
		region.drawAt(x, y);
		Rect(x - 25 + offset, y - 25, 50, 50).draw(Color(Palette::Gray));

		//アイテムの影
		if (Input::MouseR.pressed && itemGauge >= 100.)
		{
			region.drawAt(Mouse::Pos(), Alpha(200));
		}

		//右クリックのアイコン
		if (itemGauge >= 100. && handCursor)
		{
			int x = Mouse::Pos().x;
			int y = Mouse::Pos().y;
			ballon.drawAt(x, y - ballon.height / 2, Color(Palette::White, 200));
			mouseRight.drawAt(x, y - ballon.height / 2 - 5, Color(Palette::White, 200));
		}

		//カウントダウンの文字
		if (onYourMark.elapsed() < 6000ms)
		{
			String number = ToString(5 - onYourMark.s());
			if (number == L"0")number = L"はじめ！";
			FontAsset(L"Countdown")(number).drawAt(Window::Center());
		}

		//終了の文字
		if (endStopwatch.isActive())
		{
			FontAsset(L"Countdown")(L"終了！").drawAt(Window::Center());
		}

		//デバッグ情報
#ifdef _DEBUG
		ClearPrint();
		Println(L"Speed=", yLimit);
		Println(L"Time=", tenSecondPulse.elapsed());
		Println(L"Frequency=", frequency);
#endif
	}
};