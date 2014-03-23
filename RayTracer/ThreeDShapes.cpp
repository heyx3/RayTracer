#include "ThreeDShapes.h"

//TODO: Test.

bool Shape::TouchingPolygon(const Polygon & poly) const
{
    const Shape * shpe = this;
    return std::any_of(poly.GetTriangles().begin(), poly.GetTriangles().end(),
                       [shpe](const Triangle & tri) { return shpe->TouchingTriangle(tri); });
}


bool Cube::TouchingSphere(const Sphere & sphere) const
{
	Vector3f s1 = Bounds.GetTopLeftFront(),
			 s2 = Bounds.GetBottomRightBack(),
			 sph = sphere.GetCenter();
	float distSqr = BasicMath::Square(sphere.Radius);

	if (sph.x < s1.x) distSqr -= BasicMath::Square(sph.x - s1.x);
	else if (sph.x > s2.x) distSqr -= BasicMath::Square(sph.x - s2.x);
	if (sph.y < s1.y) distSqr -= BasicMath::Square(sph.y - s1.y);
	else if (sph.y > s2.y) distSqr -= BasicMath::Square(sph.y - s2.y);
	if (sph.z < s1.z) distSqr -= BasicMath::Square(sph.z - s1.z);
	else if (sph.z > s2.z) distSqr -= BasicMath::Square(sph.z - s2.z);

	return distSqr > 0.0f;
}
bool Cube::TouchingCapsule(const Capsule & capsule) const
{
    Interval cX = Bounds.GetXInterval(),
             cY = Bounds.GetYInterval(),
             cZ = Bounds.GetZInterval();
    Vector3f cap1 = capsule.GetEndpoint1(),
             cap2 = capsule.GetEndpoint2();
    Vector3f lineSource = cap1,
             lineDir = (cap2 - cap1).Normalized();
    
    //For each axis, get the subset of the capsule line inside the cube's domain along that axis.

    //First, get the min and max capsule endpoints for each axis.
    Vector3f capMinX, capMaxX,
             capMinY, capMaxY,
             capMinZ, capMaxZ;
    if (cap1.x < cap2.x)
    {
        capMinX = cap1;
        capMaxX = cap2;
    }
    else
    {
        capMaxX = cap1;
        capMinX = cap2;
    }
    if (cap1.y < cap2.y)
    {
        capMinY = cap1;
        capMaxY = cap2;
    }
    else
    {
        capMaxY = cap1;
        capMinY = cap2;
    }
    if (cap1.z < cap2.z)
    {
        capMinZ = cap1;
        capMaxZ = cap2;
    }
    else
    {
        capMaxZ = cap1;
        capMinZ = cap2;
    }

    //If the capsule lines are outside the cube bounds along this axis, don't bother checking this axis.
    if (capMinX.x < cX.GetStart())
    {
        Vector3f dirNormal = (capMinX - capMaxX).Normalized();
        capMinX = capMaxX + (dirNormal * (cX.GetStart() - capMaxX.x) / dirNormal.x);
    }
    if (capMaxX.x > cX.GetEnd())
    {
        Vector3f dirNormal = (capMaxX - capMinX).Normalized();
        capMaxX = capMinX + (dirNormal * (cX.GetEnd() - capMinX.x) / dirNormal.x);
    }



    if (capMinY.y < cY.GetStart())
    {
        Vector3f dirNormal = (capMinY - capMaxY).Normalized();
        capMinY = capMaxY + (dirNormal * (cY.GetStart() - capMaxY.y) / dirNormal.y);
    }
    if (capMaxY.y > cY.GetEnd())
    {
        Vector3f dirNormal = (capMaxY - capMinY).Normalized();
        capMaxY = capMinY + (dirNormal * (cY.GetEnd() - capMinY.y) / dirNormal.y);
    }
    if (capMinZ.z < cZ.GetStart())
    {
        Vector3f dirNormal = (capMinZ - capMaxZ).Normalized();
        capMinZ = capMaxZ + (dirNormal * (cZ.GetStart() - capMaxZ.z) / dirNormal.z);
    }
    if (capMaxZ.z > cZ.GetEnd())
    {
        Vector3f dirNormal = (capMaxZ - capMinZ).Normalized();
        capMaxZ = capMinZ + (dirNormal * (cZ.GetEnd() - capMinZ.z) / dirNormal.z);
    }





	return false;
}
bool Cube::TouchingPlane(const Plane & plane) const
{
	return false;
}
bool Cube::TouchingTriangle(const Triangle & tris) const
{
    return false;
}

