#include "vector3.h"
#include "libserver/util_string.h"

#include <cmath>

Vector3 Vector3::Zero = Vector3(0, 0, 0);

void Vector3::ParserFromProto(Proto::Vector3 position)
{
	this->X = position.x();
	this->Y = position.y();
	this->Z = position.z();
}

void Vector3::SerializeToProto(Proto::Vector3* pProto) const
{
	pProto->set_x(X);
	pProto->set_y(Y);
	pProto->set_z(Z);
}

double Vector3::GetDistance(Vector3 point) const
{
	const auto xv = point.X - this->X;
	const auto zv = point.Z - this->Z;
	return sqrt(xv * xv + zv * zv);
}

std::ostream& operator<<(std::ostream& os, Vector3& v)
{
    os << "x:" << v.X << " y:" << v.Y << " z:" << v.Z;
    return os;
}