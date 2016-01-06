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
    Coord northPole = matrix[0][matrixWidth / 2];
    
    // The south pole is taken from the "extra" row (33) data
    // unless the source texture is 32 pixels tall. In that case
    // it is taken from the center of the last valid row. The
    // results may be undefined.
    Coord southPole = matrix[matrixHeight - 1][matrixWidth / 2];
    
    if (sculptType == sphere)
    {
        // Spherical sculpts get all vertices in the 1st and 33rd
        // row stitched to a common "pole" taken from the center
        // of the top and the last valid input row.
        // The coordinates in the top row get overwritten by this
        // operation.
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
                uvs.Add(UVCoord(divisionU * imageX, divisionV * imageY));
            }
            
            if (imageY > 0 && imageX > 0)
            {
                Face f1, f2;
                
                if (viewerMode)
                {
                    if (invert)
                    {
                        f1 = Face(p1, p4, p3, p1, p4, p3);
                        f1.uv1 = p1;
                        f1.uv2 = p4;
                        f1.uv3 = p3;
                        
                        f2 = Face(p1, p2, p4, p1, p2, p4);
                        f2.uv1 = p1;
                        f2.uv2 = p2;
                        f2.uv3 = p4;
                    }
                    else
                    {
                        f1 = Face(p1, p3, p4, p1, p3, p4);
                        f1.uv1 = p1;
                        f1.uv2 = p3;
                        f1.uv3 = p4;
                        
                        f2 = Face(p1, p4, p2, p1, p4, p2);
                        f2.uv1 = p1;
                        f2.uv2 = p4;
                        f2.uv3 = p2;
                    }
                }
                else
                {
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
                }
                
                faces.Add(f1);
                faces.Add(f2);
            }
        }
    }
    
    if (viewerMode)
        calcVertexNormals(sculptType, matrixWidth, matrixHeight);
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
    
    int numNormals = normals.Num();
    for (int i = 0; i < numNormals; i++)
        normals[i] = normals[i].Normalize();
    
    if (sculptType != plane)
    { // blend the vertex normals at the cylinder seam
        for (int y = 0; y < ySize; y++)
        {
            int rowOffset = y * xSize;
            
            normals[rowOffset] = normals[rowOffset + xSize - 1] = (normals[rowOffset] + normals[rowOffset + xSize - 1]).Normalize();
        }
    }
    
    for (auto it = faces.CreateConstIterator() ; it ; ++it)
    {
        Face face = (*it);
        
        ViewerFace vf(0);
        vf.v1 = coords[face.v1];
        vf.v2 = coords[face.v2];
        vf.v3 = coords[face.v3];
        
        vf.coordIndex1 = face.v1;
        vf.coordIndex2 = face.v2;
        vf.coordIndex3 = face.v3;
        
        vf.n1 = normals[face.n1];
        vf.n2 = normals[face.n2];
        vf.n3 = normals[face.n3];
        
        vf.uv1 = uvs[face.uv1];
        vf.uv2 = uvs[face.uv2];
        vf.uv3 = uvs[face.uv3];
        
        viewerFaces.Add(vf);
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
