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
#include "Box2D\Dynamics\b2Fixture.h"
class b2Fixture;
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
struct PlayerEntity
{
	std::string type;
	Audiosystem* m_audioSystem;
	b2Body* gatherer_body;
	b2Body* defender_body;
	PlayerEntity* m_link;
	bool m_respawn = false;
	sf::CircleShape gatherer;
	sf::CircleShape defender;
	b2Vec2 m_respawn_pos;
	void SetLink(PlayerEntity* link)
	{
		m_link = link;
	}
};
struct Gatherer : public PlayerEntity
{



};
struct Defender : public PlayerEntity
{

	b2Vec2 m_respawn_pos;

};
struct Player
{
	Player()
	{
		timer.reset();
		timer.stop();
	}
	thor::StopWatch timer;
	sf::CircleShape defender;
	sf::CircleShape gatherer;

	Gatherer* m_gatherer;
	Defender* m_defender;

	sf::Color* GetStartColor() { return m_Start_color; }
	sf::Color* m_Start_color;
	sf::Texture* texture;

};

void SetStartingColors(std::vector<Player*> players, int value)
{
	int i = value;
	if (0 < i)
	{
		players[0]->m_gatherer->gatherer.setFillColor(sf::Color::Blue); players[0]->m_Start_color = new sf::Color(players[0]->m_gatherer->gatherer.getFillColor());
		players[0]->m_defender->defender.setFillColor(players[0]->m_gatherer->gatherer.getFillColor());
	}
	if (1 < i)
	{
		players[1]->m_gatherer->gatherer.setFillColor(sf::Color::Red); players[1]->m_Start_color = new sf::Color(players[1]->m_gatherer->gatherer.getFillColor());
		players[1]->m_defender->defender.setFillColor(players[1]->m_gatherer->gatherer.getFillColor());
	}
	if (2 < i)
	{
		players[2]->m_gatherer->gatherer.setFillColor(sf::Color::Magenta); players[2]->m_Start_color = new sf::Color(players[2]->m_gatherer->gatherer.getFillColor());
		players[2]->m_defender->defender.setFillColor(players[2]->m_gatherer->gatherer.getFillColor());
	}
	if (3 < i)
	{
		players[3]->m_gatherer->gatherer.setFillColor(sf::Color::Cyan); players[3]->m_Start_color = new sf::Color(players[3]->m_gatherer->gatherer.getFillColor());
		players[3]->m_defender->defender.setFillColor(players[3]->m_gatherer->gatherer.getFillColor());
	}
}

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

			PlayerEntity* playerA = static_cast<PlayerEntity*>(bodyUserDataA);
			PlayerEntity* playerB = static_cast<PlayerEntity*>(bodyUserDataB);
			playerA->m_audioSystem->playSound("Ball_Contact");
			if (playerA->type == "defender" && playerB->type == "gatherer")
			{
				if (playerA->m_link != playerB)
				{
					std::cout << "YOU ARE MY SWORN ENEMY!!!!!" << std::endl;
					playerB->gatherer_body->SetTransform(playerB->m_respawn_pos, 90);
				}
				else if (playerA->m_link == playerB)
				{
					std::cout << "YOU ARE MY FRIEND?????" << std::endl;
				}

			}
			else if (playerB->type == "defender" && playerA->type == "gatherer")
			{
				if (playerB->m_link != playerA)
				{
					std::cout << "!!!!!!!!!!!!YOU ARE MY SWORN ENEMY!!!!!" << std::endl;

				}
				else if (playerB->m_link == playerA)
				{
					std::cout << "!!!!!!!!!!!!!!!YOU ARE MY FRIEND?????" << std::endl;
				}
			}

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
			PlayerEntity* playerA = static_cast<PlayerEntity*>(bodyUserDataA);
			PlayerEntity* playerB = static_cast<PlayerEntity*>(bodyUserDataB);

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

	//Sound & music
	Audiosystem* audioSystem = new Audiosystem();
	audioSystem->createSound("Ball_Contact", "../assets/sound/test.ogg");

	// load textures
	sf::Texture cursor;
	cursor.loadFromFile("../cursor.png");

	sf::Texture backgroundTex = sf::Texture();
	backgroundTex.loadFromFile("../assets/png/bgr.png", sf::IntRect(0, 0, 1920, 1080));
	sf::Sprite background = sf::Sprite(backgroundTex);


	// Physics world
	b2Vec2 gravity(0.0f, 0.0f);
	b2World* world = new b2World(gravity);
	world->SetAllowSleeping(true); // Allow Box2D to exclude resting bodies from simulation

	WorldContactListener myContactListener;
	world->SetContactListener(&myContactListener);


	createWall(world, sf::Vector2f(0, 0), sf::Vector2f(1920, 0));
	createWall(world, sf::Vector2f(1920, 0), sf::Vector2f(1920, 1080));
	createWall(world, sf::Vector2f(1920, 1080), sf::Vector2f(0, 1080));
	createWall(world, sf::Vector2f(0, 1080), sf::Vector2f(0, 0));

	sf::CircleShape innerCircle;
	innerCircle.setRadius(200);
	innerCircle.setFillColor(sf::Color(255, 0, 0, 50));
	innerCircle.setOrigin(200, 200);
	innerCircle.setPosition(sf::Vector2f(1920 / 2, 1080 / 2));


	std::vector<sf::Vector2f> defenderPositions;
	std::vector<sf::Vector2f> gathererPositions;

	defenderPositions.emplace_back(100, 100);
	defenderPositions.emplace_back(1820, 100);
	defenderPositions.emplace_back(1820, 980);
	defenderPositions.emplace_back(100, 980);

	gathererPositions.emplace_back(50, 50);
	gathererPositions.emplace_back(1870, 50);
	gathererPositions.emplace_back(1870, 1030);
	gathererPositions.emplace_back(50, 1030);

	// create player
	std::vector<Player*> players;
	for (int i = 0; i < numDevices; i++)
	{
		Player* player = new Player();

		player->m_gatherer = new Gatherer();
		player->m_defender = new Defender();
		player->m_gatherer->SetLink(player->m_defender);
		player->m_defender->SetLink(player->m_gatherer);

		player->m_defender->m_audioSystem = audioSystem;
		player->m_gatherer->m_audioSystem = audioSystem;


		player->m_defender->defender.setFillColor(sf::Color::Green);
		player->m_defender->defender.setRadius(30.f);
		player->m_defender->defender.setOrigin(30.f, 30.f);
		player->m_defender->defender.setPosition(defenderPositions[i]);
		player->m_defender->m_respawn_pos = gameToPhysicsUnits(defenderPositions[i]);

		player->m_gatherer->gatherer.setFillColor(sf::Color::Green);
		player->m_gatherer->gatherer.setRadius(15.f);
		player->m_gatherer->gatherer.setOrigin(15.f, 15.f);
		player->m_gatherer->gatherer.setPosition(gathererPositions[i]);
		player->m_gatherer->m_respawn_pos = gameToPhysicsUnits(gathererPositions[i]);

		player->m_gatherer->type = "gatherer";
		//player->m_gatherer->gatherer = player->gatherer;

		player->m_defender->type = "defender";
		//player->m_defender->defender = player->defender;

		// Defender body
		{
			b2BodyDef bodyDef;
			bodyDef.position = gameToPhysicsUnits(player->m_defender->defender.getPosition());
			bodyDef.type = b2_dynamicBody;
			bodyDef.linearDamping = 0.3f;
			bodyDef.userData = player->m_defender;
			b2Body* body = world->CreateBody(&bodyDef);
			
			b2CircleShape shape;
			shape.m_radius = gameToPhysicsUnits(player->m_defender->defender.getRadius());

			b2FixtureDef fixtureDef;
			fixtureDef.density = 1;
			fixtureDef.shape = &shape;
			fixtureDef.friction = 0.3f;
			fixtureDef.restitution = 0.6;
			fixtureDef.userData = body->CreateFixture(&fixtureDef);
			player->m_defender->defender_body = body;
		}

		// Gatherer body
		{
			b2BodyDef bodyDef;
			bodyDef.position = gameToPhysicsUnits(player->m_gatherer->gatherer.getPosition());
			bodyDef.type = b2_dynamicBody;
			bodyDef.linearDamping = 0.3f;
			bodyDef.userData = player->m_gatherer;
			b2Body* body = world->CreateBody(&bodyDef);

			b2CircleShape shape;
			shape.m_radius = gameToPhysicsUnits(player->m_gatherer->gatherer.getRadius());

			b2FixtureDef fixtureDef;
			fixtureDef.density = 1;
			fixtureDef.shape = &shape;
			fixtureDef.friction = 0.3f;
			fixtureDef.restitution = 0.6;
			body->CreateFixture(&fixtureDef);
			player->m_gatherer->gatherer_body = body;
		}

		players.push_back(player);

	}
	//Setting the starting colors;
	SetStartingColors(players, numDevices);



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

	actionMap["p1_up"] = thor::Action(sf::Keyboard::W, thor::Action::Hold);
	actionMap["p1_down"] = thor::Action(sf::Keyboard::S, thor::Action::Hold);
	actionMap["p1_left"] = thor::Action(sf::Keyboard::A, thor::Action::Hold);
	actionMap["p1_right"] = thor::Action(sf::Keyboard::D, thor::Action::Hold);

	actionMap["p2_up"] = thor::Action(sf::Keyboard::Up, thor::Action::Hold);
	actionMap["p2_down"] = thor::Action(sf::Keyboard::Down, thor::Action::Hold);
	actionMap["p2_left"] = thor::Action(sf::Keyboard::Left, thor::Action::Hold);
	actionMap["p2_right"] = thor::Action(sf::Keyboard::Right, thor::Action::Hold);

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
				sf::Vector2f playerPosition = player->defender.getPosition();
				if (event.item == 0)
				{
					player->m_defender->defender_body->ApplyLinearImpulse(b2Vec2(10.f * gameToPhysicsUnits(static_cast<float>(event.value)), 0.f), player->m_defender->defender_body->GetWorldCenter(), true);
				}
				if (event.item == 1)
				{
					player->m_defender->defender_body->ApplyLinearImpulse(b2Vec2(0.f, 10.f * gameToPhysicsUnits(static_cast<float>(event.value))), player->m_defender->defender_body->GetWorldCenter(), true);
				}
			}
		}

		if (actionMap.isActive("p1_up"))
		{
			players[0]->m_gatherer->gatherer_body->ApplyLinearImpulse(b2Vec2(0.f, gameToPhysicsUnits(-2.f)), players[0]->m_gatherer->gatherer_body->GetWorldCenter(), true);
		}
		if (actionMap.isActive("p1_down"))
		{
			players[0]->m_gatherer->gatherer_body->ApplyLinearImpulse(b2Vec2(0.f, gameToPhysicsUnits(2.f)), players[0]->m_gatherer->gatherer_body->GetWorldCenter(), true);
		}
		if (actionMap.isActive("p1_left"))
		{
			players[0]->m_gatherer->gatherer_body->ApplyLinearImpulse(b2Vec2(gameToPhysicsUnits(-2.f), 0.f), players[0]->m_gatherer->gatherer_body->GetWorldCenter(), true);
		}
		if (actionMap.isActive("p1_right"))
		{
			players[0]->m_gatherer->gatherer_body->ApplyLinearImpulse(b2Vec2(gameToPhysicsUnits(2.f), 0.f), players[0]->m_gatherer->gatherer_body->GetWorldCenter(), true);
		}

		if (actionMap.isActive("p2_up"))
		{
			players[1]->m_gatherer->gatherer_body->ApplyLinearImpulse(b2Vec2(0.f, gameToPhysicsUnits(-2.f)), players[1]->m_gatherer->gatherer_body->GetWorldCenter(), true);
		}
		if (actionMap.isActive("p2_down"))
		{
			players[1]->m_gatherer->gatherer_body->ApplyLinearImpulse(b2Vec2(0.f, gameToPhysicsUnits(2.f)), players[1]->m_gatherer->gatherer_body->GetWorldCenter(), true);
		}
		if (actionMap.isActive("p2_left"))
		{
			players[1]->m_gatherer->gatherer_body->ApplyLinearImpulse(b2Vec2(gameToPhysicsUnits(-2.f), 0.f), players[1]->m_gatherer->gatherer_body->GetWorldCenter(), true);
		}
		if (actionMap.isActive("p2_right"))
		{
			players[1]->m_gatherer->gatherer_body->ApplyLinearImpulse(b2Vec2(gameToPhysicsUnits(2.f), 0.f), players[1]->m_gatherer->gatherer_body->GetWorldCenter(), true);
		}


		// Cap the speed to max speed for all players
		for (auto &player : players)
		{
			b2Vec2 MAX_VELOCITY(15.f, 15.f);
			if (player->m_defender->defender_body->GetLinearVelocity().x >= MAX_VELOCITY.x)
			{
				player->m_defender->defender_body->SetLinearVelocity(b2Vec2(MAX_VELOCITY.x, player->m_defender->defender_body->GetLinearVelocity().y));
			}
			if (player->m_defender->defender_body->GetLinearVelocity().y >= MAX_VELOCITY.y)
			{
				player->m_defender->defender_body->SetLinearVelocity(b2Vec2(player->m_defender->defender_body->GetLinearVelocity().x, MAX_VELOCITY.y));
			}
			if (player->m_defender->defender_body->GetLinearVelocity().x <= -MAX_VELOCITY.x)
			{
				player->m_defender->defender_body->SetLinearVelocity(b2Vec2(-MAX_VELOCITY.x, player->m_defender->defender_body->GetLinearVelocity().y));
			}
			if (player->m_defender->defender_body->GetLinearVelocity().y <= -MAX_VELOCITY.y)
			{
				player->m_defender->defender_body->SetLinearVelocity(b2Vec2(player->m_defender->defender_body->GetLinearVelocity().x, -MAX_VELOCITY.y));
			}

			b2Vec2 MAX_VELOCITY_GATHERER(4.f, 4.f);
			if (player->m_gatherer->gatherer_body->GetLinearVelocity().x >= MAX_VELOCITY_GATHERER.x)
			{
				player->m_gatherer->gatherer_body->SetLinearVelocity(b2Vec2(MAX_VELOCITY_GATHERER.x, player->m_gatherer->gatherer_body->GetLinearVelocity().y));
			}
			if (player->m_gatherer->gatherer_body->GetLinearVelocity().y >= MAX_VELOCITY_GATHERER.y)
			{
				player->m_gatherer->gatherer_body->SetLinearVelocity(b2Vec2(player->m_gatherer->gatherer_body->GetLinearVelocity().x, MAX_VELOCITY_GATHERER.y));
			}
			if (player->m_gatherer->gatherer_body->GetLinearVelocity().x <= -MAX_VELOCITY_GATHERER.x)
			{
				player->m_gatherer->gatherer_body->SetLinearVelocity(b2Vec2(-MAX_VELOCITY_GATHERER.x, player->m_gatherer->gatherer_body->GetLinearVelocity().y));
			}
			if (player->m_gatherer->gatherer_body->GetLinearVelocity().y <= -MAX_VELOCITY_GATHERER.y)
			{
				player->m_gatherer->gatherer_body->SetLinearVelocity(b2Vec2(player->m_gatherer->gatherer_body->GetLinearVelocity().x, -MAX_VELOCITY_GATHERER.y));
			}
		}
		world->Step(1 / 60.f, 8, 3);



		for (auto &player : players)
		{

			player->m_defender->defender.setPosition(physicsToGameUnits(player->m_defender->defender_body->GetPosition()));
			player->m_gatherer->gatherer.setPosition(physicsToGameUnits(player->m_gatherer->gatherer_body->GetPosition()));



			if (!win)
			{
				float distance = std::sqrtf(
					(player->m_gatherer->gatherer.getPosition().x - innerCircle.getPosition().x) *
					(player->m_gatherer->gatherer.getPosition().x - innerCircle.getPosition().x) +
					(player->m_gatherer->gatherer.getPosition().y - innerCircle.getPosition().y) *
					(player->m_gatherer->gatherer.getPosition().y - innerCircle.getPosition().y));
				if (distance <= innerCircle.getRadius())
				{
					player->m_gatherer->gatherer.setFillColor(sf::Color::Yellow);
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
					if (player->GetStartColor() != nullptr)
					{
						player->m_gatherer->gatherer.setFillColor(*player->GetStartColor());
					}
				}

				if (player->timer.getElapsedTime().asSeconds() >= 15)
				{
					win = true;
					MessageBoxA(window.getSystemHandle(), "A player won", "WIN!!!", 0);
					player->m_gatherer->gatherer.setFillColor(sf::Color::Blue);
				}
			}
		}

		window.clear(sf::Color::Black);
		window.draw(background);

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
			window.draw(player->m_defender->defender);
			window.draw(player->m_gatherer->gatherer);
		}
		window.display();
	}
	for (int i = 0; i < players.size(); i++)
	{
		delete players[i]->m_Start_color;
		players[i]->m_Start_color = nullptr;
		delete players[i];
		players[i] = nullptr;
	}
	ManyMouse_Quit();
}

