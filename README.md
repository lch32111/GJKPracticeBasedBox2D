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








## TODO
2. Explain Closest point on triangle from solvin the problem to getting a witness point
	- Closest point on triangle
	- Getting a witness point
	- Fix Sample code
3. Explain the structure of the loop on Distance2D() function
	- structure
	- terminate condition
4. Code Optimization (reduce unnecessary work)
5. Analyzing Box2D code on Distance2D
	- GJK Optimization + enforcement + Caching
	- GJK Raycast
6. Study and Apply on 3D
7. Boom! Done.
