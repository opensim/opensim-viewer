/*
 * Copyright (c) 2006-2016, openmetaverse.co
 * All rights reserved.
 *
 * - Redistribution and use in source and binary forms, with or without 
 *   modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * - Neither the name of the openmetaverse.co nor the names 
 *   of its contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */

using System;
using System.Collections;
using Xenko.Core.Mathematics;

namespace PotamOS.Controller.Scene
{
    public partial class Primitive : IEquatable<Primitive>
    {
        // Used for packing and unpacking parameters
        protected const float CUT_QUANTA = 0.00002f;
        protected const float SCALE_QUANTA = 0.01f;
        protected const float SHEAR_QUANTA = 0.01f;
        protected const float TAPER_QUANTA = 0.01f;
        protected const float REV_QUANTA = 0.015f;
        protected const float HOLLOW_QUANTA = 0.00002f;

        #region classes

        /// <summary>
        /// Parameters used to construct a visual representation of a primitive
        /// </summary>
        public struct ConstructionData
        {
            private const byte PROFILE_MASK = 0x0F;
            private const byte HOLE_MASK = 0xF0;

            /// <summary></summary>
            public byte profileCurve;
            /// <summary></summary>
            public PathCurve PathCurve;
            /// <summary></summary>
            public float PathEnd;
            /// <summary></summary>
            public float PathRadiusOffset;
            /// <summary></summary>
            public float PathSkew;
            /// <summary></summary>
            public float PathScaleX;
            /// <summary></summary>
            public float PathScaleY;
            /// <summary></summary>
            public float PathShearX;
            /// <summary></summary>
            public float PathShearY;
            /// <summary></summary>
            public float PathTaperX;
            /// <summary></summary>
            public float PathTaperY;
            /// <summary></summary>
            public float PathBegin;
            /// <summary></summary>
            public float PathTwist;
            /// <summary></summary>
            public float PathTwistBegin;
            /// <summary></summary>
            public float PathRevolutions;
            /// <summary></summary>
            public float ProfileBegin;
            /// <summary></summary>
            public float ProfileEnd;
            /// <summary></summary>
            public float ProfileHollow;

            /// <summary></summary>
            public Material Material;
            /// <summary></summary>
            public byte State;
            /// <summary></summary>
            public PCode PCode;

            #region Properties

            /// <summary>Attachment point to an avatar</summary>
            public AttachmentPoint AttachmentPoint
            {
                get { return (AttachmentPoint)Utils.SwapWords(State); }
                set { State = (byte)Utils.SwapWords((byte)value); }
            }

            /// <summary></summary>
            public ProfileCurve ProfileCurve
            {
                get { return (ProfileCurve)(profileCurve & PROFILE_MASK); }
                set
                {
                    profileCurve &= HOLE_MASK;
                    profileCurve |= (byte)value;
                }
            }

            /// <summary></summary>
            public HoleType ProfileHole
            {
                get { return (HoleType)(profileCurve & HOLE_MASK); }
                set
                {
                    profileCurve &= PROFILE_MASK;
                    profileCurve |= (byte)value;
                }
            }

            /// <summary></summary>
            public Vector2 PathBeginScale
            {
                get
                {
                    Vector2 begin = new Vector2(1f, 1f);
                    if (PathScaleX > 1f)
                        begin.X = 2f - PathScaleX;
                    if (PathScaleY > 1f)
                        begin.Y = 2f - PathScaleY;
                    return begin;
                }
            }

            /// <summary></summary>
            public Vector2 PathEndScale
            {
                get
                {
                    Vector2 end = new Vector2(1f, 1f);
                    if (PathScaleX < 1f)
                        end.X = PathScaleX;
                    if (PathScaleY < 1f)
                        end.Y = PathScaleY;
                    return end;
                }
            }

            #endregion Properties

            /// <summary>
            /// Calculdates hash code for prim construction data
            /// </summary>
            /// <returns>The has</returns>
            public override int GetHashCode()
            {
                return profileCurve.GetHashCode()
                    ^ PathCurve.GetHashCode()
                    ^ PathEnd.GetHashCode()
                    ^ PathRadiusOffset.GetHashCode()
                    ^ PathSkew.GetHashCode()
                    ^ PathScaleX.GetHashCode()
                    ^ PathScaleY.GetHashCode()
                    ^ PathShearX.GetHashCode()
                    ^ PathShearY.GetHashCode()
                    ^ PathTaperX.GetHashCode()
                    ^ PathTaperY.GetHashCode()
                    ^ PathBegin.GetHashCode()
                    ^ PathTwist.GetHashCode()
                    ^ PathTwistBegin.GetHashCode()
                    ^ PathRevolutions.GetHashCode()
                    ^ ProfileBegin.GetHashCode()
                    ^ ProfileEnd.GetHashCode()
                    ^ ProfileHollow.GetHashCode()
                    ^ Material.GetHashCode()
                    ^ State.GetHashCode()
                    ^ PCode.GetHashCode();
            }
        }

