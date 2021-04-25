#pragma once
#include <Siv3D.hpp>
#include "GameData.h"
#include "Bubble.h"
#include "ShakeEffect.h"
#include "LoadTextureRegions.h"

//�Q�[��
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
	const String scoreLabel = L"�X�R�A";
	const String timerLabel = L"����";
	const String itemsLabel = L"�A�C�e��";
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
	bool isDouble;//����������鋅�̓_�u����
	int frequentCount;//�����̉�

	Effect shakeEffect;
	Effect boardEffect;
	Effect scoreEffect;

	int32 score; int32 scoreAdd;
	double healthGauge, itemGauge;//�ǂ�����ő傪100
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
		scoreFont = Font(GameInfo::ScoreFontSize, L"�l�r �o�S�V�b�N");

		itemGraph = LoadTextureRegions(L"Graph/items.png", { 50,50 });

		mouseRight = Texture(L"Graph/right.png");
		ballon = Texture(L"Graph/ballon.png");

	}
	void update() override
	{
		//�J�[�\���̕ϐ�������
		handCursor = false;

		//5�b��������X�^�[�g
		if (!timer.isActive() && onYourMark.elapsed() > 5000ms)
		{
			timer.start();
			pulse.start();
			tenSecondPulse.start();
			pulse.set(frequency * (isDouble ? 2 : 1) * (frequentCount < 8 ? 2 : 1));
		}

		//10�b����
		if (tenSecondPulse.elapsed() >= 10000ms)
		{
			frequency -= (preFrequency - maxFrequency) / 20;//(850ms);//�ő���ʑ��x���グ��
			if (frequency < maxFrequency)
			{
				frequency = maxFrequency;
			}
			yLimit = lineHeight / (60.0 * DurationCast<Milliseconds>(frequency).count() / 1000.0);//lineHeight * DurationCast<Seconds>(frequency).count() / 2;
			tenSecondPulse.restart();
		}

		//score��scoreAdd�𑫂�����
		int32 clause = scoreAdd - (int)(scoreAdd * 0.9);
		score += clause;
		scoreAdd -= clause;
		//�J���X�g���ꉞ
		score = Min(score, 999999);

		//�Q�[�W�ނ��������Ƃ�����
		{
			healthGauge += healthAdd * 0.1;
			healthAdd *= 0.9;
		}
		//HP��0�ɂȂ�����I��
		if (healthGauge + healthAdd <= 0)gameOver = true;

		//�����Ńo�u���𔭐�������
		if (!endStopwatch.isActive())
		{
			const int32 rad = 50;
			if (pulse.elapsed() > frequency * (isDouble ? 2 : 1) / 2)
			{
				isDouble = (Random(3) == 0);
				if (frequentCount < 4)isDouble = false;//4��܂ł͖���
				for (int i = 0; i < (isDouble ? 2 : 1); ++i)
				{
					int32 xCoord = Random(Window::Width() - rad * 2) + rad;//�߂��āI�I
					bubbles.push_back(Bubble(Random() * (numberMax - 2) + 2, Point(xCoord, -rad), rad, m_data->options.hexaDecimal));
				}
				++frequentCount;
				pulse.restart();
			}
		}

		//bubbles�̓����蔻��m�F
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
					//x����
					double len = vec.length();
					//�d�Ȃ��Ă���ꍇ�͓��ꏈ��
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
					//y����
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

		//�A�C�e���Q�[�W�����Ԃő��₷
		if (timer.isActive())if (itemsActive)itemGauge += 4 / 60.;

		//�Q�[�W���
		if (healthGauge > 100.)healthGauge = 100.;
		if (itemGauge > 100.)itemGauge = 100.;
		if (oneLife && m_data->options.noMiss)healthGauge = Min(healthGauge, 1.);

		//����100�b���[�h��������
		if (!m_data->endless)
		{
			if (timer.elapsed() >= GameInfo::timeLimit)
			{
				//�Q�[���I��
				for (auto& b : bubbles)
				{
					b.turnYellow();
					endStopwatch.restart();
					m_data->lastResult.equation = L"�N���A�I";
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


		Array<Bubble>cash;//�ꎞ�ۊǏꏊ
		bool thirdclick = false;
		//�A�C�e��
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
						String out = Format(str, L"��");
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

		//bubbles���A�b�v�f�[�g
		int scoreTemp = 0;//���̃t���[���̓��_������
		for (auto itr = bubbles.begin(); itr != bubbles.end();)
		{
			double speedLimit = yLimit;
			if (m_data->options.freeFall)speedLimit = 114514;
			bool lastpull = thirdclick && itr->rightClick();
			int wentGreen = -1;//�f�����ʉ߂������ǂ���
			auto result = itr->update(speedLimit, inspectLine, boardEffect, scoreTemp, handCursor, healthAdd, m_data->options.fiftySeven, wentGreen, lastpull);
			if (wentGreen > 0)
			{
				m_data->primeSet.insertPrime(wentGreen);
			}
			if (itr->isOutOfBound())
			{
				//��ʊO�ɂ����������
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
					//���щ����E57
					bool fiftySeven = (itr->getValue() == 57);
					if (fiftySeven)
					{
						if (!m_data->trophy[30])
						{
							m_data->trophy[30] = true;
							//�T�V
							m_data->optionActive[4] = true;
						}
					}
					xNotch = 2.0; yNotch = 0.8;
					//��ʂ�U��
					shakeEffect.add<ShakeEffect>(16, 300);
					//���������˂�
					itr->bounce(0, 0.05);
					//�c��P�̏ꍇ�A�Q�[���I�[�o�[
					if (oneLife)
					{
						gameOver = true;
						healthGauge = 0;
					}
					else
					{
						//�_���[�W��������
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
				//���􂳂���
				auto sub1 = result.first->node1;
				auto sub2 = result.first->node2;
				auto place = itr->getCoordinate();
				//���߂Ă���
				const int rad = 50;
				cash.push_back(Bubble(sub1, Point(place.x, place.y), rad, itr->getHex()));
				cash.back().bounce(-xNotch, yNotch);
				cash.push_back(Bubble(sub2, Point(place.x, place.y), rad, itr->getHex()));
				cash.back().bounce(+xNotch, yNotch);
				//��������
				itr = bubbles.erase(itr);
			}
			else
			{
				if (result.second == Shudder)
				{
					//��ʂ�U��
					shakeEffect.add<ShakeEffect>(16, 300);
					//���������˂�
					itr->bounce(0, 0.05);
					//�c��P�̏ꍇ�A�Q�[���I�[�o�[
					if (oneLife)
					{
						gameOver = true;
						healthGauge = 0;
					}
					else
					{
						//�_���[�W��������
						double damage = 40. + 60. * ((double)timer.s() / 1000);
						if (damage > 99)damage = 99;
						healthAdd -= (damage);
					}
					m_data->lastResult.equation = Format(itr->getValue(), L"=prime");
				}
				++itr;
			}
		}



		//�Ō�ɓ���
		while (!cash.empty())
		{
			bubbles.push_back(cash.back());
			cash.pop_back();
		}

		//�Q�[���I�[�o�[
		if (gameOver && !endStopwatch.isActive())
		{
			//�Q�[���I��
			for (auto& b : bubbles)
			{
				b.turnOver();
				endStopwatch.restart();
				timer.pause();
			}
		}

		//���_�̃G�t�F�N�g��ǉ�
		if (scoreTemp > 0)
		{
			scoreAdd += scoreTemp;
			scoreEffect.add<EquationEffect>(Format(L"+", scoreTemp), Palette::Orange, Point(scoreCenter, Window::Height() - lowerSize / 2), 1.0, scoreFont);
		}

		//�J�[�\���X�V
		Cursor::SetStyle(handCursor ? CursorStyle::Hand : CursorStyle::Default);

	}
	void draw() const override
	{
		//��ʂ�U��
		shakeEffect.update();

		//�o�u��
		for (const auto& bubble : bubbles)
		{
			bubble.draw();
		}

		//�{�[�h��̃G�t�F�N�g
		boardEffect.update();

		if (!endStopwatch.isActive())
		{
			//�Ԃ���
			inspectLine.draw(Palette::Red);
		}
		else
		{
			//�D�F��
			inspectLine.draw(Palette::Gray);
		}

		//�Q�[�W
		if (m_data->endless)
			Rect({ 0,Window::Height() - lowerSize - gaugeSize }, { Window::Width() * (healthGauge / 100),gaugeSize }).draw(Palette::Green);

		//��ʂ�U��I���
		Graphics2D::SetTransform(Mat3x2::Identity());

		//����ʂ̔w�i
		lowerBackground.draw(Palette::Gray);

		//���_
		FontAsset(L"ScoreLetter")(scoreLabel).drawCenter(scoreCenter, Window::Height() - lowerSize + FontAsset(L"ScoreLetter").ascent / 2);
		String scoreString = Format(L"{0:06d}"_fmt, score);
		FontAsset(L"Score")(scoreString).drawCenter(scoreCenter, Window::Height() - FontAsset(L"Score").ascent / 2);

		//���_�̃G�t�F�N�g
		scoreEffect.update();

		//����
		timerBackground.draw(Palette::Darkgray);
		FontAsset(L"ScoreLetter")(timerLabel).drawCenter(timeCenter, Window::Height() - lowerSize + FontAsset(L"ScoreLetter").ascent);
		String timerString = Format(L"{0:02d}:{1:02d}"_fmt, timer.min(), timer.s() - timer.min() * 60);
		if (!m_data->endless)
		{
			//100�b���[�h�͎c�莞��
			auto timeLeft = 100999ms - timer.elapsed();
			timeLeft = Max(timeLeft, 0ms);
			auto minute = DurationCast<Minutes>(timeLeft);
			auto second = DurationCast<Seconds>(timeLeft);
			timerString = Format(L"{0:02d}:{1:02d}"_fmt, minute.count(), (second % 60).count());
		}
		FontAsset(L"Score")(timerString).drawCenter(timeCenter, Window::Height() - FontAsset(L"Score").ascent);

		//�A�C�e��
		FontAsset(L"ScoreLetter")(itemsLabel).drawCenter(itemCenter, Window::Height() - lowerSize + FontAsset(L"ScoreLetter").ascent / 2);
		auto region = itemGraph[nowItem];
		int offset = 50 * (itemGauge / 100.);
		int x = itemCenter;
		int y = Window::Height() - lowerSize / 5. * 2;
		region.drawAt(x, y);
		Rect(x - 25 + offset, y - 25, 50, 50).draw(Color(Palette::Gray));

		//�A�C�e���̉e
		if (Input::MouseR.pressed && itemGauge >= 100.)
		{
			region.drawAt(Mouse::Pos(), Alpha(200));
		}

		//�E�N���b�N�̃A�C�R��
		if (itemGauge >= 100. && handCursor)
		{
			int x = Mouse::Pos().x;
			int y = Mouse::Pos().y;
			ballon.drawAt(x, y - ballon.height / 2, Color(Palette::White, 200));
			mouseRight.drawAt(x, y - ballon.height / 2 - 5, Color(Palette::White, 200));
		}

		//�J�E���g�_�E���̕���
		if (onYourMark.elapsed() < 6000ms)
		{
			String number = ToString(5 - onYourMark.s());
			if (number == L"0")number = L"�͂��߁I";
			FontAsset(L"Countdown")(number).drawAt(Window::Center());
		}

		//�I���̕���
		if (endStopwatch.isActive())
		{
			FontAsset(L"Countdown")(L"�I���I").drawAt(Window::Center());
		}

		//�f�o�b�O���
#ifdef _DEBUG
		ClearPrint();
		Println(L"Speed=", yLimit);
		Println(L"Time=", tenSecondPulse.elapsed());
		Println(L"Frequency=", frequency);
#endif
	}
};