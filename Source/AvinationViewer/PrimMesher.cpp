// Fill out your copyright notice in the Description page of Project Settings.

#include "AvinationViewer.h"
#include "PrimMesher.h"

Face::Face()
{
    primFace = v1 = v2 = v3 = n1 = n2 = n3 = uv1 = uv2 = uv3 = 0;
}

Face::Face(int v1in, int v2in, int v3in)
{
    primFace = 0;
    
    v1 = v1in;
    v2 = v2in;
    v3 = v3in;
    
    n1 = 0;
    n2 = 0;
    n3 = 0;
    
    uv1 = 0;
    uv2 = 0;
    uv3 = 0;
    
}

Face::Face(int v1in, int v2in, int v3in, int n1in, int n2in, int n3in)
{
    primFace = 0;
    
    v1 = v1in;
    v2 = v2in;
    v3 = v3in;
    
    n1 = n1in;
    n2 = n2in;
    n3 = n3in;
    
    uv1 = 0;
    uv2 = 0;
    uv3 = 0;
}

Coord Face::SurfaceNormal(TArray<Coord> coordList)
{
    Coord c1 = coordList[v1];
    Coord c2 = coordList[v2];
    Coord c3 = coordList[v3];
    
    Coord edge1(c2.X - c1.X, c2.Y - c1.Y, c2.Z - c1.Z);
    Coord edge2(c3.X - c1.X, c3.Y - c1.Y, c3.Z - c1.Z);
    
    return Coord::Cross(edge1, edge2).Normalize();
}

ViewerFace::ViewerFace(int primFaceNumberIn)
{
    primFaceNumber = primFaceNumberIn;
    
    coordIndex1 = coordIndex2 = coordIndex3 = -1; // -1 means not assigned yet
}

void ViewerFace::Scale(float x, float y, float z)
{
    v1.X *= x;
    v1.Y *= y;
    v1.Z *= z;
    
    v2.X *= x;
    v2.Y *= y;
    v2.Z *= z;
    
    v3.X *= x;
    v3.Y *= y;
    v3.Z *= z;
}
void ViewerFace::AddPos(float x, float y, float z)
{
    v1.X += x;
    v2.X += x;
    v3.X += x;
    
    v1.Y += y;
    v2.Y += y;
    v3.Y += y;
    
    v1.Z += z;
    v2.Z += z;
    v3.Z += z;
}

void ViewerFace::AddRot(PrimQuat q)
{
    v1 *= q;
    v2 *= q;
    v3 *= q;
    
    n1 *= q;
    n2 *= q;
    n3 *= q;
}

void ViewerFace::CalcSurfaceNormal()
{
    
    Coord edge1(v2.X - v1.X, v2.Y - v1.Y, v2.Z - v1.Z);
    Coord edge2(v3.X - v1.X, v3.Y - v1.Y, v3.Z - v1.Z);
    
    n1 = n2 = n3 = Coord::Cross(edge1, edge2).Normalize();
}

Angle::Angle(float angleIn, float xIn, float yIn)
{
    angle = angleIn;
    X = xIn;
    Y = yIn;
}

Angle::Angle()
{
    angle = X = Y = 0.0f;
}

const Angle AngleList::angles3[] =
{
    Angle(0.0f, 1.0f, 0.0f),
    Angle(0.33333333333333333f, -0.5f, 0.86602540378443871f),
    Angle(0.66666666666666667f, -0.5f, -0.86602540378443837f),
    Angle(1.0f, 1.0f, 0.0f)
};

const Coord AngleList::normals3[] =
{
    Coord(0.25f, 0.4330127019f, 0.0f).Normalize(),
    Coord(-0.5f, 0.0f, 0.0f).Normalize(),
    Coord(0.25f, -0.4330127019f, 0.0f).Normalize(),
    Coord(0.25f, 0.4330127019f, 0.0f).Normalize()
};
    
const Angle AngleList::angles4[] =
{
    Angle(0.0f, 1.0f, 0.0f),
    Angle(0.25f, 0.0f, 1.0f),
    Angle(0.5f, -1.0f, 0.0f),
    Angle(0.75f, 0.0f, -1.0f),
    Angle(1.0f, 1.0f, 0.0f)
};

const Coord AngleList::normals4[] =
{
    Coord(0.5f, 0.5f, 0.0f).Normalize(),
    Coord(-0.5f, 0.5f, 0.0f).Normalize(),
    Coord(-0.5f, -0.5f, 0.0f).Normalize(),
    Coord(0.5f, -0.5f, 0.0f).Normalize(),
    Coord(0.5f, 0.5f, 0.0f).Normalize()
};
    
const Angle AngleList::angles24[] =
{
    Angle(0.0f, 1.0f, 0.0f),
    Angle(0.041666666666666664f, 0.96592582628906831f, 0.25881904510252074f),
    Angle(0.083333333333333329f, 0.86602540378443871f, 0.5f),
    Angle(0.125f, 0.70710678118654757f, 0.70710678118654746f),
    Angle(0.16666666666666667f, 0.5f, 0.8660254037844386f),
    Angle(0.20833333333333331f, 0.25881904510252096f, 0.9659258262890682f),
    Angle(0.25f, 0.0f, 1.0f),
    Angle(0.29166666666666663f, -0.25881904510252063f, 0.96592582628906831f),
    Angle(0.33333333333333333f, -0.5f, 0.86602540378443871f),
    Angle(0.375f, -0.70710678118654746f, 0.70710678118654757f),
    Angle(0.41666666666666663f, -0.86602540378443849f, 0.5f),
    Angle(0.45833333333333331f, -0.9659258262890682f, 0.25881904510252102f),
    Angle(0.5f, -1.0f, 0.0f),
    Angle(0.54166666666666663f, -0.96592582628906842f, -0.25881904510252035f),
    Angle(0.58333333333333326f, -0.86602540378443882f, -0.5f),
    Angle(0.62499999999999989f, -0.70710678118654791f, -0.70710678118654713f),
    Angle(0.66666666666666667f, -0.5f, -0.86602540378443837f),
    Angle(0.70833333333333326f, -0.25881904510252152f, -0.96592582628906809f),
    Angle(0.75f, 0.0f, -1.0f),
    Angle(0.79166666666666663f, 0.2588190451025203f, -0.96592582628906842f),
    Angle(0.83333333333333326f, 0.5f, -0.86602540378443904f),
    Angle(0.875f, 0.70710678118654735f, -0.70710678118654768f),
    Angle(0.91666666666666663f, 0.86602540378443837f, -0.5f),
    Angle(0.95833333333333326f, 0.96592582628906809f, -0.25881904510252157f),
    Angle(1.0f, 1.0f, 0.0f)
};

Angle AngleList::interpolatePoints(float newPoint, Angle p1, Angle p2)
{
    float m = (newPoint - p1.angle) / (p2.angle - p1.angle);
    return Angle(newPoint, p1.X + m * (p2.X - p1.X), p1.Y + m * (p2.Y - p1.Y));
}
    
void AngleList::intersection(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{ // ref: http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/
    double denom = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);
    double uaNumerator = (x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3);
    
    if (denom != 0.0)
    {
        double ua = uaNumerator / denom;
        iX = (float)(x1 + ua * (x2 - x1));
        iY = (float)(y1 + ua * (y2 - y1));
    }
}