        /// <summary>
        /// Information on the flexible properties of a primitive
        /// </summary>
        public class FlexibleData
        {
            /// <summary></summary>
            public int Softness;
            /// <summary></summary>
            public float Gravity;
            /// <summary></summary>
            public float Drag;
            /// <summary></summary>
            public float Wind;
            /// <summary></summary>
            public float Tension;
            /// <summary></summary>
            public Vector3 Force;

            /// <summary>
            /// Default constructor
            /// </summary>
            public FlexibleData()
            {
            }

            /// <summary>
            /// 
            /// </summary>
            /// <param name="data"></param>
            /// <param name="pos"></param>
            public FlexibleData(byte[] data, int pos)
            {
                if (data.Length >= 5)
                {
                    Softness = ((data[pos] & 0x80) >> 6) | ((data[pos + 1] & 0x80) >> 7);

                    Tension = (float)(data[pos++] & 0x7F) / 10.0f;
                    Drag = (float)(data[pos++] & 0x7F) / 10.0f;
                    Gravity = (float)(data[pos++] / 10.0f) - 10.0f;
                    Wind = (float)data[pos++] / 10.0f;
                    Force = Utils.Vector3FromBytes(data, pos);
                }
                else
                {
                    Softness = 0;

                    Tension = 0.0f;
                    Drag = 0.0f;
                    Gravity = 0.0f;
                    Wind = 0.0f;
                    Force = Vector3.Zero;
                }
            }

            /// <summary>
            /// 
            /// </summary>
            /// <returns></returns>
            public byte[] GetBytes()
            {
                byte[] data = new byte[16];
                int i = 0;

                // Softness is packed in the upper bits of tension and drag
                data[i] = (byte)((Softness & 2) << 6);
                data[i + 1] = (byte)((Softness & 1) << 7);

                data[i++] |= (byte)((byte)(Tension * 10.01f) & 0x7F);
                data[i++] |= (byte)((byte)(Drag * 10.01f) & 0x7F);
                data[i++] = (byte)((Gravity + 10.0f) * 10.01f);
                data[i++] = (byte)(Wind * 10.01f);

                Utils.Vector3ToBytes(Force).CopyTo(data, i);

                return data;
            }

            public override int GetHashCode()
            {
                return
                    Softness.GetHashCode() ^
                    Gravity.GetHashCode() ^
                    Drag.GetHashCode() ^
                    Wind.GetHashCode() ^
                    Tension.GetHashCode() ^
                    Force.GetHashCode();
            }
        }

        /// <summary>
        /// Information on the light properties of a primitive
        /// </summary>
        public class LightData
        {
            /// <summary></summary>
            public Color4 Color;
            /// <summary></summary>
            public float Intensity;
            /// <summary></summary>
            public float Radius;
            /// <summary></summary>
            public float Cutoff;
            /// <summary></summary>
            public float Falloff;

            /// <summary>
            /// Default constructor
            /// </summary>
            public LightData()
            {
                Color = Color4.White;
            }

            /// <summary>
            /// 
            /// </summary>
            /// <param name="data"></param>
            /// <param name="pos"></param>
            public LightData(byte[] data, int pos)
            {
                if (data.Length - pos >= 16)
                {
                    Color = Utils.Color4FromBytes(data, pos, false);
                    Radius = Utils.BytesToFloat(data, pos + 4);
                    Cutoff = Utils.BytesToFloat(data, pos + 8);
                    Falloff = Utils.BytesToFloat(data, pos + 12);

                    // Alpha in color is actually intensity
                    Intensity = Color.A;
                    Color.A = 1f;
                }
                else
                {
                    Color = Color4.Black;
                    Radius = 0f;
                    Cutoff = 0f;
                    Falloff = 0f;
                    Intensity = 0f;
                }
            }

