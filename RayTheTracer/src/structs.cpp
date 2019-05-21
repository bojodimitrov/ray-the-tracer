struct point
{
	float x;
	float y;
	float z;
	point(): x(0), y(0), z(0)
	{
	}

	point(const float x, const float y, const float z) : x(x), y(y), z(z) {}
};