void AngleList::makeAngles(int sides, float startAngle, float stopAngle)
{
    angles.Empty();
    normals.Empty();
    
    double twoPi = M_PI * 2.0;
    float twoPiInv = 1.0f / (float)twoPi;
    
    if (sides < 1)
        throw std::exception(); //("number of sides not greater than zero");
    if (stopAngle <= startAngle)
        throw std::exception(); //("stopAngle not greater than startAngle");
    
    if ((sides == 3 || sides == 4 || sides == 24))
    {
        startAngle *= twoPiInv;
        stopAngle *= twoPiInv;
        
        const Angle *sourceAngles;
        int endAngleIndex = 0;
        if (sides == 3)
        {
            sourceAngles = angles3;
            endAngleIndex = 3;
        }
        else if (sides == 4)
        {
            sourceAngles = angles4;
            endAngleIndex = 4;
        }
        else
        {
            sourceAngles = angles24;
            endAngleIndex = 24;
        }
        
        int startAngleIndex = (int)(startAngle * sides);
        if (stopAngle < 1.0f)
            endAngleIndex = (int)(stopAngle * sides) + 1;
        if (endAngleIndex == startAngleIndex)
            endAngleIndex++;
        
        for (int angleIndex = startAngleIndex; angleIndex < endAngleIndex + 1; angleIndex++)
        {
            angles.Add(sourceAngles[angleIndex]);
            if (sides == 3)
                normals.Add(normals3[angleIndex]);
            else if (sides == 4)
                normals.Add(normals4[angleIndex]);
        }
        
        if (startAngle > 0.0f)
            angles[0] = interpolatePoints(startAngle, angles[0], angles[1]);
        if (stopAngle < 1.0f)
        {
            int lastAngleIndex = angles.Num() - 1;
            angles[lastAngleIndex] = interpolatePoints(stopAngle, angles[lastAngleIndex - 1], angles[lastAngleIndex]);
        }
    }
    else
    {
        double stepSize = twoPi / sides;
        
        int startStep = (int)(startAngle / stepSize);
        double angle = stepSize * startStep;
        int step = startStep;
        double stopAngleTest = stopAngle;
        if (stopAngle < twoPi)
        {
            stopAngleTest = stepSize * ((int)(stopAngle / stepSize) + 1);
            if (stopAngleTest < stopAngle)
                stopAngleTest += stepSize;
            if (stopAngleTest > twoPi)
                stopAngleTest = twoPi;
        }
        
        while (angle <= stopAngleTest)
        {
            Angle newAngle;
            newAngle.angle = (float)angle;
            newAngle.X = (float)cos(angle);
            newAngle.Y = (float)sin(angle);
            angles.Add(newAngle);
            step += 1;
            angle = stepSize * step;
        }
        
        if (startAngle > angles[0].angle)
        {
            Angle newAngle;
            intersection(angles[0].X, angles[0].Y, angles[1].X, angles[1].Y, 0.0f, 0.0f, (float)cos(startAngle), (float)sin(startAngle));
            newAngle.angle = startAngle;
            newAngle.X = iX;
            newAngle.Y = iY;
            angles[0] = newAngle;
        }
        
        int index = angles.Num() - 1;
        if (stopAngle < angles[index].angle)
        {
            Angle newAngle;
            intersection(angles[index - 1].X, angles[index - 1].Y, angles[index].X, angles[index].Y, 0.0f, 0.0f, (float)cos(stopAngle), (float)sin(stopAngle));
            newAngle.angle = stopAngle;
            newAngle.X = iX;
            newAngle.Y = iY;
            angles[index] = newAngle;
        }
    }
}

Profile::Profile()
{
    faceNormal = Coord(0.0f, 0.0f, 1.0f);
    numOuterVerts = 0;
    numHollowVerts = 0;
    outerFaceNumber = -1;
    hollowFaceNumber = -1;
    calcVertexNormals = false;
    bottomFaceNumber = 0;
    numPrimFaces = 0;
    
    coords.Empty();
    faces.Empty();
    vertexNormals.Empty();
    us.Empty();
    faceUVs.Empty();
    faceNumbers.Empty();
}
    
