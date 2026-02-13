#pragma once
#include "context.h"
#include "field.h"
#include "player.h"
#include "inventory.h"
#include <memory>

class Game {
public:
    Game(){

    }

    Game(int w, int h)  //кастыль для тестов
        : field_{w, h}{

    }

    Game(AssetLoader& loader , int w, int h)
        : field_{w, h}
        , asset_loader_(loader){

    }

    void AddObject(const std::shared_ptr<Object>& object){
        storage_.push_back(object);
    }

    void AddWall(const std::shared_ptr<Wall>& wall, Direction dir, Coordinate2D where, int level){
        field_.GetFloor(level).SetWall(where, dir, wall);
    }

    void AddTile(const std::shared_ptr<Tile>& tile, Coordinate2D where, int level){
        field_.GetFloor(level).SetTile(where, tile);
    }

    void DrawFrame(DrawContext& context) const{
        int w = context_.field.GetWidth();
        int h = context_.field.GetHeight();
        Coordinate pos = player_->GetPosition();

        field_.GetFloor(pos.z_pos).DrawFloor(context);
        for (int y = h; y >= 0; --y) {
            field_.GetFloor(pos.z_pos).DrawHWalls(context, y);
            for (int x = w; x >= 0; --x) {
                field_.GetFloor(pos.z_pos).DrawVWall(context, Coordinate {x, y, pos.z_pos});
                const auto& objects = object_map_.Get(Coordinate {x, y, pos.z_pos});
                for(const auto& object : objects){
                    if(object->GetPosition() != Coordinate {x, y, pos.z_pos} || !object->IsVisible()){
                        continue;
                    }
                    object->Draw(context);
                }
            }
        }
    }

    GameContext& GetContext() {
        return context_;
    }

    Floor& AddFloor(int level){
        field_.AddFloor(level);
        return field_.GetFloor(level);
    }

    void SetPlayer(std::shared_ptr<Player> player){
        player_ = player;
    }

    Field& GetField(){
        return field_;
    }

    Player& GetPlayer(){
        return *player_;
    }

    Floor& GetCurrentFloor(){
        int level = player_->GetPosition().z_pos;
        return field_.GetFloor(level);
    }

    void Reset(Size size){
        field_ = Field(size.width, size.height);
        context_.field = field_;
    }

private:
    ObjectMap<Object> object_map_;
    Field field_;
    std::shared_ptr<Player> player_;
    RandomGen random_;
    AssetLoader loader; //кастыль для тестов
    AssetLoader& asset_loader_ = loader;
    Timer timer_;
    Inventory inventory_;
    GameContext context_{
        .object_map = object_map_,
        .field = field_,
        .random = random_,
        .asset_loader = asset_loader_,
        .timer = timer_,
        .inventory = inventory_
    };
    std::vector<std::shared_ptr<Object>> storage_;
};
