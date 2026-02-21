#pragma once

#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "game.h"


template<class T>
T LoadData(const QJsonValue &);

template<>
inline Coordinate LoadData<Coordinate>(const QJsonValue &json) {
    Coordinate pos;
    if(json.isObject()){
        pos.x_pos = json["x"].toInt();
        pos.y_pos = json["y"].toInt();
        pos.z_pos = json["z"].toInt();
    }else if(json.isArray()){
        QJsonArray arr = json.toArray();
        if(arr.size() != 3){
            throw std::runtime_error("Wrong array length for Coordinate");
        }
        pos.x_pos = arr[0].toInt();
        pos.y_pos = arr[1].toInt();
        pos.z_pos = arr[2].toInt();
    }else{
        throw std::runtime_error("Expected array or object for Coordinate");
    }
    return pos;
}

template<>
inline Size LoadData<Size>(const QJsonValue &json) {
    Size size;
    if(json.isObject()){
        size.height = json["h"].toInt();
        size.width = json["w"].toInt();
    }else if(json.isArray()){
        QJsonArray arr = json.toArray();
        if(arr.size() != 2){
            throw std::runtime_error("Wrong array length for size");
        }
        size.width = arr[0].toInt();
        size.height = arr[1].toInt();
    }else{
        throw std::runtime_error("Expected array or object for size");
    }
    return size;
}

template<>
inline Direction LoadData<Direction>(const QJsonValue &json) {
    Direction dir;
    QString value = json.toString();
    if(!json.isString()){
        QString("Unknown direction " + value).toStdString();
    }

    if(value == "r"){
        dir = Direction::kRight;
    }else if (value == "l"){
        dir = Direction::kLeft;
    }else if (value == "u"){
        dir = Direction::kUp;
    }else if (value == "d"){
        dir = Direction::kDown;
    }else{
        throw std::runtime_error(QString("Unknown directon " + value).toStdString());
    }
    return dir;
}

template<>
inline Coordinate2D LoadData<Coordinate2D>(const QJsonValue &json){
    QJsonArray arr = json.toArray();
    return Coordinate2D {arr[0].toInt(), arr[1].toInt()};
}


template<class T>
class Loader {
public:
    static std::shared_ptr<T> Load(const QJsonValue &json, GameContext& context, Floor* floor) {
        QJsonObject obj = json.toObject();
        QString name_type;
        if(obj.contains("type")){
            name_type = "type";
        }else if(obj.contains("tile")){
            name_type = "tile";
        }else if(obj.contains("wall")){
            name_type = "wall";
        }
        auto type = json.toObject()[name_type].toString(); // Определяем тип объекта.
        auto loader = loaders_.at(type.toStdString());   // Выбираем нужный загрузчик.
        return loader(json.toObject(), context, floor); // Вызываем его.
    }
    using LoaderImpl = std::function<std::shared_ptr<T>(const QJsonObject& json, GameContext& context, Floor* floor)>;

    static void AddLoader(const std::string& type, LoaderImpl impl) {
        loaders_[type] = impl;
    }

private:
    // Словарь с загрузчиками.
    inline static std::map<std::string, LoaderImpl> loaders_;
};

class LevelLoader{
public:
    bool LoadFromFile(Game& game, const QString& file_name){
        QFile file(file_name);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            qWarning() << QString("Cannot open file for reading: %1").arg(file_name);
            return false;
        }

        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if(!doc.isObject()){
            qWarning() << ("Invalid JSON format");
            return false;
        }
        Load(game, doc.object());
        file.close();
        return true;
    }

    void Load(Game& game, const QJsonObject& json){
        std::map<QString, Direction> str_to_dir{
            {"r", Direction::kRight},
            {"l", Direction::kLeft},
            {"u", Direction::kUp},
            {"d", Direction::kDown}
        };

        game.Reset(LoadData<Size>(json["size"]));
        game.SetPlayer(Loader<Player>::Load(json["player"], game.GetContext(), nullptr));
        QJsonArray floors = json["floors"].toArray();
        for(const auto& floor : floors){
            QJsonObject lvl = floor.toObject();
            int level = lvl["level"].toInt();
            Floor& cur_floor = game.AddFloor(level);

            // Загрузка регионов (тайлы)
            QJsonArray regions = lvl["regions"].toArray();
            for (const auto& region : regions) {
                auto tile = Loader<Tile>::Load(region.toObject(), game.GetContext(), nullptr);
                Coordinate2D start = LoadData<Coordinate2D>(region.toObject()["pos"]);
                Coordinate2D end = LoadData<Coordinate2D>(region.toObject()["size"]);
                AddTiles(game, tile, start, end, level);
            }

            // Загрузка стен
            QJsonArray walls = lvl["walls"].toArray();
            for (const auto& wall : walls) {
                auto wall_obj = Loader<Wall>::Load(wall.toObject(), game.GetContext(), &cur_floor);
                Direction dir = str_to_dir[wall.toObject()["dir"].toString()];
                Coordinate2D start = LoadData<Coordinate2D>(wall.toObject()["pos"]);
                int length = wall.toObject()["length"].toInt();
                //AddWalls(game, wall_obj, start, dir, length, level);
                AddWalls(game, wall_obj, start, Direction::kUp, length, level);
            }

            // Загрузка объектов (Victim, Stairs и др.)
            QJsonArray objects = lvl["objects"].toArray();
            for (const auto& object : objects) {
                auto obj = Loader<Object>::Load(object.toObject(), game.GetContext(), &cur_floor);
                game.AddObject(obj);
            }
        }
    }
private:
    void AddWalls(Game& game, const std::shared_ptr<Wall> wall, Coordinate2D start, Direction dir, int length, int level){
        Coordinate2D pos = start;
        for(auto i = 0; i < length; ++i){
            pos = pos + Coordinate2D(Coordinate::FromDirection(dir)) * i;
            game.AddWall(wall, dir, pos, level);
        }
    }

    void AddTiles(Game& game, const std::shared_ptr<Tile> tile, Coordinate2D start, Coordinate2D end, int level){
        Coordinate2D where;
        for(auto x = start.x_pos; x <= end.x_pos; ++x){
            for(auto y = start.y_pos; y <= end.y_pos; ++y){
                where = Coordinate2D {x, y};
                game.AddTile(tile, where, level);
            }
        }
    }
};