Profile::Profile(int sides, float profileStart, float profileEnd, float hollow, int hollowSides, bool createFaces, bool calcVertexNormalsIn)
{
    faceNormal = Coord(0.0f, 0.0f, 1.0f);
    numOuterVerts = 0;
    numHollowVerts = 0;
    outerFaceNumber = -1;
    hollowFaceNumber = -1;
    calcVertexNormals = false;
    bottomFaceNumber = 0;
    numPrimFaces = 0;
    
    calcVertexNormals = calcVertexNormalsIn;
    coords.Empty();
    faces.Empty();
    vertexNormals.Empty();
    us.Empty();
    faceUVs.Empty();
    faceNumbers.Empty();
    
    Coord center(0.0f, 0.0f, 0.0f);
    
    TArray<Coord> hollowCoords;
    TArray<Coord> hollowNormals;
    TArray<float> hollowUs;
    
    if (calcVertexNormals)
    {
        outerCoordIndices.Empty();
        hollowCoordIndices.Empty();
        cut1CoordIndices.Empty();
        cut2CoordIndices.Empty();
    }
    
    bool hasHollow = (hollow > 0.0f);
    
    bool hasProfileCut = (profileStart > 0.0f || profileEnd < 1.0f);
    
    AngleList angles;
    AngleList hollowAngles;
    
    float xScale = 0.5f;
    float yScale = 0.5f;
    if (sides == 4)  // corners of a square are sqrt(2) from center
    {
        xScale = 0.707107f;
        yScale = 0.707107f;
    }
    
    float startAngle = profileStart * twoPi;
    float stopAngle = profileEnd * twoPi;
    
    try { angles.makeAngles(sides, startAngle, stopAngle); }
//    catch (std::exception& ex)
    catch (...)
    {
        
//        errorMessage = "makeAngles failed: Exception: " + ex.ToString()
//        + "\nsides: " + sides.ToString() + " startAngle: " + startAngle.ToString() + " stopAngle: " + stopAngle.ToString();
        
        return;
    }
    
    numOuterVerts = angles.angles.Num();
    
    // flag to create as few triangles as possible for 3 or 4 side profile
    bool simpleFace = (sides < 5 && !hasHollow && !hasProfileCut);
    
    if (hasHollow)
    {
        if (sides == hollowSides)
            hollowAngles = angles;
        else
        {
            try { hollowAngles.makeAngles(hollowSides, startAngle, stopAngle); }
//            catch (std::exception& ex)
            catch (...)
            {
//                errorMessage = "makeAngles failed: Exception: " + ex.ToString()
//                + "\nsides: " + sides.ToString() + " startAngle: " + startAngle.ToString() + " stopAngle: " + stopAngle.ToString();
                
                return;
            }
        }
        numHollowVerts = hollowAngles.angles.Num();
    }
    else if (!simpleFace)
    {
        coords.Add(center);
        if (calcVertexNormals)
            vertexNormals.Add(Coord(0.0f, 0.0f, 1.0f));
        us.Add(0.0f);
    }
    
    float z = 0.0f;
    
    Angle angle;
    Coord newVert;
    if (hasHollow && hollowSides != sides)
    {
        int numHollowAngles = hollowAngles.angles.Num();
        for (int i = 0; i < numHollowAngles; i++)
        {
            angle = hollowAngles.angles[i];
            newVert.X = hollow * xScale * angle.X;
            newVert.Y = hollow * yScale * angle.Y;
            newVert.Z = z;
            
            hollowCoords.Add(newVert);
            if (calcVertexNormals)
            {
                if (hollowSides < 5)
                    hollowNormals.Add(hollowAngles.normals[i].Invert());
                else
                    hollowNormals.Add(Coord(-angle.X, -angle.Y, 0.0f));
                
                if (hollowSides == 4)
                    hollowUs.Add(angle.angle * hollow * 0.707107f);
                else
                    hollowUs.Add(angle.angle * hollow);
            }
        }
    }
    
    int index = 0;
    int numAngles = angles.angles.Num();
    
    for (int i = 0; i < numAngles; i++)
    {
        angle = angles.angles[i];
        newVert.X = angle.X * xScale;
        newVert.Y = angle.Y * yScale;
        newVert.Z = z;
        coords.Add(newVert);
        if (calcVertexNormals)
        {
            outerCoordIndices.Add(coords.Num() - 1);
            
            if (sides < 5)
            {
                vertexNormals.Add(angles.normals[i]);
                float u = angle.angle;
                us.Add(u);
            }
            else
            {
                vertexNormals.Add(Coord(angle.X, angle.Y, 0.0f));
                us.Add(angle.angle);
            }
        }
        
        if (hasHollow)
        {
            if (hollowSides == sides)
            {
                newVert.X *= hollow;
                newVert.Y *= hollow;
                newVert.Z = z;
                hollowCoords.Add(newVert);
                if (calcVertexNormals)
                {
                    if (sides < 5)
                    {
                        hollowNormals.Add(angles.normals[i].Invert());
                    }
                    
                    else
                        hollowNormals.Add(Coord(-angle.X, -angle.Y, 0.0f));
                    
                    hollowUs.Add(angle.angle * hollow);
                }
            }
        }
        else if (!simpleFace && createFaces && angle.angle > 0.0001f)
        {
            Face newFace;
            newFace.v1 = 0;
            newFace.v2 = index;
            newFace.v3 = index + 1;
            
            faces.Add(newFace);
        }
        index += 1;
    }
    
    if (hasHollow)
    {
        TArray<Coord> r;
        for (int i = 0 ; i < hollowCoords.Num() ; ++i)
            r.Add(hollowCoords.Last(i));
        hollowCoords = r;
        if (calcVertexNormals)
        {
            r.Empty();
            for (int i = 0 ; i < hollowNormals.Num() ; ++i)
                r.Add(hollowNormals.Last(i));
            hollowNormals = r;
            
            TArray<float> u;
            for (int i = 0 ; i < hollowUs.Num() ; ++i)
                u.Add(hollowUs.Last(i));
            hollowUs = u;
        }
        
        if (createFaces)
        {
            int numTotalVerts = numOuterVerts + numHollowVerts;
            
            if (numOuterVerts == numHollowVerts)
            {
                Face newFace;
                for (int coordIndex = 0; coordIndex < numOuterVerts - 1; coordIndex++)
                {
                    newFace.v1 = coordIndex;
                    newFace.v2 = coordIndex + 1;
                    newFace.v3 = numTotalVerts - coordIndex - 1;
                    faces.Add(newFace);
                    
                    newFace.v1 = coordIndex + 1;
                    newFace.v2 = numTotalVerts - coordIndex - 2;
                    newFace.v3 = numTotalVerts - coordIndex - 1;
                    faces.Add(newFace);
                }
            }
            else
            {
                if (numOuterVerts < numHollowVerts)
                {
                    Face newFace;
                    int j = 0; // j is the index for outer vertices
                    int maxJ = numOuterVerts - 1;
                    for (int i = 0; i < numHollowVerts; i++) // i is the index for inner vertices
                    {
                        if (j < maxJ)
                            if (angles.angles[j + 1].angle - hollowAngles.angles[i].angle < hollowAngles.angles[i].angle - angles.angles[j].angle + 0.000001f)
                            {
                                newFace.v1 = numTotalVerts - i - 1;
                                newFace.v2 = j;
                                newFace.v3 = j + 1;
                                
                                faces.Add(newFace);
                                j += 1;
                            }
                        
                        newFace.v1 = j;
                        newFace.v2 = numTotalVerts - i - 2;
                        newFace.v3 = numTotalVerts - i - 1;
                        
                        faces.Add(newFace);
                    }
                }
                else // numHollowVerts < numOuterVerts
                {
                    Face newFace;
                    int j = 0; // j is the index for inner vertices
                    int maxJ = numHollowVerts - 1;
                    for (int i = 0; i < numOuterVerts; i++)
                    {
                        if (j < maxJ)
                            if (hollowAngles.angles[j + 1].angle - angles.angles[i].angle < angles.angles[i].angle - hollowAngles.angles[j].angle + 0.000001f)
                            {
                                newFace.v1 = i;
                                newFace.v2 = numTotalVerts - j - 2;
                                newFace.v3 = numTotalVerts - j - 1;
                                
                                faces.Add(newFace);
                                j += 1;
                            }
                        
                        newFace.v1 = numTotalVerts - j - 1;
                        newFace.v2 = i;
                        newFace.v3 = i + 1;
                        
                        faces.Add(newFace);
                    }
                }
            }
        }
        
        if (calcVertexNormals)
        {
            for (auto it = hollowCoords.CreateConstIterator() ; it ; ++it)
            {
                coords.Add((*it));
                hollowCoordIndices.Add(coords.Num() - 1);
            }
        }
        else
            coords.Append(hollowCoords);
        
        if (calcVertexNormals)
        {
            vertexNormals.Append(hollowNormals);
            us.Append(hollowUs);
            
        }
    }
    
    if (simpleFace && createFaces)
    {
        if (sides == 3)
            faces.Add(Face(0, 1, 2));
        else if (sides == 4)
        {
            faces.Add(Face(0, 1, 2));
            faces.Add(Face(0, 2, 3));
        }
    }
    
    if (calcVertexNormals && hasProfileCut)
    {
        int lastOuterVertIndex = numOuterVerts - 1;
        
        if (hasHollow)
        {
            cut1CoordIndices.Add(0);
            cut1CoordIndices.Add(coords.Num() - 1);
            
            cut2CoordIndices.Add(lastOuterVertIndex + 1);
            cut2CoordIndices.Add(lastOuterVertIndex);
            
            cutNormal1.X = coords[0].Y - coords[coords.Num() - 1].Y;
            cutNormal1.Y = -(coords[0].X - coords[coords.Num() - 1].X);
            
            cutNormal2.X = coords[lastOuterVertIndex + 1].Y - coords[lastOuterVertIndex].Y;
            cutNormal2.Y = -(coords[lastOuterVertIndex + 1].X - coords[lastOuterVertIndex].X);
        }
        
        else
        {
            cut1CoordIndices.Add(0);
            cut1CoordIndices.Add(1);
            
            cut2CoordIndices.Add(lastOuterVertIndex);
            cut2CoordIndices.Add(0);
            
            cutNormal1.X = vertexNormals[1].Y;
            cutNormal1.Y = -vertexNormals[1].X;
            
            cutNormal2.X = -vertexNormals[vertexNormals.Num() - 2].Y;
            cutNormal2.Y = vertexNormals[vertexNormals.Num() - 2].X;
            
        }
        cutNormal1.Normalize();
        cutNormal2.Normalize();
    }
    
    MakeFaceUVs();
    
    hollowCoords.Empty();
    hollowNormals.Empty();
    hollowUs.Empty();
    
    if (calcVertexNormals)
    { // calculate prim face numbers
        
        // face number order is top, outer, hollow, bottom, start cut, end cut
        // I know it's ugly but so is the whole concept of prim face numbers
        
        int faceNum = 1; // start with outer faces
        outerFaceNumber = faceNum;
        
        int startVert = hasProfileCut && !hasHollow ? 1 : 0;
        if (startVert > 0)
            faceNumbers.Add(-1);
        for (int i = 0; i < numOuterVerts - 1; i++)
            faceNumbers.Add(sides < 5 && i <= sides ? faceNum++ : faceNum);
        
        faceNumbers.Add(hasProfileCut ? -1 : faceNum++);
        
        if (sides > 4 && (hasHollow || hasProfileCut))
            faceNum++;
        
        if (sides < 5 && (hasHollow || hasProfileCut) && numOuterVerts < sides)
            faceNum++;
        
        if (hasHollow)
        {
            for (int i = 0; i < numHollowVerts; i++)
                faceNumbers.Add(faceNum);
            
            hollowFaceNumber = faceNum++;
        }
        
        bottomFaceNumber = faceNum++;
        
        if (hasHollow && hasProfileCut)
            faceNumbers.Add(faceNum++);
        
        for (int i = 0; i < faceNumbers.Num(); i++)
            if (faceNumbers[i] == -1)
                faceNumbers[i] = faceNum++;
        
        numPrimFaces = faceNum;
    }
    
}
    
