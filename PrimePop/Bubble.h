#pragma once
#include <Siv3D.hpp>
#include "Node.h"
#include "EquationEffect.h"

enum SplitState
{
	NothingSplit, ClickSplit, DownSplit, Shudder
};

//バブルのクラス
class Bubble
{
private:
	Circle circle;
	double x, y;
	Vec2 speed;
	const double ddy = 0.50;
	const double friction = 0.8;
	const double energySplit = 0.5;
	const double stickiness = 0.3;
	const double gaugeIncr = 3.0;
	enum { Float, Stone_Normal, Stone_Green, Vapor, Stone_After, Stone_Yellow, }state;
	std::shared_ptr<Node>tree;
	bool isFloating()
	{
		return state == Float;
	}
	bool isStone()
	{
		return state == Stone_Normal || state == Stone_Green || state == Stone_Yellow;
	}
	void init_circle(Point coordinate, int32 radius)
	{
		//circleを入れる
		circle = Circle(coordinate, radius);
		x = coordinate.x;
		y = coordinate.y;
		//各種変数初期化
		speed = Vec2(0.0, 0.0);
		state = Float;
	}
	bool hex;
	
public:
	Bubble(int32 num, Point coordinate, int32 radius, bool hexa)
	{
		//変数初期化
		init_circle(coordinate, radius);
		//numが親となるように頑張って構築しよう！
		//素因数分解
		Array<int32>primes;
		int32 buf = num;
		for (int32 i = 2; i * i <= num; ++i)
		{
			while (buf % i == 0)
			{
				buf /= i;
				primes.push_back(i);
			}
		}
		hex = hexa;
		if (buf > 1)primes.push_back(buf);
		//テーブルにすべての素数を乗せる
		Array<std::shared_ptr<Node>>table;
		for (int32 elm : primes)
		{
			table.emplace_back(new Node(elm));
		}
		//ランダムに2つを選び、掛けていく
		while (table.size() > 1)
		{
			Shuffle(table);
			auto ch1 = table[table.size() - 1];
			auto ch2 = table[table.size() - 2];
			table.pop_back(); table.pop_back();
			table.emplace_back(new Node(ch1, ch2));
		}
		//table[0]が答え
		tree = table[0];

	}
	Bubble(std::shared_ptr<Node>ptr, Point coordinate, int32 radius, bool hexa)
	{
		//前の結果を流用
		tree = ptr;
		hex = hexa;
		//変数初期化
		init_circle(coordinate, radius);
	}

