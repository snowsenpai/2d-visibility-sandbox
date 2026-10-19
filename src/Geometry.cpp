#include "Geometry.h"
#include "MathUtils.h"

#include <SFML/System/Vector2.hpp>

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <optional>
#include <vector>

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

	// Math recap:
	//   t = cross(AO, E) / cross(D, E)   // distance along the ray
	//   u = cross(AO, D) / cross(D, E)   // fraction along the segment
	const float denom = cross(ray.direction, E);

	if (std::abs(denom) < kParallelEpsilon)
	{
		return std::nullopt;
	}
	
	const auto t = cross(AO, E) / denom;
	const auto u = cross(AO, ray.direction) / denom;

	// reject (what do you do about t == 0 and u == 0/1? see defaults above)
	if (t < 0.f || u < 0.f || u > 1.f)
	{
		return std::nullopt;
	}
	
	return RayHit{ ray.origin + ray.direction * t, t, 0 };
}

std::optional<RayHit> castRay(const Ray& ray, const std::vector<Segment>& segments, float maxDistance)
{
	std::optional<RayHit> best;

	for (std::size_t i = 0; i < segments.size(); ++i)
	{
		std::optional<RayHit> hit = intersectRaySegment(ray, segments[i]);
		if (!hit) continue;

		if (hit->distance > maxDistance)
		{
			continue;
		}
		
		if (!best || hit->distance < best->distance)
		{
			hit->segmentIndex = i;
			best = hit;
		}
	}

	return best;
}

std::vector<sf::Vector2f>
castUniformRays(const sf::Vector2f& origin,
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

		auto direction = sf::Vector2f{ std::cos(angle), std::sin(angle) };

		const auto hit = castRay(Ray{ origin, direction }, segments, maxDistance);

		if (hit)
		{
			points.push_back(hit->point);
		}
		else
		{
			points.push_back(origin + direction * maxDistance);
		}
	}

	return points;
}