#include "GeometryTests.h"
#include "Geometry.h"

#include <SFML/System/Vector2.hpp>

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

namespace
{
	bool nearlyEqual(float a, float b, float eps = 1e-3f) { return std::abs(a - b) <= eps; }

	Ray makeRay(float ox, float oy, float dx, float dy)
	{
		return Ray{ { ox, oy }, sf::Vector2f(dx, dy).normalized() };
	}
}

void runGeometryTests()
{
	// 1. horizontal ray hits a vertical wall
	{
		const std::vector<Segment> walls = { Segment{ { 5.f, -2.f }, { 5.f, 3.f } } };
		const auto hit = castRay(makeRay(0, 0, 1, 0), walls, 1000.f);
		assert(hit.has_value());
		assert(nearlyEqual(hit->point.x, 5.f) && nearlyEqual(hit->point.y, 0.f));
		assert(nearlyEqual(hit->distance, 5.f));
	}

	// 2. ray points away from the wall
	{
		const std::vector<Segment> walls = { Segment{ { 5.f, -2.f }, { 5.f, 3.f } } };
		assert(!castRay(makeRay(0, 0, -1, 0), walls, 1000.f).has_value());
	}

	// 3. nearest of two walls wins, with the right index
	{
		const std::vector<Segment> walls = {
			Segment{ { 5.f, -2.f }, { 5.f, 3.f } },
			Segment{ { 3.f, -2.f }, { 3.f, 3.f } },
		};
		const auto hit = castRay(makeRay(0, 0, 1, 0), walls, 1000.f);
		assert(hit.has_value() && hit->segmentIndex == 1);
		assert(nearlyEqual(hit->distance, 3.f));
	}

	// hit ~(4,4), distance ~5.657
	// 4. diagonal ray vs horizontal wall
	{
		const std::vector<Segment> walls = {
			Segment{ { 2.f, 4.f }, { 6.f, 4.f } }
		};

		const auto hit = castRay(
			makeRay(0, 0, 1, 1), walls, 1000.f
		);

		assert(hit.has_value());
		assert(nearlyEqual(hit->point.x, 4.f));
		assert(nearlyEqual(hit->point.y, 4.f));
		assert(nearlyEqual(hit->distance, std::sqrt(32.f)));
	}

	// hit lies outside the segment -> nullopt
	// 5. intersection lies outside the segment
	{
		const std::vector<Segment> walls = {
			Segment{ { 5.f, 2.f }, { 5.f, 5.f } }
		};

		const auto hit = castRay(
			makeRay(0, 0, 1, 0), walls, 1000.f
		);

		assert(!hit.has_value());
	}

	// 6. parallel ray
	{
		const std::vector<Segment> walls = {
			Segment{ { 0.f, 5.f }, { 10.f, 5.f } }
		};

		const auto hit = castRay(
			makeRay(0, 0, 1, 0), walls, 1000.f
		);

		assert(!hit.has_value());
	}

	// 7. ray starts on the segment
	{
		const std::vector<Segment> walls = {
			Segment{ { 0.f, -2.f }, { 0.f, 2.f } }
		};

		const auto hit = castRay(
			makeRay(0, 0, 1, 0), walls, 1000.f
		);

		assert(hit.has_value());
		assert(nearlyEqual(hit->distance, 0.f));
		assert(nearlyEqual(hit->point.x, 0.f));
		assert(nearlyEqual(hit->point.y, 0.f));
	}

	// 8. ray hits exactly at a segment endpoint
	{
		const std::vector<Segment> walls = {
			Segment{ { 5.f, 0.f }, { 5.f, 4.f } }
		};

		const auto hit = castRay(
			makeRay(0, 0, 1, 0), walls, 1000.f
		);

		assert(hit.has_value());
		assert(nearlyEqual(hit->point.x, 5.f));
		assert(nearlyEqual(hit->point.y, 0.f));
	}

	// 9. collinear ray and segment
	{
		const std::vector<Segment> walls = {
			Segment{ { 5.f, 0.f }, { 10.f, 0.f } }
		};

		const auto hit = castRay(
			makeRay(0, 0, 1, 0), walls, 1000.f
		);

		assert(!hit.has_value());
	}

	// 10. wall beyond maxDistance
	{
		const std::vector<Segment> walls = {
			Segment{ { 10.f, -2.f }, { 10.f, 2.f } }
		};

		const auto hit = castRay(
			makeRay(0, 0, 1, 0), walls, 5.f
		);

		assert(!hit.has_value());
	}

	std::cout << "[PASS] All geometry tests passed!\n";
}