Cube::RayTraceResult Cube::RayHitCheck(Vector3f rayStart, Vector3f rayDir) const
{
    typedef GeometricMath::PointOnLineAtValueResult<Vector3f> PointOnFaces;

    Vector3f center = Bounds.GetCenter();
    Interval dInts[3] = { Bounds.GetXInterval(), Bounds.GetYInterval(), Bounds.GetZInterval() };

    //Get the closest x, y, and z faces. If the vector is pointing parallel to a face, there is no intersection along that face.
    Vector3f faces((rayDir.x == 0.0f) ?
                       BasicMath::NaN :
                       ((rayDir.x > 0.0f) ?
                           (rayStart.x > Bounds.GetXMin() ? Bounds.GetXMax() : Bounds.GetXMin()) :
                           (rayStart.x < Bounds.GetXMax() ? Bounds.GetXMin() : Bounds.GetXMax())),
                   (rayDir.y == 0.0f) ?
                       BasicMath::NaN :
                       ((rayDir.y > 0.0f) ?
                           (rayStart.y > Bounds.GetYMin() ? Bounds.GetYMax() : Bounds.GetYMin()) :
                           (rayStart.y < Bounds.GetYMax() ? Bounds.GetYMin() : Bounds.GetYMax())),
                   (rayDir.z == 0.0f) ?
                       BasicMath::NaN :
                       ((rayDir.z > 0.0f) ?
                           (rayStart.z > Bounds.GetZMin() ? Bounds.GetZMax() : Bounds.GetZMin()) :
                           (rayStart.z < Bounds.GetZMax() ? Bounds.GetZMin() : Bounds.GetZMax())));

    //For each face, get the ray's intersection with that face.
    PointOnFaces faceIntersectData[3] =
    {
        (faces.x == BasicMath::NaN ? PointOnFaces(Vector3f(), -1.0f) : GeometricMath::GetPointOnLineAtValue(rayStart, rayDir, 0, faces.x)),
        (faces.y == BasicMath::NaN ? PointOnFaces(Vector3f(), -1.0f) : GeometricMath::GetPointOnLineAtValue(rayStart, rayDir, 1, faces.y)),
        (faces.z == BasicMath::NaN ? PointOnFaces(Vector3f(), -1.0f) : GeometricMath::GetPointOnLineAtValue(rayStart, rayDir, 2, faces.z)),
    };

    //Get the closest intersection.
    //Ignore any intersections that are behind the ray start or outside the face's bounds.
    RayTraceResult res;
    float closestDistSqr = std::numeric_limits<float>::max();
    int closestAxis = 0;
    float tempDistSqre;
    for (unsigned int axis = 0; axis < 3; ++axis)
    {
        if (faceIntersectData[axis].t > 0.0f &&
            dInts[(axis + 1) % 3].Touches(faceIntersectData[axis].Point[(axis + 1) % 3]) &&
            dInts[(axis + 2) % 3].Touches(faceIntersectData[axis].Point[(axis + 2) % 3]))
        {
            tempDistSqre = rayStart.DistanceSquared(faceIntersectData[axis].Point);
            if (tempDistSqre < closestDistSqr)
            {
                res = RayTraceResult(faceIntersectData[axis].Point, Vector3f());
                closestDistSqr = tempDistSqre;
                closestAxis = axis;
            }
            else
            {

            }
        }
    }
    res.ReflectNormal[closestAxis] = BasicMath::Sign(res.HitPos[closestAxis] - GetCenter()[closestAxis]);

    return res;
}


