#include "characters.h"
#include "field_elements.h"
#include "mainwindow.h"

#include "objects.h"
#include "player.h"

#include <QApplication>

void MakeRoom(Floor& floor, GameContext& context, std::shared_ptr<Wall> wall, std::shared_ptr<Tile> tile, int x1, int x2, int y1, int y2) {

    // Лямбда для создания стены.
    auto make_wall = [&]{ return std::make_shared<EdgeWall>(context); };

    for(int i = x1; i <= x2; ++i) {
        for(int j = y1; j <= y2; ++j) {
            floor.SetTile({i, j}, tile);
            if (i == x1) {
                floor.SetWall({i, j}, Direction::kLeft, make_wall());
            }
            if (i == x2) {
                floor.SetWall({i, j}, Direction::kRight, make_wall());
            }
            if (j == y1) {
                floor.SetWall({i, j}, Direction::kUp, make_wall());
            }
            if (j == y2) {
                floor.SetWall({i, j}, Direction::kDown, make_wall());
            }
        }
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AssetLoader loader;

    Game game{loader, 15, 15};
    auto player = std::make_shared<Player>(game.GetContext(), Coordinate{5, 5, 0}, Direction::kRight);
    game.SetPlayer(player);
    game.AddObject(player);

    Floor& floor0 = game.AddFloor(0);
    Floor& floor1 = game.AddFloor(1);

    auto edge_wall = std::make_shared<EdgeWall>(game.GetContext());
    auto victim = std::make_shared<Victim>(game.GetContext(), Coordinate{7, 12, 0}, Direction::kLeft);
    auto empty_wall = std::make_shared<EmptyWall>();
    auto door1 = std::make_shared<Door>();
    auto door2 = std::make_shared<Door>();

    game.AddObject(victim);

    auto marble_tile = std::make_shared<FloorTile>(game.GetContext(), "floor4");

    floor0.SetWall({4, 4}, Direction::kUp, edge_wall);
    floor0.SetWall({5, 4}, Direction::kUp, door1);
    floor0.SetWall({6, 4}, Direction::kUp, edge_wall);
    floor0.SetWall({4, 3}, Direction::kUp, edge_wall);
    floor0.SetWall({5, 3}, Direction::kUp, door2);
    floor0.SetWall({6, 3}, Direction::kUp, edge_wall);

    MakeRoom(floor0, game.GetContext(), edge_wall, marble_tile, 4, 6, 0, 5);
    MakeRoom(floor1, game.GetContext(), edge_wall, marble_tile, 4, 6, 0, 2);
    MakeRoom(floor0, game.GetContext(), edge_wall, marble_tile, 0, 10, 6, 14);
    floor0.SetWall({5, 5}, Direction::kDown,  empty_wall);

    Stairs stair_up{game.GetContext(), {5, 1, 0}, Direction::kDown, false};
    Stairs stair_down{game.GetContext(), {5, 1, 1}, Direction::kUp, true};

    Controller controller{game};

    MainWindow w{game, controller};
    w.show();
    return a.exec();
}