            /// <summary>
            /// 
            /// </summary>
            /// <returns></returns>
            public byte[] GetBytes()
            {
                byte[] data = new byte[16];

                // Alpha channel in color is intensity
                Color4 tmpColor = Color;
                tmpColor.A = Intensity;
                Utils.Color4ToBytes(tmpColor, false).CopyTo(data, 0);
                Utils.FloatToBytes(Radius).CopyTo(data, 4);
                Utils.FloatToBytes(Cutoff).CopyTo(data, 8);
                Utils.FloatToBytes(Falloff).CopyTo(data, 12);

                return data;
            }

            public override int GetHashCode()
            {
                return
                    Color.GetHashCode() ^
                    Intensity.GetHashCode() ^
                    Radius.GetHashCode() ^
                    Cutoff.GetHashCode() ^
                    Falloff.GetHashCode();
            }

            /// <summary>
            /// 
            /// </summary>
            /// <returns></returns>
            public override string ToString()
            {
                return String.Format("Color: {0} Intensity: {1} Radius: {2} Cutoff: {3} Falloff: {4}",
                    Color, Intensity, Radius, Cutoff, Falloff);
            }
        }

        /// <summary>
        /// Information on the light properties of a primitive as texture map
        /// </summary>
        public class LightImage
        {
            /// <summary></summary>
            public Guid LightTexture;
            /// <summary></summary>
            public Vector3 Params;

            /// <summary>
            /// Default constructor
            /// </summary>
            public LightImage()
            {
            }

            /// <summary>
            /// 
            /// </summary>
            /// <param name="data"></param>
            /// <param name="pos"></param>
            public LightImage(byte[] data, int pos)
            {
                if (data.Length - pos >= 28)
                {
                    LightTexture = Utils.GuidFromBytes(data, pos);
                    Params = Utils.Vector3FromBytes(data, pos + 16);
                }
                else
                {
                    LightTexture = Guid.Empty;
                    Params = Vector3.Zero;
                }
            }

            /// <summary>
            /// 
            /// </summary>
            /// <returns></returns>
            public byte[] GetBytes()
            {
                byte[] data = new byte[28];

                // Alpha channel in color is intensity
                LightTexture.ToByteArray().CopyTo(data, 0);
                Utils.Vector3ToBytes(Params).CopyTo(data, 16);

                return data;
            }

            public override int GetHashCode()
            {
                return LightTexture.GetHashCode() ^ Params.GetHashCode();
            }

            /// <summary>
            /// 
            /// </summary>
            /// <returns></returns>
            public override string ToString()
            {
                return String.Format("LightTexture: {0} Params; {1]", LightTexture, Params);
            }
        }

        /// <summary>
        /// Information on the sculpt properties of a sculpted primitive
        /// </summary>
        public class SculptData
        {
            public Guid SculptTexture;
            private byte type;

            public SculptType Type
            {
                get { return (SculptType)(type & 7); }
                set { type = (byte)value; }
            }

            /// <summary>
            /// Render inside out (inverts the normals).
            /// </summary>
            public bool Invert
            {
                get { return ((type & (byte)SculptType.Invert) != 0); }
            }

            /// <summary>
            /// Render an X axis mirror of the sculpty.
            /// </summary>
            public bool Mirror
            {
                get { return ((type & (byte)SculptType.Mirror) != 0); }
            }

            /// <summary>
            /// Default constructor
            /// </summary>
            public SculptData()
            {
            }

            /// <summary>
            /// 
            /// </summary>
            /// <param name="data"></param>
            /// <param name="pos"></param>
            public SculptData(byte[] data, int pos)
            {
                if (data.Length >= 17)
                {
                    SculptTexture = Utils.GuidFromBytes(data, pos);
                    type = data[pos + 16];
                }
                else
                {
                    SculptTexture = Guid.Empty;
                    type = (byte)SculptType.None;
                }
            }

            public byte[] GetBytes()
            {
                byte[] data = new byte[17];

                SculptTexture.ToByteArray().CopyTo(data, 0);
                data[16] = type;

                return data;
            }

            public override int GetHashCode()
            {
                return SculptTexture.GetHashCode() ^ type.GetHashCode();
            }
        }


        /// <summary>
        /// Describes physics attributes of the prim
        /// </summary>
        public class PhysicsProperties
        {
            /// <summary>Primitive's local ID</summary>
            public uint LocalID;
            /// <summary>Density (1000 for normal density)</summary>
            public float Density;
            /// <summary>Friction</summary>
            public float Friction;
            /// <summary>Gravity multiplier (1 for normal gravity) </summary>
            public float GravityMultiplier;
            /// <summary>Type of physics representation of this primitive in the simulator</summary>
            public PhysicsShapeType PhysicsShapeType;
            /// <summary>Restitution</summary>
            public float Restitution;

        }

