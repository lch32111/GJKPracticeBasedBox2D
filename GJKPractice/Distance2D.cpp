#include "Distance2D.h"

int Chan::Polygon::GetSupport(const ChVector2 & d) const
{
	int bestIndex = 0;
	float bestValue = dot(m_points[0], d);
	for (int i = 1; i < m_count; ++i)
	{
		float value = dot(m_points[i], d);
		if (value > bestValue)
		{
			bestIndex = i;
			bestValue = value;
		}
	}

	return bestIndex;
}

Chan::ChVector2 Chan::Simplex::GetSearchDirection() const
{
	switch (m_count)
	{
	case 1:
		return -m_vertexA.point;
	case 2:
	{
		ChVector2 edgeAB = m_vertexB.point - m_vertexA.point;
		ChReal sgn = Cross(edgeAB, -m_vertexA.point);
		if (sgn > ChReal(0.0)) // CounterClockWise BOA
		{
			// Turn Left 1-Simplex, the line-segment AB
			// Origin is left of edgeAB.
			return Cross(ChReal(1.0), edgeAB);
		}
		else // ClockWise BOA
		{
			// Turn Right 1-simplex, the line-segment AB
			// Origin is right of edgeAB
			return Cross(edgeAB, ChReal(1.0));
		}
	}
	default:
		assert(false);
		return ChVector2(0.0, 0.0);
	}
}

Chan::ChVector2 Chan::Simplex::GetClosestPoint() const
{
	switch (m_count)
	{
	case 1:
		return m_vertexA.point;

	case 2:
	{
		ChReal s = static_cast<ChReal>(1.0) / m_divisor;
		return (s * m_vertexA.u) * m_vertexA.point + (s * m_vertexB.u) * m_vertexB.point;
	}

	case 3:
		return ChVector2(ChReal(0.0), ChReal(0.0));

	default:
		assert(false);
		return ChVector2(ChReal(0.0), ChReal(0.0));
	}
}

void Chan::Simplex::GetWitnessPoints(ChVector2 * point1, ChVector2 * point2) const
{
	ChReal factor = ChReal(1.0) / m_divisor;

	switch (m_count)
	{
	case 1:
		*point1 = m_vertexA.point1;
		*point2 = m_vertexA.point2;
		break;

	case 2:
	{
		ChReal s = ChReal(1.0) / m_divisor;
		*point1 = (s * m_vertexA.u) * m_vertexA.point1 + (s * m_vertexB.u) * m_vertexB.point1;
		*point2 = (s * m_vertexA.u) * m_vertexA.point2 + (s * m_vertexB.u) * m_vertexB.point2;
		break;
	}

	case 3:
	{
		ChReal s = ChReal(1.0) / m_divisor;
		*point1 = (s * m_vertexA.u) * m_vertexA.point1 
			+ (s * m_vertexB.u) * m_vertexB.point1
			+ (s * m_vertexC.u) * m_vertexC.point1;
		*point2 = *point1;
		break;
	}

	default:
		assert(false);
		break;
	}
}

// Closest point on line segment to Q
// Voronoi regions : A, B, AB
void Chan::Simplex::Solve2(const ChVector2 & Q)
{
	ChVector2 A = m_vertexA.point;
	ChVector2 B = m_vertexB.point;

	// Compute barycentric coordinates (pre-division).
	ChReal u = dot(Q - B, A - B);
	ChReal v = dot(Q - A, B - A);

	// Region A
	if (v <= ChReal(0.0))
	{
		// Simplex is reduced to just vertex A
		m_vertexA.u = ChReal(1.0);
		m_divisor = ChReal(1.0);
		m_count = 1;
		return;
	}

	// Region B
	if (u <= ChReal(0.0))
	{
		// Simplex is reduced to just vertex B
		// WEmove vertexB into vertx A and reduce the count.
		m_vertexA = m_vertexB;
		m_vertexA.u = ChReal(1.0);
		m_divisor = ChReal(1.0);
		m_count = 1;
		return;
	}

	// Region AB. Due to the conditions above, we are
	// guaranteed that the edge has non-zero length and division
	// is safe
	m_vertexA.u = u;
	m_vertexB.u = v;
	ChVector2 e = B - A;
	m_divisor = dot(e, e);
	m_count = 2;
}

