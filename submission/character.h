#pragma once

#include "object.h"
#include "field.h"
#include "utility/animation.h"
#include <algorithm>


class Character : public Object {
public:
    Character(GameContext& context, const Coordinate& position, const Direction& dir)
        : Object(context, position)
        , dir_{dir}
    {

    }

    void Draw(DrawContext& context) const override {
        Asset sprite;
        if(animated_sprites_.has_value()){
            AnimatedAsset an_sprite = animated_sprites_->Get(dir_);
            sprite = transition_.has_value() ? an_sprite.GetAt(transition_->GetProgress()) : an_sprite.GetAt(0);
        }else if(sprites_.has_value()){
            sprite = sprites_->Get(dir_);
        }else{
            throw std::runtime_error ("Sprites are not initialized");
        }

        CoordinateF pos = GetExactPos();
        double darkness = context.darkener.GetDarkness(pos);
        context.painter.DrawObject(sprite, pos, darkness);
    }

    void SetDirection(const Direction& dir){
        dir_ = dir;
    }

    void SetPosition(Coordinate position) override{
        if(transition_.has_value()){
            transition_->StopNow();
            transition_.reset();
        }
        Object::SetPosition(position);
    }

    Direction GetDirection() const{
        return dir_;
    }

    //Проверяет возможность прохода через стену
    bool CanPassWall(Direction dir) const{
        if(GetWall(dir) == nullptr){
            return true;
        }
        return GetWall(dir)->CanPass(*this, dir);
    }

    //Проверяет возможность перехода персонажа в соседню клетку
    bool CanGo(Direction dir) const{
        std::set<Object*> objects = GetContext().object_map.Get(GetPosition() + Coordinate::FromDirection(dir));
        auto predicate = [&](Object* object){return object->CanCover(*this, dir);};
        return std::all_of(objects.begin(), objects.end(), predicate);
    }

    //Устанавливает уровень доступа персонажа
    virtual bool IsActive() const {
        return false;
    }

    void DoTransition(Direction dir, Callback at_end = {}){
        if(transition_.has_value()){
            transition_->StopNow();
        }
        Coordinate cur_pos = GetPosition();
        Coordinate dst_pos = cur_pos + Coordinate::FromDirection(dir);

        SetTargetPosition(dst_pos);
        auto post_effect = [=, this](){
            SetPosition(dst_pos);
            if(at_end != nullptr){
                at_end();
            }
        };

        transition_.emplace(Transition<CoordinateF>(GetContext().timer, cur_pos, dst_pos, GetStepDuration(), post_effect));
    }

    virtual int GetStepDuration() const{
        return 200;
    }

    CoordinateF GetExactPos() const override{
        if(transition_.has_value()){
            return transition_->GetPosition();
        }
        return GetPosition();
    }

    bool NowComing() const{
        return transition_.has_value();
    }

protected:
    void SetSprites(const AssetInDirections& sprites) {
        sprites_ = sprites;
    }

    void SetWalkSprites(const AnimatedAssetInDirections& sprites){
        animated_sprites_ = sprites;
    }

private:
    Direction dir_;
    std::optional<Transition<CoordinateF>> transition_;
    //Вспомогательный метод получение стены
    std::shared_ptr<Wall> GetWall(Direction dir) const{
        auto pos = GetPosition();
        return GetContext().field.GetFloor(pos.z_pos).GetWall(pos, dir);
    }

    std::optional<AssetInDirections> sprites_;
    std::optional<AnimatedAssetInDirections> animated_sprites_;
};
