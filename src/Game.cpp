#include "Components.h"
#include "Game.h"
#include "Geometry.h"
#include "Vec2.h"

//#ifndef NDEBUG
//runGeometryTests();
//#endif

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/VideoMode.hpp>

#include <iterator>
#include <memory>

namespace
{
	sf::Vector2f toSfVec2(const Vec2& v) { return { v.x, v.y }; }
}

void Game::run()
{
	init();

	while (m_running)
	{
		update();
	}
	m_window.close();
}

void Game::update()
{
	m_entityManager.update();

	sPlayerInput();
	sMovement();
	sCollision();
	sRaycast();
	sRender();
}

void Game::quit()
{
	m_running = false;
}

void Game::init()
{
	m_window.create(sf::VideoMode({ 800, 600 }), "Magic!");
	m_window.setFramerateLimit(60);

	spawnPlayer();
	m_running = true;
}

void Game::spawnPlayer()
{
	auto player = m_entityManager.addEntity("player");
	
	float playerRadius = 40.f;
	int shapePoints = 10;
	sf::Color fillColor(sf::Color::Transparent);
	sf::Color outlineColor(sf::Color::Red);
	float thickness = 3.f;

	player->addComponent<CShape>(playerRadius, shapePoints, fillColor, outlineColor, thickness);
	
	// spawn player at a random position within the window
	float maxPosX = static_cast<float>(m_window.getSize().x - playerRadius);
	float maxPosY = static_cast<float>(m_window.getSize().y - playerRadius);
	
	float posX = rng(playerRadius, maxPosX);
	float posY = rng(playerRadius, maxPosY);

	player->addComponent<CTransform>(Vec2(posX, posY));

	float boxSize = playerRadius * 2;
	player->addComponent<CBoundingBox>(Vec2(boxSize, boxSize));
	
	player->addComponent<CInput>();
	
	m_player = player;
}

void Game::sRender()
{
	m_window.clear();

	for (auto& e : m_entityManager.getEntities())
	{
		if (!e->hasComponent<CTransform>() || !e->hasComponent<CShape>()) continue;

		auto& entityCircle = e->getComponent<CShape>().value().circle;
		auto& entityPos = e->getComponent<CTransform>().value().pos;
		
		entityCircle.setPosition({ entityPos.x, entityPos.y });

		m_window.draw(entityCircle);
		
		if (m_drawCollisison)
		{
			if (e->hasComponent<CBoundingBox>())
			{
				auto& eBox = e->getComponent<CBoundingBox>().value().size;
			
				sf::RectangleShape box({ eBox.x, eBox.y });

				box.setOrigin({ eBox.x / 2.f, eBox.y / 2.f });
				box.setPosition({ entityPos.x, entityPos.y });
				box.setOutlineColor(sf::Color::Green);
				box.setFillColor(sf::Color::Transparent);
				box.setOutlineThickness(1.f);
			
				m_window.draw(box);
			}
			else if (e->hasComponent<CBoundingCircle>())
			{
				auto& eCircle = e->getComponent<CBoundingCircle>().value();

				sf::CircleShape circle(eCircle.radius);

				circle.setOrigin({ eCircle.radius, eCircle.radius });
				circle.setPosition({ entityPos.x, entityPos.y });
				circle.setOutlineColor(sf::Color::Green);
				circle.setFillColor(sf::Color::Transparent);
				circle.setOutlineThickness(1.f);

				m_window.draw(circle);
			}
		}
	}

	m_window.display();
}

void Game::sCollision()
{
	// player circle x window collision (in this case, player's bounding box or circle is not relevant for window collision)
	auto windowSize = m_window.getView().getSize();

	float playerRadius = m_player->getComponent<CShape>().value().circle.getRadius();
	
	auto& playerTransform = m_player->getComponent<CTransform>().value();
	
	// x axis
	if (
		// left
		playerTransform.pos.x - playerRadius <= 0.0f ||
		// right
		playerTransform.pos.x + playerRadius >= static_cast<float>(windowSize.x)
		)
	{
		playerTransform.pos.x = playerTransform.prevPos.x;
	}

	// y axis
	if (
		// bottom
		playerTransform.pos.y + playerRadius >= static_cast<float>(windowSize.y) ||
		// top
		playerTransform.pos.y - playerRadius <= 0.0f
		)
	{
		playerTransform.pos.y = playerTransform.prevPos.y;
	}
}

void Game::sMovement()
{
	auto& pTransform = m_player->getComponent<CTransform>().value();
	auto& pInput = m_player->getComponent<CInput>().value();

	// reset player velocity on each frame
	pTransform.velocity = { 0.0f, 0.0f };

	float speed = 8.f;
	if (pInput.up)
	{
		pTransform.velocity.y = -speed;
	}
	else if (pInput.down)
	{
		pTransform.velocity.y = speed;
	}
	else if (pInput.left)
	{
		pTransform.velocity.x = -speed;
	}
	else if (pInput.right)
	{
		pTransform.velocity.x = speed;
	}

	// update players position
	pTransform.prevPos = pTransform.pos;
	pTransform.pos += pTransform.velocity;
}

