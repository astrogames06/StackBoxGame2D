#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cstdlib>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#include "box2d/box2d.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

const int SCREEN_WIDTH = 1400;
const int SCREEN_HEIGHT = 800;

struct PhysicsObject
{
    b2BodyId physicsBody;
    Vector2 size;
};

b2WorldId physicsWorld;

std::vector<PhysicsObject> dynamicBoxes;
std::vector<PhysicsObject> groundObjects;

Vector2 defaultBoxSize = { 40, 40 };

void DrawPhysicsObject(const PhysicsObject& object)
{
    b2Vec2 bodyPosition = b2Body_GetPosition(object.physicsBody);
    b2Rot bodyRotation = b2Body_GetRotation(object.physicsBody);

    float rotationDegrees = RAD2DEG * b2Rot_GetAngle(bodyRotation);

    DrawRectanglePro(
        {
            bodyPosition.x - 1,
            bodyPosition.y - 1,
            object.size.x + 2,
            object.size.y + 2
        },
        {
            object.size.x * 0.5f,
            object.size.y * 0.5f
        },
        rotationDegrees,
        BLACK
    );

    DrawRectanglePro(
        {
            bodyPosition.x,
            bodyPosition.y,
            object.size.x,
            object.size.y
        },
        {
            object.size.x * 0.5f,
            object.size.y * 0.5f
        },
        rotationDegrees,
        RED
    );
}

void UpdateDrawFrame()
{
    b2World_Step(physicsWorld, GetFrameTime(), 4);

    BeginDrawing();

    ClearBackground(DARKGRAY);

    DrawText("Box2D + Raylib", 20, 20, 20, LIGHTGRAY);

    for (auto& groundObject : groundObjects)
    {
        DrawPhysicsObject(groundObject);
    }

    for (auto& boxObject : dynamicBoxes)
    {
        DrawPhysicsObject(boxObject);

        // b2Vec2 movementVelocity = {0, 0};

        // float movementSpeed = 200.0f;

        // if (IsKeyDown(KEY_W)) movementVelocity.y = -movementSpeed;
        // if (IsKeyDown(KEY_S)) movementVelocity.y = movementSpeed;
        // if (IsKeyDown(KEY_A)) movementVelocity.x = -movementSpeed;
        // if (IsKeyDown(KEY_D)) movementVelocity.x = movementSpeed;

        // b2Body_SetLinearVelocity(boxObject.physicsBody, movementVelocity);
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        b2BodyDef bodyDefinition = b2DefaultBodyDef();

        bodyDefinition.type = b2_dynamicBody;

        bodyDefinition.position =
        {
            (float)GetMouseX(),
            (float)GetMouseY()
        };

        PhysicsObject newBox;

        newBox.physicsBody = b2CreateBody(physicsWorld, &bodyDefinition);
        newBox.size = defaultBoxSize;

        b2Polygon collisionBox = b2MakeBox(
            defaultBoxSize.x * 0.5f,
            defaultBoxSize.y * 0.5f
        );

        b2ShapeDef shapeDefinition = b2DefaultShapeDef();

        b2CreatePolygonShape(
            newBox.physicsBody,
            &shapeDefinition,
            &collisionBox
        );

        dynamicBoxes.push_back(newBox);
    }

    EndDrawing();
}

int main(void)
{
    InitWindow(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        "box2d raylib"
    );

    SetTargetFPS(60);

    b2WorldDef worldDefinition = b2DefaultWorldDef();

    worldDefinition.gravity = { 10.0f, 1000.0f };

    physicsWorld = b2CreateWorld(&worldDefinition);

    Vector2 groundSize = { 200, 40 };

    for (int i = 0; i < 5; i++)
    {
        b2BodyDef bodyDefinition = b2DefaultBodyDef();

        bodyDefinition.position =
        {
            150.0f + i * 120.0f,
            380.0f
        };

        PhysicsObject groundObject;

        groundObject.physicsBody =
            b2CreateBody(physicsWorld, &bodyDefinition);

        groundObject.size = groundSize;

        b2Polygon groundCollisionBox =
            b2MakeBox(
                groundSize.x * 0.5f,
                groundSize.y * 0.5f
            );

        b2ShapeDef shapeDefinition = b2DefaultShapeDef();

        b2CreatePolygonShape(
            groundObject.physicsBody,
            &shapeDefinition,
            &groundCollisionBox
        );

        groundObjects.push_back(groundObject);
    }

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }
#endif

    CloseWindow();

    return 0;
}