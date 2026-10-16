#pragma once

#include <SFML/System/Vector2.hpp>

#include <optional>
#include <vector>

struct Ray
{
	sf::Vector2f origin;
	sf::Vector2f direction; // MUST be normalized
};

struct Segment
{
	sf::Vector2f start;
	sf::Vector2f end;
};

struct RayHit
{
	sf::Vector2f point;
	float        distance = 0.f;
	std::size_t  segmentIndex = 0;   // filled in by castRay, not by intersectRaySegment
};

float cross(const sf::Vector2f& a, const sf::Vector2f& b);

std::optional<RayHit> intersectRaySegment(const Ray& ray, const Segment& segment);

std::optional<RayHit> castRay(const Ray& ray,
	const std::vector<Segment>& segments,
	float maxDistance);

// Lab A, part 2. One endpoint per ray, in increasing angle order.
std::vector<sf::Vector2f>
castUniformRays(const sf::Vector2f& origin,
	const std::vector<Segment>& segments,
	int rayCount,
	float maxDistance);