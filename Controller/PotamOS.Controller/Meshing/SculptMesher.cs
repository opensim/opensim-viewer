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

namespace PotamOS.Meshing
{
    enum SculptType { sphere = 1, torus = 2, plane = 3, cylinder = 4 };

    class SculptMesh
    {
        public List<Vector3> coords = new List<Vector3>();
        public List<Face> faces = new List<Face>();
        public List<Vector3> normals = new List<Vector3>();
        public List<Vector2> uvs = new List<Vector2>();

        public SculptMesh(List<List<Vector3>> rows, SculptType sculptType, bool viewerMode, int lod)
        {
            uint vertsWanted = 1024;
            switch ((LevelDetail)lod)
            {
                case LevelDetail.Highest:
                    vertsWanted = 1024;
                    break;
                case LevelDetail.High:
                    vertsWanted = 1024;
                    break;
                case LevelDetail.Low:
                    vertsWanted = 256;
                    break;
                default:
                    vertsWanted = 32;
                    break;
            }

            // It is defined that the sculpt texture shall be no smaller than 64 pixels
            // and that the behavior with a 32 x 32 texture is
            uint height = (uint)rows.Count();
            if (height < 2)
                return;

            uint width = (uint)rows[0].Count();
            if (width < 2)
                return;

            // Sculpt textures must be powers of two.
            if (!((height & (~height + 1)) == height) ||
                !((width & (~width + 1)) == width))
                return;

            // Sculpties are limited to 1024 vertices. If the count is exceeded, the texture
            // sample rate will be reduced until the vertex count fits.
            ushort step = 1;

            uint verts = width * height;

            while (verts > vertsWanted)
            {
                step <<= 1;

                // In the below cases, this LOD has no geometry (doesn't show)
                if (step >= width)
                    return;
                if (step >= height)
                    return;

                verts >>= 2;
            }

            sculptType = (SculptType)(((int)sculptType) & 0x07);

            ushort horizontalDivisions = (ushort)(width / step);
            ushort verticalDivisions = (ushort)(height / step);
            ushort matrixWidth = (ushort)(horizontalDivisions + 1);
            ushort matrixHeight = (ushort)(verticalDivisions + 1);

            float divisionU = 1.0f / (float)horizontalDivisions;
            float divisionV = 1.0f / (float)verticalDivisions;

            // Reduce the input texture to our coordinate matrix. This greatly simplifies
            // what is to come.
            Vector3[,] matrix = new Vector3[matrixHeight, matrixWidth];

            int v, vv;
            int h, hh;

            int oneMinushalfStep = -step / 2;
            oneMinushalfStep += step;

            for (v = 0, vv = 0; v < (int)height; ++vv, v += step)
            {
                for (h = 0, hh = 0; h < (int)width; ++hh, h += step)
                {
                    matrix[vv, hh] = rows[v][h];
                }
                if (step > 1)
                {
                    if (step > 2)
                        matrix[vv, hh] = rows[v][h - oneMinushalfStep];
                    else
                        matrix[vv, hh] = rows[v][h - 1];
                }
                else
                {
                    // This WILL cause texture mapping errors but we have no
                    // closer approximation to the data needed here.
                    matrix[vv, hh] = matrix[vv, hh - 1];
                }
            }

            for (h = 0, hh = 0; h < (int)width; ++hh, h += step)
            {
                if (step > 1)
                {
                    if (step > 2)
                        matrix[vv, hh] = rows[v - oneMinushalfStep][h];
                    else
                        matrix[vv, hh] = rows[v - 1][h];
                }
                else
                {
                    matrix[vv, hh] = matrix[vv - 1, hh];
                }
            }

            if (hh == matrixWidth - 1 && h == width && step > 1)
            {
                if (step > 2)
                    matrix[vv, hh] = rows[v - oneMinushalfStep][h - 1];
                else
                    matrix[vv, hh] = rows[v - 1][h - 1];
            }

            if (sculptType == SculptType.sphere)
            {
                // Find the poles for spherical sitching

                // The south pole is taken from the "extra" row (33) data
                // unless the source texture is 32 pixels tall. In that case
                // it is taken from the center of the last valid row. The
                // results may be undefined.

                Vector3 northPole = matrix[0, matrixWidth / 2];
                Vector3 southPole = matrix[matrixHeight - 1, matrixWidth / 2];

                // Spherical sculpts get all vertices in the 1st and 33rd
                // row stitched to a common "pole" taken from the center
                // of the top and the last valid input row.
                // The coordinates in the top row get overwritten by this
                // operation.

                for (int h1 = 0; h1 < matrixWidth; ++h1)
                {
                    matrix[0, h1] = northPole;
                    matrix[matrixHeight - 1, h1] = southPole;
                }
            }

            // For these two, stitch sides left to right
            if (sculptType == SculptType.sphere || sculptType == SculptType.cylinder || sculptType == SculptType.torus)
            {
                for (int v1 = 0; v1 < matrixHeight; ++v1)
                    matrix[v1, matrixWidth - 1] = matrix[v1, 0];
            }

            // For this one, stitch top to bottom as well
            if (sculptType == SculptType.torus)
            {
                for (int h1 = 0; h1 < matrixWidth; ++h1)
                    matrix[matrixHeight - 1, h1] = matrix[0, h1];
            }

            coords.Clear();
            normals.Clear();
            faces.Clear();

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

                    coords.Add(matrix[imageY, imageX]);

                    if (viewerMode)
                    {
                        normals.Add(new Vector3());
                        uvs.Add(new Vector2(divisionU * imageX, divisionV * imageY));
                    }

                    if (imageY > 0 && imageX > 0)
                    {
                        Face f;
                        f = new Face(p1, p3, p4);
                        faces.Add(f);
                        f = new Face(p1, p4, p2);
                        faces.Add(f);
                    }
                }
            }

