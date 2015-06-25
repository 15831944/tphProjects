#include <vector>
#include <string>

typedef enum BOXTYPE
{
	BOXTYPE_BEGIN = 0,
	BOXTYPE_TEU,
	BOXTYPE_FEU,
	BOXTYPE_END
};

class Unit_Box
{
private:
	BOXTYPE m_boxType;
	int m_boxWeight;
	int m_boxLength;
	int m_boxWidth;
	int m_boxHeight;
	std::string m_strDescription;
};
class Conveyor_S
{
private:
	int m_capacity; // TEU
	std::vector<Unit_Box> m_vecBox; // Containers in the ship, it's count can never be larger than m_capacity.
public:
};

