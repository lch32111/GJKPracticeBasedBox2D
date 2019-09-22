# GJKPracticeBasedBox2D
This is a repository for practicing GJK algorithm, based on GDC 2010 Presentation of Erin Catto From Box2D



Result : 

![GJKResult](E:\###Visual Studio Project Folder###\Visual Studio 2017\Projects\GJKPrac\GJKPracticeBasedBox2D\GJKResult.PNG)



## Summary for myself

I will summarize the hard one for me to understand, or the hard one for me to memorize.

This summary is based on Erin Catto GDC Presentation. This explanation is only for me.



### Analyzing the GetSearchDirection() code

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
SearchDirection = -P;
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
$$
edgeAB = (b_x - a_x, b_y - a_y) \\
edgeAO = (o_x - a_x, o_y - a_y)\\
R =Cross(edgeAB, edgeAO) = (b_x -a_x)(o_y - a_y) - (b_y -a_y)(o_x-a_x) \\

\therefore \triangle  BOA \textrm{ is counter-clockwise if}\ R \ > \ 0\\
\textrm{otherwise, it's clockwise}
$$
![Case2](E:\###Visual Studio Project Folder###\Visual Studio 2017\Projects\GJKPrac\GJKPracticeBasedBox2D\Case2.PNG)



Now, I hope someone who read this will get to know why the equations and the code works like above.

So the first one is clear.



