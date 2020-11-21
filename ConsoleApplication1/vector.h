class Vec3 {
public:
	Vec3() {
		this->x = 0;
		this->y = 0;
		this->z = 0;
	};

	Vec3(float px, float py, float pz) {
		this->x = px;
		this->y = py;
		this->z = pz;
	};

	Vec3 sub(Vec3 v) {
		Vec3 p = Vec3(this->x, this->y, this->z);
		p.x -= v.x;
		p.y -= v.y;
		p.z -= v.z;
		return p;
	};

	float dot(Vec3 vec) {
		float temp = 0;
		temp = this->x * vec.x + this->y * vec.y + this->z * vec.z;
		return temp;
	};

	float dist(Vec3 vec) {
		return sqrt((this->x - vec.x) * (this->x - vec.x) + (this->y - vec.y) * (this->y - vec.y) + (this->z - vec.z) * (this->z - vec.z));
	}

	Vec3 mul(Vec3 v) {
		return Vec3(x * v.x, y * v.y, z * v.z);
	};

	Vec3 mul(float f) {
		return Vec3(x * f, y * f, z * f);
	};

	Vec3 div(Vec3 v) {
		return Vec3(x / v.x, y / v.y, z / v.z);
	};

	Vec3& operator + (const Vec3& v) const {
		return Vec3(x + v.x, y + v.y, z + v.z);
	}

	Vec3& operator / (const float& div) const {
		return Vec3(x / div, y / div, z / div);
	}

	Vec3& operator - (const Vec3& v) const {
		return Vec3(x - v.x, y - v.y, z - v.z);
	}

	float Length() {
		float flLengthX, flLengthY, flLengthZ, flLength;
		flLengthX = x * x;
		flLengthY = y * y;
		flLengthZ = z * z;
		flLength = sqrt(flLengthX + flLengthY + flLengthZ);
		return fabs(flLength);
	}

	void Normalize() {
		float flLength, iLength;
		flLength = this->Length();
		if (flLength) {
			iLength = 1 / flLength;
			this->x *= iLength;
			this->y *= iLength;
			this->z *= iLength;
		}
	}

	Vec3& div(float f) {
		x /= f;
		y /= f;
		z /= f;
		return *this;
	}

	float x;
	float y;
	float z;
};