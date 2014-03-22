#pragma once

#include "Matrix4f.h"
#include "Vectors.h"

//Basic camera functionality: UVN rotation, transform calculation, etc.
//Can specify a "closest dot variance", which means the closest the "forward" vector can get
//     to pointing straight down/up, in terms of the dot product value.
class Camera
{
public:
	
	ProjectionInfo Info;

	Camera(void);
	Camera(Vector3f pos, Vector3f forward = Vector3f(1.0f, 0.0f, 0.0f), Vector3f up = Vector3f(0.0f, 0.0f, 1.0f), bool alreadyNormalized = false);

	Vector3f GetPosition(void) const { return pos; }
	Vector3f GetForward(void) const { return forward; }
	Vector3f GetUpward(void) const { return up; }
	Vector3f GetSideways(void) const { return up.Cross(forward); }
	float GetClosestDotVariance(void) const { return closestDotVariance; }

	void SetPosition(Vector3f newPos) { pos = newPos; }
	void SetPositionX(float newX) { SetPosition(Vector3f(newX, pos.y, pos.z)); }
	void SetPositionY(float newY) { SetPosition(Vector3f(pos.x, newY, pos.z)); }
	void SetPositionZ(float newZ) { SetPosition(Vector3f(pos.x, pos.y, newZ)); }
	void IncrementPosition(Vector3f amount) { SetPosition(pos + amount); }
	void IncrementPositionX(float newX) { IncrementPosition(Vector3f(newX, 0.0f, 0.0f)); }
	void IncrementPositionY(float newY) { IncrementPosition(Vector3f(0.0f, newY, 0.0f)); }
	void IncrementPositionZ(float newZ) { IncrementPosition(Vector3f(0.0f, 0.0f, newZ)); }
	void SetClosestDotVariance(float newVal) { closestDotVariance = newVal; }

	void SetRotation(Vector3f newForward, Vector3f newUp, bool alreadyNormalized);
	void AddPitch(float radians);
	void AddYaw(float radians);
	void AddRoll(float radians);

	void GetViewTransform(Matrix4f & outM) const;
	
private:

	Vector3f pos,
			 forward, up;

	float closestDotVariance;
};