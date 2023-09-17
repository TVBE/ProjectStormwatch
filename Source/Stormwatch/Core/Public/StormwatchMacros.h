// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#pragma once

#define PLAYER_COMPONENT_BODY() \
protected: \
    APlayerCharacter* GetPlayerCharacter() const \
    { \
        return static_cast<APlayerCharacter>(GetOwner()); \
    } \
    APlayerCharacterController* GetPlayerCharacterController() const \
    { \
        APlayerCharacter* PlayerCharacter = GetPlayerCharacter(); \
        if (PlayerCharacter) \
        { \
            return static_cast<APlayerCharacterController*>(PlayerCharacter->GetController()); \
        } \
        return nullptr; \
    } \
private:


