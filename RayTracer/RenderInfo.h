#pragma once

#include "SFMLOpenGLWorld.h"
#include "OpenGLIncludes.h"
#include "Camera.h"
#include "TransformObject.h"

//Represents all useful rendering data.
struct RenderInfo
{
	SFMLOpenGLWorld * World;
	Camera * Cam;
	TransformObject * Trans;

	Matrix4f * mWVP, * mWorld, * mView, * mProj;

	RenderInfo(SFMLOpenGLWorld * world, Camera * camera, TransformObject * trans, Matrix4f * wvpM, Matrix4f * worldM, Matrix4f * viewM, Matrix4f * projM)
		: World(world), Cam(camera), Trans(trans), mWVP(wvpM), mWorld(worldM), mView(viewM), mProj(projM) { }
	RenderInfo(const RenderInfo & copy) : World(copy.World), Cam(copy.Cam), Trans(copy.Trans), mWVP(copy.mWVP), mWorld(copy.mWorld), mView(copy.mView), mProj(copy.mProj) { }
};