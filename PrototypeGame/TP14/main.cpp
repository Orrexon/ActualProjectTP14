#include "SFML/Graphics.hpp"

#include <sstream>
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

	sf::Vector2f m_textposition;
	bool IsGatheringTime = false;
	sf::CircleShape gatherer;
	sf::CircleShape defender;
	b2Vec2 m_respawn_pos;
	void SetLink(PlayerEntity* link)
	{
		m_link = link;
	}
	bool GathererIsHit = false;
};
struct Gatherer : public PlayerEntity
{

	sf::Sprite m_sprite;

};
struct Defender : public PlayerEntity
{
	sf::Sprite m_sprite;
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

	b2CircleShape shape1;
	shape1.m_radius = gameToPhysicsUnits(stalker->shape.getRadius());

	b2FixtureDef fixtureDef;
	fixtureDef.density = 1;
	fixtureDef.shape = &shape1;
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
					playerB->GathererIsHit = true;

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
					playerA->GathererIsHit = true;
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

	//background
	sf::Texture backgroundTex = sf::Texture();
	backgroundTex.loadFromFile("../assets/png/masker-mot-bakgrund.png", sf::IntRect(0, 0, 1920, 1080));
	sf::Sprite background = sf::Sprite(backgroundTex);

	//stone
	sf::Texture stone = sf::Texture();
	stone.loadFromFile("../assets/png/stone.png", sf::IntRect(0, 0, 194, 121));
	sf::Sprite s_stone1 = sf::Sprite(stone);
	s_stone1.setPosition(sf::Vector2f(thor::random(100.f, 1000.f), thor::random(100.f, 1000.f)));
	sf::Sprite s_stone2 = sf::Sprite(stone);
	s_stone2.setPosition(sf::Vector2f(thor::random(100.f, 1000.f), thor::random(100.f, 1000.f)));
	sf::Sprite s_stone3 = sf::Sprite(stone);
	s_stone3.setPosition(sf::Vector2f(thor::random(100.f, 1000.f), thor::random(100.f, 1000.f)));
	std::vector<sf::Sprite> stones;
	stones.push_back(s_stone1);
	stones.push_back(s_stone2);
	stones.push_back(s_stone3);

	//GUI
	sf::Texture GUI = sf::Texture();
	GUI.loadFromFile("../assets/png/gui2.png", sf::IntRect(0, 0, 1920, 1080));
	sf::Sprite s_GUI = sf::Sprite(GUI);

	//font
	sf::Font font; font.loadFromFile("../assets/BRLNSR.TTF");
	sf::Vector2f redsText(10.f,580.f);
	sf::Vector2f bluesText(10.f, 810.f);
	sf::Vector2f yellowsText(10.f, 350.f);
	sf::Vector2f purplesText(10.f, 100.f);
	std::vector<sf::Vector2f> textpositions;
	textpositions.push_back(redsText);
	textpositions.push_back(bluesText);
	textpositions.push_back(yellowsText);
	textpositions.push_back(purplesText);

	//Red gatherer and defender
	sf::Texture red_g = sf::Texture();
	sf::Texture red_d = sf::Texture();
	red_g.loadFromFile("../assets/png/red_g.png", sf::IntRect(0, 0, 163, 164));
	red_d.loadFromFile("../assets/png/red_d.png", sf::IntRect(0, 0, 315, 336));

	//blue gatherer and defender
	sf::Texture blue_g = sf::Texture();
	sf::Texture blue_d = sf::Texture();
	blue_g.loadFromFile("../assets/png/blue_g.png", sf::IntRect(0, 0, 163, 164));
	blue_d.loadFromFile("../assets/png/blue_d.png", sf::IntRect(0, 0, 315, 336));

	//yellow gatherer and defender
	sf::Texture yellow_g = sf::Texture();
	sf::Texture yellow_d = sf::Texture();
	yellow_g.loadFromFile("../assets/png/yellow_g.png", sf::IntRect(0, 0, 163, 164));
	yellow_d.loadFromFile("../assets/png/yellow_d.png", sf::IntRect(0, 0, 315, 336));

