#pragma once

#include "utility/utility.h"
#include "utility/random.h"
#include "utility/timer.h"
#include "utility/darkener.h"
#include "utility/darkeners.h"


class Object;
class Field;
class RandomGen;
class AssetLoader;
class Inventory;

struct GameContext {
    ObjectMap<Object>& object_map;  //Хранит все объекты игрового мира
    Field& field;                   //Хранит игровую карту со всеми этажами
    RandomGen& random;              //Для внесения случайных изменений
    AssetLoader& asset_loader;      //Для загрузки ассетов
    Timer& timer;
    Inventory& inventory;
};

class Painter;

struct DrawContext {
    Painter& painter;
    FlashlightDarkener darkener;
};