// Closest point on triangle to Q.
// Voronoi regions : A, B, C, AB, BC, CA, ABC
void Chan::Simplex::Solve3(const ChVector2 & Q)
{
	ChVector2 A = m_vertexA.point;
	ChVector2 B = m_vertexB.point;
	ChVector2 C = m_vertexC.point;

	// Compute edge barycentric coordinates (pre-division).
	ChReal uAB = dot(Q - B, A - B);
	ChReal vAB = dot(Q - A, B - A);

	ChReal uBC = dot(Q - C, B - C);
	ChReal vBC = dot(Q - B, C - B);

	ChReal uCA = dot(Q - A, C - A);
	ChReal vCA = dot(Q - C, A - C);

	// Region A
	if (vAB <= ChReal(0.0) && uCA <= ChReal(0.0))
	{
		m_vertexA.u = ChReal(1.0);
		m_divisor = 1.f;
		m_count = 1;
		return;
	}

	// Region B
	if (uAB <= ChReal(0.0) && vBC <= ChReal(0.0))
	{
		m_vertexA = m_vertexB;
		m_vertexA.u = ChReal(1.0);
		m_divisor = 1.f;
		m_count = 1;
		return;
	}

	// Region C
	if (uBC <= ChReal(0.0) && vCA <= ChReal(0.0))
	{
		m_vertexA = m_vertexC;
		m_vertexA.u = ChReal(1.0);
		m_divisor = 1.f;
		m_count = 1;
		return;
	}

	// Compute signed triangle area.
	ChReal area = Cross(B - A, C - A);

	// Compute triangle barycentric coordinates (pre-division).
	ChReal uABC = Cross(B - Q, C - Q);
	ChReal vABC = Cross(C - Q, A - Q);
	ChReal wABC = Cross(A - Q, B - Q);

	// Region AB
	if (uAB > ChReal(0.0) && vAB > ChReal(0.0) && wABC * area <= ChReal(0.0))
	{
		m_vertexA.u = uAB;
		m_vertexB.u = vAB;
		ChVector2 e = B - A;
		m_divisor = dot(e, e);
		m_count = 2;
		return;
	}

	// Region BC
	if (uBC > ChReal(0.0) && vBC > ChReal(0.0) && uABC * area <= ChReal(0.0))
	{
		m_vertexA = m_vertexB;
		m_vertexB = m_vertexC;
		m_vertexA.u = uBC;
		m_vertexB.u = vBC;
		ChVector2 e = C - B;
		m_divisor = dot(e, e);
		m_count = 2;
		return;
	}

	// Region CA
	if (uCA > ChReal(0.0) && vCA > ChReal(0.0) && vABC * area <= ChReal(0.0))
	{
		m_vertexB = m_vertexA;
		m_vertexA = m_vertexC;

		m_vertexA.u = uCA;
		m_vertexB.u = vCA;
		ChVector2 e = A - C;
		m_divisor = dot(e, e);
		m_count = 2;
		return;
	}

	// Region ABC
	// The triangle area is guaranteed to be non-zero.
	assert
	( 
		(uABC > ChReal(0.0) && vABC > ChReal(0.0) && wABC > ChReal(0.0)) ||
		(uABC < ChReal(0.0) && vABC < ChReal(0.0) && wABC < ChReal(0.0))
	);

	m_vertexA.u = uABC;
	m_vertexB.u = vABC;
	m_vertexC.u = wABC;
	m_divisor = area;
	m_count = 3;
}

// Compute the distance between two polygons using the GJK algorithm
void Chan::Distance2D(Output * output, const Input & input)
{
	const Polygon* polygon1 = &input.polygon1;
	const Polygon* polygon2 = &input.polygon2;

	ChTransform transform1 = input.transform1;
	ChTransform transform2 = input.transform2;

	// Initialize the simplex
	Simplex simplex;
	ChVector2 localPoint1 = polygon1->m_points[0];
	ChVector2 localPoint2 = polygon2->m_points[0];
	simplex.m_vertexA.point1 = Mul(transform1, localPoint1);
	simplex.m_vertexA.point2 = Mul(transform2, localPoint2);
	simplex.m_vertexA.point = simplex.m_vertexA.point2 - simplex.m_vertexA.point1;
	simplex.m_vertexA.u = ChReal(1.0);
	simplex.m_vertexA.index1 = 0;
	simplex.m_vertexA.index2 = 0;
	simplex.m_count = 1;

	// Begin recording the simplices for visualization
	output->simplexCount = 0;

	// Get simplex vertices as an array
	SimplexVertex* vertices = &simplex.m_vertexA;

	// These store the vertices of the last simplex so that we
	// can check for duplicates and prevent cycling.
	int save1[3], save2[3];
	int saveCount = 0;

	// Main iteration loop.
	const int k_maxIters = 20;
	int iter = 0;
	while (iter < k_maxIters)
	{
		// Copy simplex so we can identify duplicates.
		saveCount = simplex.m_count;
		for (int i = 0; i < saveCount; ++i)
		{
			save1[i] = vertices[i].index1;
			save2[i] = vertices[i].index2;
		}

		// Determine the closest point on the simplex and
		// remove unused vertices
		switch (simplex.m_count)
		{
		case 1:
			break;
			
		case 2:
			simplex.Solve2(ChVector2(ChReal(0.0), ChReal(0.0)));
			break;
			
		case 3:
			simplex.Solve3(ChVector2(ChReal(0.0), ChReal(0.0)));
			break;

		default:
			assert(false);
		}

		// Record for visualization.
		output->simplices[output->simplexCount++] = simplex;

		// If we have 3 points, then the origin is in the corresponding triangle
		if (simplex.m_count == 3)
		{
			break;
		}

		// Get search direction.
		ChVector2 d = simplex.GetSearchDirection();

		// Ensure the search direction non-zero
		if (dot(d, d) == ChReal(0.0))
		{
			break;
		}

		// Compute a tentative new simplex vertex using support points.
		SimplexVertex* vertex = vertices + simplex.m_count;
		vertex->index1 = polygon1->GetSupport(transform1.R.Transpose() * -d);
		vertex->point1 = Mul(transform1, polygon1->m_points[vertex->index1]);
		vertex->index2 = polygon2->GetSupport(transform2.R.Transpose() * d);
		vertex->point2 = Mul(transform2, polygon2->m_points[vertex->index2]);
		vertex->point = vertex->point2 - vertex->point1;

		// Iteration count is equated to the number of support point calls.
		++iter;

		// Check for duplicate support points. This is the main termination criteria
		bool duplicate = false;
		for (int i = 0; i < saveCount; ++i)
		{
			if (vertex->index1 == save1[i] && vertex->index2 == save2[i])
			{
				duplicate = true;
				break;
			}
		}

		// If we found a duplicate support point we must exit to avoid cycling.
		if (duplicate)
		{
			break;
		}

		// New vertex is ok and needed.
		++simplex.m_count;
	}

	// Prepare output
	simplex.GetWitnessPoints(&output->point1, &output->point2);
	output->distance = Distance(output->point1, output->point2);
	output->iterations = iter;
}