	//purple gatherer and defender
	sf::Texture purple_g = sf::Texture();
	sf::Texture purple_d = sf::Texture();
	purple_g.loadFromFile("../assets/png/purple_g.png", sf::IntRect(0, 0, 163, 164));
	purple_d.loadFromFile("../assets/png/purple_d.png", sf::IntRect(0, 0, 315, 336));

	std::vector<sf::Sprite> g_sprites;
	std::vector<sf::Sprite> d_sprites;
	//the sprites
	sf::Sprite s_red_g = sf::Sprite(red_g);
	sf::Sprite s_red_d = sf::Sprite(red_d);
	s_red_g.setScale(0.75f,0.75f);
	s_red_d.setScale(0.75f, 0.75f);
	g_sprites.push_back(s_red_g);
	d_sprites.push_back(s_red_d);

	sf::Sprite s_blue_g = sf::Sprite(blue_g);
	sf::Sprite s_blue_d = sf::Sprite(blue_d);
	s_blue_g.setScale(0.75f, 0.75f);
	s_blue_d.setScale(0.75f, 0.75f);
	g_sprites.push_back(s_blue_g);
	d_sprites.push_back(s_blue_d);

	sf::Sprite s_yellow_g = sf::Sprite(yellow_g);
	sf::Sprite s_yellow_d = sf::Sprite(yellow_d);
	g_sprites.push_back(s_yellow_g);
	d_sprites.push_back(s_yellow_d);

	sf::Sprite s_purple_g = sf::Sprite(purple_g);
	sf::Sprite s_purple_d = sf::Sprite(purple_d);
	g_sprites.push_back(s_purple_g);
	d_sprites.push_back(s_purple_d);

	// Physics world
	b2Vec2 gravity(0.0f, 0.0f);
	b2World* world = new b2World(gravity);
	world->SetAllowSleeping(true); // Allow Box2D to exclude resting bodies from simulation


	WorldContactListener myContactListener;
	world->SetContactListener(&myContactListener);


	createWall(world, sf::Vector2f(0, 0), sf::Vector2f(1920, 0));
	createWall(world, sf::Vector2f(1920, 0), sf::Vector2f(1920, 1080));
	createWall(world, sf::Vector2f(1920, 1080), sf::Vector2f(0, 1080));
	createWall(world, sf::Vector2f(102, 1080), sf::Vector2f(102, 0));

	sf::CircleShape innerCircle;
	innerCircle.setRadius(200);
	innerCircle.setFillColor(sf::Color(255, 0, 0, 50));
	innerCircle.setOrigin(200, 200);
	innerCircle.setPosition(sf::Vector2f(1920 / 2, 1080 / 2));


	std::vector<sf::Vector2f> defenderPositions;
	std::vector<sf::Vector2f> gathererPositions;

	defenderPositions.emplace_back(102 + 100, 250 - 150);
	defenderPositions.emplace_back(1820, 250 - 150);
	defenderPositions.emplace_back(1820, 980);
	defenderPositions.emplace_back(102 + 100, 980);

	gathererPositions.emplace_back(102 + 50, 175 - 150);
	gathererPositions.emplace_back(1870, 175 - 150);
	gathererPositions.emplace_back(1870, 1030);
	gathererPositions.emplace_back(102 + 50, 1030);

	// create player
	std::vector<Player*> players;
	for (int i = 0; i < numDevices; i++)
	{
		Player* player = new Player();

		player->m_gatherer = new Gatherer();
		player->m_defender = new Defender();
		player->m_gatherer->SetLink(player->m_defender);
		player->m_defender->SetLink(player->m_gatherer);

		player->m_gatherer->m_textposition = textpositions[i];

		player->m_defender->m_audioSystem = audioSystem;
		player->m_gatherer->m_audioSystem = audioSystem;


		player->m_defender->defender.setFillColor(sf::Color::Green);
		player->m_defender->defender.setRadius(60.f);
		player->m_defender->defender.setOrigin(60.f, 60.f);
		player->m_defender->defender.setPosition(defenderPositions[i]);
		player->m_defender->m_respawn_pos = gameToPhysicsUnits(defenderPositions[i]);
		player->m_defender->m_sprite = d_sprites[i];
		player->m_defender->m_sprite.setOrigin(player->m_defender->m_sprite.getGlobalBounds().width / 2, player->m_defender->m_sprite.getGlobalBounds().height / 2);

		player->m_gatherer->gatherer.setFillColor(sf::Color::Green);
		player->m_gatherer->gatherer.setRadius(15.f);
		player->m_gatherer->gatherer.setOrigin(30.f, 30.f);
		player->m_gatherer->gatherer.setPosition(gathererPositions[i]);
		player->m_gatherer->m_respawn_pos = gameToPhysicsUnits(gathererPositions[i]);
		player->m_gatherer->m_sprite = g_sprites[i];
		player->m_gatherer->m_sprite.setOrigin(player->m_gatherer->m_sprite.getGlobalBounds().width / 2, player->m_gatherer->m_sprite.getGlobalBounds().height / 2);

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
			player->m_defender->m_sprite.setPosition(player->m_defender->defender.getPosition());
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
			player->m_gatherer->m_sprite.setPosition(player->m_gatherer->gatherer.getPosition());
		}