void Profile::MakeFaceUVs()
{
    faceUVs.Empty();
    for (auto it = coords.CreateConstIterator() ; it ; ++it)
        faceUVs.Add(UVCoord(1.0f - (0.5f + (*it).X), 1.0f - (0.5f - (*it).Y)));
}

Profile Profile::Copy()
{
    return Copy(true);
}

Profile Profile::Copy(bool needFaces)
{
    Profile copy;
    
    copy.coords.Append(coords);
    copy.faceUVs.Append(faceUVs);
    
    if (needFaces)
        copy.faces.Append(faces);
    if ((copy.calcVertexNormals = calcVertexNormals) == true)
    {
        copy.vertexNormals.Append(vertexNormals);
        copy.faceNormal = faceNormal;
        copy.cutNormal1 = cutNormal1;
        copy.cutNormal2 = cutNormal2;
        copy.us.Append(us);
        copy.faceNumbers.Append(faceNumbers);
        
        copy.cut1CoordIndices.Append(cut1CoordIndices);
        copy.cut2CoordIndices.Append(cut2CoordIndices);
        copy.hollowCoordIndices.Append(hollowCoordIndices);
        copy.outerCoordIndices.Append(outerCoordIndices);
    }
    copy.numOuterVerts = numOuterVerts;
    copy.numHollowVerts = numHollowVerts;
    
    return copy;
}
    
void Profile::AddPos(Coord v)
{
    AddPos(v.X, v.Y, v.Z);
}

void Profile::AddPos(float x, float y, float z)
{
    int i;
    int numVerts = coords.Num();
    Coord vert;
    
    for (i = 0; i < numVerts; i++)
    {
        vert = coords[i];
        vert.X += x;
        vert.Y += y;
        vert.Z += z;
        coords[i] = vert;
    }
}

void Profile::AddRot(PrimQuat q)
{
    int i;
    int numVerts = coords.Num();
    
    for (i = 0; i < numVerts; i++)
        coords[i] *= q;
    
    if (calcVertexNormals)
    {
        int numNormals = vertexNormals.Num();
        for (i = 0; i < numNormals; i++)
            vertexNormals[i] *= q;
        
        faceNormal *= q;
        cutNormal1 *= q;
        cutNormal2 *= q;
        
    }
}

void Profile::Scale(float x, float y)
{
    int i;
    int numVerts = coords.Num();
    Coord vert;
    
    for (i = 0; i < numVerts; i++)
    {
        vert = coords[i];
        vert.X *= x;
        vert.Y *= y;
        coords[i] = vert;
    }
}

/// <summary>
/// Changes order of the vertex indices and negates the center vertex normal. Does not alter vertex normals of radial vertices
/// </summary>
void Profile::FlipNormals()
{
    int i;
    int numFaces = faces.Num();
    Face tmpFace;
    int tmp;
    
    for (i = 0; i < numFaces; i++)
    {
        tmpFace = faces[i];
        tmp = tmpFace.v3;
        tmpFace.v3 = tmpFace.v1;
        tmpFace.v1 = tmp;
        faces[i] = tmpFace;
    }
    
    if (calcVertexNormals)
    {
        int normalCount = vertexNormals.Num();
        if (normalCount > 0)
        {
            Coord n = vertexNormals[normalCount - 1];
            n.Z = -n.Z;
            vertexNormals[normalCount - 1] = n;
        }
    }
    
    faceNormal.X = -faceNormal.X;
    faceNormal.Y = -faceNormal.Y;
    faceNormal.Z = -faceNormal.Z;
    
    int numfaceUVs = faceUVs.Num();
    for (i = 0; i < numfaceUVs; i++)
    {
        UVCoord uv = faceUVs[i];
        uv.V = 1.0f - uv.V;
        faceUVs[i] = uv;
    }
}

void Profile::AddValue2FaceVertexIndices(int num)
{
    int numFaces = faces.Num();
    Face tmpFace;
    for (int i = 0; i < numFaces; i++)
    {
        tmpFace = faces[i];
        tmpFace.v1 += num;
        tmpFace.v2 += num;
        tmpFace.v3 += num;
        
        faces[i] = tmpFace;
    }
}

void Profile::AddValue2FaceNormalIndices(int num)
{
    if (calcVertexNormals)
    {
        int numFaces = faces.Num();
        Face tmpFace;
        for (int i = 0; i < numFaces; i++)
        {
            tmpFace = faces[i];
            tmpFace.n1 += num;
            tmpFace.n2 += num;
            tmpFace.n3 += num;
            
            faces[i] = tmpFace;
        }
    }
}

void Profile::DumpRaw(FString path, FString name, FString title)
{
    /*
    if (path == null)
        return;
    String fileName = name + "_" + title + ".raw";
    String completePath = System.IO.Path.Combine(path, fileName);
    StreamWriter sw = new StreamWriter(completePath);
    
    for (int i = 0; i < faces.Count; i++)
    {
        string s = coords[faces[i].v1].ToString();
        s += " " + coords[faces[i].v2].ToString();
        s += " " + coords[faces[i].v3].ToString();
        
        sw.WriteLine(s);
    }
    
    sw.Close();
    */
}

