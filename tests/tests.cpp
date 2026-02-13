
// Core headers
#include <QtTest>
#include <QtWidgets/QApplication>
#include <memory>

#include "characters.h"
#include "field_elements.h"
#include "game.h"
#include "objects.h"
#include "player.h"
#include "utility/geometry.h"

class TestYourApp : public QObject
{
    Q_OBJECT
public:
    TestYourApp()=default;
    ~TestYourApp()=default;

private slots:

    void init();
    void cleanup();

    void TestInventory();
    void TestInventoryObjects();
    
    void TestKey();
    void TestFire();
    void TestHydrant();

    void TestDoor();

};

void TestYourApp::init()
{

}

class InventoryTestObject : public InventoryObject {
public:
    InventoryTestObject()=default;
    void Draw(DrawContext& context, int offset) const override{}
};

void TestYourApp::TestInventory() {

    Inventory inventory;
    Game game{12, 5};

    const auto test_object = std::make_shared<InventoryTestObject>();
    const auto test_key = std::make_shared<InventoryKey>(game.GetContext(), "test_key", QColor(Qt::black));

    QVERIFY(test_object->IsWater() == false);
    QVERIFY(test_object->AsKey() == nullptr);

    QVERIFY(inventory.Store(test_object) == true);
    QVERIFY(inventory.Store(test_object) == false);
    QVERIFY(inventory.HasItem(test_object.get()) == true);

    inventory.Store(test_key);

    const auto item = inventory.FindItem([](std::shared_ptr<InventoryObject> item) {
        auto as_key = item->AsKey();
        if (as_key != nullptr) {
            qDebug() << "Found Key";
            return as_key->GetName() == "test_key";

        }
        return false;
    });
    QVERIFY(item == test_key.get());
    const auto non_item = inventory.FindItem([](std::shared_ptr<InventoryObject> item) {
        auto as_key = item->AsKey();
        if (as_key != nullptr) {
            return as_key->GetName() == "testy_test";
        }
        return false;
    });

    QVERIFY(non_item == nullptr);


    QVERIFY(inventory.Remove(test_object.get()) == true);
    QVERIFY(inventory.Remove(test_object.get()) == false);

    QVERIFY(inventory.HasItem(test_object.get()) == false);

}

void TestYourApp::TestInventoryObjects() {
    Game game{12, 7};

    const InventoryKey test_key{game.GetContext(), "TestKey", QColor(Qt::blue)};

    QVERIFY(test_key.AsKey() == &test_key);
    QVERIFY(test_key.IsWater() == false);
    QVERIFY(test_key.GetName() == "TestKey");

    const InventoryWater test_water{game.GetContext()};

    QVERIFY(test_water.IsWater() == true);
    QVERIFY(test_water.AsKey() == nullptr);
}



void TestYourApp::TestKey() {

    Game game{15, 15};
    Player player{game.GetContext(), {5, 5, 0}, Direction::kRight};
    game.AddFloor(0);

    game.SetPlayer(std::make_shared<Player>(player));
    Victim victim(game.GetContext(), {6, 5, 0}, Direction::kRight);

    const auto items = game.GetContext().object_map.Get(Coordinate{5, 6, 0});
    QCOMPARE(items.size(), 0);

    Key key{game.GetContext(), Coordinate{5, 6, 0}, "Key", QColor::fromRgb(255, 255, 255)};

    auto items_with_key = game.GetContext().object_map.Get(Coordinate{5, 6, 0});
    QCOMPARE(items_with_key.size(), 1);

    key.Interact(victim, Direction::kLeft);

    auto items_with_key2 = game.GetContext().object_map.Get(Coordinate{5, 6, 0});
    QCOMPARE(items_with_key2.size(), 1);

    key.Interact(player, Direction::kRight);

    const auto items_without_key = game.GetContext().object_map.Get(Coordinate{5, 6, 0});
    QCOMPARE(items_without_key.size(), 0);


    const auto key_ptr = player.GetContext().inventory.FindItem([](auto item) {
        if (item->AsKey() != nullptr) {
            return item->AsKey()->GetName() == "Key";
        }
            return false;
    });

    QVERIFY(key_ptr != nullptr);
}


void TestYourApp::TestFire() {
    Game game{15, 15};
    Player player{game.GetContext(), {5, 5, 0}, Direction::kRight};
    game.AddFloor(0);

    game.SetPlayer(std::make_shared<Player>(player));
    Victim victim(game.GetContext(), {6, 5, 0}, Direction::kRight);

    Fire fire(game.GetContext(), Coordinate{5, 6, 0});

    auto fire_found = [&]() {
        const auto items = game.GetContext().object_map.Get(Coordinate{5, 6, 0});
        return items.size() == 1;
    };

    QVERIFY(fire_found() == true);

    fire.Interact(player, Direction::kLeft);

    QVERIFY(fire_found() == true);

    game.GetContext().inventory.Store(std::make_shared<InventoryWater>(game.GetContext()));

    fire.Interact(victim, Direction::kRight);

    QVERIFY(fire_found() == true);
    fire.Interact(player, Direction::kLeft);
    QVERIFY(fire_found() == false);

    const auto water = game.GetContext().inventory.FindItem([&](auto item) {return item->IsWater();});

    // Fire should remove water from the inventory
    QVERIFY(water == nullptr);
}