void Game::sPlayerInput()
{
	while (const std::optional event = m_window.pollEvent())
	{
		if (event->is<sf::Event::Closed>())
		{
			quit();
		}

		if (const auto* windowResized = event->getIf<sf::Event::Resized>())
		{
			sf::FloatRect visibleArea({ 0.0f, 0.0f }, { static_cast<float>(windowResized->size.x), static_cast<float>(windowResized->size.y) });

			m_window.setView(sf::View(visibleArea));
		}

		if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
		{
			switch (keyPressed->code)
			{
			case sf::Keyboard::Key::W:
				m_player->getComponent<CInput>().value().up = true;
				break;
			case sf::Keyboard::Key::S:
				m_player->getComponent<CInput>().value().down = true;
				break;
			case sf::Keyboard::Key::A:
				m_player->getComponent<CInput>().value().left = true;
				break;
			case sf::Keyboard::Key::D:
				m_player->getComponent<CInput>().value().right = true;
				break;
			case sf::Keyboard::Key::C:
				m_drawCollisison = !m_drawCollisison;
				break;
			case sf::Keyboard::Key::R:
				m_showRays = !m_showRays;
				break;
			case sf::Keyboard::Key::F:
				m_showFan = !m_showFan;
				break;
			case sf::Keyboard::Key::Up:
				if (m_rayCountIndex < static_cast<int>(std::size(kRayCounts)) - 1) ++m_rayCountIndex;
				break;
			case sf::Keyboard::Key::Down:
				if (m_rayCountIndex > 0) --m_rayCountIndex;
				break;
			default:
				break;
			}
		}
		else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>())
		{
			switch (keyReleased->code)
			{
			case sf::Keyboard::Key::W:
				m_player->getComponent<CInput>().value().up = false;
				break;
			case sf::Keyboard::Key::S:
				m_player->getComponent<CInput>().value().down = false;
				break;
			case sf::Keyboard::Key::A:
				m_player->getComponent<CInput>().value().left = false;
				break;
			case sf::Keyboard::Key::D:
				m_player->getComponent<CInput>().value().right = false;
				break;
			default:
				break;
			}
		}
	}
}

void Game::spawnWalls()
{
	const float w = static_cast<float>(m_window.getSize().x);
	const float h = static_cast<float>(m_window.getSize().y);

	m_segments = {
		// boundary box: keeps the scene enclosed
		Segment{ { 0.f, 0.f }, { w,   0.f } },
		Segment{ { w,   0.f }, { w,   h   } },
		Segment{ { w,   h   }, { 0.f, h   } },
		Segment{ { 0.f, h   }, { 0.f, 0.f } },
		// interior walls at awkward angles
		Segment{ { 200.f, 150.f }, { 350.f, 300.f } },   // diagonal
		Segment{ { 500.f, 100.f }, { 500.f, 350.f } },   // vertical
		Segment{ { 150.f, 450.f }, { 400.f, 450.f } },   // horizontal
		Segment{ { 600.f, 420.f }, { 620.f, 420.f } },   // short wall, for the Step 7 experiment
	};
}

void Game::sRaycast()
{
	m_rayOrigin = toSfVec2(m_player->getComponent<CTransform>().value().pos);

	const sf::Vector2f mouseWorld = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
	const sf::Vector2f toMouse = mouseWorld - m_rayOrigin;

	// TODO 1: guard zero length BEFORE normalizing (normalized() asserts on a zero vector).
	//         If the mouse is exactly on the origin, use a fallback direction like (1, 0).
	const sf::Vector2f direction = /* TODO */ toMouse.normalized();

	m_hit = castRay(Ray{ m_rayOrigin, direction }, m_segments, m_maxRayDistance);

	// TODO 2: m_rayEnd = hit point if m_hit, otherwise origin + direction * m_maxRayDistance

	// Lab A part 2: leave this commented out until Step 6
	// m_visPoints = castUniformRays(m_rayOrigin, m_segments, kRayCounts[m_rayCountIndex], m_maxRayDistance);
}

void Game::drawVisibilityDebug()
{
	// 1. visibility fan (Lab A part 2)
	if (m_showFan && !m_visPoints.empty())
	{
		sf::VertexArray fan(sf::PrimitiveType::TriangleFan);
		const sf::Color fanColor(255, 255, 0, 70);

		fan.append({ m_rayOrigin, fanColor });
		for (const sf::Vector2f& p : m_visPoints)
			fan.append({ p, fanColor });
		// TODO: close the fan by appending the first point again. Why is it necessary?

		m_window.draw(fan);
	}

	// 2. all debug rays (Lab A part 2)
	if (m_showRays && !m_visPoints.empty())
	{
		sf::VertexArray rays(sf::PrimitiveType::Lines);
		const sf::Color rayColor(0, 200, 255, 80);
		for (const sf::Vector2f& p : m_visPoints)
		{
			rays.append({ m_rayOrigin, rayColor });
			rays.append({ p, rayColor });
		}
		m_window.draw(rays);
	}

	// 3. walls
	for (const Segment& s : m_segments)
	{
		const sf::Vertex line[] = { { s.start, sf::Color::White }, { s.end, sf::Color::White } };
		m_window.draw(line, 2, sf::PrimitiveType::Lines);
	}

	// 4. the Checkpoint 1 mouse ray and hit marker
	{
		const sf::Vertex line[] = { { m_rayOrigin, sf::Color::Yellow }, { m_rayEnd, sf::Color::Yellow } };
		m_window.draw(line, 2, sf::PrimitiveType::Lines);
	}

	if (m_hit)
	{
		sf::CircleShape marker(5.f);
		marker.setOrigin({ 5.f, 5.f });
		marker.setPosition(m_hit->point);
		marker.setFillColor(sf::Color::Red);
		m_window.draw(marker);
	}
}