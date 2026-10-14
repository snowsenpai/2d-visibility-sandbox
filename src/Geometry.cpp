#include "Geometry.h"
#include "MathUtils.h"

#include <cassert>
#include <cmath>

namespace
{
	// Used ONLY for the parallel check. Why not for t or u? (design question 4)
	constexpr float kParallelEpsilon = 1e-6f;
}

float cross(const sf::Vector2f& a, const sf::Vector2f& b)
{
	return a.x * b.y - a.y * b.x;
}

std::optional<RayHit> intersectRaySegment(const Ray& ray, const Segment& seg)
{
	assert(std::abs(ray.direction.lengthSquared() - 1.f) < 1e-3f && "ray direction must be normalized");

	const sf::Vector2f E = seg.end - seg.start;     // segment direction
	const sf::Vector2f AO = seg.start - ray.origin;  // origin -> segment start

	// Math recap (derived in the previous message):
	//   t = cross(AO, E) / cross(D, E)   // distance along the ray
	//   u = cross(AO, D) / cross(D, E)   // fraction along the segment
	const float denom = cross(ray.direction, E);

	// TODO 1: if |denom| < kParallelEpsilon -> parallel or collinear -> return std::nullopt
	// TODO 2: compute t and u
	// TODO 3: reject if t < 0, u < 0, or u > 1 (what do you do about t == 0 and u == 0/1? see defaults above)
	// TODO 4: return RayHit{ ray.origin + ray.direction * t, t, 0 }

	return std::nullopt; // placeholder so it compiles
}

std::optional<RayHit> castRay(const Ray& ray, const std::vector<Segment>& segments, float maxDistance)
{
	std::optional<RayHit> best;

	for (std::size_t i = 0; i < segments.size(); ++i)
	{
		std::optional<RayHit> hit = intersectRaySegment(ray, segments[i]);
		if (!hit) continue;

		// TODO 5: skip if hit->distance > maxDistance
		// TODO 6: if best is empty OR hit->distance < best->distance:
		//             set hit->segmentIndex = i, then best = hit
	}

	return best;
}

std::vector<sf::Vector2f> castUniformRays(const sf::Vector2f& origin,
	const std::vector<Segment>& segments,
	int rayCount,
	float maxDistance)
{
	assert(rayCount > 0);

	std::vector<sf::Vector2f> points;
	points.reserve(static_cast<std::size_t>(rayCount));

	const float angleStep = math::TwoPi / static_cast<float>(rayCount);

	for (int i = 0; i < rayCount; ++i)
	{
		const float angle = static_cast<float>(i) * angleStep;   // radians

		// TODO 1: direction = { cos(angle), sin(angle) }  (already length 1, why?)
		// TODO 2: hit = castRay(Ray{ origin, direction }, segments, maxDistance)
		// TODO 3: push the hit point, or origin + direction * maxDistance if no hit
	}

	return points;
}