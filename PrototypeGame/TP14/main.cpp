#include "SFML/Graphics.hpp"

#include "Thor/Shapes/ConcaveShape.hpp"
#include "Thor/Shapes/Shapes.hpp"
#include "Thor/Math/Random.hpp"
#include "Thor/Input.hpp"
#include "Thor/Input/ActionMap.hpp"
#include "Thor/Math/Trigonometry.hpp"
#include "Thor\Time.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>
#include <stdio.h>
#include <assert.h>
#include <xmmintrin.h>

#include "Box2D/Box2D.h"
#include "manymouse.hpp"
#include "AudioSystem.h"

#include "Stalker.h"

b2Vec2 gameToPhysicsUnits(sf::Vector2f p_unit)
{
	return b2Vec2(p_unit.x / 32.f, p_unit.y / 32.f);
}
b2Vec2 gameToPhysicsUnits(sf::Vector2i p_unit)
{
	return b2Vec2(static_cast<float>(p_unit.x) / 32.f, static_cast<float>(p_unit.y) / 32.f);
}

float gameToPhysicsUnits(float p_unit)
{
	return p_unit / 32.f;
}

sf::Vector2f physicsToGameUnits(float p_x, float p_y)
{
	return sf::Vector2f(p_x * 32.f, p_y * 32.f);
}
sf::Vector2f physicsToGameUnits(b2Vec2 p_position)
{
	return sf::Vector2f(p_position.x * 32.f, p_position.y * 32.f);
}
void createWall(b2World* p_world, sf::Vector2f p_vec1, sf::Vector2f p_vec2)
{
	// The body
	b2BodyDef bodyDef;
	bodyDef.position = b2Vec2(0, 0);
	bodyDef.type = b2_staticBody;
	bodyDef.angle = 0;
	b2Body* body = p_world->CreateBody(&bodyDef);

	// The shape
	b2EdgeShape shape;
	b2Vec2 v1 = gameToPhysicsUnits(p_vec1);
	b2Vec2 v2 = gameToPhysicsUnits(p_vec2);
	shape.Set(v1, v2);

	// The fixture
	b2FixtureDef fixtureDef;
	fixtureDef.density = 1;
	fixtureDef.shape = &shape;
	fixtureDef.friction = 1;
	fixtureDef.restitution = 1;
	body->CreateFixture(&fixtureDef);
}

struct Player
{
	Player()
	{
		timer.reset();
		timer.stop();
	}
	thor::StopWatch timer;
	sf::CircleShape shape;
	sf::CircleShape gatherer;
	b2Body* body;
	Audiosystem* m_audioSystem;
};

Stalker* CreateStalker(b2World* world)
{
	Stalker* stalker = new Stalker;
	stalker->shape.setFillColor(sf::Color::Blue);
	stalker->shape.setRadius(30.f);
	stalker->shape.setOrigin(30.f, 30.f);
	stalker->shape.setPosition(sf::Vector2f(thor::random(300.f, 600.f), thor::random(300.f, 600.f)));

	b2BodyDef bodyDef;
	bodyDef.position = gameToPhysicsUnits(stalker->shape.getPosition());
	bodyDef.type = b2_dynamicBody;
	bodyDef.linearDamping = 0.3f;
	bodyDef.userData = stalker;
	b2Body* body = world->CreateBody(&bodyDef);

	b2CircleShape shape;
	shape.m_radius = gameToPhysicsUnits(stalker->shape.getRadius());

	b2FixtureDef fixtureDef;
	fixtureDef.density = 1;
	fixtureDef.shape = &shape;
	fixtureDef.friction = 0.3f;
	fixtureDef.restitution = 1;
	body->CreateFixture(&fixtureDef);
	stalker->body = body;
	
	return stalker;
}

class WorldContactListener : public b2ContactListener
{
	void BeginContact(b2Contact* p_contact)
	{
		playerContactBegin(p_contact);
	}

	void EndContact(b2Contact* p_contact)
	{
		playerContactEnd(p_contact);
	}

