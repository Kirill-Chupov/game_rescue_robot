#pragma once

#include "utility/geometry.h"
#include "game.h"
#include <stdexcept>

class Controller {
public:
    Controller(Game& game) : game_{game} {}

    void SetRedrawCallback(const std::function<void()>& callback){
        cb_redraw_ = callback;
    }

    void OnMoveKey(Direction dir) {
        // Напишите реализацию метода.
        qInfo() << "<<< Moving in direction" << QString::fromStdString(ToString(dir));
        if(game_.GetPlayer().GetDirection() != dir && !game_.GetPlayer().NowComing()){
            qInfo() << "Changing direction";
            game_.GetPlayer().SetDirection(dir);
        }else{
            qInfo() << "Trying to go";
            if(!game_.GetPlayer().NowComing()){
                dir_ = dir;
            }
            MovePlayer();
        }

        qInfo() << ">>> New player position:" << QString::fromStdString(ToString(game_.GetPlayer().GetPosition())) << "dir" << QString::fromStdString(ToString(dir));
    }

    void OnReleaseMoveKey([[maybe_unused]]Direction dir) {
        dir_.reset();
    }

    void Tick(){
        game_.GetContext().timer.UpdateTime(clock_.GetElapsedTime());

        if(cb_redraw_ == nullptr){
            throw std::runtime_error("Redraw callback is not initialized");
        }
        cb_redraw_();
    }

private:
    Game& game_;
    Clock clock_;
    std::optional<Direction> dir_;
    std::function<void()> cb_redraw_;

    void MovePlayer(){
        if(dir_.has_value()){
            Callback post_effect = [&](){MovePlayer();};
            game_.GetPlayer().GoCommand(dir_.value(), post_effect);
        }
    }
};
