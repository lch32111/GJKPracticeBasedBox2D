# GJKPracticeBasedBox2D
This is a repository for practicing GJK algorithm, based on GDC 2010 Presentation of Erin Catto From Box2D



Result : 

![GJKResult](https://github.com/lch32111/GJKPracticeBasedBox2D/blob/master/GJKResult.PNG)



## Summary for myself

I will summarize the hard one for me to understand, or the hard one for me to memorize.

This summary is based on Erin Catto GDC Presentation. This explanation is only for me.



## Analyzing the GetSearchDirection() code

```c++
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
		if (sgn > ChReal(0.0))
		{
			// Origin is left of edgeAB.
			return Cross(ChReal(1.0), edgeAB);
		}
		else
		{
			// Origin is right of edgeAB
			return Cross(edgeAB, ChReal(1.0));
		}
	}
	default:
		assert(false);
		return ChVector2(0.0, 0.0);
	}
}
```

GetSearchDirection() means getting the direction to the query point **Q** from the Simplex.  But, You should note that the code above is written based on the GJK algorithm, which uses Minkowski difference. So, The origin (0,0 on the 2D) of the coordinate becomes the Query point **Q**. So, the vector from support point A to the origin is `-m_vertexA.point;`.

Case 2 on the code means 1-simplex, the line-segment. So, you also have to find the search direction from the simplex to the query point **Q**.  I will analyze each line on the Case 2. before progressing, I will study GJK algorithm and the properties of Minkowski Difference again.

As you know, the GJK algorithm uses Minkowski Difference, which subtracts between all of vertices from two polygons. the result of the operation is convex polygon again with the assumption that the two polygons are already convex. And If the two polygons overlap, the result polygon include the origin (0,0). otherwise it doesn't include the origin. Thanks to this theory, we can convert polygon-polygon collision to point-polygon collision. So,the query point is the origin. According to the presentation, There are some properties resulted from the Minkowski difference:

* Property 1 : distances are equal

  The distance Between Polygon **A** and **B** is equal to the distance between the origin and the super polygon **Z** (resulted from the operation).

* Property 2 : support points

  We can construct a support point of **Z** by doing `support(Z, d) = support(B, d) - support(A, d)`.

Now it's time to explain each line of the code.

```c++
ChVector2 edgeAB = m_vertexB.point - m_vertexA.point;
```

You have to know what the m_vertexB/A.point is. As you know in the code,

```c++
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
```

`SimplexVertex.point` is the difference between the support points from two polygons. it means the support point of the super polygon. So `edgeAB` is the 1-simplex (edge) from super polygon, evaluated by Minkowski Difference operation. Here is the next line now.

```c++
ChReal sgn = Cross(edgeAB, -m_vertexA.point);
```

This is quite tricky, but you must try to understand this. The explanation from the presentation is the pages from 114 to 118. I will understand it and write in my way again.

We use barycentric coordinate to calculate the closest point. But As Erin says, Barycentric coordinates suffer from some round error. this precision problem can lead to incorrect support points, then result in excessive iterations or incorrect results.

Anyway, In the situation of finding the search direction from line segment to query point **Q**, the line segment must be perpendicular to the search direction. It means 

```c++
line_segment = edgeAB = m_vertexB.point - m_vertexA.point;

ChReal s = static_cast<ChReal>(1.0) / m_divisor;
ChReal P = (s * m_vertexA.u) * m_vertexA.point + (s * m_vertexB.u) * m_vertexB.point;
SearchDirection = Q - P;
Dot(line_segment, SearchDirection) == 0   
```

However, the result of the last line may not be true due to the round error problem. So, Erin says, we just rotate the line_segment by rotating perpendicularly.

```c++
inline ChVector2 Cross(ChReal s, const ChVector2& a)
{
    return ChVector2(-s * a.y, s * a.x);
}

inline ChVector2 Cross(const ChVector2& a, ChReal s)
{
    return ChVector2(s * a.y, -s * a.x);
}

// Origin is left of edgeAB
Cross(ChReal(1.0), edgeAB);

// Origin is right of edgeAB
Cross(edgeAB, ChReal(1.0));
```

You can just rotate the line segment perpendicularly by doing the above code. the reason why the cross works like above is that the rotation matrix of 2D with theta pi/2 and -pi/2 works like above. You can check it by yourself. Anyway the result of cross function above is rotating left/right perpendicularly with scaling s.

By doing the rotation, you can get the better precision with the search direction. However, we have to decide the way of rotation. the code is

```c++
ChVector2 edgeAB = m_vertexB.point - m_vertexA.point;
ChReal sgn = Cross(edgeAB, -m_vertexA.point);
if (sgn > ChReal(0.0))
{
    // Turn Left 1-Simplex, the line-segment AB
    // Origin is left of edgeAB.
    return Cross(ChReal(1.0), edgeAB);
}
else
{
    // Turn Right 1-simplex, the line-segment AB
    // Origin is right of edgeAB
    return Cross(edgeAB, ChReal(1.0));
}
```

To understand the code above, We need some of linear algebra. However, I can't explain all of things here. I replace it with the link : https://www.cs.cmu.edu/~quake/robust.html

Anyway, I will use the mathematics why the code is like this.

![Orientation2D](https://github.com/lch32111/GJKPracticeBasedBox2D/blob/master/Orientation2D.PNG)

![Case2](https://github.com/lch32111/GJKPracticeBasedBox2D/blob/master/Case2.PNG)



Now, I hope someone who read this will get to know why the equations and the code works like above.

So the first one is clear.



## Closest point on line segment

Refer to the Erin's GDC presentation pdf to understand what I write here. I just rewrite what's on the PDF for just studying it again.



Assume that we have a line segment, which consists of Point **A**, and **B**. and There is an Query Point **Q**. Our goal is to calculate the closest point **P** on the line segment with Query point **Q**.



By projecting **Q** onto the line segment **AB**, we can have 3 Voronoi regions : 

* region **A** : **Q** == **A**
* region **AB** : **Q** == interior of **AB**
* region **B** : **Q** == **B**



We use barycentric coordinates for computing the projection of Q onto the line passing through segment AB. In line segment, Any point **G** on line passing through **AB** can be represetned as a `weighted sum of A and B`. the weights are u, v, and must sum up to one. We can view the barycentric coordinates as the fractional lengths of partial segments.

```
G(u, v) = uA + vB;
u + v = 1;
n = Normalize(B - A);

// (u, v) from G
v = dot(G - A, n) / Length(B - A);
u = dot(B - G, n) / Length(B - A);

// (u, v) from Q
v = dot(Q - A, n) / Length(B - A);
u = dot(B - Q, n) / Length(B - A);

// Voronoi region from (u, v)
if(u > 0 && v >0) return regionAB;
if(v <= 0) return regionA;
if(u <= 0) return regionB;
```



It's time to understand the actual code.

```c++
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

Chan::ChVector2 Chan::Simplex::GetClosestPoint() const
{
	switch (m_count)
	{
	...
	case 2:
	{
		ChReal s = static_cast<ChReal>(1.0) / m_divisor;
		return (s * m_vertexA.u) * m_vertexA.point + (s * m_vertexB.u) * m_vertexB.point;
	}
    ...
	}
}
```

First, We have to notice that the equation of calculating the weights (u, v) is different from the explanation. However, we can figure out why it is like. Remember the equation of calculating (u, v) again :

```c++
// (u, v) from G (Query Point)
v = dot(G - A, n) / Length(B - A);
u = dot(B - G, n) / Length(B - A);
```

As you already know, the solve function is based on GJK algorithm. so the Query point is always the origin (0, 0). and the code of getting the weights is :

```c++
ChVector2 A = m_vertexA.point;
ChVector2 B = m_vertexB.point;

// Compute barycentric coordinates (pre-division).
ChReal u_0 = dot(B - Q, B - A)
ChReal u_1 = -dot(Q - B, B - A);
ChReal u_2 = dot(Q - B, A - B);
ChReal u = dot(Q - B, A - B);
ChReal v = dot(Q - A, B - A);
```

the process from u_o to u_2 shows why the code is like. But there is still a difference that the code doesn't have the code to divide the weights by the length of (B - A). Howevver, If you see other codes to get the closest point : 

```c++
m_vertexA.u = u;
m_vertexB.u = v;
ChVector2 e = B - A;
m_divisor = dot(e, e);
m_count = 2;
...
...
ChReal s = static_cast<ChReal>(1.0) / m_divisor;
		return (s * m_vertexA.u) * m_vertexA.point + (s * m_vertexB.u) * m_vertexB.point;

(s * m_vertexA.u) == InverseSquaredLength(A - B) * dot(Q - B, A - B)
--> dot(Q - B, A - B) / SquaredLength(A - B) == dot(Q - B, normalize(A - B)) / Length(A - B)
```

I'm sure that the code above does not explain well why it's same as the original explanation. But, I want to try to explain one more.

```c++
// (u, v) from G (Query Point)
n = normalize(B - A);
v = dot(G - A, n) / Length(B - A);
u = dot(B - G, n) / Length(B - A);

u2 = dot(G - B, A - B) / SquaredLength(A - B);

u == u2;

float ABLength = Length(A - B);
float ABSqLength = SquaredLength(A - B);
float uNumerator = dot(G - A, n) * ABLength == dot(G - A, A - B);
float uDenomiantor = Length(B - A) * ABLength == SquaredLength(A - B);

if you know the property of fractional numbers, you will know that you can make the same number with multiplying or dividing something onto the denominator and the numerator.
```

I think the thing is clear now. 



## Closest point on triangle

Also refer to the Erin's GDC presentation pdf to understand what I write here. I just rewrite what's on the PDF for just studying it again. You must see the figures of this part on the pdf. Because the directions of triangle, line-segments are important to understand why the equation and the code are like that.



Assume we have a triangle, which consists of 3 vertices, **ABC**. the triangle is counter-clockwise. The closest features of a triangle are :

* Vertex **A**, **B**, **C**
* Edge **AB**, **BC**, **CA**
* Interior of the triangle

So, the voronoi regions of a trianle are :

* Region **A**, **B**, **C**
* Region **AB**, **BC**, **CA**
* Region **ABC**

We gonna handle with the vertex regions. Even though dealing with the vertex regions, we need edges weights. And we should notice the directions of each line segment.  :

```c++
EdgeAB -> u_AB, v_AB -> vA + uB
EdgeBC -> u_BC, v_BC -> uB + vC
EdgeCA -> u_CA, v_CA -> uC + vA

Region A : v_AB <= 0 && u_CA <= 0
Region B : u_AB <= 0 && v_BC <= 0
Region C : u_BC <= 0 && v_CA <= 0
```

You should align with the figure on the part of closest point on line segment with this triangle part. Because the directions of each edge are different, You should know which weights Each vertex use. If you recognize this one, the triangle part is the easy one.

After realizing the Vertex Region, we should enter the edge and interior region of a triangle. To do this, we should know The **Triangle Barycentric Coordinates**. Because I just want to intensify what's in my mind about the coordinate, I will use the geometric explanation of Triangle Barycentric Coorindate

Imagine that there is a query point **Q** in the interior of a triangle **ABC**. You can inscribe three triangles by connecting each vertex of the triangle to the query point **Q**. After doing that, you can get next equations :

```c++
Area(ABQ) + Area(BCQ) + Area(CAQ) = Area(ABC)
Area(ABQ) / Area(ABC) + Area(BCQ) / Area(ABC) + ARea(CAQ)/ Area(ABC) = 1

// Change the order for making equation uA + vB + wC =1.
Area(BCQ) / Area(ABC) + ARea(CAQ)/ Area(ABC) + Area(ABQ) / Area(ABC) = 1

// You must see the figure on page 45
Area(BCQ) ~ u -> related to vertex A
Area(CAQ) ~ v -> related to vertex B
Area(ABQ) ~ w -> related to vertex C

u = Area(QBC) / Area(ABC)
v = Area(QCA) / Area(ABC)
w = Area(QAB) / Area(ABC)
u + v + w = 1

what if u = 1, v = 0, w = 0? The query point will be vertex A. Therefore,
u = 1, v = 0, w = 0 -> vertex A
u = 0, v = 1, w = 0 -> vertex B
u = 0, v = 0, w = 1 -> vertex C

It implies that
uA + vB + wC = Q

and It implies on the geometry field that
Line Sement : Fractional Length
Triangles : Fractional Area
Tetrahedrons : Fractional Volume

Singed Area of Triangle ABC (Counter-ClockWise!!, Winding is important for the sign of Area)
signedArea = 1/2 * Cross(B - A, C - B).
    
what if u or v or w is negative?
it means the query point Q is behind edgeBC if u < 0
it means the query point Q is behind edgeCA if v < 0
it means the query point Q is behind edgeAB if w < 0
```

Now that we know the triangle barycentric coordinate, we can recognize the edge and interior region of the triangle. 

```c++
Region AB : u_AB > 0 && v_AB > 0 && wABC <= 0
Region BC : u_BC > 0 && v_BC > 0 && uABC <= 0
Region CA : u_CA > 0 && v_CA > 0 && vABC <= 0
    
Region ABC : uABC > 0 && vABC > 0 && wABC > 0
```

We know how to calculate the barycentric coordinate of edges and the triangle and how to evaluate the voronoi region from Query point Q. So, We have to determine the closest  point **P** from the evaluated barycentric coordinates and the evaluated voronoi region.

It's time to analyze the actual code. the code is a little different from the Erin's Sample code. When i just use the sample code, i got the assert which uABC, vABC, wABC is not more than 0 when the query point is inside a triangle. Anyway, The code below works well now. So, I will explain it based on my code. You can check the comment in the code below.

```c++
// Closest point on triangle to Q.
// Voronoi regions : A, B, C, AB, BC, CA, ABC
void Chan::Simplex::Solve3(const ChVector2 & Q)
{
    // Triangle in Counter-ClockWise 
    // You should ensure the direction of triangle vertices
	ChVector2 A = m_vertexA.point;
	ChVector2 B = m_vertexB.point;
	ChVector2 C = m_vertexC.point;

	// Compute edge barycentric coordinates (pre-division).
    // You should take care of the direction of vectors
    // As you already know with the previous explanation on closest point on line-segment,
    // The A is on the left side, B on the right side.
    // The line-segment is A->B, which is evaluated with (B - A)
    // And the Barycentric coordinates are evaluted with (Q - A), (B - Q).
    // the coordinate u is on the B side, v on the A side.
    // However, In this case, the triangle is in counter-clockwise.
    // That means you have to reverse all of the directions.
    
    // Remeber the explanation above again
    /*
    EdgeAB -> u_AB, v_AB -> vA + uB
	EdgeBC -> u_BC, v_BC -> uB + vC
	EdgeCA -> u_CA, v_CA -> uC + vA

	Region A : v_AB <= 0 && u_CA <= 0
	Region B : u_AB <= 0 && v_BC <= 0
	Region C : u_BC <= 0 && v_CA <= 0
    */
    // So That's the reason why the equation below is like that.
    // You have to use Point B for caclulating the barycentric coordinate u.
    // If you understand this, the three 'if' conditions are clear.
    // Let's jump to the Edge Case
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
    // I already explained how to calculate the area of the triangle
    // by doing the cross product.
    // But there is no divide with 0.5 to calculate the exact area.
    // The reason is that You don't need the exact area 
    // for getting voronoi resion and bary centric coordinates
    // Because You can tell what region the query point belongs to
    // by making use of the sign of barycentric coordinates.
    // And Even though you need to calculate the closest point,
    // You can get the point due to the ratio. 
	ChReal area = Cross(B - A, C - A);

	// Compute triangle barycentric coordinates (pre-division).
    // I already explained the way to evaluate the barycentric coordinates
    // of a triangle. But the equation below is different what I described.
    // Actually, You should divide the inscribed area 
    // with the original triangle area.
    // But you don't meed to. As I already mentioned, 
    // You can tell what region the query point belongs to by distinguishing
    // the sing of the barycentric coordinates.
    // Remember this equation for gettting voronoi region
    /*
    Region AB : u_AB > 0 && v_AB > 0 && wABC <= 0
    Region BC : u_BC > 0 && v_BC > 0 && uABC <= 0
    Region CA : u_CA > 0 && v_CA > 0 && vABC <= 0

    Region ABC : uABC > 0 && vABC > 0 && wABC > 0
    */
    // I hope you will understand this.
    // Let's jump to the interior case.
	ChReal uABC = area * Cross(B - Q, C - Q);
	ChReal vABC = area * Cross(C - Q, A - Q);
	ChReal wABC = area * Cross(A - Q, B - Q);

	// Region AB
	if (uAB > ChReal(0.0) && vAB > ChReal(0.0) && wABC <= ChReal(0.0))
	{
		m_vertexA.u = uAB;
		m_vertexB.u = vAB;
		ChVector2 e = B - A;
		m_divisor = dot(e, e);
		m_count = 2;
		return;
	}

	// Region BC
	if (uBC > ChReal(0.0) && vBC > ChReal(0.0) && uABC <= ChReal(0.0))
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
	if (uCA > ChReal(0.0) && vCA > ChReal(0.0) && vABC <= ChReal(0.0))
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
    // If all the other cases pass, the barycentric coordinates of the triangle
    // must be more than zero.
	assert(uABC > ChReal(0.0) && vABC > ChReal(0.0) && wABC > ChReal(0.0));

    // And You should know why i put the divisor 
    // as the sum of all of the barycentric coordinates.
    // I will explain it below.
	m_vertexA.u = uABC;
	m_vertexB.u = vABC;
	m_vertexC.u = wABC;
	m_divisor = uABC + vABC + wABC;
	m_count = 3;
}
```

To evaluate the  closest point on the triangle, You should know this equation again.

`Q = uA + vB + wC`

And the actual code is :

```c++
void Chan::Simplex::GetWitnessPoints(ChVector2 * point1, ChVector2 * point2) const
{
	switch (m_count)
	{
	case 1:
	...
	case 2:
	...
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
```

According to the code, if we see the 'u' part, the actual equation is

```
s == (1.0) / (uABC + vABC + wABC)
s * m_vertexA.u == (uABC) / (uABC + vABC + wABC)

So the equation of evaluating the closest point on triangle is
P = uA + vB + wC;
the u part is now (uABC) / (uABC + vABC + wABC)

uABC + vABC + wABC == (TriangleArea * 2) * 
					  ((Area(QBC) + Area(QCA)+ Area(QAB)) * 2)
				   == (TriangleArea * 2) * (TriangleArea * 2)
				   == TriangleArea^2 * 4
uABC == (TriangleArea * 2) * Area(QBC) * 2
     == TriangleArea * Area(QBC) * 4
     
Therefore,
s * m_vertexA.u == (TriangleArea * Area(QBC) * 4) /
					(TriangleArea^2 * 4)
				== Area(QBC) / TriangleArea
```

I hope it's clear to evaluate the closest point on triangle now.



I clear the hardest one!



## Termination conditions

According to the Erin, There are three conditions to break the GJK algorithm.

1. The case of having 2-simplex (The interior point of the triangle)
2. Point Overlap -> Cause zero length of Search Direction Vector
3. Duplicate support points.

In the code

```c++
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
```

I will summarize this according to the pdf.

* Duplicate Support Points
  * Case 1 : Repeated Support Point
  * Case 3b : Edge Overlap
  * Case 4 : Interior edge
* 2-simplex Case
  * Case 2 : Containment in super polygon
* Zero Length Search Vector
  * Case 3a : vertex overlap

To understand this well, You should see the figure on the pdf.



## Analyzing Box2D code on Distance2D And then optimizing sample code

### 1) GJK Optimization + Enforcement + Caching

I just want to analyze the caching first. The code is

```c++
struct b2Simplex
{
	void ReadCache(	const b2SimplexCache* cache,
					const b2DistanceProxy* proxyA, const b2Transform& transformA,
					const b2DistanceProxy* proxyB, const b2Transform& transformB)
	{
		b2Assert(cache->count <= 3);
		
		// Copy data from cache.
		m_count = cache->count;
		b2SimplexVertex* vertices = &m_v1;
		for (int32 i = 0; i < m_count; ++i)
		{
			b2SimplexVertex* v = vertices + i;
			v->indexA = cache->indexA[i];
			v->indexB = cache->indexB[i];
			b2Vec2 wALocal = proxyA->GetVertex(v->indexA);
			b2Vec2 wBLocal = proxyB->GetVertex(v->indexB);
			v->wA = b2Mul(transformA, wALocal);
			v->wB = b2Mul(transformB, wBLocal);
			v->w = v->wB - v->wA;
			v->a = 0.0f;
		}

		// Compute the new simplex metric, if it is substantially different than
		// old metric then flush the simplex.
		if (m_count > 1)
		{
			float32 metric1 = cache->metric;
			float32 metric2 = GetMetric();
			if (metric2 < 0.5f * metric1 || 2.0f * metric1 < metric2 || metric2 < b2_epsilon)
			{
				// Reset the simplex.
				m_count = 0;
			}
		}

		// If the cache is empty or invalid ...
		if (m_count == 0)
		{
			b2SimplexVertex* v = vertices + 0;
			v->indexA = 0;
			v->indexB = 0;
			b2Vec2 wALocal = proxyA->GetVertex(0);
			b2Vec2 wBLocal = proxyB->GetVertex(0);
			v->wA = b2Mul(transformA, wALocal);
			v->wB = b2Mul(transformB, wBLocal);
			v->w = v->wB - v->wA;
			v->a = 1.0f;
			m_count = 1;
		}
	}

	void WriteCache(b2SimplexCache* cache) const
	{
		cache->metric = GetMetric();
		cache->count = uint16(m_count);
		const b2SimplexVertex* vertices = &m_v1;
		for (int32 i = 0; i < m_count; ++i)
		{
			cache->indexA[i] = uint8(vertices[i].indexA);
			cache->indexB[i] = uint8(vertices[i].indexB);
		}
	}
    
    float32 GetMetric() const
	{
		switch (m_count)
		{
		case 0:
			b2Assert(false);
			return 0.0f;

		case 1:
			return 0.0f;

		case 2:
			return b2Distance(m_v1.w, m_v2.w);

		case 3:
			return b2Cross(m_v2.w - m_v1.w, m_v3.w - m_v1.w);

		default:
			b2Assert(false);
			return 0.0f;
		}
	}

void b2Distance(b2DistanceOutput* output,
				b2SimplexCache* cache,
				const b2DistanceInput* input)
{
	++b2_gjkCalls;

	const b2DistanceProxy* proxyA = &input->proxyA;
	const b2DistanceProxy* proxyB = &input->proxyB;

	b2Transform transformA = input->transformA;
	b2Transform transformB = input->transformB;

	// Initialize the simplex.
	b2Simplex simplex;
	simplex.ReadCache(cache, proxyA, transformA, proxyB, transformB);

	// Get simplex vertices as an array.
	b2SimplexVertex* vertices = &simplex.m_v1;
	const int32 k_maxIters = 20;

	// These store the vertices of the last simplex so that we
	// can check for duplicates and prevent cycling.
	int32 saveA[3], saveB[3];
	int32 saveCount = 0;

	// Main iteration loop.
	int32 iter = 0;
	while (iter < k_maxIters)
	{
		// Copy simplex so we can identify duplicates.
		saveCount = simplex.m_count;
		for (int32 i = 0; i < saveCount; ++i)
		{
			saveA[i] = vertices[i].indexA;
			saveB[i] = vertices[i].indexB;
		}

		switch (simplex.m_count)
		{
		case 1:
			break;

		case 2:
			simplex.Solve2();
			break;

		case 3:
			simplex.Solve3();
			break;

		default:
			b2Assert(false);
		}

		// If we have 3 points, then the origin is in the corresponding triangle.
		if (simplex.m_count == 3)
		{
			break;
		}

		// Get search direction.
		b2Vec2 d = simplex.GetSearchDirection();

		// Ensure the search direction is numerically fit.
		if (d.LengthSquared() < b2_epsilon * b2_epsilon)
		{
			// The origin is probably contained by a line segment
			// or triangle. Thus the shapes are overlapped.

			// We can't return zero here even though there may be overlap.
			// In case the simplex is a point, segment, or triangle it is difficult
			// to determine if the origin is contained in the CSO or very close to it.
			break;
		}

		// Compute a tentative new simplex vertex using support points.
		b2SimplexVertex* vertex = vertices + simplex.m_count;
		vertex->indexA = proxyA->GetSupport(b2MulT(transformA.q, -d));
		vertex->wA = b2Mul(transformA, proxyA->GetVertex(vertex->indexA));
		b2Vec2 wBLocal;
		vertex->indexB = proxyB->GetSupport(b2MulT(transformB.q, d));
		vertex->wB = b2Mul(transformB, proxyB->GetVertex(vertex->indexB));
		vertex->w = vertex->wB - vertex->wA;

		// Iteration count is equated to the number of support point calls.
		++iter;
		++b2_gjkIters;

		// Check for duplicate support points. This is the main termination criteria.
		bool duplicate = false;
		for (int32 i = 0; i < saveCount; ++i)
		{
			if (vertex->indexA == saveA[i] && vertex->indexB == saveB[i])
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

	b2_gjkMaxIters = b2Max(b2_gjkMaxIters, iter);

	// Prepare output.
	simplex.GetWitnessPoints(&output->pointA, &output->pointB);
	output->distance = b2Distance(output->pointA, output->pointB);
	output->iterations = iter;

	// Cache the simplex.
	simplex.WriteCache(cache);

	// Apply radii if requested.
	if (input->useRadii)
	{
		float32 rA = proxyA->m_radius;
		float32 rB = proxyB->m_radius;

		if (output->distance > rA + rB && output->distance > b2_epsilon)
		{
			// Shapes are still no overlapped.
			// Move the witness points to the outer surface.
			output->distance -= rA + rB;
			b2Vec2 normal = output->pointB - output->pointA;
			normal.Normalize();
			output->pointA += rA * normal;
			output->pointB -= rB * normal;
		}
		else
		{
			// Shapes are overlapped when radii are considered.
			// Move the witness points to the middle.
			b2Vec2 p = 0.5f * (output->pointA + output->pointB);
			output->pointA = p;
			output->pointB = p;
			output->distance = 0.0f;
		}
	}
}
```

I think Erin makes use of temporal coherence for efficient GJK algorithm. `ReadCache()` function just use **metrics** to check whether the previous cache data is valid or not. The metrics is the length of line segment in 1-simplex, and triangle area * 2 in 2-simplex. The metrics is zero in other cases.

```c++
// Compute the new simplex metric, if it is substantially different than
// old metric then flush the simplex.
if (m_count > 1)
{
    float32 metric1 = cache->metric;
    float32 metric2 = GetMetric();
    if (metric2 < 0.5f * metric1 || 2.0f * metric1 < metric2 || metric2 < b2_epsilon)
    {
        // Reset the simplex.
        m_count = 0;
    }
}
```

So the invalid condition is

* current metric is smaller than half of the cached mteric
* current metric is bigger than double of the cached metric.
* current metric is close to the zero.









### 2) GJK Raycast










## TODO
5. Analyzing Box2D code on Distance2D And then optimize sample code
	- GJK Optimization + enforcement + Caching
	- GJK Raycast
6. Study and Apply on 3D
7. Boom! Done.
