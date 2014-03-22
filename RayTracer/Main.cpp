#include "RayTracerWorld.h"

#include <string>
#include "TransformObject.h"
#include <iostream>

using namespace std;

//Matrix-to-string conversion macros.

#define MTS(x, y, m) (to_string(m[Vector2i(x, y)]))
#define MTSSPACE "       "
#define MSTRROW(y, m) (string("[") + MTSSPACE + MTS(0, y, m) + MTSSPACE + MTS(1, y, m) + MTSSPACE + MTS(2, y, m) + MTSSPACE + MTS(3, y, m))
#define MSTRCOLSPACE (string(MTSSPACE) + "]\n")
#define MSTR(m) (MSTRROW(0, m) + MSTRCOLSPACE + MSTRROW(1, m) + MSTRCOLSPACE + MSTRROW(2, m) + MSTRCOLSPACE + MSTRROW(3, m) + MSTRCOLSPACE)

string ToString(Matrix4f & toPrint)
{
	return MSTR(toPrint);
}
string ToString(Vector3f & v)
{
	return string("{ ") + to_string(v.x) + ", " + to_string(v.y) + ", " + to_string(v.z) + " }";
}

void PauseConsole(void)
{
	char dummy;
	cin >> dummy;

	cout << "-----------------------------------------------------------------\n";
}

int main(void)
{
	RayTracerWorld().RunWorld();
}