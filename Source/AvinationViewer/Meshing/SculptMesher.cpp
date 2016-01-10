// Fill out your copyright notice in the Description page of Project Settings.

#include "AvinationViewer.h"
#include "SculptMesher.h"

SculptMesh::SculptMesh(TArray<TArray<Coord>> rows, SculptType sculptType, bool viewerMode, bool mirror, bool invert, int lod)
{
    _SculptMesh(rows, sculptType, viewerMode, mirror, invert, lod);
}

void SculptMesh::_SculptMesh(TArray<TArray<Coord>> rows, SculptType sculptType, bool viewerMode, bool mirror, bool invert, int lod)
{
    uint32_t vertsWanted = 1024;
    switch ((LevelDetail)lod)
    {
    case Highest:
        vertsWanted = 1024;
        break;
    case High:
        vertsWanted = 512;
        break;
    case Low:
        vertsWanted = 128;
        break;
    default:
        vertsWanted = 32;
        break;
    }

    // It is defined that the sculpt texture shall be no smaller than 64 pixels
    // and that the behavior with a 32 x 32 texture is
    uint32_t height = rows.Num();
    if (height < 2)
        return;

    uint32_t width = rows[0].Num();
    if (width < 2)
        return;

    // Sculpt textures must be powers of two.
    if (!((height & (~height + 1)) == height) ||
        !((width & (~width + 1)) == width))
        return;

    // Sculpties are limited to 1024 vertices. If the count is exceeded, the texture
    // sample rate will be reduced until the vertex count fits.
    uint16_t horizontalStepWidth = 1;
    uint16_t verticalStepHeight = 1;

    uint32_t verts = width * height;

    while (verts > vertsWanted)
    {
        horizontalStepWidth *= 2;
        verticalStepHeight *= 2;

        // In the below cases, this LOD has no geometry (doesn't show)
        if (horizontalStepWidth >= width)
            return;
        if (verticalStepHeight >= height)
            return;

        verts = (width / horizontalStepWidth) * (height / verticalStepHeight);
    }
    
    sculptType = (SculptType)(((int)sculptType) & 0x07);

    if (mirror)
        invert = !invert;
    
    uint16_t horizontalDivisions = width / horizontalStepWidth;
    uint16_t verticalDivisions = height / verticalStepHeight;
    uint16_t matrixWidth = horizontalDivisions + 1;
    uint16_t matrixHeight = verticalDivisions + 1;
    
    float divisionU = 1.0f / (float)horizontalDivisions;
    float divisionV = 1.0f / (float)verticalDivisions;

    // Reduce the input texture to our coordinate matrix. This greatly simplifies
    // what is to come.
    TArray<TArray<Coord>> matrix;
    
    int v, vv;
    int h, hh;

    matrix.AddDefaulted(matrixHeight);
    
    for (v = 0, vv = 0 ; v < (int)height ; ++vv, v += verticalStepHeight)
    {
        matrix[vv].AddDefaulted(matrixWidth);
        
        for (h = 0, hh = 0 ; h < (int)width ; ++hh, h += horizontalStepWidth)
        {
            matrix[vv][hh] = rows[v][h];
        }
        if (horizontalStepWidth > 1)
        {
            if (horizontalStepWidth > 2)
                matrix[vv][hh] = rows[v][h - horizontalStepWidth + horizontalStepWidth / 2];
            else
                matrix[vv][hh] = rows[v][h - 1];
        }
        else
        {
            // This WILL cause texture mapping errors but we have no
            // closer approximation to the data needed here.
            matrix[vv][hh] = matrix[vv][hh - 1];
        }
    }
    
    matrix[vv].AddDefaulted(matrixWidth);
    
    for (h = 0, hh = 0 ; h < (int)width ; ++hh, h += horizontalStepWidth)
    {
        if (verticalStepHeight > 1)
        {
            if (verticalStepHeight > 2)
                matrix[vv][hh] = rows[v - verticalStepHeight + verticalStepHeight / 2][h];
            else
                matrix[vv][hh] = rows[v - 1][h];
        }
        else
        {
            matrix[vv][hh] = matrix[vv - 1][hh];
        }
    }
    
    // Find the poles for spherical sitching
    
    // The south pole is taken from the "extra" row (33) data
    // unless the source texture is 32 pixels tall. In that case
    // it is taken from the center of the last valid row. The
    // results may be undefined.
    
    
    if (sculptType == sphere)
    {
        // Spherical sculpts get all vertices in the 1st and 33rd
        // row stitched to a common "pole" taken from the center
        // of the top and the last valid input row.
        // The coordinates in the top row get overwritten by this
        // operation.

        Coord northPole = matrix[0][matrixWidth / 2];
        Coord southPole = matrix[matrixHeight - 1][matrixWidth / 2];

        for (int h1 = 0 ; h1 < matrixWidth ; ++h1)
        {
            matrix[0][h1] = northPole;
            matrix[matrixHeight - 1][h1] = southPole;
        }
    }
    
    // Plane mode is handled by the default case.
    
    // For these two, stitch sides left to right
    if (sculptType == sphere || sculptType == cylinder || sculptType == torus)
    {
        for (int v1 = 0 ; v1 < matrixHeight ; ++v1)
            matrix[v1][matrixWidth - 1] = matrix[v1][0];
    }
    
    // For this one, stitch top to bottom as well
    if (sculptType == torus)
    {
        for (int h1 = 0 ; h1 < matrixWidth ; ++h1)
            matrix[matrixHeight - 1][h1] = matrix[0][h1];
    }

    int p1, p2, p3, p4;
    for (int imageY = 0; imageY < matrixHeight; imageY++)
    {
        int rowOffset = imageY * matrixWidth;
        
        for (int imageX = 0; imageX < matrixWidth; imageX++)
        {
            //*
            //*   p1-----p2
            //*   | \ f2 |
            //*   |   \  |
            //*   | f1  \|
            //*   p3-----p4
            //*
            
            p4 = rowOffset + imageX;
            p3 = p4 - 1;
            
            p2 = p4 - matrixWidth;
            p1 = p3 - matrixWidth;
            
            coords.Add(matrix[imageY][imageX]);
            if (viewerMode)
            {
                normals.Add(Coord());
                tangents.Add(Coord());
                uvs.Add(UVCoord(divisionU * imageX, divisionV * imageY));
            }
            
            if (imageY > 0 && imageX > 0)
            {
                Face f1, f2;
                if (invert)
                {
                    f1 = Face(p1, p4, p3);
                    f2 = Face(p1, p2, p4);
                }
                else
                {
                    f1 = Face(p1, p3, p4);
                    f2 = Face(p1, p4, p2);
                }
                faces.Add(f1);
                faces.Add(f2);
            }
        }
    }
    
    if (viewerMode)
    {
        calcVertexNormals(sculptType, matrixWidth, matrixHeight);
        CalcTangents();
    }
}

