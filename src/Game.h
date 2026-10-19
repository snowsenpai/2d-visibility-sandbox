#pragma once

#include "DebugState.h"
#include "DebugUI.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Geometry.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/VideoMode.hpp>

#include <memory>
#include <optional>
#include <random>
#include <type_traits>
#include <vector>

class Game
{
	// IMPORTANT: declare the window BEFORE m_debugUI.
	// Members are destroyed in reverse order, so DebugUI shuts down while the window is still alive.
	sf::Clock m_deltaClock;
	sf::RenderWindow m_window;
	DebugUI          m_debugUI;
	DebugState       m_debugState;
	EntityManager m_entityManager;

	std::shared_ptr<Entity> m_player;

	std::random_device m_randomDevice;
	mutable std::mt19937 m_engine{ m_randomDevice() }; // seeded once; rng() draws from this, not a fresh engine per call

	bool m_running = false;
	bool m_drawCollisison = false;

	std::vector<Segment>        m_segments;
	std::optional<RayHit>       m_hit;
	sf::Vector2f                m_rayOrigin;
	sf::Vector2f                m_rayEnd;
	float                       m_maxRayDistance = 1000.f;

	// Lab A part 2:
	std::vector<sf::Vector2f>   m_visPoints;
	static constexpr int        kRayCounts[] = { 16, 32, 64, 128, 256, 512 };
	int                         m_rayCountIndex = 3;   // 128
	bool                        m_showRays = true;
	bool                        m_showFan = true;

	void spawnWalls();
	void sRaycast();
	void drawVisibilityDebug();

	void init();
	void update();
	void quit();
	void spawnPlayer();
	
	void sGui();
	void sRender();
	void sPlayerInput();
	void sMovement();
	void sCollision();

	template <typename T>
	T rng(T min, T max) const;

public:
	Game::Game()
		: m_window(sf::VideoMode({ 800u, 600u }), "Visibility Sandbox")
		, m_debugUI(m_window)
	{
		m_window.setFramerateLimit(60);
	}

	void run();
};

template <typename T>
T Game::rng(T min, T max) const
{
	static_assert(std::is_arithmetic_v<T>, "rng<T>: T must be an arithmetic type");
	static_assert(!std::is_same_v<T, bool>, "rng<T>: bool is not supported by uniform_int_distribution");
	static_assert(!std::is_same_v<T, char> && !std::is_same_v<T, signed char> && !std::is_same_v<T, unsigned char>,
		"rng<T>: char types are not supported by uniform_int_distribution; use int instead");

	using Dist = std::conditional_t<std::is_integral_v<T>,
		std::uniform_int_distribution<T>,
		std::uniform_real_distribution<T>>;

	Dist dist(min, max);
	return dist(m_engine); // m_engine is seeded once at construction, not rebuilt per call
}