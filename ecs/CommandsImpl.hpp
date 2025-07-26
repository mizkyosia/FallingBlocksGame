#pragma once
#include "Global.hpp"
#include <bits/shared_ptr.h>
#include <variant>


// ====================== ENTITY COMMANDS ======================

struct SpawnEntityCommand
{
};

struct DespawnEntityCommand
{
};

struct RemoveComponentCommand
{
    ComponentID component;
};

struct AddComponentCommand
{
    ComponentID component;
    std::shared_ptr<void> data;
};

using AnyEntityCommand = std::variant<SpawnEntityCommand, DespawnEntityCommand, AddComponentCommand, RemoveComponentCommand>;

struct EntityCommand {
    Entity entity;
    AnyEntityCommand action;
};

using AnyCommand = std::variant<EntityCommand>;