void Path::Create(PathType pathType, int steps)
{
    if (taperX > 0.999f)
        taperX = 0.999f;
    if (taperX < -0.999f)
        taperX = -0.999f;
    if (taperY > 0.999f)
        taperY = 0.999f;
    if (taperY < -0.999f)
        taperY = -0.999f;
    
    if (pathType == Linear || pathType == Flexible)
    {
        int step = 0;
        
        float length = pathCutEnd - pathCutBegin;
        float twistTotal = twistEnd - twistBegin;
        float twistTotalAbs = fabs(twistTotal);
        if (twistTotalAbs > 0.01f)
            steps += (int)(twistTotalAbs * 3.66); //  dahlia's magic number
        
        float start = -0.5f;
        float stepSize = length / (float)steps;
        float percentOfPathMultiplier = stepSize * 0.999999f;
        float xOffset = topShearX * pathCutBegin;
        float yOffset = topShearY * pathCutBegin;
        float zOffset = start;
        float xOffsetStepIncrement = topShearX * length / steps;
        float yOffsetStepIncrement = topShearY * length / steps;
        
        float percentOfPath = pathCutBegin;
        zOffset += percentOfPath;
        
        // sanity checks
        
        bool done = false;
        
        while (!done)
        {
            PathNode newNode;
            
            newNode.xScale = 1.0f;
            if (taperX == 0.0f)
                newNode.xScale = 1.0f;
            else if (taperX > 0.0f)
                newNode.xScale = 1.0f - percentOfPath * taperX;
            else newNode.xScale = 1.0f + (1.0f - percentOfPath) * taperX;
            
            newNode.yScale = 1.0f;
            if (taperY == 0.0f)
                newNode.yScale = 1.0f;
            else if (taperY > 0.0f)
                newNode.yScale = 1.0f - percentOfPath * taperY;
            else newNode.yScale = 1.0f + (1.0f - percentOfPath) * taperY;
            
            float twist = twistBegin + twistTotal * percentOfPath;
            
            newNode.rotation = PrimQuat(Coord(0.0f, 0.0f, 1.0f), twist);
            newNode.position = Coord(xOffset, yOffset, zOffset);
            newNode.percentOfPath = percentOfPath;
            
            pathNodes.Add(newNode);
            
            if (step < steps)
            {
                step += 1;
                percentOfPath += percentOfPathMultiplier;
                xOffset += xOffsetStepIncrement;
                yOffset += yOffsetStepIncrement;
                zOffset += stepSize;
                if (percentOfPath > pathCutEnd)
                    done = true;
            }
            else done = true;
        }
    } // end of linear path code
    
    else // pathType == Circular
    {
        float twistTotal = twistEnd - twistBegin;
        
        // if the profile has a lot of twist, add more layers otherwise the layers may overlap
        // and the resulting mesh may be quite inaccurate. This method is arbitrary and doesn't
        // accurately match the viewer
        float twistTotalAbs = fabs(twistTotal);
        if (twistTotalAbs > 0.01f)
        {
            if (twistTotalAbs > M_PI* 1.5f)
                steps *= 2;
            if (twistTotalAbs > M_PI * 3.0f)
                steps *= 2;
        }
        
        float yPathScale = holeSizeY * 0.5f;
        float pathLength = pathCutEnd - pathCutBegin;
        float totalSkew = skew * 2.0f * pathLength;
        float skewStart = pathCutBegin * 2.0f * skew - skew;
        float xOffsetTopShearXFactor = topShearX * (0.25f + 0.5f * (0.5f - holeSizeY));
        float yShearCompensation = 1.0f + fabs(topShearY) * 0.25f;
        
        // It's not quite clear what pushY (Y top shear) does, but subtracting it from the start and end
        // angles appears to approximate it's effects on path cut. Likewise, adding it to the angle used
        // to calculate the sine for generating the path radius appears to approximate it's effects there
        // too, but there are some subtle differences in the radius which are noticeable as the prim size
        // increases and it may affect megaprims quite a bit. The effect of the Y top shear parameter on
        // the meshes generated with this technique appear nearly identical in shape to the same prims when
        // displayed by the viewer.
        
        float startAngle = (twoPi * pathCutBegin * revolutions) - topShearY * 0.9f;
        float endAngle = (twoPi * pathCutEnd * revolutions) - topShearY * 0.9f;
        float stepSize = twoPi / stepsPerRevolution;
        
        int step = (int)(startAngle / stepSize);
        float angle = startAngle;
        
        bool done = false;
        while (!done) // loop through the length of the path and add the layers
        {
            PathNode newNode;
            
            float xProfileScale = (1.0f - fabs(skew)) * holeSizeX;
            float yProfileScale = holeSizeY;
            
            float percentOfPath = angle / (twoPi * revolutions);
            float percentOfAngles = (angle - startAngle) / (endAngle - startAngle);
            
            if (taperX > 0.01f)
                xProfileScale *= 1.0f - percentOfPath * taperX;
            else if (taperX < -0.01f)
                xProfileScale *= 1.0f + (1.0f - percentOfPath) * taperX;
            
            if (taperY > 0.01f)
                yProfileScale *= 1.0f - percentOfPath * taperY;
            else if (taperY < -0.01f)
                yProfileScale *= 1.0f + (1.0f - percentOfPath) * taperY;
            
            newNode.xScale = xProfileScale;
            newNode.yScale = yProfileScale;
            
            float radiusScale = 1.0f;
            if (radius > 0.001f)
                radiusScale = 1.0f - radius * percentOfPath;
            else if (radius < 0.001f)
                radiusScale = 1.0f + radius * (1.0f - percentOfPath);
            
            float twist = twistBegin + twistTotal * percentOfPath;
            
            float xOffset = 0.5f * (skewStart + totalSkew * percentOfAngles);
            xOffset += (float)sin(angle) * xOffsetTopShearXFactor;
            
            float yOffset = yShearCompensation * (float)cos(angle) * (0.5f - yPathScale) * radiusScale;
            
            float zOffset = (float)sin(angle + topShearY) * (0.5f - yPathScale) * radiusScale;
            
            newNode.position = Coord(xOffset, yOffset, zOffset);
            
            // now orient the rotation of the profile layer relative to it's position on the path
            // adding taperY to the angle used to generate the quat appears to approximate the viewer
            
            newNode.rotation = PrimQuat(Coord(1.0f, 0.0f, 0.0f), angle + topShearY);
            
            // next apply twist rotation to the profile layer
            if (twistTotal != 0.0f || twistBegin != 0.0f)
                newNode.rotation = newNode.rotation * PrimQuat(Coord(0.0f, 0.0f, 1.0f), twist);
            
            newNode.percentOfPath = percentOfPath;
            
            pathNodes.Add(newNode);
            
            // calculate terms for next iteration
            // calculate the angle for the next iteration of the loop
            
            if (angle >= endAngle - 0.01)
                done = true;
            else
            {
                step += 1;
                angle = stepSize * step;
                if (angle > endAngle)
                    angle = endAngle;
            }
        }
    }
}

/// <summary>
/// Human readable string representation of the parameters used to create a mesh.
/// </summary>
/// <returns></returns>
FString PrimMesh::ParamsToDisplayString()
{
    FString s = "";
    /*
    s += "sides..................: " + sides.ToString();
    s += "\nhollowSides..........: " + hollowSides.ToString();
    s += "\nprofileStart.........: " + profileStart.ToString();
    s += "\nprofileEnd...........: " + profileEnd.ToString();
    s += "\nhollow...............: " + hollow.ToString();
    s += "\ntwistBegin...........: " + twistBegin.ToString();
    s += "\ntwistEnd.............: " + twistEnd.ToString();
    s += "\ntopShearX............: " + topShearX.ToString();
    s += "\ntopShearY............: " + topShearY.ToString();
    s += "\npathCutBegin.........: " + pathCutBegin.ToString();
    s += "\npathCutEnd...........: " + pathCutEnd.ToString();
    s += "\ndimpleBegin..........: " + dimpleBegin.ToString();
    s += "\ndimpleEnd............: " + dimpleEnd.ToString();
    s += "\nskew.................: " + skew.ToString();
    s += "\nholeSizeX............: " + holeSizeX.ToString();
    s += "\nholeSizeY............: " + holeSizeY.ToString();
    s += "\ntaperX...............: " + taperX.ToString();
    s += "\ntaperY...............: " + taperY.ToString();
    s += "\nradius...............: " + radius.ToString();
    s += "\nrevolutions..........: " + revolutions.ToString();
    s += "\nstepsPerRevolution...: " + stepsPerRevolution.ToString();
    s += "\nsphereMode...........: " + sphereMode.ToString();
    s += "\nhasProfileCut........: " + hasProfileCut.ToString();
    s += "\nhasHollow............: " + hasHollow.ToString();
    s += "\nviewerMode...........: " + viewerMode.ToString();
    */
    
    return s;
}


/// <summary>
/// Constructs a PrimMesh object and creates the profile for extrusion.
/// </summary>
/// <param name="sides"></param>
/// <param name="profileStart"></param>
/// <param name="profileEnd"></param>
/// <param name="hollow"></param>
/// <param name="hollowSides"></param>
PrimMesh::PrimMesh(int sidesIn, float profileStartIn, float profileEndIn, float hollowIn, int hollowSidesIn)
{
    coords.Empty();
    faces.Empty();
    
    sides = sidesIn;
    profileStart = profileStartIn;
    profileEnd = profileEndIn;
    hollow = hollowIn;
    hollowSides = hollowSidesIn;
    
    if (sides < 3)
        sides = 3;
    if (hollowSides < 3)
        hollowSides = 3;
    if (profileStart < 0.0f)
        profileStart = 0.0f;
    if (profileEnd > 1.0f)
        profileEnd = 1.0f;
    if (profileEnd < 0.02f)
        profileEnd = 0.02f;
    if (profileStart >= profileEnd)
        profileStart = profileEnd - 0.02f;
    if (hollow > 0.99f)
        hollow = 0.99f;
    if (hollow < 0.0f)
        hollow = 0.0f;
}

