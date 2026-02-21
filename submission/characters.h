#pragma once

#include "utility/geometry.h"
#include "character.h"
#include "utility/random.h"

enum Phase {
    BROWNIAN, // Ходит хаотично, пока не встретил робота.
    FOLLOW,   // Встретил робота, ходит за роботом.
};

class Victim : public Character{
public:
    Victim(GameContext& context, const Coordinate& position, const Direction& dir)
        : Character(context, position, dir){
        SetSprites(context.asset_loader.LoadWithDirection("characters", "victim"));
        NextStep();
    }

    void Interact(Character& character, Direction) override{
        phase_ = Phase::FOLLOW;
        player_ = std::make_shared<Character>(character);
    }

    void MakeBrownianStep(){
        if(!CanPassWall(GetDirection()) || !CanGo(GetDirection()) || remaining_steps_ <= 0){
            SetDirection(NewDir());
            remaining_steps_ = GetContext().random.GetInRange(2, 8);
            Wait();
            return;
        }
        auto cb_next_step = [this](){NextStep();};
        DoTransition(GetDirection(), cb_next_step);
        --remaining_steps_;
    }

    void MakeFollowStep(){
        if(GetDistance(GetPosition(), player_->GetExactPos()) > 5){
            phase_ = Phase::BROWNIAN;
            SetDirection(NewDir());
            Wait();
            return;
        }

        Coordinate cur_pos_ = GetPosition();
        Coordinate dst_pos_ = player_->GetPrevPosition();
        std::optional<Direction> dir = GetDirectionToward(cur_pos_, dst_pos_);
        if((dir == std::nullopt) || !CanPassWall(dir.value()) || !CanGo(dir.value())){
            Wait();
        }else{
            SetDirection(dir.value());
            auto cb_next_step = [this](){NextStep();};
            DoTransition(dir.value(), cb_next_step);
        }
    }

    void Wait(){
        auto cb_next_step = [this](){NextStep();};
        guard_ = GetContext().timer.PlanCallback(GetContext().timer.Now() + 200, cb_next_step);
    }

    void NextStep(){
        switch (phase_) {
        case Phase::BROWNIAN:
            MakeBrownianStep();
            break;
        case Phase::FOLLOW:
            MakeFollowStep();
            break;
        default:
            break;
        }
    }

private:
    Phase phase_ = Phase::BROWNIAN;
    std::shared_ptr<Character> player_ = nullptr;
    int remaining_steps_ = GetContext().random.GetInRange(2, 8);
    Timer::Guard guard_;

    Direction NewDir(){
        auto dirs = std::vector{
            Direction::kDown, Direction::kLeft,
            Direction::kRight, Direction::kUp
        };

        // Удаляем текущее направление.
        dirs.erase(std::find(dirs.begin(), dirs.end(), GetDirection()));

        // Выбираем случайный элемент вектора.
        return GetContext().random.GetRandomElem(dirs);
    }
};