	//この関数の実装は正直大失敗でしたね
	//次からこんなことはないように気を付けます
	//戻り値は分裂時に使う
	//戻り値をpairにする？
	std::pair < std::shared_ptr<Node>, SplitState> update(double speedLimit, Line redLine, Effect&effect, int&scoreValue, bool& handCursor, double&gaugeAdd, bool flag57, int &wentGreen, bool click = false)
	{
		//カーソル判定
		if (circle.mouseOver)
		{
			//Cursor::SetStyle(CursorStyle::Hand);
			handCursor = true;
		}

		//クリック判定
		//左クリック
		if (circle.leftClicked || click)
		{
			if (state == Float)
			{
				if (tree->prime())
				{
					if (!click)
						//石化する
						state = Stone_Normal;
					else
					{
						state = Stone_Green;
						//Effect
						effect.add<EquationEffect>(
							Format(L"{0}=prime"_fmt, tree->value()),
							Palette::Green, circle.center);
					}
				}
				else
				{
					//分裂する
					//Effect{}={}*{}
					effect.add<EquationEffect>(
						Format(L"{0}={1}*{2}"_fmt, tree->value(), tree->node1->value(), tree->node2->value()),
						Palette::Green, circle.center);
					//得点を足す(分裂のうち小さい方)
					int getPoint = tree->value() - ((tree->node1->value() + tree->node2->value()));
					scoreValue += getPoint;
					return{ tree,ClickSplit };
				}
			}
		}

		//動かす
		//if (speed.y > 0)speed.x *= stickiness;//下に落ちている間は横に動かない
		speed.y += ddy;
		if (state == Float)speed.y = Min(speed.y, speedLimit);//スピードを落とす
		if (circle.intersects(Line(0, 0, 0, Window::Height())))
		{
			//左の壁
			speed.x = abs(speed.x * friction);
		}
		if (circle.intersects(Line(Window::Width(), 0, Window::Width(), Window::Height())))
		{
			//右の壁
			speed.x = -abs(speed.x * friction);
		}
		if (circle.intersects(redLine) && speed.y > 0)
		{
			//下の線、かつ下向きに動いている
			if (state == Float)
			{
				if (tree->prime() || (flag57 && tree->value() == 57))
				{
					//OK
					state = Stone_Green;
					//Effect
					effect.add<EquationEffect>(
						Format(L"{0}=prime"_fmt, tree->value()),
						Palette::Green, circle.center);
					//得点を足す(その値)
					int getPoint = tree->value();
					scoreValue += getPoint;
					//ゲージ回復
					gaugeAdd += (gaugeIncr);
					wentGreen = tree->value();
				}
				else
				{
					//分裂する
					//Effect{}={}*{}
					effect.add<EquationEffect>(
						Format(L"{0}={1}*{2}"_fmt, tree->value(), tree->node1->value(), tree->node2->value()),
						Palette::Red, circle.center);
					return{ tree,DownSplit };
				}
			}
			else if (state == Stone_Normal)
			{
				//一回だけダメージを入れる
				state = Stone_After;
				effect.add<EquationEffect>(
					Format(L"{0}=prime"_fmt, tree->value()),
					Palette::Red, circle.center);
				return{ nullptr,Shudder };
			}
		}
		//算出したスピードを足す
		if (!(y < circle.r))
		{
			y += speed.y;
		}
		else if (speed.y > 0)
		{
			y += speed.y;
		}
		x += speed.x;

		//最後にx,yにcircleを合わせる
		circle.x = x;
		circle.y = y;

		return{ nullptr, NothingSplit };
	}

	std::shared_ptr<Node> rightClick()
	{
		if (circle.rightReleased)
		{
			return tree;
		}
		return nullptr;
	}

	void draw()const
	{
		Color c = Palette::White;
		switch (state)
		{
		case Float:
			break;
		case Stone_Normal:
		case Stone_After:
			c = Palette::Slategray;
			break;
		case Stone_Green:
			c = Palette::Lime;
			break;
		case Vapor:
			c = Palette::Blue;
			break;
		case Stone_Yellow:
			c = Palette::Yellow;
			break;
		default:assert(false); break;
		}
		circle.draw(c);
		FontAsset(L"Bubble")(valueString()).drawCenter(circle.center, Palette::Black);
	}

	String valueString()const
	{
		if (hex)
			return ToString(tree->value(), 16);
		return ToString(tree->value());
	}

	//跳ね返らせる
	void bounce(double new_dx, double dyPercentage = 0.4)
	{
		speed.x = new_dx;
		//どのくらいまで飛ばすかの数字から初速を逆算
		speed.y = -sqrt(2 * Window::Height() * dyPercentage * ddy);
	}

	//座標の取得
	Vec2 getCoordinate()
	{
		return Vec2(circle.center.x, circle.center.y);
	}

	//半径の取得
	double getRadius()
	{
		return circle.r;
	}

	//画面の下まで行ったか
	bool isOutOfBound()
	{
		return Window::Height() <= -circle.r + circle.y;
	}

	int getValue()
	{
		return tree->value();
	}

	//横方向に押し出す
	void pushHorizontal(double k)
	{
		if (speed.x * k >= 0)
		{
			//同符号
			speed.x += k * energySplit;
		}
		else
		{
			speed.x += k;
		}
	}

	//縦方向に押し出す
	void pushVertical(double k)
	{
		if (speed.y * k >= 0)
		{
			speed.y += k;
		}
		else
		{
			speed.y += k;
		}
	}

	//黄色くする
	void turnYellow()
	{
		state = Stone_Yellow;
	}

	//ゲームオーバーの時
	void turnOver()
	{
		state = Stone_After;
	}

	bool getHex()
	{
		return hex;
	}

	Bubble& operator = (const Bubble& o)
	{
		circle = o.circle;
		x = o.x;
		y = o.y;
		speed = o.speed;
		state = o.state;
		tree = o.tree;
		hex = o.hex;
		return *this;
	}
};