/// <summary>
/// Extrudes a profile along a path.
/// </summary>
void PrimMesh::Extrude(PathType pathType)
{
    bool needEndFaces = false;
    
    coords.Empty();
    faces.Empty();
    
    if (viewerMode)
    {
        viewerFaces.Empty();
        calcVertexNormals = true;
    }
    
    if (calcVertexNormals)
        normals.Empty();
    
    int steps = 1;
    
    float length = pathCutEnd - pathCutBegin;
    normalsProcessed = false;
    
    if (viewerMode && sides == 3)
    {
        // prisms don't taper well so add some vertical resolution
        // other prims may benefit from this but just do prisms for now
        if (fabs(taperX) > 0.01 || fabs(taperY) > 0.01)
            steps = (int)(steps * 4.5 * length);
    }
    
    if (sphereMode)
        hasProfileCut = profileEnd - profileStart < 0.4999f;
    else
        hasProfileCut = profileEnd - profileStart < 0.9999f;
    hasHollow = (hollow > 0.001f);
    
    float localtwistBegin = twistBegin / 360.0f * twoPi;
    float localtwistEnd = twistEnd / 360.0f * twoPi;
    float twistTotal = localtwistEnd - localtwistBegin;
    float twistTotalAbs = fabs(twistTotal);
    if (twistTotalAbs > 0.01f)
        steps += (int)(twistTotalAbs * 3.66); //  dahlia's magic number
    
    float localhollow = hollow;
    
    if (pathType == Circular)
    {
        needEndFaces = false;
        if (pathCutBegin != 0.0f || pathCutEnd != 1.0f)
            needEndFaces = true;
        else if (taperX != 0.0f || taperY != 0.0f)
            needEndFaces = true;
        else if (skew != 0.0f)
            needEndFaces = true;
        else if (twistTotal != 0.0f)
            needEndFaces = true;
        else if (radius != 0.0f)
            needEndFaces = true;
    }
    else needEndFaces = true;
    
    // sanity checks
    float initialProfileRot = 0.0f;
    if (pathType == Circular)
    {
        if (sides == 3)
        {
            initialProfileRot = (float)M_PI;
            if (hollowSides == 4)
            {
                if (localhollow > 0.7f)
                    localhollow = 0.7f;
                localhollow *= 0.707f;
            }
            else localhollow *= 0.5f;
        }
        else if (sides == 4)
        {
            initialProfileRot = 0.25f * (float)M_PI;
            if (hollowSides != 4)
                localhollow *= 0.707f;
        }
        else if (sides > 4)
        {
            initialProfileRot = (float)M_PI;
            if (hollowSides == 4)
            {
                if (localhollow > 0.7f)
                    localhollow = 0.7f;
                localhollow /= 0.7f;
            }
        }
    }
    else
    {
        if (sides == 3)
        {
            if (hollowSides == 4)
            {
                if (localhollow > 0.7f)
                    localhollow = 0.7f;
                localhollow *= 0.707f;
            }
            else localhollow *= 0.5f;
        }
        else if (sides == 4)
        {
            initialProfileRot = 1.25f * (float)M_PI;
            if (hollowSides != 4)
                localhollow *= 0.707f;
        }
        else if (sides == 24 && hollowSides == 4)
            localhollow *= 1.414f;
    }
    
    Profile profile = Profile(sides, profileStart, profileEnd, localhollow, hollowSides, true, calcVertexNormals);
    errorMessage = profile.errorMessage;
    
    numPrimFaces = profile.numPrimFaces;
    
    int cut1FaceNumber = profile.bottomFaceNumber + 1;
    int cut2FaceNumber = cut1FaceNumber + 1;
    if (!needEndFaces)
    {
        cut1FaceNumber -= 2;
        cut2FaceNumber -= 2;
    }
    
    profileOuterFaceNumber = profile.outerFaceNumber;
    if (!needEndFaces)
        profileOuterFaceNumber--;
    
    if (hasHollow)
    {
        profileHollowFaceNumber = profile.hollowFaceNumber;
        if (!needEndFaces)
            profileHollowFaceNumber--;
    }
    
    int cut1Vert = -1;
    int cut2Vert = -1;
    if (hasProfileCut)
    {
        cut1Vert = hasHollow ? profile.coords.Num() - 1 : 0;
        cut2Vert = hasHollow ? profile.numOuterVerts - 1 : profile.numOuterVerts;
    }
    
    if (initialProfileRot != 0.0f)
    {
        profile.AddRot(PrimQuat(Coord(0.0f, 0.0f, 1.0f), initialProfileRot));
        if (viewerMode)
            profile.MakeFaceUVs();
    }
    
    Coord lastCutNormal1 = Coord();
    Coord lastCutNormal2 = Coord();
    float thisV = 0.0f;
    float lastV = 0.0f;
    
    Path path = Path();
    path.twistBegin = localtwistBegin;
    path.twistEnd = localtwistEnd;
    path.topShearX = topShearX;
    path.topShearY = topShearY;
    path.pathCutBegin = pathCutBegin;
    path.pathCutEnd = pathCutEnd;
    path.dimpleBegin = dimpleBegin;
    path.dimpleEnd = dimpleEnd;
    path.skew = skew;
    path.holeSizeX = holeSizeX;
    path.holeSizeY = holeSizeY;
    path.taperX = taperX;
    path.taperY = taperY;
    path.radius = radius;
    path.revolutions = revolutions;
    path.stepsPerRevolution = stepsPerRevolution;
    
    path.Create(pathType, steps);
    
    for (int nodeIndex = 0; nodeIndex < path.pathNodes.Num(); nodeIndex++)
    {
        PathNode node = path.pathNodes[nodeIndex];
        Profile newLayer = profile.Copy();
        newLayer.Scale(node.xScale, node.yScale);
        
        newLayer.AddRot(node.rotation);
        newLayer.AddPos(node.position);
        
        if (needEndFaces && nodeIndex == 0)
        {
            newLayer.FlipNormals();
            
            // add the bottom faces to the viewerFaces list
            if (viewerMode)
            {
                Coord faceNormal = newLayer.faceNormal;
                ViewerFace newViewerFace = ViewerFace(profile.bottomFaceNumber);
                int numFaces = newLayer.faces.Num();
                TArray<Face> localfaces = newLayer.faces;
                
                for (int i = 0; i < numFaces; i++)
                {
                    Face face = localfaces[i];
                    newViewerFace.v1 = newLayer.coords[face.v1];
                    newViewerFace.v2 = newLayer.coords[face.v2];
                    newViewerFace.v3 = newLayer.coords[face.v3];
                    
                    newViewerFace.coordIndex1 = face.v1;
                    newViewerFace.coordIndex2 = face.v2;
                    newViewerFace.coordIndex3 = face.v3;
                    
                    newViewerFace.n1 = faceNormal;
                    newViewerFace.n2 = faceNormal;
                    newViewerFace.n3 = faceNormal;
                    
                    newViewerFace.uv1 = newLayer.faceUVs[face.v1];
                    newViewerFace.uv2 = newLayer.faceUVs[face.v2];
                    newViewerFace.uv3 = newLayer.faceUVs[face.v3];
                    
                    if (pathType == Linear)
                    {
                        newViewerFace.uv1.Flip();
                        newViewerFace.uv2.Flip();
                        newViewerFace.uv3.Flip();
                    }
                    
                    viewerFaces.Add(newViewerFace);
                }
            }
        } // if (nodeIndex == 0)
        
        // append this layer
        
        int coordsLen = coords.Num();
        newLayer.AddValue2FaceVertexIndices(coordsLen);
        
        coords.Append(newLayer.coords);
        
        if (calcVertexNormals)
        {
            newLayer.AddValue2FaceNormalIndices(normals.Num());
            normals.Append(newLayer.vertexNormals);
        }
        
        if (node.percentOfPath < pathCutBegin + 0.01f || node.percentOfPath > pathCutEnd - 0.01f)
            faces.Append(newLayer.faces);
        
        // fill faces between layers
        
        int numVerts = newLayer.coords.Num();
        Face newFace1;
        Face newFace2;
        
        thisV = 1.0f - node.percentOfPath;
        
        if (nodeIndex > 0)
        {
            int startVert = coordsLen + 1;
            int endVert = coords.Num();
            
            if (sides < 5 || hasProfileCut || hasHollow)
                startVert--;
            
            for (int i = startVert; i < endVert; i++)
            {
                int iNext = i + 1;
                if (i == endVert - 1)
                    iNext = startVert;
                
                int whichVert = i - startVert;
                
                newFace1.v1 = i;
                newFace1.v2 = i - numVerts;
                newFace1.v3 = iNext;
                
                newFace1.n1 = newFace1.v1;
                newFace1.n2 = newFace1.v2;
                newFace1.n3 = newFace1.v3;
                faces.Add(newFace1);
                
                newFace2.v1 = iNext;
                newFace2.v2 = i - numVerts;
                newFace2.v3 = iNext - numVerts;
                
                newFace2.n1 = newFace2.v1;
                newFace2.n2 = newFace2.v2;
                newFace2.n3 = newFace2.v3;
                faces.Add(newFace2);
                
                if (viewerMode)
                {
                    // add the side faces to the list of viewerFaces here
                    
                    int primFaceNum = profile.faceNumbers[whichVert];
                    if (!needEndFaces)
                        primFaceNum -= 1;
                    
                    ViewerFace newViewerFace1 = ViewerFace(primFaceNum);
                    ViewerFace newViewerFace2 = ViewerFace(primFaceNum);
                    
                    int uIndex = whichVert;
                    if (!hasHollow && sides > 4 && uIndex < newLayer.us.Num() - 1)
                    {
                        uIndex++;
                    }
                    
                    float u1 = newLayer.us[uIndex];
                    float u2 = 1.0f;
                    if (uIndex < (int)newLayer.us.Num() - 1)
                        u2 = newLayer.us[uIndex + 1];
                    
                    if (whichVert == cut1Vert || whichVert == cut2Vert)
                    {
                        u1 = 0.0f;
                        u2 = 1.0f;
                    }
                    else if (sides < 5)
                    {
                        if (whichVert < profile.numOuterVerts)
                        { // boxes and prisms have one texture face per side of the prim, so the U values have to be scaled
                            // to reflect the entire texture width
                            u1 *= sides;
                            u2 *= sides;
                            u2 -= (int)u1;
                            u1 -= (int)u1;
                            if (u2 < 0.1f)
                                u2 = 1.0f;
                        }
                    }
                    
                    if (sphereMode)
                    {
                        if (whichVert != cut1Vert && whichVert != cut2Vert)
                        {
                            u1 = u1 * 2.0f - 1.0f;
                            u2 = u2 * 2.0f - 1.0f;
                            
                            if (whichVert >= newLayer.numOuterVerts)
                            {
                                u1 -= localhollow;
                                u2 -= localhollow;
                            }
                            
                        }
                    }
                    
                    newViewerFace1.uv1.U = u1;
                    newViewerFace1.uv2.U = u1;
                    newViewerFace1.uv3.U = u2;
                    
                    newViewerFace1.uv1.V = thisV;
                    newViewerFace1.uv2.V = lastV;
                    newViewerFace1.uv3.V = thisV;
                    
                    newViewerFace2.uv1.U = u2;
                    newViewerFace2.uv2.U = u1;
                    newViewerFace2.uv3.U = u2;
                    
                    newViewerFace2.uv1.V = thisV;
                    newViewerFace2.uv2.V = lastV;
                    newViewerFace2.uv3.V = lastV;
                    
                    newViewerFace1.v1 = coords[newFace1.v1];
                    newViewerFace1.v2 = coords[newFace1.v2];
                    newViewerFace1.v3 = coords[newFace1.v3];
                    
                    newViewerFace2.v1 = coords[newFace2.v1];
                    newViewerFace2.v2 = coords[newFace2.v2];
                    newViewerFace2.v3 = coords[newFace2.v3];
                    
                    newViewerFace1.coordIndex1 = newFace1.v1;
                    newViewerFace1.coordIndex2 = newFace1.v2;
                    newViewerFace1.coordIndex3 = newFace1.v3;
                    
                    newViewerFace2.coordIndex1 = newFace2.v1;
                    newViewerFace2.coordIndex2 = newFace2.v2;
                    newViewerFace2.coordIndex3 = newFace2.v3;
                    
                    // profile cut faces
                    if (whichVert == cut1Vert)
                    {
                        newViewerFace1.primFaceNumber = cut1FaceNumber;
                        newViewerFace2.primFaceNumber = cut1FaceNumber;
                        newViewerFace1.n1 = newLayer.cutNormal1;
                        newViewerFace1.n2 = newViewerFace1.n3 = lastCutNormal1;
                        
                        newViewerFace2.n1 = newViewerFace2.n3 = newLayer.cutNormal1;
                        newViewerFace2.n2 = lastCutNormal1;
                    }
                    else if (whichVert == cut2Vert)
                    {
                        newViewerFace1.primFaceNumber = cut2FaceNumber;
                        newViewerFace2.primFaceNumber = cut2FaceNumber;
                        newViewerFace1.n1 = newLayer.cutNormal2;
                        newViewerFace1.n2 = lastCutNormal2;
                        newViewerFace1.n3 = lastCutNormal2;
                        
                        newViewerFace2.n1 = newLayer.cutNormal2;
                        newViewerFace2.n3 = newLayer.cutNormal2;
                        newViewerFace2.n2 = lastCutNormal2;
                    }
                    
                    else // outer and hollow faces
                    {
                        if ((sides < 5 && whichVert < newLayer.numOuterVerts) || (hollowSides < 5 && whichVert >= newLayer.numOuterVerts))
                        { // looks terrible when path is twisted... need vertex normals here
                            newViewerFace1.CalcSurfaceNormal();
                            newViewerFace2.CalcSurfaceNormal();
                        }
                        else
                        {
                            newViewerFace1.n1 = normals[newFace1.n1];
                            newViewerFace1.n2 = normals[newFace1.n2];
                            newViewerFace1.n3 = normals[newFace1.n3];
                            
                            newViewerFace2.n1 = normals[newFace2.n1];
                            newViewerFace2.n2 = normals[newFace2.n2];
                            newViewerFace2.n3 = normals[newFace2.n3];
                        }
                    }
                    
                    viewerFaces.Add(newViewerFace1);
                    viewerFaces.Add(newViewerFace2);
                    
                }
            }
        }
        
        lastCutNormal1 = newLayer.cutNormal1;
        lastCutNormal2 = newLayer.cutNormal2;
        lastV = thisV;
        
        if (needEndFaces && nodeIndex == path.pathNodes.Num() - 1 && viewerMode)
        {
            // add the top faces to the viewerFaces list here
            Coord faceNormal = newLayer.faceNormal;
            ViewerFace newViewerFace = ViewerFace(0);
            int numFaces = newLayer.faces.Num();
            TArray<Face> localfaces = newLayer.faces;
            
            for (int i = 0; i < numFaces; i++)
            {
                Face face = localfaces[i];
                newViewerFace.v1 = newLayer.coords[face.v1 - coordsLen];
                newViewerFace.v2 = newLayer.coords[face.v2 - coordsLen];
                newViewerFace.v3 = newLayer.coords[face.v3 - coordsLen];
                
                newViewerFace.coordIndex1 = face.v1 - coordsLen;
                newViewerFace.coordIndex2 = face.v2 - coordsLen;
                newViewerFace.coordIndex3 = face.v3 - coordsLen;
                
                newViewerFace.n1 = faceNormal;
                newViewerFace.n2 = faceNormal;
                newViewerFace.n3 = faceNormal;
                
                newViewerFace.uv1 = newLayer.faceUVs[face.v1 - coordsLen];
                newViewerFace.uv2 = newLayer.faceUVs[face.v2 - coordsLen];
                newViewerFace.uv3 = newLayer.faceUVs[face.v3 - coordsLen];
                
                if (pathType == Linear)
                {
                    newViewerFace.uv1.Flip();
                    newViewerFace.uv2.Flip();
                    newViewerFace.uv3.Flip();
                }
                
                viewerFaces.Add(newViewerFace);
            }
        }
        
        
    } // for (int nodeIndex = 0; nodeIndex < path.pathNodes.Count; nodeIndex++)
    
}

