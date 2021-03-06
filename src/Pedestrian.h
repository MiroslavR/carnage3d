#pragma once

#include "GameDefs.h"
#include "PhysicsDefs.h"
#include "CharacterController.h"
#include "GameObject.h"
#include "Sprite2D.h"
#include "PedestrianStates.h"

// defines generic city pedestrian
class Pedestrian final: public GameObject
{
    friend class GameObjectsManager;
    friend class PedPhysicsBody;
    friend class PedestrianStatesManager;

public:
    // public for convenience, should not be modified directly
    CharacterController* mController; // controls pedestrian actions
    PedPhysicsBody* mPhysicsBody;

    PedestrianCtlState mCtlState;

    float mCurrentStateTime = 0.0f; // time since current state has started
    float mWeaponRechargeTime = 0.0f; // next time weapon can be used again
    float mBurnStartTime = 0.0f;

    float mDrawHeight;
    int mRemapIndex;
    
    ePedestrianDeathReason mDeathReason = ePedestrianDeathReason_null; // has meaning only in 'dead state'

    // in car
    Vehicle* mCurrentCar = nullptr;
    eCarSeat mCurrentSeat;

    // inventory
    eWeaponID mCurrentWeapon;
    int mWeaponsAmmo[eWeapon_COUNT]; // -1 means infinite, 'fists' is good example

public:
    // @param id: Unique object identifier, constant
    Pedestrian(GameObjectID id);
    ~Pedestrian();

    // override GameObject
    void UpdateFrame() override;
    void PreDrawFrame() override;
    void DebugDraw(DebugRenderer& debugRender) override;

    // Process damage, it may be ignored depending on type of damage and objects current state
    // @param damageInfo: Damage details
    bool ReceiveDamage(const DamageInfo& damageInfo) override;

    // setup initial state when spawned or respawned on level
    void Spawn(const glm::vec3& startPosition, cxx::angle_t startRotation);

    // set current weapon type
    void ChangeWeapon(eWeaponID weapon);

    // Instant kill, pedestrian will remain in dead state until respawn
    void DieFromDamage(eDamageCause damageCause);

    // get seat in car, pedestrian should be on foot
    // @param targetCar: Cannot be null
    // @param targetSeat: Cannot be 'any'
    void EnterCar(Vehicle* targetCar, eCarSeat targetSeat);

    // gracefully leave current vehicle
    void ExitCar();

    // check if pedestrian entering/exiting or driving car at this moment
    bool IsCarPassenger() const;
    bool IsCarDriver() const;
    bool IsEnteringOrExitingCar() const;

    // check if pedestrian is in specific state
    bool IsIdle() const; // standing, shooting, walking
    bool IsStanding() const;
    bool IsShooting() const;
    bool IsWalking() const;
    bool IsUnconscious() const;
    bool IsDead() const;
    bool IsBurn() const;

    // detects identifier of current pedestrian state
    ePedestrianState GetCurrentStateID() const;

private:
    void SetAnimation(ePedestrianAnimID animation, eSpriteAnimLoop loopMode);
    void ComputeDrawHeight(const glm::vec3& position);

    void SetDead(ePedestrianDeathReason deathReason);

    void SetCarEntered(Vehicle* targetCar, eCarSeat targetSeat);
    void SetCarExited();

    void SetBurnEffectActive(bool isActive);
    void UpdateBurnEffect();

    void SetDrawOrder(eSpriteDrawOrder drawOrder);

private:
    ePedestrianAnimID mCurrentAnimID;
    SpriteAnimation mCurrentAnimState;
    PedestrianStatesManager mStatesManager;
    // active effects
    Decoration* mFireEffect = nullptr;
};

const int Sizeof_Pedestrian = sizeof(Pedestrian);