	void playerContactBegin(b2Contact* p_contact)
	{
		b2Fixture* fixtureA = p_contact->GetFixtureA();
		b2Fixture* fixtureB = p_contact->GetFixtureB();

		void* bodyUserDataA = fixtureA->GetBody()->GetUserData();
		void* bodyUserDataB = fixtureB->GetBody()->GetUserData();

		if (bodyUserDataA != nullptr && bodyUserDataB != nullptr)
		{
			Player* playerA = static_cast<Player*>(bodyUserDataA);
			Player* playerB = static_cast<Player*>(bodyUserDataB);

			playerA->m_audioSystem->playSound("Ball_Contact");
		}
	}

	void playerContactEnd(b2Contact* p_contact)
	{
		b2Fixture* fixtureA = p_contact->GetFixtureA();
		b2Fixture* fixtureB = p_contact->GetFixtureB();

		void* bodyUserDataA = fixtureA->GetBody()->GetUserData();
		void* bodyUserDataB = fixtureB->GetBody()->GetUserData();

		if (bodyUserDataA != nullptr && bodyUserDataB != nullptr)
		{
			Player* playerA = static_cast<Player*>(bodyUserDataA);
			Player* playerB = static_cast<Player*>(bodyUserDataB);
		}
	}
};

int main(int argc, char *argv[])
{
	for (int i = 0; i < argc; i++)
	{
		std::cout << "Argument " << i << ": " << argv[i] << std::endl;
	}

	sf::Texture t;
	t.loadFromFile("../d.psd");
	sf::Sprite s;
	s.setTexture(t);

	int numDevices = ManyMouse_Init();

	printf("Driver: %s\n", ManyMouse_DriverName());
	for (int i = 0; i < numDevices; i++)
	{
		std::cout << "Mouse " << i << ": " << ManyMouse_DeviceName(i) << std::endl;
	}

	Audiosystem* audioSystem = new Audiosystem();
	audioSystem->createSound("Ball_Contact", "../assets/sound/test.ogg");

	// load textures
	sf::Texture cursor;
	cursor.loadFromFile("../cursor.png");

	// Physics world
	b2Vec2 gravity(0.0f, 0.0f);
	b2World* world = new b2World(gravity);
	world->SetAllowSleeping(true); // Allow Box2D to exclude resting bodies from simulation

	WorldContactListener myContactListener;
	world->SetContactListener(&myContactListener);

	createWall(world, sf::Vector2f(50, 50), sf::Vector2f(1870, 50));
	createWall(world, sf::Vector2f(1870, 50), sf::Vector2f(1870, 1030));
	createWall(world, sf::Vector2f(1870, 1030), sf::Vector2f(50, 1030));
	createWall(world, sf::Vector2f(50, 1030), sf::Vector2f(50, 50));

	sf::CircleShape innerCircle;
	innerCircle.setRadius(200);
	innerCircle.setFillColor(sf::Color(255, 0, 0, 50));
	innerCircle.setOrigin(200,200);
	innerCircle.setPosition(sf::Vector2f(1920 / 2, 1080 / 2));

	// create player
	std::vector<Player*> players;
	for (int i = 0; i < numDevices; i++)
	{
		Player* player = new Player();
		player->m_audioSystem = audioSystem;
		player->shape.setFillColor(sf::Color::Green);
		player->shape.setRadius(30.f);
		player->shape.setOrigin(30.f, 30.f);
		player->shape.setPosition(sf::Vector2f(thor::random(300.f, 600.f), thor::random(300.f, 600.f)));

		// Create gatherer

		// The body
		b2BodyDef bodyDef;
		bodyDef.position = gameToPhysicsUnits(player->shape.getPosition());
		bodyDef.type = b2_dynamicBody;
		bodyDef.linearDamping = 0.3f;
		bodyDef.userData = player;
		b2Body* body = world->CreateBody(&bodyDef);

		// The shape
		b2CircleShape shape;
		shape.m_radius = gameToPhysicsUnits(player->shape.getRadius());

		// The fixture
		b2FixtureDef fixtureDef;
		fixtureDef.density = 1;
		fixtureDef.shape = &shape;
		fixtureDef.friction = 0.3f;
		fixtureDef.restitution = 0.6;
		body->CreateFixture(&fixtureDef);
		player->body = body;
		players.push_back(player);
	}

	// Create render window
	sf::RenderWindow window(sf::VideoMode(1920, 1080), "Doodlemeat", sf::Style::None);
	window.setVerticalSyncEnabled(true);
	window.setMouseCursorVisible(false);
	window.setKeyRepeatEnabled(false);

	sf::Texture box;
	box.loadFromFile("../assets/png/box_32.png");
	box.setRepeated(true);

	thor::ActionMap<std::string> actionMap;
	actionMap["test"] = thor::Action(sf::Keyboard::X, thor::Action::PressOnce);
	actionMap["test2"] = thor::Action(sf::Mouse::Left, thor::Action::PressOnce);

	thor::ActionMap<std::string> actionMap2;
	actionMap2["test3"] = thor::Action(sf::Mouse::Left, thor::Action::PressOnce);
	bool win = false;

	while (window.isOpen())
	{
		audioSystem->update();
		actionMap.update(window);
		actionMap2.update(window);

		ManyMouseEvent event;
		while (ManyMouse_PollEvent(&event))
		{
			Player* player = players[event.device];

			if (event.type == MANYMOUSE_EVENT_RELMOTION)
			{
				sf::Vector2f playerPosition = player->shape.getPosition();
				if (event.item == 0)
				{
					player->body->ApplyLinearImpulse(b2Vec2(10.f * gameToPhysicsUnits(static_cast<float>(event.value)), 0.f), player->body->GetWorldCenter(), true);
				}
				if (event.item == 1)
				{
					player->body->ApplyLinearImpulse(b2Vec2(0.f, 10.f * gameToPhysicsUnits(static_cast<float>(event.value))), player->body->GetWorldCenter(), true);
				}
			}
		}
		world->Step(1 / 60.f, 8, 3);

		for (auto &player : players)
		{
			player->shape.setPosition(physicsToGameUnits(player->body->GetPosition()));

			if (!win)
			{
				float distance = std::sqrtf(
						(player->shape.getPosition().x - innerCircle.getPosition().x) *
						(player->shape.getPosition().x - innerCircle.getPosition().x) +
						(player->shape.getPosition().y - innerCircle.getPosition().y) *
						(player->shape.getPosition().y - innerCircle.getPosition().y));
				if (distance <= innerCircle.getRadius())
				{
					player->shape.setFillColor(sf::Color::Yellow);
					if (!player->timer.isRunning())
					{
						player->timer.start();
					}
				}
				else
				{
					if (player->timer.isRunning())
					{
						player->timer.stop();
					}
					player->shape.setFillColor(sf::Color::Green);
				}

				if (player->timer.getElapsedTime().asSeconds() >= 15)
				{
					win = true;
					MessageBoxA(window.getSystemHandle(), "A player won", "WIN!!!", 0);
					player->shape.setFillColor(sf::Color::Blue);
				}
			}
		}

		std::cout << players[0]->body->GetLinearVelocity().x << std::endl;

		window.clear(sf::Color::Black);

		for (b2Body* bodyIt = world->GetBodyList(); bodyIt != 0; bodyIt = bodyIt->GetNext())
		{
			if (bodyIt->GetType() == b2_staticBody)
			{
				b2Shape* shape = bodyIt->GetFixtureList()[0].GetShape();
				if (shape->GetType() == b2Shape::e_edge)
				{
					b2EdgeShape* edge_shape = static_cast<b2EdgeShape*>(shape);
					sf::Vertex line[] =
					{
						sf::Vertex(physicsToGameUnits(edge_shape->m_vertex1.x, edge_shape->m_vertex1.y)),
						sf::Vertex(physicsToGameUnits(edge_shape->m_vertex2.x, edge_shape->m_vertex2.y))
					};
					window.draw(line, 2, sf::Lines);
				}
			}
		}

		window.draw(innerCircle);
		for (auto &player : players)
		{
			window.draw(player->shape);
		}
		window.draw(s);
		//window.draw(stalker->shape);
		window.display();
	}
	ManyMouse_Quit();
}