Coord PrimMesh::SurfaceNormal(Coord c1, Coord c2, Coord c3)
{
    Coord edge1(c2.X - c1.X, c2.Y - c1.Y, c2.Z - c1.Z);
    Coord edge2(c3.X - c1.X, c3.Y - c1.Y, c3.Z - c1.Z);
    
    Coord normal = Coord::Cross(edge1, edge2);
    
    normal.Normalize();
    
    return normal;
}

Coord PrimMesh::SurfaceNormal(Face face)
{
    return SurfaceNormal(coords[face.v1], coords[face.v2], coords[face.v3]);
}

/// <summary>
/// Calculate the surface normal for a face in the list of faces
/// </summary>
/// <param name="faceIndex"></param>
/// <returns></returns>
Coord PrimMesh::SurfaceNormal(int faceIndex)
{
    int numFaces = faces.Num();
    if (faceIndex < 0 || faceIndex >= numFaces)
        throw std::exception(); //("faceIndex out of range");
    
    return SurfaceNormal(faces[faceIndex]);
}

/// <summary>
/// Duplicates a PrimMesh object. All object properties are copied by value, including lists.
/// </summary>
/// <returns></returns>
PrimMesh PrimMesh::Copy()
{
    PrimMesh copy = PrimMesh(sides, profileStart, profileEnd, hollow, hollowSides);
    copy.twistBegin = twistBegin;
    copy.twistEnd = twistEnd;
    copy.topShearX = topShearX;
    copy.topShearY = topShearY;
    copy.pathCutBegin = pathCutBegin;
    copy.pathCutEnd = pathCutEnd;
    copy.dimpleBegin = dimpleBegin;
    copy.dimpleEnd = dimpleEnd;
    copy.skew = skew;
    copy.holeSizeX = holeSizeX;
    copy.holeSizeY = holeSizeY;
    copy.taperX = taperX;
    copy.taperY = taperY;
    copy.radius = radius;
    copy.revolutions = revolutions;
    copy.stepsPerRevolution = stepsPerRevolution;
    copy.calcVertexNormals = calcVertexNormals;
    copy.normalsProcessed = normalsProcessed;
    copy.viewerMode = viewerMode;
    copy.numPrimFaces = numPrimFaces;
    copy.errorMessage = errorMessage;
    
    copy.coords.Append(coords);
    copy.faces.Append(faces);
    copy.viewerFaces.Append(viewerFaces);
    copy.normals.Append(normals);
    
    return copy;
}

