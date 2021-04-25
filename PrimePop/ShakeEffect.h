#pragma once
# include <Siv3D.hpp>

//ƒRƒsƒy
class ShakeEffect : public IEffect
{
private:
	double m_maxOffset, m_lengthMillisec;
public:
	ShakeEffect(double maxOffset, double lengthMillisec)
		: m_maxOffset(maxOffset)
		, m_lengthMillisec(lengthMillisec) {}

	bool update(double timeSec) override
	{
		const double timeMillisec = timeSec * 1000.0;
		const Vec2 offset = RandomVec2(Circle(m_maxOffset * Max(1.0 - timeMillisec / m_lengthMillisec, 0.0)));
		Graphics2D::SetTransform(Mat3x2::Translate(offset));
		return timeMillisec < m_lengthMillisec;
	}
};