void SculptMesh::calcVertexNormals(SculptType sculptType, int xSize, int ySize)
{
    int numFaces = faces.Num();
    for (int i = 0; i < numFaces; i++)
    {
        Face face = faces[i];
        Coord surfaceNormal = face.SurfaceNormal(coords);
        normals[face.n1] += surfaceNormal;
        normals[face.n2] += surfaceNormal;
        normals[face.n3] += surfaceNormal;
    }

    if (sculptType == sphere)
    {
        Coord avg(0,0,0);
        for (int i = 0; i < xSize; i++)
        {
            avg.X += normals[i].X;
            avg.Y += normals[i].Y;
            avg.Z += normals[i].Z;
        }
        for (int i = 0; i < xSize; i++)
        {
            normals[i].X = avg.X;
            normals[i].Y = avg.Y;
            normals[i].Z = avg.Z;
        }
        avg.X = 0;
        avg.Y = 0;
        avg.Z = 0;
        int lastrow = xSize * (ySize - 1);
        for (int i = lastrow; i < lastrow + xSize; i++)
        {
            avg.X += normals[i].X;
            avg.Y += normals[i].Y;
            avg.Z += normals[i].Z;
        }
        for (int i = lastrow; i < lastrow + xSize; i++)
        {
            normals[i].X = avg.X;
            normals[i].Y = avg.Y;
            normals[i].Z = avg.Z;
        }
    }
    
    if (sculptType == sphere || sculptType == cylinder || sculptType == torus)
    { // blend the vertex normals at the cylinder seam
        int xminusOne = xSize - 1;
        for (int y = 0; y < ySize; y++)
        {
            int rowOffset = y * xSize;

            normals[rowOffset] = normals[rowOffset + xminusOne] = (normals[rowOffset] + normals[rowOffset + xminusOne]);
        }
    }

    if (sculptType == torus)
    {
        int lastrow = xSize * (ySize - 1);
        for (int x = 0; x < xSize; x++)
        {
            normals[x] = normals[lastrow + x] = (normals[x] + normals[lastrow + x]);
        }
    }

    int numNormals = normals.Num();
    for (int i = 0; i < numNormals; i++)
        normals[i] = normals[i].Normalize();
}

