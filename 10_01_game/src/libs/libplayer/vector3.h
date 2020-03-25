#pragma once

#include "libserver/common.h"
#include <string>

struct Vector3
{
	Vector3(const float x, const float y, const float z) :X(x), Y(y), Z(z) {}

	void ParserFromProto(Proto::Vector3 position);
	void SerializeToProto(Proto::Vector3* pProto) const;
	double GetDistance(Vector3 point) const;

	float X{ 0 };
	float Y{ 0 };
	float Z{ 0 };

	static Vector3 Zero;
};

std::ostream& operator <<(std::ostream& os, Vector3& v);

