#pragma once
#ifndef __CH_MATH_H__
#define __CH_MATH_H__

#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <float.h>

namespace Chan
{
	

#define CH_MATH_PRECISION 1

#if CH_MATH_PRECISION == 1

	typedef float ChReal;
#define ChReal_sqrt sqrtf
#define ChReal_cos cosf
#define ChReal_sin sinf
#define Chreal_max FLT_MAX
#define Chreal_epsilon 0.0001
#define Chreal_abs fabsf
#elif
	typedef double ChReal;
#define ChReal_sqrt std::sqrt
#define ChReal_cos std::cos
#define ChReal_sin std::sin
#define Chreal_max DBL_MAX
#define Chreal_epsilon 0.0001
#define Chreal_abs fabs
#endif

	class ChVector2
	{
	public:
		ChVector2() { }
		ChVector2(ChReal s)
			: x(s), y(s) { }
		ChVector2(ChReal _x, ChReal _y) 
			: x(_x), y(_y) { }

		void Set(ChReal x_, ChReal y_) { x = x_; y = y_; }

		ChVector2 operator -() const { return ChVector2(-x, -y); }
		
		void operator += (const ChVector2& v)
		{
			x += v.x; y += v.y;
		}

		void operator -= (const ChVector2& v)
		{
			x -= v.x; y -= v.y;
		}

		void operator *= (const ChVector2& v)
		{
			x *= v.x; y *= v.y;
		}

		ChReal operator[](unsigned i) const
		{
			assert(i >= 0 && i < 2);
			return (&x)[i];
		}

		float Length() const
		{
			return ChReal_sqrt(x * x + y * y);
		}


		ChReal x;
		ChReal y;
	};

	class ChVector3
	{
	public:
		ChVector3() { }
		ChVector3(ChReal _x, ChReal _y, ChReal _z) 
			: x(_x), y(_y), z(_z) { }

		ChReal x;
		ChReal y;
		ChReal z;
	};

	class ChMat22
	{
	public:
		ChMat22() { }
		ChMat22(ChReal angle)
		{
			ChReal c = ChReal_cos(angle), s = ChReal_sin(angle);
			col1.x = c; col2.x = -s;
			col1.y = s; col2.y = c;
		}

		ChMat22(const ChVector2& col1, const ChVector2& col2) : col1(col1), col2(col2) { }

		ChMat22 Transpose() const
		{
			return ChMat22(ChVector2(col1.x, col2.x), ChVector2(col1.y, col2.y));
		}

		ChMat22 Invert() const
		{
			ChReal a = col1.x, b = col2.x, c = col1.y, d = col2.y;
			ChMat22 B;
			ChReal det = a * d - b * c;
			assert(det != 0.0f);
			det = ChReal(1.0) / det;
			B.col1.x = det * d;		B.col2.x = -det * b;
			B.col1.y = -det * c;	B.col2.y = det * a;
			return B;
		}


		ChVector2 col1, col2;
	};

	struct ChTransform
	{
		ChMat22 R;
		ChVector2 p;
	};

	inline ChReal dot(const ChVector2& a, const ChVector2& b)
	{
		return a.x * b.x + a.y * b.y;
	}

	inline ChReal Cross(const ChVector2& a, const ChVector2& b)
	{
		return a.x * b.y - a.y * b.x;
	}

	inline ChVector2 Cross(const ChVector2& a, ChReal s)
	{
		return ChVector2(s * a.y, -s * a.x);
	}

	inline ChVector2 Cross(ChReal s, const ChVector2& a)
	{
		return ChVector2(-s * a.y, s * a.x);
	}

	inline ChVector2 Mul(const ChTransform& T, const ChVector2& v)
	{
		return T.R * v + T.p;
	}

