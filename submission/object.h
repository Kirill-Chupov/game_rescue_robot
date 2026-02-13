#pragma once

#include "utility/assets.h"
#include "utility/painter.h"
#include "utility/darkeners.h"
#include "context.h"
#include <QDebug>

class Character;

class Object {
public:
    Object(GameContext& context, const Coordinate& position)
        :context_{context}
        ,position_{position}
    {
        context_.object_map.Place(position_, this);
    }

    ~Object(){
        context_.object_map.Remove(position_, this);
    }

    //Отрисовщик
    virtual void Draw(DrawContext& context) const = 0;

    //Возвращает текущую позицию объекта
    Coordinate GetPosition() const{
        return position_;
    }

    virtual CoordinateF GetExactPos() const{
        return position_;
    }

    Coordinate GetPrevPosition() const{
        return prev_pos_;
    }

    //Удаляет объект с карты и добавляет новый по указаным координатам
    virtual void SetPosition(Coordinate position){
        RemoveFromMap();
        prev_pos_ = position_;
        position_ = position;
        target_position_.reset();
        PlaceToMap();
    }

    void SetTargetPosition(Coordinate position){
        RemoveFromMap();
        target_position_ = position;
        PlaceToMap();
    }

    //Устанавливает признак видимости и удаляет объект с карты
    void Disapear(){
        visibility_ = false;
        RemoveFromMap();
    }

    //Сообщает виден объект или нет
    bool IsVisible() const{
        return visibility_;
    }

    //Возвращает текущий контекст игры
    GameContext& GetContext() const{
        return context_;
    }

    //Определяет взаимодействие с объектом
    virtual void Interact([[maybe_unused]]Character& character, [[maybe_unused]]Direction dir){

    }

    //Определяет возможность прохождения сквозь объект
    virtual bool CanCover([[maybe_unused]]const Character& character, [[maybe_unused]]Direction dir) const{
        return !IsVisible();
    }

private:
    GameContext& context_;
    Coordinate position_;
    Coordinate prev_pos_;
    bool visibility_ = true;
    std::optional<Coordinate> target_position_;

    void RemoveFromMap(){
        context_.object_map.Remove(position_, this);
        if(target_position_.has_value()){
            context_.object_map.Remove(target_position_.value(), this);
        }
    }

    void PlaceToMap(){
        context_.object_map.Place(position_, this);
        if(target_position_.has_value()){
            context_.object_map.Place(target_position_.value(), this);
        }
    }
};