void TestYourApp::TestHydrant() {
    Game game{15, 15};
    Player player{game.GetContext(), {5, 5, 0}, Direction::kRight};
    game.AddFloor(0);

    game.SetPlayer(std::make_shared<Player>(player));
    Victim victim(game.GetContext(), {6, 5, 0}, Direction::kRight);

    Hydrant hydrant(game.GetContext(), Coordinate{5, 6, 0});

    auto count_water = [&]() {
        const auto inventory = game.GetContext().inventory.GetItems();
        const auto amount_of_water = std::count_if(inventory.begin(), inventory.end(), ([&](auto item) {return item->IsWater();}));

        return amount_of_water;
    };

    QVERIFY(count_water() == 0);

    hydrant.Interact(victim, Direction::kLeft);
    QVERIFY(count_water() == 0);
    hydrant.Interact(player, Direction::kRight);
    QVERIFY(count_water() == 1);
    hydrant.Interact(victim, Direction::kLeft);
    QVERIFY(count_water() == 1);
    hydrant.Interact(player, Direction::kRight);
    QVERIFY(count_water() == 1);
}

void TestYourApp::TestDoor() {
    Game game{15, 15};
    Player player{game.GetContext(), {5, 5, 0}, Direction::kRight};
    auto& floor = game.AddFloor(0);

    game.SetPlayer(std::make_shared<Player>(player));
    Victim victim(game.GetContext(), {6, 5, 0}, Direction::kRight);

    auto simple_door = std::make_shared<Door>(game.GetContext(), std::nullopt, std::nullopt);
    floor.SetWall(Coordinate2D{5, 7}, Direction::kRight, simple_door);

    QVERIFY(!simple_door->CanPass(victim, Direction::kRight));
    QVERIFY(!simple_door->CanPass(player, Direction::kRight));
    simple_door->Interact(victim, Direction::kRight);
    QVERIFY(!simple_door->CanPass(victim, Direction::kRight));
    QVERIFY(!simple_door->CanPass(player, Direction::kRight));
    simple_door->Interact(player, Direction::kRight);
    QVERIFY(simple_door->CanPass(victim, Direction::kRight));
    QVERIFY(simple_door->CanPass(player, Direction::kRight));


    auto door = std::make_shared<Door>(game.GetContext(), "test_key", std::nullopt);
    floor.SetWall(Coordinate2D{4, 12}, Direction::kRight, door);

    QVERIFY(!door->CanPass(victim, Direction::kRight));
    QVERIFY(!door->CanPass(player, Direction::kRight));
    door->Interact(victim, Direction::kRight);
    QVERIFY(!door->CanPass(victim, Direction::kRight));
    QVERIFY(!door->CanPass(player, Direction::kRight));
    door->Interact(player, Direction::kRight);
    QVERIFY(!door->CanPass(victim, Direction::kRight));
    QVERIFY(!door->CanPass(player, Direction::kRight));

    game.GetContext().inventory.Store(std::make_shared<InventoryKey>(game.GetContext(), "wrong_key", QColor(Qt::red)));

    QVERIFY(!door->CanPass(victim, Direction::kRight));
    QVERIFY(!door->CanPass(player, Direction::kRight));
    door->Interact(victim, Direction::kRight);
    door->Interact(player, Direction::kRight);
    QVERIFY(!door->CanPass(victim, Direction::kRight));
    QVERIFY(!door->CanPass(player, Direction::kRight));

    game.GetContext().inventory.Store(std::make_shared<InventoryKey>(game.GetContext(), "test_key", QColor(Qt::cyan)));

    QVERIFY(!door->CanPass(victim, Direction::kRight));
    QVERIFY(!door->CanPass(player, Direction::kRight));
    door->Interact(victim, Direction::kRight);
    QVERIFY(!door->CanPass(victim, Direction::kRight));
    QVERIFY(!door->CanPass(player, Direction::kRight));
    door->Interact(player, Direction::kRight);
    QVERIFY(door->CanPass(victim, Direction::kRight));
    QVERIFY(door->CanPass(player, Direction::kRight));
}


void TestYourApp::cleanup()
{
}

QTEST_MAIN(TestYourApp)

#include "tests.moc"