        #endregion Subclasses

        #region Public Members

        public Vector4 CollisionPlane;
        /// <summary></summary>
        public FlexibleData Flexible;
        /// <summary></summary>
        public LightData Light;
        /// <summary></summary>
        public LightImage LightMap;
        /// <summary></summary>
        public SculptData Sculpt;
        public ConstructionData PrimData;
        /// <summary>Objects physics engine propertis</summary>
        public PhysicsProperties PhysicsProps;

        #endregion Public Members

        #region Properties

        /// <summary>Uses basic heuristics to estimate the primitive shape</summary>
        public PrimType Type
        {
            get
            {
                if (Sculpt != null && Sculpt.Type != SculptType.None && Sculpt.SculptTexture != Guid.Empty)
                {
                    if (Sculpt.Type == SculptType.Mesh)
                        return PrimType.Mesh;
                    else
                        return PrimType.Sculpt;
                }

                bool linearPath = (PrimData.PathCurve == PathCurve.Line || PrimData.PathCurve == PathCurve.Flexible);
                float scaleY = PrimData.PathScaleY;

                if (linearPath)
                {
                    switch (PrimData.ProfileCurve)
                    {
                        case ProfileCurve.Circle:
                            return PrimType.Cylinder;
                        case ProfileCurve.Square:
                            return PrimType.Box;
                        case ProfileCurve.IsoTriangle:
                        case ProfileCurve.EqualTriangle:
                        case ProfileCurve.RightTriangle:
                            return PrimType.Prism;
                        case ProfileCurve.HalfCircle:
                        default:
                            return PrimType.Unknown;
                    }
                }
                else
                {
                    switch (PrimData.PathCurve)
                    {
                        case PathCurve.Flexible:
                            return PrimType.Unknown;
                        case PathCurve.Circle:
                            switch (PrimData.ProfileCurve)
                            {
                                case ProfileCurve.Circle:
                                    if (scaleY > 0.75f)
                                        return PrimType.Sphere;
                                    else
                                        return PrimType.Torus;
                                case ProfileCurve.HalfCircle:
                                    return PrimType.Sphere;
                                case ProfileCurve.EqualTriangle:
                                    return PrimType.Ring;
                                case ProfileCurve.Square:
                                    if (scaleY <= 0.75f)
                                        return PrimType.Tube;
                                    else
                                        return PrimType.Unknown;
                                default:
                                    return PrimType.Unknown;
                            }
                        case PathCurve.Circle2:
                            if (PrimData.ProfileCurve == ProfileCurve.Circle)
                                return PrimType.Sphere;
                            else
                                return PrimType.Unknown;
                        default:
                            return PrimType.Unknown;
                    }
                }
            }
        }

        #endregion Properties

        #region Constructors

        /// <summary>
        /// Default constructor
        /// </summary>
        public Primitive()
        {
            Flexible = new FlexibleData();
            Light = new LightData();
            LightMap = new LightImage();
            Sculpt = new SculptData();
            PrimData = new ConstructionData();
            PhysicsProps = new PhysicsProperties();
        }

        public Primitive(Primitive prim)
        {
            CollisionPlane = prim.CollisionPlane;
            Flexible = prim.Flexible;
            Light = prim.Light;
            LightMap = prim.LightMap;
            Sculpt = prim.Sculpt;
            PrimData = prim.PrimData;
            // FIXME: Get a real copy constructor for TextureEntry instead of serializing to bytes and back
            if (prim.Textures != null)
            {
                byte[] textureBytes = prim.Textures.GetBytes();
                Textures = new TextureEntry(textureBytes, 0, textureBytes.Length);
            }
            else
            {
                Textures = null;
            }
            TextureAnim = prim.TextureAnim;
            //ParticleSys = prim.ParticleSys;
        }

        #endregion Constructors

        #region Public Methods


