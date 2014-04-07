#include "Math.h"
#include <cmath>

namespace Math
{
	bool pointInCircle(sf::Vector2f point, sf::Vector2f circle_center, float circle_radius)
	{
		return (euclideanDistance(point, circle_center) <= circle_radius);
	}

	float euclideanDistance(sf::Vector2f v1, sf::Vector2f v2)
	{
		return std::sqrt(std::pow(v1.x - v2.x, 2) + std::pow(v1.y - v2.y, 2));
	}
}
