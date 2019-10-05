#ifndef __DISTANCE_2D_H__
#define __DISTANCE_2D_H__

// This header file has the purpose of practicing GJK from Box2D articles
// I'm tracing the code for the practice

#include "chMath.hpp"

namespace Chan
{
	/// Polygon used by the GJK algorithm
	struct Polygon
	{
		/// Get the supporting point index in the given direction
		int GetSupport(const ChVector2& d) const;

		const ChVector2* m_points;
		int m_count;
	};

	/// A simplex vertex
	struct SimplexVertex
	{
		ChVector2 point1;	// support point in polygon1
		ChVector2 point2;	// support point in polygon2
		ChVector2 point;	// point2 - point1
		float u;			// unnormalized barycentric coordinate for closest point
		int index1;			// point 1 index
		int index2;			// point 2 index
	};

	/// Used to warm start b2Distance.
	/// Set count to zero on first call.
	struct SimplexCache
	{
		ChReal metric;		///< length or area
		int count;
		int indexA[3];	///< vertices on shape A
		int indexB[3];	///< vertices on shape B
	};

	/// Input for the distance function
	struct Input
	{
		Polygon polygon1;
		Polygon polygon2;
		ChTransform transform1;
		ChTransform transform2;
	};

	/// Simplex used by the GJK algorithm
	struct Simplex
	{
		void ReadCache(const SimplexCache& cache, const Input& input);
		void WriteCache(SimplexCache* cache) const;

		ChVector2 GetSearchDirection() const;
		ChVector2 GetClosestPoint() const;
		ChReal GetMetric() const;
		void GetWitnessPoints(ChVector2* point1, ChVector2* point2) const;
		void Solve2();
		void Solve3();

		SimplexVertex m_vertexA, m_vertexB, m_vertexC;
		float m_divisor; // denominator to normalize barycentric coordinates
		int m_count;
	};

	/// Output for the distance function
	struct Output
	{
		enum
		{
			e_maxSimplices = 20
		};

		ChVector2 point1;		///< closest point on polygon 1
		ChVector2 point2;		///< closest point on polygon 2
		float distance;
		int iterations;			///< number of GJK iterations used

		Simplex simplices[e_maxSimplices];
		int simplexCount;
	};

	/// Get the closest points between two point clouds.
	void Distance2D(Output* output, const Input& input);
	void Distance2D(Output* output, SimplexCache* cache,  const Input& input);
}

#endif