        public int SetExtraParamsFromBytes(byte[] data, int pos)
        {
            int i = pos;
            int totalLength = 1;

            if (data.Length == 0 || pos >= data.Length)
                return 0;

            byte extraParamCount = data[i++];

            for (int k = 0; k < extraParamCount; k++)
            {
                ExtraParamType type = (ExtraParamType)Utils.BytesToUInt16(data, i);
                i += 2;

                uint paramLength = Utils.BytesToUInt(data, i);
                i += 4;

                if (type == ExtraParamType.Flexible)
                    Flexible = new FlexibleData(data, i);
                else if (type == ExtraParamType.Light)
                    Light = new LightData(data, i);
                else if (type == ExtraParamType.LightImage)
                    LightMap = new LightImage(data, i);
                else if (type == ExtraParamType.Sculpt || type == ExtraParamType.Mesh)
                    Sculpt = new SculptData(data, i);

                i += (int)paramLength;
                totalLength += (int)paramLength + 6;
            }

            return totalLength;
        }

        public byte[] GetExtraParamsBytes()
        {
            byte[] flexible = null;
            byte[] light = null;
            byte[] lightmap = null;
            byte[] sculpt = null;
            byte[] buffer = null;
            int size = 1;
            int pos = 0;
            byte count = 0;

            if (Flexible != null)
            {
                flexible = Flexible.GetBytes();
                size += flexible.Length + 6;
                ++count;
            }
            if (Light != null)
            {
                light = Light.GetBytes();
                size += light.Length + 6;
                ++count;
            }
            if (LightMap != null)
            {
                lightmap = LightMap.GetBytes();
                size += lightmap.Length + 6;
                ++count;
            }
            if (Sculpt != null)
            {
                sculpt = Sculpt.GetBytes();
                size += sculpt.Length + 6;
                ++count;
            }

            buffer = new byte[size];
            buffer[0] = count;
            ++pos;

            if (flexible != null)
            {
                Buffer.BlockCopy(Utils.UInt16ToBytes((ushort)ExtraParamType.Flexible), 0, buffer, pos, 2);
                pos += 2;

                Buffer.BlockCopy(Utils.UIntToBytes((uint)flexible.Length), 0, buffer, pos, 4);
                pos += 4;

                Buffer.BlockCopy(flexible, 0, buffer, pos, flexible.Length);
                pos += flexible.Length;
            }
            if (light != null)
            {
                Buffer.BlockCopy(Utils.UInt16ToBytes((ushort)ExtraParamType.Light), 0, buffer, pos, 2);
                pos += 2;

                Buffer.BlockCopy(Utils.UIntToBytes((uint)light.Length), 0, buffer, pos, 4);
                pos += 4;

                Buffer.BlockCopy(light, 0, buffer, pos, light.Length);
                pos += light.Length;
            }
            if (lightmap != null)
            {
                Buffer.BlockCopy(Utils.UInt16ToBytes((ushort)ExtraParamType.LightImage), 0, buffer, pos, 2);
                pos += 2;

                Buffer.BlockCopy(Utils.UIntToBytes((uint)lightmap.Length), 0, buffer, pos, 4);
                pos += 4;

                Buffer.BlockCopy(lightmap, 0, buffer, pos, lightmap.Length);
                pos += lightmap.Length;
            }
            if (sculpt != null)
            {
                if (Sculpt.Type == SculptType.Mesh)
                {
                    Buffer.BlockCopy(Utils.UInt16ToBytes((ushort)ExtraParamType.Mesh), 0, buffer, pos, 2);
                }
                else
                {
                    Buffer.BlockCopy(Utils.UInt16ToBytes((ushort)ExtraParamType.Sculpt), 0, buffer, pos, 2);
                }
                pos += 2;

                Buffer.BlockCopy(Utils.UIntToBytes((uint)sculpt.Length), 0, buffer, pos, 4);
                pos += 4;

                Buffer.BlockCopy(sculpt, 0, buffer, pos, sculpt.Length);
                pos += sculpt.Length;
            }

            return buffer;
        }

        #endregion Public Methods

        #region Overrides

        public override bool Equals(object obj)
        {
            return (obj is Primitive) ? this == (Primitive)obj : false;
        }

        public bool Equals(Primitive other)
        {
            return this == other;
        }

        public override string ToString()
        {
            switch (PrimData.PCode)
            {
                case PCode.Prim:
                    return String.Format("{0}", Type);
                default:
                    return String.Format("{0}", PrimData.PCode);
            }
        }

