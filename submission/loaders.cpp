#include "level_loader.h"
#include "characters.h"
#include "objects.h"
#include "field_elements.h"

struct PlayerLoader {
    PlayerLoader(){
        Loader<Player>::AddLoader("player", [](const QJsonObject& json, GameContext& context, Floor* floor){
            Coordinate pos = LoadData<Coordinate>(json["pos"]);
            Direction dir = LoadData<Direction>(json["dir"]);
            return std::make_shared<Player>(context, pos, dir);
        });
    }
} player_loader;

struct VictimLoader{
    VictimLoader(){
        Loader<Object>::AddLoader("victim", [](const QJsonObject& json, GameContext& context, Floor* floor){
            Coordinate2D pos_2D = LoadData<Coordinate2D>(json["pos"]);
            Coordinate pos = Coordinate{pos_2D.x_pos, pos_2D.y_pos, floor->GetLevel()};
            Direction dir = LoadData<Direction>(json["dir"]);
            return std::make_shared<Victim>(context, pos, dir);
        });
    }
} victim_loader;

struct StairsLoader{
    StairsLoader(){
        Loader<Object>::AddLoader("stair", [](const QJsonObject& json, GameContext& context, Floor* floor){
            Coordinate2D pos_2D = LoadData<Coordinate2D>(json["pos"]);
            Coordinate pos = Coordinate{pos_2D.x_pos, pos_2D.y_pos, floor->GetLevel()};
            Direction dir = LoadData<Direction>(json["dir"]);
            bool down = json["down"].isBool();
            return std::make_shared<Stairs>(context, pos, dir, down);
        });
    }
} stairs_loader;

struct DoorLoader {
    DoorLoader(){
        Loader<Wall>::AddLoader("door", [](const QJsonObject& json, GameContext& context, Floor* floor){
            return std::make_shared<Door>(context);
        });
    }
} door_loader;

struct MarbleLoader {
    MarbleLoader(){
        Loader<Tile>::AddLoader("marble-black",[](const QJsonObject& json, GameContext& context, Floor* floor){
            return std::make_shared<FloorTile>(context, "floor4");
        });
    }
} marble_loader;

struct GrassLoader {
    GrassLoader(){
        Loader<Tile>::AddLoader("grass",[](const QJsonObject& json, GameContext& context, Floor* floor){
            return std::make_shared<FloorTile>(context, "grass");
        });
    }
} grass_loader;

struct EdgeWallLoader {
    EdgeWallLoader(){
        Loader<Wall>::AddLoader("edge", [](const QJsonObject& json, GameContext& context, Floor* floor){
            return std::make_shared<EdgeWall>(context);
        });
    }
} edge_loader;

struct EmptyWallLoader{
    EmptyWallLoader(){
        Loader<Wall>::AddLoader("empty", [](const QJsonObject& json, GameContext& context, Floor* floor){
            return std::make_shared<EmptyWall>(context);
        });
    }
} empty_loader;