/// <summary>
/// Calculate surface normals for all of the faces in the list of faces in this mesh
/// </summary>
void PrimMesh::CalcNormals()
{
    if (normalsProcessed)
        return;
    
    normalsProcessed = true;
    
    int numFaces = faces.Num();
    
    if (!calcVertexNormals)
        normals.Empty();
    
    for (int i = 0; i < numFaces; i++)
    {
        Face face = faces[i];
        
        normals.Add(SurfaceNormal(i).Normalize());
        
        int normIndex = normals.Num() - 1;
        face.n1 = normIndex;
        face.n2 = normIndex;
        face.n3 = normIndex;
        
        faces[i] = face;
    }
}

/// <summary>
/// Adds a value to each XYZ vertex coordinate in the mesh
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
void PrimMesh::AddPos(float x, float y, float z)
{
    int i;
    int numVerts = coords.Num();
    Coord vert;
    
    for (i = 0; i < numVerts; i++)
    {
        vert = coords[i];
        vert.X += x;
        vert.Y += y;
        vert.Z += z;
        coords[i] = vert;
    }
    
    int numViewerFaces = viewerFaces.Num();
    
    for (i = 0; i < numViewerFaces; i++)
    {
        ViewerFace v = viewerFaces[i];
        v.AddPos(x, y, z);
        viewerFaces[i] = v;
    }
}

/// <summary>
/// Rotates the mesh
/// </summary>
/// <param name="q"></param>
void PrimMesh::AddRot(PrimQuat q)
{
    int i;
    int numVerts = coords.Num();
    
    for (i = 0; i < numVerts; i++)
        coords[i] *= q;
    
    int numNormals = normals.Num();
    for (i = 0; i < numNormals; i++)
        normals[i] *= q;
    
    int numViewerFaces = viewerFaces.Num();
    
    for (i = 0; i < numViewerFaces; i++)
    {
        ViewerFace v = viewerFaces[i];
        v.v1 *= q;
        v.v2 *= q;
        v.v3 *= q;
        
        v.n1 *= q;
        v.n2 *= q;
        v.n3 *= q;
        viewerFaces[i] = v;
    }
}

#if VERTEX_INDEXER
public VertexIndexer PrimMesh::GetVertexIndexer()
{
    if (viewerMode && viewerFaces.Count > 0)
        return new VertexIndexer(this);
    return null;
}
#endif

/// <summary>
/// Scales the mesh
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
void PrimMesh::Scale(float x, float y, float z)
{
    int i;
    int numVerts = coords.Num();
    //Coord vert;
    
    Coord m(x, y, z);
    for (i = 0; i < numVerts; i++)
        coords[i] *= m;
    
    int numViewerFaces = viewerFaces.Num();
    for (i = 0; i < numViewerFaces; i++)
    {
        ViewerFace v = viewerFaces[i];
        v.v1 *= m;
        v.v2 *= m;
        v.v3 *= m;
        viewerFaces[i] = v;
    }
}

/// <summary>
/// Dumps the mesh to a Blender compatible "Raw" format file
/// </summary>
/// <param name="path"></param>
/// <param name="name"></param>
/// <param name="title"></param>
void PrimMesh::DumpRaw(FString path, FString name, FString title)
{
    /*
    if (path == null)
        return;
    String fileName = name + "_" + title + ".raw";
    String completePath = System.IO.Path.Combine(path, fileName);
    StreamWriter sw = new StreamWriter(completePath);
    
    for (int i = 0; i < faces.Count; i++)
    {
        string s = coords[faces[i].v1].ToString();
        s += " " + coords[faces[i].v2].ToString();
        s += " " + coords[faces[i].v3].ToString();
        
        sw.WriteLine(s);
    }
    
    sw.Close();
    */
}