	inline ChReal dot(const ChVector3& a, const ChVector3& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	inline ChVector2 operator * (const ChVector2& v, const ChReal& s)
	{
		return ChVector2(v.x * s, v.y * s);
	}

	inline ChVector2 operator * (const ChMat22& A, const ChVector2& v)
	{
		return ChVector2(A.col1.x * v.x + A.col2.x * v.y, A.col1.y * v.x + A.col2.y * v.y);
	}

	inline ChVector2 operator + (const ChVector2& a, const ChVector2& b)
	{
		return ChVector2(a.x + b.x, a.y + b.y);
	}

	inline ChVector2 operator - (const ChVector2& a, const ChVector2& b)
	{
		return ChVector2(a.x - b.x, a.y - b.y);
	}

	inline ChVector2 operator * (const ChVector2& a, const ChVector2& b)
	{
		return ChVector2(a.x * b.x, a.y * b.y);
	}

	inline ChVector2 operator * (ChReal s, const ChVector2& v)
	{
		return ChVector2(s * v.x, s * v.y);
	}

	inline ChMat22 operator + (const ChMat22& A, const ChMat22& B)
	{
		return ChMat22(A.col1 + B.col1, A.col2 + B.col2);
	}

	inline ChMat22 operator * (const ChMat22& A, const ChMat22& B)
	{
		return ChMat22(A * B.col1, A * B.col2);
	}

	inline ChVector3 operator - (const ChVector3& a, const ChVector3& b)
	{
		return ChVector3(a.x - b.x, a.y - b.y, a.z - b.z);
	}

	inline ChReal Abs(ChReal a)
	{
		return a > ChReal(0.0) ? a : -a;
	}

	inline ChVector2 Abs(const ChVector2& a)
	{
		return ChVector2(Abs(a.x), Abs(a.y));
	}

	inline ChMat22 Abs(const ChMat22& A)
	{
		return ChMat22(Abs(A.col1), Abs(A.col2));
	}

	inline ChReal Sign(ChReal x)
	{
		return x < ChReal(0.0) ? ChReal(-1.0) : ChReal(1.0);
	}

	inline ChReal Min(ChReal a, ChReal b)
	{
		return a < b ? a : b;
	}

	inline ChReal Max(ChReal a, ChReal b)
	{
		return a > b ? a : b;
	}

	inline ChVector2 Min(const ChVector2& a, const ChVector2& b)
	{
		return ChVector2(Min(a.x, b.x), Min(b.x, b.y));
	}

	inline ChVector2 Max(const ChVector2& a, const ChVector2& b)
	{
		return ChVector2(Max(a.x, b.x), Max(b.x, b.y));
	}

	inline ChReal Clamp(ChReal a, ChReal low, ChReal high)
	{
		return Max(low, Min(a, high));
	}

	inline ChVector2 Normalize(const ChVector2& v)
	{
		ChReal length = dot(v, v);
		return v * (ChReal(1.0) / ChReal_sqrt(length));
	}

	inline ChReal Distance(const ChVector2& a, const ChVector2& b)
	{
		return ChReal_sqrt(dot(a, b));
	}

	template<typename T> inline void Swap(T& a, T& b)
	{
		T tmp = a;
		a = b;
		b = tmp;
	}

	// Random number in range [-1, 1]
	inline ChReal Random()
	{
		ChReal r = (ChReal)rand();
		r /= RAND_MAX;
		r = ChReal(2.0) * r - ChReal(1.0);
		return r;
	}

	inline ChReal Random(ChReal lo, ChReal hi)
	{
		ChReal r = (ChReal)rand();
		r /= RAND_MAX;
		r = (hi - lo) * r + lo;
		return r;
	}

#define aabbExtension 0.01
#define aabbMultiplier 2.0
	struct c2AABB // CG Project 2-dimenstional AABB
	{
		ChVector2 min;
		ChVector2 max;

		ChReal GetPerimeter() const
		{
			ChReal wx = max.x - min.x;
			ChReal wy = max.y - min.y;

			return ChReal(4) * (wx + wy);
		}

		void Combine(const c2AABB& aabb)
		{
			min = Min(min, aabb.min);
			max = Max(max, aabb.max);
		}

		void Combine(const c2AABB& aabb1, const c2AABB& aabb2)
		{
			min = Min(aabb1.min, aabb2.min);
			max = Max(aabb1.max, aabb2.max);
		}

		// Does this aabb contain the provided AABB;
		bool Contains(const c2AABB& aabb) const
		{
			bool result = true;
			result = result && min.x <= aabb.min.x;
			result = result && min.y <= aabb.min.y;
			result = result && aabb.max.x <= max.x;
			result = result && aabb.max.y <= max.y;
			return result;
		}
	};

	struct chLiteBody;
	void makeAABB(const chLiteBody* body, c2AABB* out);

	struct c2RayInput
	{
		c2RayInput()
			: startPoint(0, 0), direction(0, 0)
		{	}

		c2RayInput(const ChVector2& rayFrom, const ChVector2& rayTo)
			: startPoint(rayFrom)
		{
			direction = Normalize(rayTo - rayFrom);
		}

		ChVector2 startPoint;
		ChVector2 direction;
	};

	struct c2RayOutput
	{
		ChVector2 hitPoint; // literally, hitPoint between ray and object
		ChReal t; // startPoint + t * direction from rayInput
		ChVector2 startPoint; // From RayInput
	};

	inline bool aabbOverlap(const c2AABB& a, const c2AABB& b)
	{
		// Exit with no intersection if separated along an axis
		if (a.max.x < b.min.y || a.min.x > b.max.x) return false;
		if (a.max.y < b.min.y || a.min.y > b.max.y) return false;

		// Overlapping on all axes means AABBs are intersecting
		return true;
	}

	// RTCD p180 ~ 181.
	inline bool rayaabbOverlap(const c2AABB& a, const c2RayInput& ray)
	{
		ChReal tmin = -Chreal_max;
		ChReal tmax = Chreal_max;
		// For all three slabs
		for (int i = 0; i < 3; ++i)
		{
			if (Chreal_abs(ray.direction[i]) < Chreal_epsilon)
			{
				// Ray is parallel to slab. No hit if origin not within slab
				if (ray.startPoint[i] < a.min[i] || ray.startPoint[i] > a.max[i]) return false;
			}
			else
			{
				// Compute intersection t value of ray with near and far plane of slabe
				ChReal ood = ChReal(1.0) / ray.direction[i];
				ChReal t1 = (a.min[i] - ray.startPoint[i]) * ood;
				ChReal t2 = (a.max[i] - ray.startPoint[i]) * ood;
				// Make t1 be intersection with near plane, t2 with far plane
				if (t1 > t2) Swap(t1, t2);
				// Compute the intersection of slab intersection intervals
				tmin = Max(tmin, t1);
				tmax = Min(tmax, t2);
				// Exit with no collision as soon as slab intersection becomes empty
				if (tmin > tmax) return false;
			}
		}
		// Ray intersects all 3slabs. Return point (q) and intersection t value (tmin)
		// return ray.startPoint + ray.direction * tmin;
		return true;
	}

	inline bool rayaabbIntersection(c2RayOutput& output,
		const c2RayInput& input, const c2AABB& aaabb)
	{
		ChReal tmin = -Chreal_max;
		ChReal tmax = Chreal_max;
		// For all three slabs
		for (int i = 0; i < 3; ++i)
		{
			if (Chreal_abs(input.direction[i]) < Chreal_epsilon)
			{
				// Ray is parallel to slab. No hit if origin not within slab
				if (input.startPoint[i] < aaabb.min[i] || input.startPoint[i] > aaabb.max[i]) return false;
			}
			else
			{
				// Compute intersection t value of ray with near and far plane of slabe
				ChReal ood = ChReal(1.0) / input.direction[i];
				ChReal t1 = (aaabb.min[i] - input.startPoint[i]) * ood;
				ChReal t2 = (aaabb.max[i] - input.startPoint[i]) * ood;
				// Make t1 be intersection with near plane, t2 with far plane
				if (t1 > t2) Swap(t1, t2);
				// Compute the intersection of slab intersection intervals
				tmin = Max(tmin, t1);
				tmax = Min(tmax, t2);
				// Exit with no collision as soon as slab intersection becomes empty
				if (tmin > tmax) return false;
			}
		}
		// Ray intersects all 3slabs. Return point (q) and intersection t value (tmin)
		output.startPoint = input.startPoint;
		output.t = tmin;
		output.hitPoint = input.startPoint + input.direction * tmin;
		return true;
	}
}
#endif