bool Sphere::TouchingCube(const Cube & cube) const
{
	Vector3f s1 = cube.GetBounds().GetTopLeftFront(),
			 s2 = cube.GetBounds().GetBottomRightBack(),
			 sph = GetCenter();
	float distSqr = BasicMath::Square(Radius);

	if (sph.x < s1.x) distSqr -= BasicMath::Square(sph.x - s1.x);
	else if (sph.x > s2.x) distSqr -= BasicMath::Square(sph.x - s2.x);
	if (sph.y < s1.y) distSqr -= BasicMath::Square(sph.y - s1.y);
	else if (sph.y > s2.y) distSqr -= BasicMath::Square(sph.y - s2.y);
	if (sph.z < s1.z) distSqr -= BasicMath::Square(sph.z - s1.z);
	else if (sph.z > s2.z) distSqr -= BasicMath::Square(sph.z - s2.z);

	return distSqr > 0.0f;
}
bool Sphere::TouchingCapsule(const Capsule & capsule) const
{
	Vector3f capPoint = GeometricMath::ClosestToLine(capsule.GetEndpoint1(), capsule.GetEndpoint2(), GetCenter(), false);

	return capPoint.DistanceSquared(GetCenter()) <= (BasicMath::Square(Radius) + BasicMath::Square(capsule.Radius));
}
bool Sphere::TouchingPlane(const Plane & plane) const
{
    //Get the min/max distances along the plane normal that would make the sphere touch the cube.

    Vector3f dirRad = plane.Normal * Radius;
    Vector3f min = GetCenter() - dirRad,
             max = GetCenter() + dirRad;
    float minDist = min.Dot(plane.Normal),
          maxDist = max.Dot(plane.Normal);

    float dist = plane.GetCenter().Dot(plane.Normal);

	return dist >= minDist && dist <= maxDist;
}
bool Sphere::TouchingTriangle(const Triangle & tris) const
{
    return false;
}

Sphere::RayTraceResult Sphere::RayHitCheck(Vector3f rayStart, Vector3f rayDir) const
{
	Vector3f cent = GetCenter();
	float r = Radius;

	Vector3f centerToRayStart = rayStart - cent;

	float rayDirSqr = rayDir.Dot(rayDir),
		  b = (centerToRayStart * 2.0f).Dot(rayDir),
		  c = (centerToRayStart.Dot(centerToRayStart)) - BasicMath::Square(r);

	float discriminant = BasicMath::Square(b) - (4.0f * rayDirSqr * c);

	//No roots.
	if (discriminant < 0.0f)
	{
		return RayTraceResult();
	}
	else
	{
		float denom = 1.0f / (2.0f * rayDirSqr);
		float t;
		
		//One root.
		if (discriminant == 0.0f)
		{
			t = -b * denom;
		}
		//Two roots.
		else
		{
			float sqt = sqrtf(discriminant);
			float t1 = -(b + sqt) * denom,
				  t2 = (sqt - b) * denom;
			if (t1 >= 0.0f && t1 < t2)
			{
				t = t1;
			}
			else
			{
				t = t2;
			}
		}

		if (t < 0.0f) return RayTraceResult();

		Vector3f p = (rayStart + (rayDir * t));
		return RayTraceResult(p, (p - cent).Normalized());
	}
}


bool Capsule::TouchingCube(const Cube & cube) const
{
	return false;
}
bool Capsule::TouchingSphere(const Sphere & sphere) const
{
	Vector3f capPoint = GeometricMath::ClosestToLine(l1, l2, sphere.GetCenter(), false);
	
	return capPoint.DistanceSquared(sphere.GetCenter()) <= (BasicMath::Square(sphere.Radius) + BasicMath::Square(Radius));
}
bool Capsule::TouchingCapsule(const Capsule & capsule) const
{
	GeometricMath::ClosestValues<Vector3f> cvs = GeometricMath::ClosestToIntersection(l1, l2, capsule.l1, capsule.l2, false);

	return cvs.OnFirstLine.DistanceSquared(cvs.OnSecondLine) <= (BasicMath::Square(Radius) + BasicMath::Square(capsule.Radius));
}
bool Capsule::TouchingPlane(const Plane & plane) const
{
	return false;
}
bool Capsule::TouchingTriangle(const Triangle & tris) const
{
    return false;
}

