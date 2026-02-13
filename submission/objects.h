#pragma once

#include "object.h"
#include "character.h"
#include "inventory.h"

class Stairs : public Object {
public:
    Stairs(GameContext& context, const Coordinate& position, const Direction& dir, bool down)
        : Object(context, position)
        , dir_{dir}
        , down_{down}
        , sprites_{context.asset_loader.LoadWithDirection("objects", down ? "dstair" : "stair")}
    {

    }

    void Draw(DrawContext& context) const override{
        Asset sprite = sprites_->Get(dir_);
        CoordinateF pos = GetPosition();
        double darkness =  context.darkener.GetDarkness(pos);
        context.painter.DrawObject(sprite, pos, darkness);
    }

    void Interact(Character& character, Direction dir) override{
        if(dir == Invert(dir_)){
            character.SetPosition(character.GetPosition() + Coordinate{0, 0, (down_ ? -1 : 1)} + Coordinate::FromDirection(dir)*2);
            qInfo() << (down_ == 1 ? "Go stairs down" : "Go stairs up");
            return;
        }
        qInfo() << "Trying to go stairs in the wrong dir";
    }
protected:
    void SetSprites(const AssetInDirections& sprites){
        sprites_ = sprites;
    }

private:
    [[maybe_unused]] Direction dir_;
    [[maybe_unused]] bool down_;
    std::optional<AssetInDirections> sprites_;
};

class Key : public Object{
public:
    Key(GameContext& context, Coordinate position, const std::string& name, QColor color) : Object(context, position), name_{name} {
        sprite_ = context.asset_loader.LoadSprite("objects", "key");
        sprite_.ChangeColor(color);
        color_ = color;
    }

    void Interact([[maybe_unused]]Character& character, [[maybe_unused]]Direction dir){
        if(character.IsActive()){
            GetContext().inventory.Store(std::make_shared<InventoryKey>(GetContext(), name_, color_));
            Disapear();
        }
    }

    void Draw(DrawContext& context) const{
        int offset = GetContext().inventory.GetItems().size();
        context.painter.DrawInventoryItem(offset, sprite_);
    }

private:
    Asset sprite_;
    std::string name_;
    QColor color_;
};


class Fire : public Object{
public:
    Fire(GameContext& context, Coordinate position) : Object(context, position) {
        sprites_ = context.asset_loader.LoadAnimatedObject("objects", "fire/", 20);
    }

    void Interact([[maybe_unused]]Character& character, [[maybe_unused]]Direction dir){
        auto ptr = GetContext().inventory.FindItem([](const auto& item_ptr){return item_ptr->IsWater() == true;});
        if(ptr != nullptr && character.IsActive()){
            GetContext().inventory.Remove(ptr);
            Disapear();
        }
    }

    void Draw(DrawContext& context) const{
        Time time = GetContext().timer.Now();
        int frame = static_cast<int>(time / 40);
        Asset sprite = sprites_.GetFrame(frame);
        CoordinateF pos = GetPosition();
        context.painter.DrawObject(sprite, pos);
    }

private:
    AnimatedAsset sprites_;
};

class Hydrant : public Object{
public:
    Hydrant(GameContext& context, Coordinate position) : Object(context, position) {
        sprite_ = context.asset_loader.LoadSprite("objects", "hydrant");
    }

    void Interact([[maybe_unused]]Character& character, [[maybe_unused]]Direction dir){
        auto ptr = GetContext().inventory.FindItem([](const auto& item_ptr){return item_ptr->IsWater() == true;});
        if(ptr == nullptr && character.IsActive()){
            GetContext().inventory.Store(std::make_shared<InventoryWater>(GetContext()));
        }
    }

    void Draw(DrawContext& context) const{
        int offset = GetContext().inventory.GetItems().size();
        context.painter.DrawInventoryItem(offset, sprite_);
    }

private:
    Asset sprite_;
};