            if (viewerMode)
            {
                calcVertexNormals(sculptType, matrixWidth, matrixHeight);
            }

        }

        private void calcVertexNormals(SculptType sculptType, int xSize, int ySize)
        {
            normals.Clear();
            for (int i = 0; i < coords.Count(); i++)
                normals[i] = new Vector3();

            int numFaces = faces.Count();
            for (int i = 0; i < numFaces; i++)
            {
                Face face = faces[i];
                Vector3 surfaceNormal = face.SurfaceNormal(coords);
                normals[face.v1] += surfaceNormal;
                normals[face.v2] += surfaceNormal;
                normals[face.v3] += surfaceNormal;
            }

            if (sculptType == SculptType.sphere)
            {
                Vector3 avg = new Vector3(0, 0, 0);
                for (int i = 0; i < xSize; i++)
                {
                    avg += normals[i];
                }
                for (int i = 0; i < xSize; i++)
                {
                    normals[i] = avg;
                }
                avg.X = 0;
                avg.Y = 0;
                avg.Z = 0;
                int lastrow = xSize * (ySize - 1);
                for (int i = lastrow; i < lastrow + xSize; i++)
                {
                    avg += normals[i];
                }
                for (int i = lastrow; i < lastrow + xSize; i++)
                {
                    normals[i] = avg;
                }
            }

            if (sculptType == SculptType.sphere || sculptType == SculptType.cylinder || sculptType == SculptType.torus)
            { // blend the vertex normals at the cylinder seam
                int xminusOne = xSize - 1;
                for (int y = 0; y < ySize; y++)
                {
                    int rowOffset = y * xSize;
                    normals[rowOffset] = normals[rowOffset + xminusOne] = (normals[rowOffset] + normals[rowOffset + xminusOne]);
                }
            }

            if (sculptType == SculptType.torus)
            {
                int lastrow = xSize * (ySize - 1);
                for (int x = 0; x < xSize; x++)
                {
                    normals[x] = normals[lastrow + x] = (normals[x] + normals[lastrow + x]);
                }
            }

            int numNormals = normals.Count();
            for (int i = 0; i < numNormals; i++)
                normals[i] = Vector3.Normalize(normals[i]);

        }
    }
}
