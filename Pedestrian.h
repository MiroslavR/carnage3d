#pragma once

#include "GameDefs.h"
#include "PhysicsDefs.h"
#include "PedestrianControl.h"

// defines generic city pedestrian
class Pedestrian final: public cxx::noncopyable
{
    friend class GameObjectsManager;

public:
    PedestrianControl mControl; // control pedestrian actions

    // public for convenience, should not be modified directly
    const unsigned int mID; // unique identifier

    PedPhysicsComponent* mPhysicsComponent;
    bool mDead;
    bool mMarkForDeletion;

    eSpriteAnimationID mCurrentAnimID;
    SpriteAnimation mAnimation;

public:
    // @param id: Unique object identifier, constant
    Pedestrian(unsigned int id);
    ~Pedestrian();

    // setup initial state when spawned on level
    void EnterTheGame();
    void UpdateFrame(Timespan deltaTime);

    // state control
    void SetHeading(float rotationDegrees);
    void SetPosition(const glm::vec3& position);

    // change current animation
    void SwitchToAnimation(eSpriteAnimationID animation, eSpriteAnimLoop loopMode);

    // test whether pedestrian is in air
    bool IsFalling() const;

private:
    // internal stuff that can be touched only by PedestrianManager
    cxx::intrusive_node<Pedestrian> mActivePedsNode;
    cxx::intrusive_node<Pedestrian> mDeletePedsNode;
};
