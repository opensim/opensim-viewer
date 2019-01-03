/*
 * Copyright (c) Contributors, http://opensimulator.org/
 * See CONTRIBUTORS.TXT for a full list of copyright holders.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the OpenSimulator Project nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE DEVELOPERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
using System;
using System.Collections.Generic;
using System.Linq;
using Xenko.Core.Mathematics;
using System.Drawing;

namespace PotamOS.Meshing
{

    public enum PathType { Linear = 0, Circular = 1, Flexible = 2 };

    public enum ProfileShapeType : byte
    {
        pstCircle = 0,
        pstSquare = 1,
        pstIsometricTriangle = 2,
        pstEquilateralTriangle = 3,
        pstRightTriangle = 4,
        pstHalfCircle = 5
    };

    public enum HollowShapeType : byte
    {
        hstSame = 0,
        hstCircle = 16,
        hstSquare = 32,
        hstTriangle = 48
    };

    public enum ExtrusionType : byte
    {
        etStraight = 0x10,
        etCurve1 = 0x20,
        etCurve2 = 0x30,
        etFlexible = 0x80
    };

    public enum LevelDetail
    {
        Lowest = 0,
        Low = 1,
        High = 2,
        Highest = 3
    };

    /**
     * 
     */

    class Face
    {
        public int primFace;

        // vertices
        public int v1;
        public int v2;
        public int v3;

        // uvs
        public int uv1;
        public int uv2;
        public int uv3;

        public Face()
        {
            primFace = v1 = v2 = v3 = uv1 = uv2 = uv3 = 0;
        }

        public Face(int v1in, int v2in, int v3in)
        {
            primFace = 0;

            v1 = v1in;
            v2 = v2in;
            v3 = v3in;

            uv1 = 0;
            uv2 = 0;
            uv3 = 0;
        }

        public Vector3 SurfaceNormal(List<Vector3> coordList)
        {
            Vector3 c1 = coordList[v1];
            Vector3 c2 = coordList[v2];
            Vector3 c3 = coordList[v3];

            Vector3 edge1 = new Vector3(c2.X - c1.X, c2.Y - c1.Y, c2.Z - c1.Z);
            Vector3 edge2 = new Vector3(c3.X - c1.X, c3.Y - c1.Y, c3.Z - c1.Z);
            Vector3 cross = Vector3.Cross(edge1, edge2);

            return Vector3.Normalize(cross);
        }

    };

    class ViewerFace
    {
        public int primFaceNumber;

        public Vector3 v1;
        public Vector3 v2;
        public Vector3 v3;

        public int coordIndex1;
        public int coordIndex2;
        public int coordIndex3;

        public Vector2 uv1;
        public Vector2 uv2;
        public Vector2 uv3;

        public ViewerFace(int primFaceNumberIn)
        {
            primFaceNumber = primFaceNumberIn;

            coordIndex1 = coordIndex2 = coordIndex3 = -1; // -1 means not assigned yet

        }

        public void Scale(float x, float y, float z)
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

        public void AddPos(float x, float y, float z)
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

        public void AddRot(Quaternion q)
        {
            v1 = Vector3.Transform(v1, q);
            v2 = Vector3.Transform(v2, q);
            v3 = Vector3.Transform(v3, q);
        }

    };

    class Angle
    {
        public float angle;
        public float X;
        public float Y;

        public Angle()
        {
            angle = X = Y = 0.0f;
        }

        public Angle(float angleIn, float xIn, float yIn)
        {
            angle = angleIn;
            X = xIn;
            Y = yIn;
        }
    };

    class AngleList
    {
        public float iX, iY; // intersection point
        public List<Angle> angles = new List<Angle>();

        static readonly Angle[] angles3 = new Angle[] {
            new Angle(0.0f, 1.0f, 0.0f),
            new Angle(0.33333333333333333f, -0.5f, 0.86602540378443871f),
            new Angle(0.66666666666666667f, -0.5f, -0.86602540378443837f),
            new Angle(1.0f, 1.0f, 0.0f)
        };
        static readonly Angle[] angles4 = new Angle[] {
            new Angle(0.0f, 1.0f, 0.0f),
            new Angle(0.25f, 0.0f, 1.0f),
            new Angle(0.5f, -1.0f, 0.0f),
            new Angle(0.75f, 0.0f, -1.0f),
            new Angle(1.0f, 1.0f, 0.0f)
        };
        static readonly Angle[] angles24 = new Angle[]
        {
            new Angle(0.0f, 1.0f, 0.0f),
            new Angle(0.041666666666666664f, 0.96592582628906831f, 0.25881904510252074f),
            new Angle(0.083333333333333329f, 0.86602540378443871f, 0.5f),
            new Angle(0.125f, 0.70710678118654757f, 0.70710678118654746f),
            new Angle(0.16666666666666667f, 0.5f, 0.8660254037844386f),
            new Angle(0.20833333333333331f, 0.25881904510252096f, 0.9659258262890682f),
            new Angle(0.25f, 0.0f, 1.0f),
            new Angle(0.29166666666666663f, -0.25881904510252063f, 0.96592582628906831f),
            new Angle(0.33333333333333333f, -0.5f, 0.86602540378443871f),
            new Angle(0.375f, -0.70710678118654746f, 0.70710678118654757f),
            new Angle(0.41666666666666663f, -0.86602540378443849f, 0.5f),
            new Angle(0.45833333333333331f, -0.9659258262890682f, 0.25881904510252102f),
            new Angle(0.5f, -1.0f, 0.0f),
            new Angle(0.54166666666666663f, -0.96592582628906842f, -0.25881904510252035f),
            new Angle(0.58333333333333326f, -0.86602540378443882f, -0.5f),
            new Angle(0.62499999999999989f, -0.70710678118654791f, -0.70710678118654713f),
            new Angle(0.66666666666666667f, -0.5f, -0.86602540378443837f),
            new Angle(0.70833333333333326f, -0.25881904510252152f, -0.96592582628906809f),
            new Angle(0.75f, 0.0f, -1.0f),
            new Angle(0.79166666666666663f, 0.2588190451025203f, -0.96592582628906842f),
            new Angle(0.83333333333333326f, 0.5f, -0.86602540378443904f),
            new Angle(0.875f, 0.70710678118654735f, -0.70710678118654768f),
            new Angle(0.91666666666666663f, 0.86602540378443837f, -0.5f),
            new Angle(0.95833333333333326f, 0.96592582628906809f, -0.25881904510252157f),
            new Angle(1.0f, 1.0f, 0.0f)
        };

        public void MakeAngles(int sides, float startAngle, float stopAngle)
        {
            angles.Clear();

            double twoPi = Math.PI * 2.0;
            float twoPiInv = 1.0f / (float)twoPi;

            if (sides < 1)
                throw new Exception("number of sides not greater than zero");
            if (stopAngle <= startAngle)
                throw new Exception("stopAngle not greater than startAngle");

            if ((sides == 3 || sides == 4 || sides == 24))
            {
                startAngle *= twoPiInv;
                stopAngle *= twoPiInv;

                Angle[] sourceAngles;
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
                }

                if (startAngle > 0.0f)
                    angles[0] = InterpolatePoints(startAngle, angles[0], angles[1]);
                if (stopAngle < 1.0f)
                {
                    int lastAngleIndex = angles.Count() - 1;
                    angles[lastAngleIndex] = InterpolatePoints(stopAngle, angles[lastAngleIndex - 1], angles[lastAngleIndex]);
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
                    Angle newAngle = new Angle();
                    newAngle.angle = (float)angle;
                    newAngle.X = (float)Math.Cos(angle);
                    newAngle.Y = (float)Math.Sin(angle);
                    angles.Add(newAngle);
                    step += 1;
                    angle = stepSize * step;
                }

                if (startAngle > angles[0].angle)
                {
                    Angle newAngle = new Angle();
                    Intersection(angles[0].X, angles[0].Y, angles[1].X, angles[1].Y, 0.0f, 0.0f, (float)Math.Cos(startAngle), (float)Math.Sin(startAngle));
                    newAngle.angle = startAngle;
                    newAngle.X = iX;
                    newAngle.Y = iY;
                    angles[0] = newAngle;
                }

                int index = angles.Count() - 1;
                if (stopAngle < angles[index].angle)
                {
                    Angle newAngle = new Angle();
                    Intersection(angles[index - 1].X, angles[index - 1].Y, angles[index].X, angles[index].Y, 0.0f, 0.0f, (float)Math.Cos(stopAngle), (float)Math.Sin(stopAngle));
                    newAngle.angle = stopAngle;
                    newAngle.X = iX;
                    newAngle.Y = iY;
                    angles[index] = newAngle;
                }
            }

        }

        private Angle InterpolatePoints(float newPoint, Angle p1, Angle p2)
        {
            float m = (newPoint - p1.angle) / (p2.angle - p1.angle);
            return new Angle(newPoint, p1.X + m * (p2.X - p1.X), p1.Y + m * (p2.Y - p1.Y));

        }
        private void Intersection(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
        {
            // ref: http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/

            double denom = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);
            double uaNumerator = (x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3);

            if (denom != 0.0)
            {
                double ua = uaNumerator / denom;
                iX = (float)(x1 + ua * (x2 - x1));
                iY = (float)(y1 + ua * (y2 - y1));
            }

        }
    }

    struct PathNode
    {
        public Vector3 position;
        public Quaternion rotation;
        public float xScale;
        public float yScale;
        public float percentOfPath;
    };

    class Profile
    {
        public const float twoPi = (float)Math.PI * 2;

        public String errorMessage = String.Empty;

        public List<Vector3> coords = new List<Vector3>();
        public List<Face> faces = new List<Face>();
        public List<float> us = new List<float>();
        public List<Vector2> faceUVs = new List<Vector2>();
        public List<int> faceNumbers = new List<int>();

        // use these for making individual meshes for each prim face
        public List<int> outerCoordIndices = new List<int>();
        public List<int> hollowCoordIndices = new List<int>();
        public List<int> cut1CoordIndices = new List<int>();
        public List<int> cut2CoordIndices = new List<int>();

        public int numOuterVerts;
        public int numHollowVerts;

        public int outerFaceNumber;
        public int hollowFaceNumber;

        public bool calcVertexNormals = false;
        public int bottomFaceNumber = 0;
        public int numPrimFaces = 0;

        public Profile()
        {
            numOuterVerts = 0;
            numHollowVerts = 0;
            outerFaceNumber = -1;
            hollowFaceNumber = -1;
            calcVertexNormals = false;
            bottomFaceNumber = 0;
            numPrimFaces = 0;

            coords.Clear();
            faces.Clear();
            us.Clear();
            faceUVs.Clear();
            faceNumbers.Clear();

        }

        public Profile(int sides, float profileStart, float profileEnd, float hollow, int hollowSides, bool createFaces, bool calcVertexNormalsIn)
        {
            numOuterVerts = 0;
            numHollowVerts = 0;
            outerFaceNumber = -1;
            hollowFaceNumber = -1;
            calcVertexNormals = false;
            bottomFaceNumber = 0;
            numPrimFaces = 0;

            // this actually means data for viewer
            calcVertexNormals = calcVertexNormalsIn;
            coords.Clear();
            faces.Clear();
            us.Clear();
            faceUVs.Clear();
            faceNumbers.Clear();

            Vector3 center = new Vector3(0.0f, 0.0f, 0.0f);

            List<Vector3> hollowCoords = new List<Vector3>();
            List<float> hollowUs = new List<float>();

            if (calcVertexNormals)
            {
                outerCoordIndices.Clear();
                hollowCoordIndices.Clear();
                cut1CoordIndices.Clear();
                cut2CoordIndices.Clear();
            }

            bool hasHollow = (hollow > 0.0f);

            bool hasProfileCut = (profileStart > 0.0f || profileEnd < 1.0f);

            AngleList angles = new AngleList();
            AngleList hollowAngles = new AngleList();

            float xScale = 0.5f;
            float yScale = 0.5f;
            if (sides == 4)  // corners of a square are sqrt(2) from center
            {
                xScale = 0.707107f;
                yScale = 0.707107f;
            }

            float startAngle = profileStart * twoPi;
            float stopAngle = profileEnd * twoPi;

            angles.MakeAngles(sides, startAngle, stopAngle);

            numOuterVerts = angles.angles.Count();

            // flag to create as few triangles as possible for 3 or 4 side profile
            bool simpleFace = (sides < 5 && !hasHollow && !hasProfileCut);

            if (hasHollow)
            {
                if (sides == hollowSides)
                    hollowAngles = angles;
                else
                    hollowAngles.MakeAngles(hollowSides, startAngle, stopAngle);

                numHollowVerts = hollowAngles.angles.Count();
            }
            else if (!simpleFace)
            {
                coords.Add(center);
                us.Add(0.0f);
            }

            float z = 0.0f;

            Angle angle;
            Vector3 newVert;
            if (hasHollow && hollowSides != sides)
            {
                int numHollowAngles = hollowAngles.angles.Count();
                for (int i = 0; i < numHollowAngles; i++)
                {
                    angle = hollowAngles.angles[i];
                    newVert.X = hollow * xScale * angle.X;
                    newVert.Y = hollow * yScale * angle.Y;
                    newVert.Z = z;

                    hollowCoords.Add(newVert);
                    if (calcVertexNormals)
                    {
                        if (hollowSides == 4)
                            hollowUs.Add(angle.angle * hollow * 0.707107f);
                        else
                            hollowUs.Add(angle.angle * hollow);
                    }
                }
            }

            int index = 0;
            int numAngles = angles.angles.Count();

            for (int i = 0; i < numAngles; i++)
            {
                angle = angles.angles[i];
                newVert.X = angle.X * xScale;
                newVert.Y = angle.Y * yScale;
                newVert.Z = z;
                coords.Add(newVert);
                if (calcVertexNormals)
                {
                    outerCoordIndices.Add(coords.Count() - 1);

                    if (sides < 5)
                    {
                        float u = angle.angle;
                        us.Add(u);
                    }
                    else
                    {
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
                            hollowUs.Add(angle.angle * hollow);
                        }
                    }
                }
                else if (!simpleFace && createFaces && angle.angle > 0.0001f)
                {
                    Face newFace = new Face();
                    newFace.v1 = 0;
                    newFace.v2 = index;
                    newFace.v3 = index + 1;

                    faces.Add(newFace);
                }
                index += 1;
            }

            if (hasHollow)
            {
                List<Vector3> r = new List<Vector3>();
                for (int i = 0; i < hollowCoords.Count(); ++i)
                    r.Add(hollowCoords[hollowCoords.Count - i]);
                hollowCoords = r;
                if (calcVertexNormals)
                {

                    List<float> u = new List<float>();
                    for (int i = 0; i < hollowUs.Count(); ++i)
                        u.Add(hollowUs[hollowUs.Count - i]);
                    hollowUs = u;
                }

                if (createFaces)
                {
                    int numTotalVerts = numOuterVerts + numHollowVerts;

                    if (numOuterVerts == numHollowVerts)
                    {
                        Face newFace = new Face();
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
                            Face newFace = new Face();
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
                            Face newFace = new Face();
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
                    foreach (Vector3 v in hollowCoords)
                    {
                        coords.Add(v);
                        hollowCoordIndices.Add(coords.Count() - 1);
                    }
                }
                else
                    coords.AddRange(hollowCoords);

                if (calcVertexNormals)
                {
                    us.AddRange(hollowUs);
                }
            }

            if (simpleFace && createFaces)
            {
                if (sides == 3)
                    faces.Add(new Face(0, 1, 2));
                else if (sides == 4)
                {
                    faces.Add(new Face(0, 1, 2));
                    faces.Add(new Face(0, 2, 3));
                }
            }

            if (calcVertexNormals && hasProfileCut)
            {
                int lastOuterVertIndex = numOuterVerts - 1;

                if (hasHollow)
                {
                    cut1CoordIndices.Add(0);
                    cut1CoordIndices.Add(coords.Count() - 1);

                    cut2CoordIndices.Add(lastOuterVertIndex + 1);
                    cut2CoordIndices.Add(lastOuterVertIndex);
                }

                else
                {
                    cut1CoordIndices.Add(0);
                    cut1CoordIndices.Add(1);

                    cut2CoordIndices.Add(lastOuterVertIndex);
                    cut2CoordIndices.Add(0);

                }
            }

            MakeFaceUVs();

            hollowCoords.Clear();
            hollowUs.Clear();

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

                for (int i = 0; i < faceNumbers.Count(); i++)
                    if (faceNumbers[i] == -1)
                        faceNumbers[i] = faceNum++;

                numPrimFaces = faceNum;
            }
        }

        public void MakeFaceUVs()
        {
            faceUVs.Clear();
            foreach (Vector3 c in coords)
                faceUVs.Add(new Vector2(1.0f - (0.5f + c.X), 1.0f - (0.5f - c.Y)));

        }

        public Profile Copy()
        {
            return Copy(true);
        }

        public Profile Copy(bool needFaces)
        {
            Profile copy = new Profile();

            copy.coords.AddRange(coords);
            copy.faceUVs.AddRange(faceUVs);

            if (needFaces)
                copy.faces.AddRange(faces);
            if ((copy.calcVertexNormals = calcVertexNormals) == true)
            {
                copy.us.AddRange(us);
                copy.faceNumbers.AddRange(faceNumbers);

                copy.cut1CoordIndices.AddRange(cut1CoordIndices);
                copy.cut2CoordIndices.AddRange(cut2CoordIndices);
                copy.hollowCoordIndices.AddRange(hollowCoordIndices);
                copy.outerCoordIndices.AddRange(outerCoordIndices);
            }
            copy.numOuterVerts = numOuterVerts;
            copy.numHollowVerts = numHollowVerts;

            return copy;

        }

        public void AddPos(Vector3 v)
        {
            AddPos(v.X, v.Y, v.Z);
        }

        public void AddPos(float x, float y, float z)
        {
            int i;
            int numVerts = coords.Count();
            Vector3 vert;

            for (i = 0; i < numVerts; i++)
            {
                vert = coords[i];
                vert.X += x;
                vert.Y += y;
                vert.Z += z;
                coords[i] = vert;
            }

        }

        public void AddRot(Quaternion q)
        {
            int i;
            int numVerts = coords.Count();

            for (i = 0; i < numVerts; i++)
                coords[i] = Vector3.Transform(coords[i], q); // this is as q.RotateVector(vector) == direct rotation, oposite of opensim

        }

        public void Scale(float x, float y)
        {
            int i;
            int numVerts = coords.Count();
            Vector3 vert;

            for (i = 0; i < numVerts; i++)
            {
                vert = coords[i];
                vert.X *= x;
                vert.Y *= y;
                coords[i] = vert;
            }

        }

        public void FlipNormals()
        {
            int i;
            int numFaces = faces.Count();
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
            int numfaceUVs = faceUVs.Count();
            for (i = 0; i < numfaceUVs; i++)
            {
                Vector2 uv = faceUVs[i];
                uv.Y = 1.0f - uv.Y;
                faceUVs[i] = uv;
            }
        }

        public void AddValue2FaceVertexIndices(int num)
        {
            int numFaces = faces.Count();
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

        public void DumpRaw(String path, String name, String title)
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

    };

    class Path
    {
        public List<PathNode> pathNodes = new List<PathNode>();

        public float twistBegin = 0.0f;
        public float twistEnd = 0.0f;
        public float topShearX = 0.0f;
        public float topShearY = 0.0f;
        public float pathCutBegin = 0.0f;
        public float pathCutEnd = 1.0f;
        public float dimpleBegin = 0.0f;
        public float dimpleEnd = 1.0f;
        public float skew = 0.0f;
        public float holeSizeX = 1.0f; // called pathScaleX in pbs
        public float holeSizeY = 0.25f;
        public float taperX = 0.0f;
        public float taperY = 0.0f;
        public float radius = 0.0f;
        public float revolutions = 1.0f;
        public int stepsPerRevolution = 24;

        const float twoPi = 2.0f * (float)Math.PI;

        public void Create(PathType pathType, int steps)
        {
            if (taperX > 0.999f)
                taperX = 0.999f;
            if (taperX < -0.999f)
                taperX = -0.999f;
            if (taperY > 0.999f)
                taperY = 0.999f;
            if (taperY < -0.999f)
                taperY = -0.999f;

            if (pathType == PathType.Linear || pathType == PathType.Flexible)
            {
                int step = 0;

                float length = pathCutEnd - pathCutBegin;
                float twistTotal = twistEnd - twistBegin;
                float twistTotalAbs = Math.Abs(twistTotal);
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

                    newNode.rotation = new Quaternion(new Vector3(0.0f, 0.0f, 1.0f), twist);
                    newNode.position = new Vector3(xOffset, yOffset, zOffset);
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
                float twistTotalAbs = Math.Abs(twistTotal);
                if (twistTotalAbs > 0.01f)
                {
                    if (twistTotalAbs > Math.PI * 1.5f)
                        steps *= 2;
                    if (twistTotalAbs > Math.PI * 3.0f)
                        steps *= 2;
                }

                float yPathScale = holeSizeY * 0.5f;
                float pathLength = pathCutEnd - pathCutBegin;
                float totalSkew = skew * 2.0f * pathLength;
                float skewStart = pathCutBegin * 2.0f * skew - skew;
                float xOffsetTopShearXFactor = topShearX * (0.25f + 0.5f * (0.5f - holeSizeY));
                float yShearCompensation = 1.0f + Math.Abs(topShearY) * 0.25f;

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

                    float xProfileScale = (1.0f - Math.Abs(skew)) * holeSizeX;
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
                    xOffset += (float)Math.Sin(angle) * xOffsetTopShearXFactor;

                    float yOffset = yShearCompensation * (float)Math.Cos(angle) * (0.5f - yPathScale) * radiusScale;

                    float zOffset = (float)Math.Sin(angle + topShearY) * (0.5f - yPathScale) * radiusScale;

                    newNode.position = new Vector3(xOffset, yOffset, zOffset);

                    // now orient the rotation of the profile layer relative to it's position on the path
                    // adding taperY to the angle used to generate the quat appears to approximate the viewer

                    newNode.rotation = new Quaternion(new Vector3(1.0f, 0.0f, 0.0f), angle + topShearY);

                    // next apply twist rotation to the profile layer
                    if (twistTotal != 0.0f || twistBegin != 0.0f)
                        newNode.rotation = newNode.rotation * new Quaternion(new Vector3(0.0f, 0.0f, 1.0f), twist);

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
    };

    class PrimMesh
    {
        public String errorMessage = "";
        const float twoPi = 2.0f * (float)Math.PI;

        public List<Vector3> coords = new List<Vector3>();
        public List<Vector3> normals = new List<Vector3>();
        public List<Face> faces = new List<Face>();

        public List<ViewerFace> viewerFaces = new List<ViewerFace>();

        public int sides = 4;
        public int hollowSides = 4;
        public float profileStart = 0.0f;
        public float profileEnd = 1.0f;
        public float hollow = 0.0f;
        public int twistBegin = 0;
        public int twistEnd = 0;
        public float topShearX = 0.0f;
        public float topShearY = 0.0f;
        public float pathCutBegin = 0.0f;
        public float pathCutEnd = 1.0f;
        public float dimpleBegin = 0.0f;
        public float dimpleEnd = 1.0f;
        public float skew = 0.0f;
        public float holeSizeX = 1.0f; // called pathScaleX in pbs
        public float holeSizeY = 0.25f;
        public float taperX = 0.0f;
        public float taperY = 0.0f;
        public float radius = 0.0f;
        public float revolutions = 1.0f;
        public int stepsPerRevolution = 24;

        public int profileOuterFaceNumber = -1;
        public int profileHollowFaceNumber = -1;

        public bool hasProfileCut = false;
        public bool hasHollow = false;
        public bool calcVertexNormals = false;
        public bool normalsProcessed = false;
        public bool viewerMode = false;
        public bool sphereMode = false;

        public int numPrimFaces = 0;

        public PrimMesh(int sidesIn, float profileStartIn, float profileEndIn, float hollowIn, int hollowSidesIn)
        {
            coords.Clear();
            faces.Clear();

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

        public String ParamsToDisplayString()
        {
            String s = "";
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

        public void Extrude(PathType pathType)
        {
            bool needEndFaces = false;

            coords.Clear();
            faces.Clear();

            if (viewerMode)
            {
                viewerFaces.Clear();
                calcVertexNormals = true;
            }

            if (calcVertexNormals)
                normals.Clear();

            int steps = 1;

            float length = pathCutEnd - pathCutBegin;
            normalsProcessed = false;

            if (viewerMode && sides == 3)
            {
                // prisms don't taper well so add some vertical resolution
                // other prims may benefit from this but just do prisms for now
                if (Math.Abs(taperX) > 0.01 || Math.Abs(taperY) > 0.01)
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
            float twistTotalAbs = Math.Abs(twistTotal);
            if (twistTotalAbs > 0.01f)
                steps += (int)(twistTotalAbs * 3.66); //  dahlia's magic number

            float localhollow = hollow;

            if (pathType == PathType.Circular)
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
            if (pathType == PathType.Circular)
            {
                if (sides == 3)
                {
                    initialProfileRot = (float)Math.PI;
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
                    initialProfileRot = 0.25f * (float)Math.PI;
                    if (hollowSides != 4)
                        localhollow *= 0.707f;
                }
                else if (sides > 4)
                {
                    initialProfileRot = (float)Math.PI;
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
                    initialProfileRot = 1.25f * (float)Math.PI;
                    if (hollowSides != 4)
                        localhollow *= 0.707f;
                }
                else if (sides == 24 && hollowSides == 4)
                    localhollow *= 1.414f;
            }

            Profile profile = new Profile(sides, profileStart, profileEnd, localhollow, hollowSides, true, calcVertexNormals);
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
                cut1Vert = hasHollow ? profile.coords.Count() - 1 : 0;
                cut2Vert = hasHollow ? profile.numOuterVerts - 1 : profile.numOuterVerts;
            }

            if (initialProfileRot != 0.0f)
            {
                profile.AddRot(new Quaternion(new Vector3(0.0f, 0.0f, 1.0f), initialProfileRot));
                if (viewerMode)
                    profile.MakeFaceUVs();
            }

            float thisV = 0.0f;
            float lastV = 0.0f;

            Path path = new Path();
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

            for (int nodeIndex = 0; nodeIndex < path.pathNodes.Count(); nodeIndex++)
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
                        ViewerFace newViewerFace = new ViewerFace(profile.bottomFaceNumber);
                        int numFaces = newLayer.faces.Count();
                        List<Face> localfaces = newLayer.faces;

                        for (int i = 0; i < numFaces; i++)
                        {
                            Face face = localfaces[i];
                            newViewerFace.v1 = newLayer.coords[face.v1];
                            newViewerFace.v2 = newLayer.coords[face.v2];
                            newViewerFace.v3 = newLayer.coords[face.v3];

                            newViewerFace.coordIndex1 = face.v1;
                            newViewerFace.coordIndex2 = face.v2;
                            newViewerFace.coordIndex3 = face.v3;

                            newViewerFace.uv1 = newLayer.faceUVs[face.v1];
                            newViewerFace.uv2 = newLayer.faceUVs[face.v2];
                            newViewerFace.uv3 = newLayer.faceUVs[face.v3];

                            if (pathType == PathType.Linear)
                            {
                                FlipUV(newViewerFace.uv1);
                                FlipUV(newViewerFace.uv2);
                                FlipUV(newViewerFace.uv3);
                            }

                            viewerFaces.Add(newViewerFace);
                        }
                    }
                } // if (nodeIndex == 0)

                // append this layer

                int coordsLen = coords.Count();
                newLayer.AddValue2FaceVertexIndices(coordsLen);

                coords.AddRange(newLayer.coords);

                if (node.percentOfPath < pathCutBegin + 0.01f || node.percentOfPath > pathCutEnd - 0.01f)
                    faces.AddRange(newLayer.faces);

                // fill faces between layers

                int numVerts = newLayer.coords.Count();
                Face newFace1 = new Face();
                Face newFace2 = new Face();

                thisV = 1.0f - node.percentOfPath;

                if (nodeIndex > 0)
                {
                    int startVert = coordsLen + 1;
                    int endVert = coords.Count();

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

                        faces.Add(newFace1);

                        newFace2.v1 = iNext;
                        newFace2.v2 = i - numVerts;
                        newFace2.v3 = iNext - numVerts;

                        faces.Add(newFace2);

                        if (viewerMode)
                        {
                            // add the side faces to the list of viewerFaces here

                            int primFaceNum = profile.faceNumbers[whichVert];
                            if (!needEndFaces)
                                primFaceNum -= 1;

                            ViewerFace newViewerFace1 = new ViewerFace(primFaceNum);
                            ViewerFace newViewerFace2 = new ViewerFace(primFaceNum);

                            int uIndex = whichVert;
                            if (!hasHollow && sides > 4 && uIndex < newLayer.us.Count() - 1)
                            {
                                uIndex++;
                            }

                            float u1 = newLayer.us[uIndex];
                            float u2 = 1.0f;
                            if (uIndex < (int)newLayer.us.Count() - 1)
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

                            newViewerFace1.uv1.X = u1;
                            newViewerFace1.uv2.X = u1;
                            newViewerFace1.uv3.X = u2;

                            newViewerFace1.uv1.Y = thisV;
                            newViewerFace1.uv2.Y = lastV;
                            newViewerFace1.uv3.Y = thisV;

                            newViewerFace2.uv1.X = u2;
                            newViewerFace2.uv2.X = u1;
                            newViewerFace2.uv3.X = u2;

                            newViewerFace2.uv1.Y = thisV;
                            newViewerFace2.uv2.Y = lastV;
                            newViewerFace2.uv3.Y = lastV;

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
                            }
                            else if (whichVert == cut2Vert)
                            {
                                newViewerFace1.primFaceNumber = cut2FaceNumber;
                                newViewerFace2.primFaceNumber = cut2FaceNumber;
                            }

                            viewerFaces.Add(newViewerFace1);
                            viewerFaces.Add(newViewerFace2);

                        }
                    }
                }

                lastV = thisV;

                if (needEndFaces && nodeIndex == path.pathNodes.Count() - 1 && viewerMode)
                {
                    // add the top faces to the viewerFaces list here
                    ViewerFace newViewerFace = new ViewerFace(0);
                    int numFaces = newLayer.faces.Count();
                    List<Face> localfaces = newLayer.faces;

                    for (int i = 0; i < numFaces; i++)
                    {
                        Face face = localfaces[i];
                        newViewerFace.v1 = newLayer.coords[face.v1 - coordsLen];
                        newViewerFace.v2 = newLayer.coords[face.v2 - coordsLen];
                        newViewerFace.v3 = newLayer.coords[face.v3 - coordsLen];

                        newViewerFace.uv1 = newLayer.faceUVs[face.v1 - coordsLen];
                        newViewerFace.uv2 = newLayer.faceUVs[face.v2 - coordsLen];
                        newViewerFace.uv3 = newLayer.faceUVs[face.v3 - coordsLen];

                        if (pathType == PathType.Linear)
                        {
                            FlipUV(newViewerFace.uv1);
                            FlipUV(newViewerFace.uv2);
                            FlipUV(newViewerFace.uv3);
                        }

                        viewerFaces.Add(newViewerFace);
                    }
                }
            }

        }
        public void ExtrudeLinear() { Extrude(PathType.Linear); }
        public void ExtrudeCircular() { Extrude(PathType.Circular); }


        public Vector3 SurfaceNormal(Vector3 c1, Vector3 c2, Vector3 c3)
        {
            Vector3 edge1 = new Vector3(c2.X - c1.X, c2.Y - c1.Y, c2.Z - c1.Z);
            Vector3 edge2 = new Vector3(c3.X - c1.X, c3.Y - c1.Y, c3.Z - c1.Z);

            Vector3 normal = Vector3.Cross(edge1, edge2);

            return Vector3.Normalize(normal);

        }

        public Vector3 SurfaceNormal(Face face)
        {
            return SurfaceNormal(coords[face.v1], coords[face.v2], coords[face.v3]);

        }

        public Vector3 SurfaceNormal(int faceIndex)
        {
            int numFaces = faces.Count();
            if (faceIndex < 0 || faceIndex >= numFaces)
                throw new Exception("faceIndex out of range");

            return SurfaceNormal(faces[faceIndex]);

        }

        public PrimMesh Copy()
        {
            PrimMesh copy = new PrimMesh(sides, profileStart, profileEnd, hollow, hollowSides);
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

            copy.coords.AddRange(coords);
            copy.faces.AddRange(faces);
            copy.viewerFaces.AddRange(viewerFaces);
            copy.normals.AddRange(normals);

            return copy;

        }

        public void AddPos(float x, float y, float z)
        {
            int i;
            int numVerts = coords.Count();
            Vector3 vert;

            for (i = 0; i < numVerts; i++)
            {
                vert = coords[i];
                vert.X += x;
                vert.Y += y;
                vert.Z += z;
                coords[i] = vert;
            }

            int numViewerFaces = viewerFaces.Count();

            for (i = 0; i < numViewerFaces; i++)
            {
                ViewerFace v = viewerFaces[i];
                v.AddPos(x, y, z);
                viewerFaces[i] = v;
            }

        }

        public void AddRot(Quaternion q)
        {
            int i;
            int numVerts = coords.Count();

            for (i = 0; i < numVerts; i++)
                coords[i] = Vector3.Transform(coords[i], q);

            int numViewerFaces = viewerFaces.Count();

            for (i = 0; i < numViewerFaces; i++)
            {
                ViewerFace v = viewerFaces[i];
                v.v1 = Vector3.Transform(v.v1, q);
                v.v2 = Vector3.Transform(v.v2, q);
                v.v3 = Vector3.Transform(v.v3, q);

                viewerFaces[i] = v;
            }

        }

        public void FlipUV(Vector2 uv)
        {
            uv.X = 1.0f - uv.X;
            uv.Y = 1.0f - uv.Y;

        }

        public void Scale(float x, float y, float z)
        {
            int i;
            int numVerts = coords.Count();
            //FVector vert;

            Vector3 m = new Vector3(x, y, z);
            for (i = 0; i < numVerts; i++)
                coords[i] *= m;

            int numViewerFaces = viewerFaces.Count();
            for (i = 0; i < numViewerFaces; i++)
            {
                ViewerFace v = viewerFaces[i];
                v.v1 *= m;
                v.v2 *= m;
                v.v3 *= m;
                viewerFaces[i] = v;
            }

        }

        public void DumpRaw(String path, String name, String title)
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
    };

    class ProcMeshTangent
    {
        public Vector3 TangentX;
        public bool FlipTangentY;

        public ProcMeshTangent()
        {
            TangentX = new Vector3(1f, 0f, 0f);
            FlipTangentY = false;
        }
        public ProcMeshTangent(float X, float Y, float Z)
        {
            TangentX = new Vector3(X, Y, Z);
            FlipTangentY = false;
        }

        public ProcMeshTangent(Vector3 InTangentX, bool bInFlipTangentY)
        {
            TangentX = InTangentX;
            FlipTangentY = bInFlipTangentY;
        }
    }
    struct PrimFaceMeshData
    {
        List<Vector3> vertices;
        List<int> triangles;
        List<Vector3> normals;
        List<Vector2> uv0;
        List<Color> vertexColors;
        List<ProcMeshTangent> tangents;
    }

}
