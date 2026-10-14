#include "GeometryTests.h"
#include "Geometry.h"

#include <cassert>
#include <cmath>

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

	// TODO: diagonal ray vs horizontal wall      -> hit ~(4,4), distance ~5.657
	// TODO: hit lies outside the segment         -> nullopt
	// TODO: parallel ray                         -> nullopt
	// TODO: ray starts on the segment            -> distance 0 (your defined behavior)
	// TODO: ray hits exactly at an endpoint      -> hit (defined behavior)
	// TODO: collinear ray and segment            -> nullopt (defined behavior)
	// TODO: wall beyond maxDistance              -> nullopt
}