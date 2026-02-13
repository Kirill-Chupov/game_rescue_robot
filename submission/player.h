#pragma once

#include "character.h"

class Player : public Character {
public:
    Player(GameContext& context, const Coordinate& position, const Direction& dir)
        : Character(context, position, dir){
        AnimatedAssetInDirections walk_sprites = context.asset_loader.LoadAnimatedWithDirection("characters", "robot-walk", 19);
        SetWalkSprites(walk_sprites);
        qInfo() <<"Player spawned on" << QString::fromStdString(ToString(position)) << "dir" << QString::fromStdString(ToString(dir));
    }

    void GoCommand(Direction dir, Callback at_end = {}){
        auto pos = GetPosition() + Coordinate::FromDirection(dir);
        //стена мешает проходу
        if(!CanPassWall(dir)){
            qInfo() << "Player stoped on wall";
            GetContext().field.GetFloor(GetPosition().z_pos).GetWall(GetPosition(),dir)->Interact(*this, dir);
            return;
        }

        //Клетка занята непроходимым объектом
        if(!CanGo(dir)){
            qInfo() << "Player stoped on object";
            std::set<Object*> objects = GetContext().object_map.Get(pos);
            for(auto& object : objects){
                object->Interact(*this, dir);
            }
            return;
        }

        qInfo() << "Player moves";
        DoTransition(dir, at_end);
    }

    bool IsActive() const override{
        return true;
    }
private:

};
