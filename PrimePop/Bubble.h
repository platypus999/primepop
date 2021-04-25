#pragma once
#include <Siv3D.hpp>
#include "Node.h"
#include "EquationEffect.h"

enum SplitState
{
	NothingSplit, ClickSplit, DownSplit, Shudder
};

//�o�u���̃N���X
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
		//circle������
		circle = Circle(coordinate, radius);
		x = coordinate.x;
		y = coordinate.y;
		//�e��ϐ�������
		speed = Vec2(0.0, 0.0);
		state = Float;
	}
	bool hex;
	
public:
	Bubble(int32 num, Point coordinate, int32 radius, bool hexa)
	{
		//�ϐ�������
		init_circle(coordinate, radius);
		//num���e�ƂȂ�悤�Ɋ撣���č\�z���悤�I
		//�f��������
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
		//�e�[�u���ɂ��ׂĂ̑f�����悹��
		Array<std::shared_ptr<Node>>table;
		for (int32 elm : primes)
		{
			table.emplace_back(new Node(elm));
		}
		//�����_����2��I�сA�|���Ă���
		while (table.size() > 1)
		{
			Shuffle(table);
			auto ch1 = table[table.size() - 1];
			auto ch2 = table[table.size() - 2];
			table.pop_back(); table.pop_back();
			table.emplace_back(new Node(ch1, ch2));
		}
		//table[0]������
		tree = table[0];

	}
	Bubble(std::shared_ptr<Node>ptr, Point coordinate, int32 radius, bool hexa)
	{
		//�O�̌��ʂ𗬗p
		tree = ptr;
		hex = hexa;
		//�ϐ�������
		init_circle(coordinate, radius);
	}

	//���̊֐��̎����͐����厸�s�ł�����
	//�����炱��Ȃ��Ƃ͂Ȃ��悤�ɋC��t���܂�
	//�߂�l�͕��􎞂Ɏg��
	//�߂�l��pair�ɂ���H
	std::pair < std::shared_ptr<Node>, SplitState> update(double speedLimit, Line redLine, Effect&effect, int&scoreValue, bool& handCursor, double&gaugeAdd, bool flag57, int &wentGreen, bool click = false)
	{
		//�J�[�\������
		if (circle.mouseOver)
		{
			//Cursor::SetStyle(CursorStyle::Hand);
			handCursor = true;
		}

		//�N���b�N����
		//���N���b�N
		if (circle.leftClicked || click)
		{
			if (state == Float)
			{
				if (tree->prime())
				{
					if (!click)
						//�Ή�����
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
					//���􂷂�
					//Effect{}={}*{}
					effect.add<EquationEffect>(
						Format(L"{0}={1}*{2}"_fmt, tree->value(), tree->node1->value(), tree->node2->value()),
						Palette::Green, circle.center);
					//���_�𑫂�(����̂�����������)
					int getPoint = tree->value() - ((tree->node1->value() + tree->node2->value()));
					scoreValue += getPoint;
					return{ tree,ClickSplit };
				}
			}
		}

		//������
		//if (speed.y > 0)speed.x *= stickiness;//���ɗ����Ă���Ԃ͉��ɓ����Ȃ�
		speed.y += ddy;
		if (state == Float)speed.y = Min(speed.y, speedLimit);//�X�s�[�h�𗎂Ƃ�
		if (circle.intersects(Line(0, 0, 0, Window::Height())))
		{
			//���̕�
			speed.x = abs(speed.x * friction);
		}
		if (circle.intersects(Line(Window::Width(), 0, Window::Width(), Window::Height())))
		{
			//�E�̕�
			speed.x = -abs(speed.x * friction);
		}
		if (circle.intersects(redLine) && speed.y > 0)
		{
			//���̐��A���������ɓ����Ă���
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
					//���_�𑫂�(���̒l)
					int getPoint = tree->value();
					scoreValue += getPoint;
					//�Q�[�W��
					gaugeAdd += (gaugeIncr);
					wentGreen = tree->value();
				}
				else
				{
					//���􂷂�
					//Effect{}={}*{}
					effect.add<EquationEffect>(
						Format(L"{0}={1}*{2}"_fmt, tree->value(), tree->node1->value(), tree->node2->value()),
						Palette::Red, circle.center);
					return{ tree,DownSplit };
				}
			}
			else if (state == Stone_Normal)
			{
				//��񂾂��_���[�W������
				state = Stone_After;
				effect.add<EquationEffect>(
					Format(L"{0}=prime"_fmt, tree->value()),
					Palette::Red, circle.center);
				return{ nullptr,Shudder };
			}
		}
		//�Z�o�����X�s�[�h�𑫂�
		if (!(y < circle.r))
		{
			y += speed.y;
		}
		else if (speed.y > 0)
		{
			y += speed.y;
		}
		x += speed.x;

		//�Ō��x,y��circle�����킹��
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

	//���˕Ԃ点��
	void bounce(double new_dx, double dyPercentage = 0.4)
	{
		speed.x = new_dx;
		//�ǂ̂��炢�܂Ŕ�΂����̐������珉�����t�Z
		speed.y = -sqrt(2 * Window::Height() * dyPercentage * ddy);
	}

	//���W�̎擾
	Vec2 getCoordinate()
	{
		return Vec2(circle.center.x, circle.center.y);
	}

	//���a�̎擾
	double getRadius()
	{
		return circle.r;
	}

	//��ʂ̉��܂ōs������
	bool isOutOfBound()
	{
		return Window::Height() <= -circle.r + circle.y;
	}

	int getValue()
	{
		return tree->value();
	}

	//�������ɉ����o��
	void pushHorizontal(double k)
	{
		if (speed.x * k >= 0)
		{
			//������
			speed.x += k * energySplit;
		}
		else
		{
			speed.x += k;
		}
	}

	//�c�����ɉ����o��
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

	//���F������
	void turnYellow()
	{
		state = Stone_Yellow;
	}

	//�Q�[���I�[�o�[�̎�
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