void SculptMesh::CalcTangents()
{
    int numVerts = coords.Num();
    TArray<Coord> tan1;
    TArray<Coord> tan2;
    tan1.AddZeroed(numVerts);
    tan2.AddZeroed(numVerts);

    int numFaces = faces.Num();

    for (int a = 0; a < numFaces; a++)
    {
        int i1 = faces[a].v1;
        int i2 = faces[a].v2;
        int i3 = faces[a].v3;

        const Coord v1 = coords[i1];
        const Coord v2 = coords[i2];
        const Coord v3 = coords[i3];

        const UVCoord w1 = uvs[i1];
        const UVCoord w2 = uvs[i2];
        const UVCoord w3 = uvs[i3];

        float x1 = v2.X - v1.X;
        float x2 = v3.X - v1.X;
        float y1 = v2.Y - v1.Y;
        float y2 = v3.Y - v1.Y;
        float z1 = v2.Y - v1.Z;
        float z2 = v3.Z - v1.Z;

        float s1 = w2.U - w1.U;
        float s2 = w3.U - w1.U;
        float t1 = w2.V - w1.V;
        float t2 = w3.V - w1.V;

        float r = 1.0F / (s1 * t2 - s2 * t1);
        Coord sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
            (t2 * z1 - t1 * z2) * r);
        Coord tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
            (s1 * z2 - s2 * z1) * r);

        tan1[i1] += sdir;
        tan1[i2] += sdir;
        tan1[i3] += sdir;

        tan2[i1] += tdir;
        tan2[i2] += tdir;
        tan2[i3] += tdir;
    }

    for (int a = 0; a < numVerts; a++)
    {
        Coord n = normals[a];
        Coord t = tan1[a];

        float dotnt = n.X * t.X + n.Y * t.Y + n.Z * t.Z;
        Coord crossnt = Coord::Cross(n, t);

        n.X *= dotnt;
        n.Y *= dotnt;
        n.Z *= dotnt;

        Coord tsubn;
        tsubn.X = t.X - n.X;
        tsubn.Y = t.Y - n.Y;
        tsubn.Z = t.Z - n.Z;

        // Gram-Schmidt orthogonalize
        tangents[a] = tsubn.Normalize();

        float dotCrossT2 = crossnt.X * tan2[a].X + crossnt.Y * tan2[a].Y + crossnt.Z * tan2[a].Z;
        // Calculate handedness
        float w = (dotCrossT2 < 0.0F) ? -1.0F : 1.0F;
    }
}

void SculptMesh::AddPos(float x, float y, float z)
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

void SculptMesh::AddRot(PrimQuat q)
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

void SculptMesh::Scale(float x, float y, float z)
{
    int i;
    int numVerts = coords.Num();
    
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