Capsule::RayTraceResult Capsule::RayHitCheck(Vector3f rayStart, Vector3f rayDir) const
{
	//No capsule ray hits yet.
	return RayTraceResult();
}

Box3D Capsule::GetBoundingBox(void) const
{
    return Box3D(BasicMath::Min(l1.x, l2.x) - Radius,
                 BasicMath::Max(l1.x, l2.x) + Radius,
                 BasicMath::Min(l1.y, l2.y) - Radius,
                 BasicMath::Max(l1.y, l2.y) + Radius,
                 BasicMath::Min(l1.z, l2.z) - Radius,
                 BasicMath::Max(l1.z, l2.z) + Radius);
}


bool Plane::TouchingCube(const Cube & cube) const
{
	return false;
}
bool Plane::TouchingSphere(const Sphere & sphere) const
{
    //Get the min/max distances along the plane normal that would make the sphere touch the cube.

    Vector3f dirRad = Normal * sphere.Radius;
    Vector3f min = sphere.GetCenter() - dirRad,
             max = sphere.GetCenter() + dirRad;
    float minDist = min.Dot(Normal),
          maxDist = max.Dot(Normal);

    float dist = GetCenter().Dot(Normal);

    return dist >= minDist && dist <= maxDist;
}
bool Plane::TouchingCapsule(const Capsule & capsule) const
{
	return false;
}
bool Plane::TouchingPlane(const Plane & plane) const
{
	float dot = BasicMath::Round(Normal.Dot(plane.Normal), 2);

	return (dot != 1.0f && dot != -1.0f) ||
		   (plane.GetCenter().Equals(GetCenter()));
}
bool Plane::TouchingTriangle(const Triangle & tris) const
{
    return false;
}

Plane::RayTraceResult Plane::RayHitCheck(Vector3f rayStart, Vector3f rayDir) const
{
	float denominator = rayDir.Dot(Normal);

	//If ray is perpendicular to plane, no intersection.
    if (BasicMath::Abs(denominator - 0.001f) == 0.0f) return RayTraceResult();

    float t = (GetCenter().Dot(Normal) - rayStart.Dot(Normal)) / denominator;
    if (t < 0.0f) return RayTraceResult();

    return RayTraceResult(rayStart + (rayDir * t), Normal);
}

Box3D Plane::GetBoundingBox(void) const
{
    const float min = std::numeric_limits<float>::min(),
                max = std::numeric_limits<float>::max();
    return Box3D(min, max, min, max, min, max);
}


Box3D Polygon::GetBoundingBox(void) const
{
    if (triangles.size() == 0) return Box3D();

    Vector3f min = triangles[0].GetVertices()[0],
             max = triangles[0].GetVertices()[0];

    std::_For_each(triangles.begin(), triangles.end(),
                   [&min, &max](const Triangle & tri)
                   {
                       const Vector3f * vertices = tri.GetVertices();
                       for (int i = 0; i < 3; ++i)
                       {
                           if (vertices[i].x < min.x)
                               min.x = vertices[i].x;
                           if (vertices[i].x > max.x)
                               max.x = vertices[i].x;

                           if (vertices[i].y < min.y)
                               min.y = vertices[i].y;
                           if (vertices[i].y > max.y)
                               max.y = vertices[i].y;

                           if (vertices[i].z < min.z)
                               min.z = vertices[i].z;
                           if (vertices[i].z > max.z)
                               max.z = vertices[i].z;
                       }
                   });

    return Box3D(min.x, max.x, min.y, max.y, min.z, max.z);
}