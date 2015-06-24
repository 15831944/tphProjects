#pragma once

enum ChartType
{
	Bar_2D = 0,
	Line_2D,
	Bar_3D
};

class Chart
{
public:
	Chart();
	virtual ~Chart();

private:
	ChartType m_charType;
};