		players.push_back(player);

	}
	//Setting the starting colors;
	SetStartingColors(players, numDevices);

	Stalker* stalker = CreateStalker(world);

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

	actionMap["p1_up"] = thor::Action(sf::Keyboard::/*N*/W, thor::Action::Hold);
	actionMap["p1_down"] = thor::Action(sf::Keyboard::/*Y*/S, thor::Action::Hold);
	actionMap["p1_left"] = thor::Action(sf::Keyboard::/*U*/A, thor::Action::Hold);
	actionMap["p1_right"] = thor::Action(sf::Keyboard::/*V*/D, thor::Action::Hold);

	actionMap["p2_up"] = thor::Action(sf::Keyboard::/*Down*/Up, thor::Action::Hold);
	actionMap["p2_down"] = thor::Action(sf::Keyboard::/*Up*/Down, thor::Action::Hold);
	actionMap["p2_left"] = thor::Action(sf::Keyboard::/*Right*/Left, thor::Action::Hold);
	actionMap["p2_right"] = thor::Action(sf::Keyboard::/*Left*/Right, thor::Action::Hold);

	actionMap["p3_up"] = thor::Action(sf::Keyboard::K, thor::Action::Hold);
	actionMap["p3_down"] = thor::Action(sf::Keyboard::I, thor::Action::Hold);
	actionMap["p3_left"] = thor::Action(sf::Keyboard::L, thor::Action::Hold);
	actionMap["p3_right"] = thor::Action(sf::Keyboard::J, thor::Action::Hold);

	actionMap["stalker"] = thor::Action(sf::Keyboard::Z, thor::Action::Hold);

	thor::ActionMap<std::string> actionMap2;
	actionMap2["test3"] = thor::Action(sf::Mouse::Left, thor::Action::PressOnce);
	bool win = false;

	while (window.isOpen())
	{
		for (auto player : players)
		{
			b2Vec2 velo(player->m_gatherer->gatherer_body->GetLinearVelocity().x *0.89f,
				player->m_gatherer->gatherer_body->GetLinearVelocity().y *0.89f);
			player->m_gatherer->gatherer_body->SetLinearVelocity(velo);
		}
		audioSystem->update();
		world->Step(1 / 60.f, 8, 3);
		actionMap.update(window);
		actionMap2.update(window);
		/*for (int i = 0; i < numDevices; i++)
		{
		players[i]->m_gatherer->gatherer_body->SetLinearVelocity(b2Vec2(0.f, 0.f));
		}*/

		ManyMouseEvent event;


		while (ManyMouse_PollEvent(&event))
		{
			Player* player = players[event.device];

			if (event.type == MANYMOUSE_EVENT_RELMOTION)
			{
				sf::Vector2f playerPosition = player->defender.getPosition();
				if (event.item == 0)
				{
					player->m_defender->defender_body->ApplyLinearImpulse(b2Vec2(6.0f * gameToPhysicsUnits(static_cast<float>(event.value)), 0.f), player->m_defender->defender_body->GetWorldCenter(), true);
				}
				if (event.item == 1)
				{
					player->m_defender->defender_body->ApplyLinearImpulse(b2Vec2(0.f, 6.0f * gameToPhysicsUnits(static_cast<float>(event.value))), player->m_defender->defender_body->GetWorldCenter(), true);
				}
			}
		}
		for (auto &player : players)
		{
			if (!player->m_gatherer->GathererIsHit)
			{
				if (actionMap.isActive("p1_up"))
				{
					//players[0]->m_gatherer->gatherer_body->SetLinearVelocity(b2Vec2(0.f, -8.f));
					players[0]->m_gatherer->gatherer_body->ApplyLinearImpulse(b2Vec2(0.f, gameToPhysicsUnits(-15.f)), players[0]->m_gatherer->gatherer_body->GetWorldCenter(), true);
				}
				if (actionMap.isActive("p1_down"))
				{
					players[0]->m_gatherer->gatherer_body->ApplyLinearImpulse(b2Vec2(0.f, gameToPhysicsUnits(15.f)), players[0]->m_gatherer->gatherer_body->GetWorldCenter(), true);
					//players[0]->m_gatherer->gatherer_body->SetLinearVelocity(b2Vec2(players[0]->m_gatherer->gatherer_body->GetLinearVelocity().x, 8.f));
				}
				if (actionMap.isActive("p1_left"))
				{
					players[0]->m_gatherer->gatherer_body->ApplyLinearImpulse(b2Vec2(gameToPhysicsUnits(-15.f), 0.f), players[0]->m_gatherer->gatherer_body->GetWorldCenter(), true);
					//players[0]->m_gatherer->gatherer_body->SetLinearVelocity(b2Vec2(-8.f, players[0]->m_gatherer->gatherer_body->GetLinearVelocity().y));
				}
				if (actionMap.isActive("p1_right"))
				{
					players[0]->m_gatherer->gatherer_body->ApplyLinearImpulse(b2Vec2(gameToPhysicsUnits(15.f), 0.f), players[0]->m_gatherer->gatherer_body->GetWorldCenter(), true);
					//players[0]->m_gatherer->gatherer_body->SetLinearVelocity(b2Vec2(8.f, players[0]->m_gatherer->gatherer_body->GetLinearVelocity().y));
				}

				if (actionMap.isActive("p2_up"))
				{
					players[1]->m_gatherer->gatherer_body->ApplyLinearImpulse(b2Vec2(0.f, gameToPhysicsUnits(-15.f)), players[1]->m_gatherer->gatherer_body->GetWorldCenter(), true);
					//players[1]->m_gatherer->gatherer_body->SetLinearVelocity(b2Vec2(players[1]->m_gatherer->gatherer_body->GetLinearVelocity().x, -8.f));
				}
				if (actionMap.isActive("p2_down"))
				{
					players[1]->m_gatherer->gatherer_body->ApplyLinearImpulse(b2Vec2(0.f, gameToPhysicsUnits(15.f)), players[1]->m_gatherer->gatherer_body->GetWorldCenter(), true);
					//players[1]->m_gatherer->gatherer_body->SetLinearVelocity(b2Vec2(players[1]->m_gatherer->gatherer_body->GetLinearVelocity().x, 8.f));
				}
				if (actionMap.isActive("p2_left"))
				{
					players[1]->m_gatherer->gatherer_body->ApplyLinearImpulse(b2Vec2(gameToPhysicsUnits(-15.f), 0.f), players[1]->m_gatherer->gatherer_body->GetWorldCenter(), true);
					//players[1]->m_gatherer->gatherer_body->SetLinearVelocity(b2Vec2(-8.f, players[1]->m_gatherer->gatherer_body->GetLinearVelocity().y));
				}
				if (actionMap.isActive("p2_right"))
				{
					players[1]->m_gatherer->gatherer_body->ApplyLinearImpulse(b2Vec2(gameToPhysicsUnits(15.f), 0.f), players[1]->m_gatherer->gatherer_body->GetWorldCenter(), true);
					//players[1]->m_gatherer->gatherer_body->SetLinearVelocity(b2Vec2(8.f, players[1]->m_gatherer->gatherer_body->GetLinearVelocity().y));
				}
				if (actionMap.isActive("p3_up"))
				{
					players[2]->m_gatherer->gatherer_body->ApplyLinearImpulse(b2Vec2(0.f, gameToPhysicsUnits(-15.f)), players[1]->m_gatherer->gatherer_body->GetWorldCenter(), true);
					//players[1]->m_gatherer->gatherer_body->SetLinearVelocity(b2Vec2(players[1]->m_gatherer->gatherer_body->GetLinearVelocity().x, -8.f));
				}
				if (actionMap.isActive("p3_down"))
				{
					players[2]->m_gatherer->gatherer_body->ApplyLinearImpulse(b2Vec2(0.f, gameToPhysicsUnits(15.f)), players[1]->m_gatherer->gatherer_body->GetWorldCenter(), true);
					//players[1]->m_gatherer->gatherer_body->SetLinearVelocity(b2Vec2(players[1]->m_gatherer->gatherer_body->GetLinearVelocity().x, 8.f));
				}
				if (actionMap.isActive("p3_left"))
				{
					players[2]->m_gatherer->gatherer_body->ApplyLinearImpulse(b2Vec2(gameToPhysicsUnits(-15.f), 0.f), players[1]->m_gatherer->gatherer_body->GetWorldCenter(), true);
					//players[1]->m_gatherer->gatherer_body->SetLinearVelocity(b2Vec2(-8.f, players[1]->m_gatherer->gatherer_body->GetLinearVelocity().y));
				}
				if (actionMap.isActive("p3_right"))
				{
					players[2]->m_gatherer->gatherer_body->ApplyLinearImpulse(b2Vec2(gameToPhysicsUnits(15.f), 0.f), players[1]->m_gatherer->gatherer_body->GetWorldCenter(), true);
					//players[1]->m_gatherer->gatherer_body->SetLinearVelocity(b2Vec2(8.f, players[1]->m_gatherer->gatherer_body->GetLinearVelocity().y));
				}
			}
		}
		

		
		// Cap the speed to max speed for all pla5yers
		for (auto &player : players)
		{
			b2Vec2 MAX_VELOCITY(30.f, 30.f);
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

			b2Vec2 MAX_VELOCITY_GATHERER(12.f, 12.f);
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
		///////////
		//TESTING//
		///////////
		if (actionMap.isActive("stalker"))
		{
			stalker->body->SetTransform(b2Vec2(30.f, 10.f), 0);
			
		}
		stalker->shape.setPosition(physicsToGameUnits(stalker->body->GetPosition()));

		//update sprite positions
		for (int i = 0; i < numDevices; i++)
		{
			players[i]->m_defender->m_sprite.setPosition(physicsToGameUnits(players[i]->m_defender->defender_body->GetPosition()));
			players[i]->m_gatherer->m_sprite.setPosition(physicsToGameUnits(players[i]->m_gatherer->gatherer_body->GetPosition()));
		}


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
					player->m_gatherer->IsGatheringTime = true;
					int count = 0;
					for (auto it : players)
					{
						if (it->m_gatherer->IsGatheringTime == true)
						{
							++count;
						}
					}
					if (!player->timer.isRunning())
					{
						player->timer.start();
						if (count > 1)
						{
							for (auto it2 : players)
							{
								it2->timer.stop();
							}
							
						}
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
					if (player->m_gatherer->IsGatheringTime)
					{
						player->m_gatherer->IsGatheringTime = false;
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

		for (int i = 0; i < stones.size(); i++)
		{
			window.draw(stones[i]);
		}

		for (auto i : players)
		{
			if (i->m_gatherer->GathererIsHit)
			{
				
				i->m_gatherer->gatherer_body->SetTransform(i->m_gatherer->m_respawn_pos,0);
				i->m_gatherer->gatherer_body->SetLinearVelocity(b2Vec2(0.f, 0.f));
				i->m_gatherer->GathererIsHit = false;
			}
		}

		window.draw(s_GUI);
		///////////
		//TESTING//
		///////////
		window.draw(stalker->shape);
		window.draw(innerCircle);
		for (auto player : players)
		{
			sf::Time time = player->timer.getElapsedTime();
			float ftime = time.asSeconds();
			std::ostringstream ss;
			ss << ftime;
			std::string s(ss.str());
			sf::String string(s);
			sf::Text text( sf::Text(string,font));
			text.setPosition(player->m_gatherer->m_textposition);
			window.draw(text);
			window.draw(player->m_defender->defender);
			window.draw(player->m_gatherer->gatherer);
			window.draw(player->m_gatherer->m_sprite);
			window.draw(player->m_defender->m_sprite);
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