        public override int GetHashCode()
        {
            return
                (Flexible != null ? Flexible.GetHashCode() : 0) ^
                (Light != null ? Light.GetHashCode() : 0) ^
                (Sculpt != null ? Sculpt.GetHashCode() : 0) ^
                PrimData.Material.GetHashCode() ^
                //TODO: NameValues?
                PrimData.PathBegin.GetHashCode() ^
                PrimData.PathCurve.GetHashCode() ^
                PrimData.PathEnd.GetHashCode() ^
                PrimData.PathRadiusOffset.GetHashCode() ^
                PrimData.PathRevolutions.GetHashCode() ^
                PrimData.PathScaleX.GetHashCode() ^
                PrimData.PathScaleY.GetHashCode() ^
                PrimData.PathShearX.GetHashCode() ^
                PrimData.PathShearY.GetHashCode() ^
                PrimData.PathSkew.GetHashCode() ^
                PrimData.PathTaperX.GetHashCode() ^
                PrimData.PathTaperY.GetHashCode() ^
                PrimData.PathTwist.GetHashCode() ^
                PrimData.PathTwistBegin.GetHashCode() ^
                PrimData.PCode.GetHashCode() ^
                PrimData.ProfileBegin.GetHashCode() ^
                PrimData.ProfileCurve.GetHashCode() ^
                PrimData.ProfileEnd.GetHashCode() ^
                PrimData.ProfileHollow.GetHashCode() ^
                TextureAnim.GetHashCode() ^
                (Textures != null ? Textures.GetHashCode() : 0) ^
                PrimData.State.GetHashCode();
        }

        #endregion Overrides

        #region Operators


        #endregion Operators

        #region Parameter Packing Methods

        public static ushort PackBeginCut(float beginCut)
        {
            return (ushort)Math.Round(beginCut / CUT_QUANTA);
        }

        public static ushort PackEndCut(float endCut)
        {
            return (ushort)(50000 - (ushort)Math.Round(endCut / CUT_QUANTA));
        }

        public static byte PackPathScale(float pathScale)
        {
            return (byte)(200 - (byte)Math.Round(pathScale / SCALE_QUANTA));
        }

        public static sbyte PackPathShear(float pathShear)
        {
            return (sbyte)Math.Round(pathShear / SHEAR_QUANTA);
        }

        /// <summary>
        /// Packs PathTwist, PathTwistBegin, PathRadiusOffset, and PathSkew
        /// parameters in to signed eight bit values
        /// </summary>
        /// <param name="pathTwist">Floating point parameter to pack</param>
        /// <returns>Signed eight bit value containing the packed parameter</returns>
        public static sbyte PackPathTwist(float pathTwist)
        {
            return (sbyte)Math.Round(pathTwist / SCALE_QUANTA);
        }

        public static sbyte PackPathTaper(float pathTaper)
        {
            return (sbyte)Math.Round(pathTaper / TAPER_QUANTA);
        }

        public static byte PackPathRevolutions(float pathRevolutions)
        {
            return (byte)Math.Round((pathRevolutions - 1f) / REV_QUANTA);
        }

        public static ushort PackProfileHollow(float profileHollow)
        {
            return (ushort)Math.Round(profileHollow / HOLLOW_QUANTA);
        }

        #endregion Parameter Packing Methods

        #region Parameter Unpacking Methods

        public static float UnpackBeginCut(ushort beginCut)
        {
            return (float)beginCut * CUT_QUANTA;
        }

        public static float UnpackEndCut(ushort endCut)
        {
            return (float)(50000 - endCut) * CUT_QUANTA;
        }

        public static float UnpackPathScale(byte pathScale)
        {
            return (float)(200 - pathScale) * SCALE_QUANTA;
        }

        public static float UnpackPathShear(sbyte pathShear)
        {
            return (float)pathShear * SHEAR_QUANTA;
        }

        /// <summary>
        /// Unpacks PathTwist, PathTwistBegin, PathRadiusOffset, and PathSkew
        /// parameters from signed eight bit integers to floating point values
        /// </summary>
        /// <param name="pathTwist">Signed eight bit value to unpack</param>
        /// <returns>Unpacked floating point value</returns>
        public static float UnpackPathTwist(sbyte pathTwist)
        {
            return (float)pathTwist * SCALE_QUANTA;
        }

        public static float UnpackPathTaper(sbyte pathTaper)
        {
            return (float)pathTaper * TAPER_QUANTA;
        }

        public static float UnpackPathRevolutions(byte pathRevolutions)
        {
            return (float)pathRevolutions * REV_QUANTA + 1f;
        }

        public static float UnpackProfileHollow(ushort profileHollow)
        {
            return (float)profileHollow * HOLLOW_QUANTA;
        }

        #endregion Parameter Unpacking Methods
    }
}
