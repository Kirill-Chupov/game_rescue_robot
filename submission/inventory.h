#pragma once

#include "utility/painter.h"
#include "context.h"

class InventoryKey;

class InventoryObject {
public:
    // Возвратит указатель на InventoryKey, если объект является ключом.
    virtual const InventoryKey* AsKey() const {
        return nullptr;
    }

    // Возвратит true, если объект является водой.
    virtual bool IsWater() const {
        return false;
    }

    // Нарисует элемент в инвентаре.
    virtual void Draw(DrawContext& context, int offset) const = 0;
};

class InventoryKey : public InventoryObject {
public:
    InventoryKey(const GameContext& context, const std::string& name, QColor color) : key_name_{name} {
        asset_ = context.asset_loader.LoadSprite("inventory", "inventory-key");
        asset_.ChangeColor(color);
    }

    const std::string GetName() const{
        return key_name_;
    }

    const InventoryKey* AsKey() const override{
        return this;
    }

    void Draw(DrawContext& context, int offset) const override{
        context.painter.DrawInventoryItem(offset, asset_);
    }

private:
    Asset asset_;
    std::string key_name_;
};

class InventoryWater : public InventoryObject {
public:
    InventoryWater(const GameContext& context) {
        asset_ = context.asset_loader.LoadSprite("inventory", "drop");
    }

    bool IsWater() const override{
        return true;
    }

    void Draw(DrawContext& context, int offset) const override{
        context.painter.DrawInventoryItem(offset, asset_);
    }

private:
    Asset asset_;
};

class Inventory {
public:
    // Добавить предмет в инвентарь.
    bool Store(const std::shared_ptr<InventoryObject>& object) {
        if(HasItem(object.get())){
            return false;
        }
        inventory_.push_back(object);
        return true;
    }

    // Проверить наличие конкретного предмета.
    bool HasItem(const InventoryObject* item) const {
        return FindItem([item](const auto& obj) { return obj.get() == item; }) != nullptr;
    }

    // Убрать предмет из инвентаря.
    bool Remove(const InventoryObject* item) {
        auto it = std::remove_if(inventory_.begin(), inventory_.end(), [&](const auto& ptr) { return ptr.get() == item; });
        if(it != inventory_.end()){
            inventory_.erase(it, inventory_.end());
            return true;
        }
        return false;
    }

    // Найти предмет по условию.
    template<class Comp>
    InventoryObject* FindItem(Comp compare) const {
        auto it = std::find_if(inventory_.begin(), inventory_.end(), compare);
        return (it != inventory_.end()) ? it->get() : nullptr;
    }

    const auto& GetItems() const {
        return inventory_;
    }

private:
    std::vector<std::shared_ptr<InventoryObject>> inventory_;
};
