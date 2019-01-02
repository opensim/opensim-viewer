using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Reflection;
using System.Xml;
using log4net;
using Xenko.Core.Mathematics;

namespace PotamOS.Controller.Scene
{
    public class SOPSerializer
    {
        private static readonly ILog m_log = LogManager.GetLogger(
                                             MethodBase.GetCurrentMethod().DeclaringType);

        private static Dictionary<string, Action<SceneObjectPart, XmlReader>> m_SOPXmlProcessors
            = new Dictionary<string, Action<SceneObjectPart, XmlReader>>();

        private static Dictionary<string, Action<Primitive, XmlReader>> m_ShapeXmlProcessors
            = new Dictionary<string, Action<Primitive, XmlReader>>();

        static SOPSerializer()
        {
            #region SOPXmlProcessors initialization
            m_SOPXmlProcessors.Add("UUID", ProcessUUID);
            m_SOPXmlProcessors.Add("Name", ProcessName);
            m_SOPXmlProcessors.Add("Material", ProcessMaterial);
            m_SOPXmlProcessors.Add("GroupPosition", ProcessGroupPosition);
            m_SOPXmlProcessors.Add("OffsetPosition", ProcessOffsetPosition);
            m_SOPXmlProcessors.Add("RotationOffset", ProcessRotationOffset);
            m_SOPXmlProcessors.Add("Velocity", ProcessVelocity);
            m_SOPXmlProcessors.Add("AngularVelocity", ProcessAngularVelocity);
            m_SOPXmlProcessors.Add("Acceleration", ProcessAcceleration);
            m_SOPXmlProcessors.Add("Description", ProcessDescription);
            m_SOPXmlProcessors.Add("Shape", ProcessShape);
            m_SOPXmlProcessors.Add("Scale", ProcessScale);
            m_SOPXmlProcessors.Add("GroupID", ProcessGroupID);
            m_SOPXmlProcessors.Add("OwnerID", ProcessOwnerID);
            m_SOPXmlProcessors.Add("LastOwnerID", ProcessLastOwnerID);
            m_SOPXmlProcessors.Add("Flags", ProcessFlags);

            m_SOPXmlProcessors.Add("PhysicsShapeType", ProcessPhysicsShapeType);
            m_SOPXmlProcessors.Add("Density", ProcessDensity);
            m_SOPXmlProcessors.Add("Friction", ProcessFriction);
            m_SOPXmlProcessors.Add("Bounce", ProcessBounce);
            m_SOPXmlProcessors.Add("GravityModifier", ProcessGravityModifier);

            #endregion

            #region ShapeXmlProcessors initialization
            m_ShapeXmlProcessors.Add("ProfileCurve", ProcessShpProfileCurve);
            m_ShapeXmlProcessors.Add("TextureEntry", ProcessShpTextureEntry);
            m_ShapeXmlProcessors.Add("PathBegin", ProcessShpPathBegin);
            m_ShapeXmlProcessors.Add("PathCurve", ProcessShpPathCurve);
            m_ShapeXmlProcessors.Add("PathEnd", ProcessShpPathEnd);
            m_ShapeXmlProcessors.Add("PathRadiusOffset", ProcessShpPathRadiusOffset);
            m_ShapeXmlProcessors.Add("PathRevolutions", ProcessShpPathRevolutions);
            m_ShapeXmlProcessors.Add("PathScaleX", ProcessShpPathScaleX);
            m_ShapeXmlProcessors.Add("PathScaleY", ProcessShpPathScaleY);
            m_ShapeXmlProcessors.Add("PathShearX", ProcessShpPathShearX);
            m_ShapeXmlProcessors.Add("PathShearY", ProcessShpPathShearY);
            m_ShapeXmlProcessors.Add("PathSkew", ProcessShpPathSkew);
            m_ShapeXmlProcessors.Add("PathTaperX", ProcessShpPathTaperX);
            m_ShapeXmlProcessors.Add("PathTaperY", ProcessShpPathTaperY);
            m_ShapeXmlProcessors.Add("PathTwist", ProcessShpPathTwist);
            m_ShapeXmlProcessors.Add("PathTwistBegin", ProcessShpPathTwistBegin);
            m_ShapeXmlProcessors.Add("PCode", ProcessShpPCode);
            m_ShapeXmlProcessors.Add("ProfileBegin", ProcessShpProfileBegin);
            m_ShapeXmlProcessors.Add("ProfileEnd", ProcessShpProfileEnd);
            m_ShapeXmlProcessors.Add("ProfileHollow", ProcessShpProfileHollow);
            m_ShapeXmlProcessors.Add("State", ProcessShpState);
            m_ShapeXmlProcessors.Add("HollowShape", ProcessShpHollowShape);
            m_ShapeXmlProcessors.Add("SculptTexture", ProcessShpSculptTexture);
            m_ShapeXmlProcessors.Add("SculptType", ProcessShpSculptType);
            // Ignore "SculptData"; this element is deprecated
            m_ShapeXmlProcessors.Add("LightColorR", ProcessShpLightColorR);
            m_ShapeXmlProcessors.Add("LightColorG", ProcessShpLightColorG);
            m_ShapeXmlProcessors.Add("LightColorB", ProcessShpLightColorB);
            m_ShapeXmlProcessors.Add("LightColorA", ProcessShpLightColorA);
            m_ShapeXmlProcessors.Add("LightRadius", ProcessShpLightRadius);
            m_ShapeXmlProcessors.Add("LightCutoff", ProcessShpLightCutoff);
            m_ShapeXmlProcessors.Add("LightFalloff", ProcessShpLightFalloff);
            m_ShapeXmlProcessors.Add("LightIntensity", ProcessShpLightIntensity);
            #endregion

        }

        public static void ReadSOP(XmlReader reader, SceneObjectPart sop)
        {
            reader.ReadStartElement("SceneObjectPart");

            if (sop.Primitive == null)
                sop.Primitive = new Primitive();

            bool errors = ExecuteReadProcessors(
                sop,
                m_SOPXmlProcessors,
                reader,
                (o, nodeName, e) =>
                {
                    m_log.Debug(string.Format("[SceneObjectSerializer]: Error while parsing element {0} in object {1} {2} ",
                        nodeName, ((SceneObjectPart)o).Name, ((SceneObjectPart)o).UUID), e);
                });

            if (errors)
                throw new XmlException(string.Format("Error parsing object {0} {1}", sop.Name, sop.UUID));

            reader.ReadEndElement(); // SceneObjectPart

            //m_log.DebugFormat("[SceneObjectSerializer]: parsed SOP {0} {1}", sop.Name, sop.UUID);

        }


        #region Xml Serialization Utilities
        /// <summary>
        /// Populate a node with data read from xml using a dictinoary of processors
        /// </summary>
        /// <param name="nodeToFill"></param>
        /// <param name="processors"></param>
        /// <param name="xtr"></param>
        /// <param name="parseExceptionAction">
        /// Action to take if there is a parsing problem.  This will usually just be to log the exception
        /// </param>
        /// <returns>true on successful, false if there were any processing failures</returns>
        private static bool ExecuteReadProcessors<NodeType>(
            NodeType nodeToFill,
            Dictionary<string, Action<NodeType, XmlReader>> processors,
            XmlReader xtr,
            Action<NodeType, string, Exception> parseExceptionAction)
        {
            bool errors = false;
            int numErrors = 0;

            Stopwatch timer = new Stopwatch();
            timer.Start();

            string nodeName = string.Empty;
            while (xtr.NodeType != XmlNodeType.EndElement)
            {
                nodeName = xtr.Name;

                // m_log.DebugFormat("[ExternalRepresentationUtils]: Processing node: {0}", nodeName);

                Action<NodeType, XmlReader> p = null;
                if (processors.TryGetValue(xtr.Name, out p))
                {
                    // m_log.DebugFormat("[ExternalRepresentationUtils]: Found processor for {0}", nodeName);

                    try
                    {
                        p(nodeToFill, xtr);
                    }
                    catch (Exception e)
                    {
                        errors = true;
                        parseExceptionAction(nodeToFill, nodeName, e);

                        if (xtr.EOF)
                        {
                            m_log.Debug("[ExternalRepresentationUtils]: Aborting ExecuteReadProcessors due to unexpected end of XML");
                            break;
                        }

                        if (++numErrors == 10)
                        {
                            m_log.Debug("[ExternalRepresentationUtils]: Aborting ExecuteReadProcessors due to too many parsing errors");
                            break;
                        }

                        if (xtr.NodeType == XmlNodeType.EndElement)
                            xtr.Read();
                    }
                }
                else
                {
                    // m_log.DebugFormat("[ExternalRepresentationUtils]: found unknown element \"{0}\"", nodeName);
                    xtr.ReadOuterXml(); // ignore
                }

                if (timer.Elapsed.TotalSeconds >= 60)
                {
                    m_log.Debug("[ExternalRepresentationUtils]: Aborting ExecuteReadProcessors due to timeout");
                    errors = true;
                    break;
                }
            }

            return errors;
        }

        public static bool ReadBoolean(XmlReader reader)
        {
            // AuroraSim uses "int" for some fields that are boolean in OpenSim, e.g. "PassCollisions". Don't fail because of this.
            reader.ReadStartElement();
            string val = reader.ReadContentAsString().ToLower();
            bool result = val.Equals("true") || val.Equals("1");
            reader.ReadEndElement();

            return result;
        }

        public static Guid ReadUUID(XmlReader reader, string name)
        {
            Guid id;
            string idStr;

            reader.ReadStartElement(name);

            if (reader.Name == "Guid")
                idStr = reader.ReadElementString("Guid");
            else if (reader.Name == "UUID")
                idStr = reader.ReadElementString("UUID");
            else // no leading tag
                idStr = reader.ReadContentAsString();
            Guid.TryParse(idStr, out id);
            reader.ReadEndElement();

            return id;
        }

        public static Vector3 ReadVector(XmlReader reader, string name)
        {
            Vector3 vec;

            reader.ReadStartElement(name);
            vec.X = reader.ReadElementContentAsFloat(reader.Name, String.Empty); // X or x
            vec.Y = reader.ReadElementContentAsFloat(reader.Name, String.Empty); // Y or y
            vec.Z = reader.ReadElementContentAsFloat(reader.Name, String.Empty); // Z or z
            reader.ReadEndElement();

            return vec;
        }

        public static Quaternion ReadQuaternion(XmlReader reader, string name)
        {
            Quaternion quat = new Quaternion();

            reader.ReadStartElement(name);
            while (reader.NodeType != XmlNodeType.EndElement)
            {
                switch (reader.Name.ToLower())
                {
                    case "x":
                        quat.X = reader.ReadElementContentAsFloat(reader.Name, String.Empty);
                        break;
                    case "y":
                        quat.Y = reader.ReadElementContentAsFloat(reader.Name, String.Empty);
                        break;
                    case "z":
                        quat.Z = reader.ReadElementContentAsFloat(reader.Name, String.Empty);
                        break;
                    case "w":
                        quat.W = reader.ReadElementContentAsFloat(reader.Name, String.Empty);
                        break;
                }
            }

            reader.ReadEndElement();

            return quat;
        }

        public static T ReadEnum<T>(XmlReader reader, string name)
        {
            string value = reader.ReadElementContentAsString(name, String.Empty);
            // !!!!! to deal with flags without commas
            if (value.Contains(" ") && !value.Contains(","))
                value = value.Replace(" ", ", ");

            return (T)Enum.Parse(typeof(T), value); ;
        }
        #endregion

        #region SOPXmlProcessors
        private static void ProcessUUID(SceneObjectPart obj, XmlReader reader)
        {
            obj.UUID = ReadUUID(reader, "UUID");
        }

        private static void ProcessName(SceneObjectPart obj, XmlReader reader)
        {
            obj.Name = reader.ReadElementString("Name");
        }

        private static void ProcessMaterial(SceneObjectPart obj, XmlReader reader)
        {
            obj.Material = (Material)reader.ReadElementContentAsInt("Material", String.Empty);
        }

        private static void ProcessGroupPosition(SceneObjectPart obj, XmlReader reader)
        {
            obj.GroupPosition = ReadVector(reader, "GroupPosition");
        }

        private static void ProcessOffsetPosition(SceneObjectPart obj, XmlReader reader)
        {
            obj.OffsetPosition = ReadVector(reader, "OffsetPosition"); ;
        }

        private static void ProcessRotationOffset(SceneObjectPart obj, XmlReader reader)
        {
            obj.OffsetRotation = ReadQuaternion(reader, "RotationOffset");
        }

        private static void ProcessVelocity(SceneObjectPart obj, XmlReader reader)
        {
            obj.Velocity = ReadVector(reader, "Velocity");
        }

        private static void ProcessAngularVelocity(SceneObjectPart obj, XmlReader reader)
        {
            obj.AngularVelocity = ReadVector(reader, "AngularVelocity");
        }

        private static void ProcessAcceleration(SceneObjectPart obj, XmlReader reader)
        {
            obj.Acceleration = ReadVector(reader, "Acceleration");
        }

        private static void ProcessDescription(SceneObjectPart obj, XmlReader reader)
        {
            obj.Description = reader.ReadElementString("Description");
        }

        private static void ProcessPhysicsShapeType(SceneObjectPart obj, XmlReader reader)
        {
            obj.Primitive.PhysicsProps.PhysicsShapeType = (PhysicsShapeType)reader.ReadElementContentAsInt("PhysicsShapeType", String.Empty);
        }

        private static void ProcessDensity(SceneObjectPart obj, XmlReader reader)
        {
            obj.Primitive.PhysicsProps.Density = reader.ReadElementContentAsFloat("Density", String.Empty);
        }

        private static void ProcessFriction(SceneObjectPart obj, XmlReader reader)
        {
            obj.Primitive.PhysicsProps.Friction = reader.ReadElementContentAsFloat("Friction", String.Empty);
        }

        private static void ProcessBounce(SceneObjectPart obj, XmlReader reader)
        {
            obj.Primitive.PhysicsProps.Restitution = reader.ReadElementContentAsFloat("Bounce", String.Empty);
        }

        private static void ProcessGravityModifier(SceneObjectPart obj, XmlReader reader)
        {
            obj.Primitive.PhysicsProps.GravityMultiplier = reader.ReadElementContentAsFloat("GravityModifier", String.Empty);
        }

        private static void ProcessShape(SceneObjectPart obj, XmlReader reader)
        {
            List<string> errorNodeNames;
            obj.Primitive = ReadShape(reader, "Shape", out errorNodeNames, obj);

            if (errorNodeNames != null)
            {
                m_log.DebugFormat(
                    "[SceneObjectSerializer]: Parsing Primitive for object part {0} {1} encountered errors in properties {2}.",
                    obj.Name, obj.UUID, string.Join(", ", errorNodeNames.ToArray()));
            }
        }

        private static void ProcessScale(SceneObjectPart obj, XmlReader reader)
        {
            obj.Scale = ReadVector(reader, "Scale");
        }

        private static void ProcessGroupID(SceneObjectPart obj, XmlReader reader)
        {
            obj.GroupID = ReadUUID(reader, "GroupID");
        }

        private static void ProcessOwnerID(SceneObjectPart obj, XmlReader reader)
        {
            obj.OwnerID = ReadUUID(reader, "OwnerID");
        }

        private static void ProcessLastOwnerID(SceneObjectPart obj, XmlReader reader)
        {
            obj.LastOwnerID = ReadUUID(reader, "LastOwnerID");
        }

        private static void ProcessShpScale(SceneObjectPart obj, XmlReader reader)
        {
            obj.Scale = ReadVector(reader, "Scale");
        }


        private static void ProcessFlags(SceneObjectPart obj, XmlReader reader)
        {
            obj.Flags = ReadEnum<PrimFlags>(reader, "Flags");
        }

        #endregion


        #region ShapeXmlProcessors
        /// <summary>
        /// Read a shape from xml input
        /// </summary>
        /// <param name="reader"></param>
        /// <param name="name">The name of the xml element containing the shape</param>
        /// <param name="errors">a list containing the failing node names.  If no failures then null.</param>
        /// <returns>The shape parsed</returns>
        private static Primitive ReadShape(XmlReader reader, string name, out List<string> errorNodeNames, SceneObjectPart obj)
        {
            List<string> internalErrorNodeNames = null;

            Primitive prim = new Primitive();

            if (reader.IsEmptyElement)
            {
                reader.Read();
                errorNodeNames = null;
                return prim;
            }

            reader.ReadStartElement(name, String.Empty); // Shape

            ExecuteReadProcessors(
                prim,
                m_ShapeXmlProcessors,
                reader,
                (o, nodeName, e) => {
                    m_log.Debug(string.Format("[SceneObjectSerializer]: Error while parsing element {0} in Shape property of object {1} {2} ",
                        nodeName, obj.Name, obj.UUID), e);

                    if (internalErrorNodeNames == null)
                        internalErrorNodeNames = new List<string>();
                    internalErrorNodeNames.Add(nodeName);
                });

            reader.ReadEndElement(); // Shape

            errorNodeNames = internalErrorNodeNames;

            return prim;
        }

        private static void ProcessShpProfileCurve(Primitive prim, XmlReader reader)
        {
            prim.PrimData.ProfileCurve = ReadEnum<ProfileCurve>(reader, "ProfileCurve");
        }

        private static void ProcessShpTextureEntry(Primitive prim, XmlReader reader)
        {
            byte[] teData = Convert.FromBase64String(reader.ReadElementString("TextureEntry"));
            prim.Textures = new Primitive.TextureEntry(teData, 0, teData.Length);
        }

        private static void ProcessShpPathBegin(Primitive prim, XmlReader reader)
        {
            prim.PrimData.PathBegin = (ushort)reader.ReadElementContentAsInt("PathBegin", String.Empty);
        }

        private static void ProcessShpPathCurve(Primitive prim, XmlReader reader)
        {
            prim.PrimData.PathCurve = ReadEnum<PathCurve>(reader, "PathCurve");
        }

        private static void ProcessShpPathEnd(Primitive prim, XmlReader reader)
        {
            prim.PrimData.PathEnd = (ushort)reader.ReadElementContentAsInt("PathEnd", String.Empty);
        }

        private static void ProcessShpPathRadiusOffset(Primitive prim, XmlReader reader)
        {
            prim.PrimData.PathRadiusOffset = (sbyte)reader.ReadElementContentAsInt("PathRadiusOffset", String.Empty);
        }

        private static void ProcessShpPathRevolutions(Primitive prim, XmlReader reader)
        {
            prim.PrimData.PathRevolutions = (byte)reader.ReadElementContentAsInt("PathRevolutions", String.Empty);
        }

        private static void ProcessShpPathScaleX(Primitive prim, XmlReader reader)
        {
            prim.PrimData.PathScaleX = (byte)reader.ReadElementContentAsInt("PathScaleX", String.Empty);
        }

        private static void ProcessShpPathScaleY(Primitive prim, XmlReader reader)
        {
            prim.PrimData.PathScaleY = (byte)reader.ReadElementContentAsInt("PathScaleY", String.Empty);
        }

        private static void ProcessShpPathShearX(Primitive prim, XmlReader reader)
        {
            prim.PrimData.PathShearX = (byte)reader.ReadElementContentAsInt("PathShearX", String.Empty);
        }

        private static void ProcessShpPathShearY(Primitive prim, XmlReader reader)
        {
            prim.PrimData.PathShearY = (byte)reader.ReadElementContentAsInt("PathShearY", String.Empty);
        }

        private static void ProcessShpPathSkew(Primitive prim, XmlReader reader)
        {
            prim.PrimData.PathSkew = (sbyte)reader.ReadElementContentAsInt("PathSkew", String.Empty);
        }

        private static void ProcessShpPathTaperX(Primitive prim, XmlReader reader)
        {
            prim.PrimData.PathTaperX = (sbyte)reader.ReadElementContentAsInt("PathTaperX", String.Empty);
        }

        private static void ProcessShpPathTaperY(Primitive prim, XmlReader reader)
        {
            prim.PrimData.PathTaperY = (sbyte)reader.ReadElementContentAsInt("PathTaperY", String.Empty);
        }

        private static void ProcessShpPathTwist(Primitive prim, XmlReader reader)
        {
            prim.PrimData.PathTwist = (sbyte)reader.ReadElementContentAsInt("PathTwist", String.Empty);
        }

        private static void ProcessShpPathTwistBegin(Primitive prim, XmlReader reader)
        {
            prim.PrimData.PathTwistBegin = (sbyte)reader.ReadElementContentAsInt("PathTwistBegin", String.Empty);
        }

        private static void ProcessShpPCode(Primitive prim, XmlReader reader)
        {
            prim.PrimData.PCode = ReadEnum<PCode>(reader, "PCode");
        }

        private static void ProcessShpProfileBegin(Primitive prim, XmlReader reader)
        {
            prim.PrimData.ProfileBegin = (ushort)reader.ReadElementContentAsInt("ProfileBegin", String.Empty);
        }

        private static void ProcessShpProfileEnd(Primitive prim, XmlReader reader)
        {
            prim.PrimData.ProfileEnd = (ushort)reader.ReadElementContentAsInt("ProfileEnd", String.Empty);
        }

        private static void ProcessShpProfileHollow(Primitive prim, XmlReader reader)
        {
            prim.PrimData.ProfileHollow = (ushort)reader.ReadElementContentAsInt("ProfileHollow", String.Empty);
        }

        private static void ProcessShpState(Primitive prim, XmlReader reader)
        {
            prim.PrimData.State = (byte)reader.ReadElementContentAsInt("State", String.Empty);
        }

        private static void ProcessShpHollowShape(Primitive prim, XmlReader reader)
        {
            prim.PrimData.ProfileHole = ReadEnum<HoleType>(reader, "HollowShape");
        }

        private static void ProcessShpSculptTexture(Primitive prim, XmlReader reader)
        {
            prim.Sculpt.SculptTexture = ReadUUID(reader, "SculptTexture");
        }

        private static void ProcessShpSculptType(Primitive prim, XmlReader reader)
        {
            prim.Sculpt.Type = ReadEnum<SculptType>(reader, "SculptType");
        }

        private static void ProcessShpLightColorR(Primitive prim, XmlReader reader)
        {
            prim.Light.Color.R = reader.ReadElementContentAsFloat("LightColorR", String.Empty);
        }

        private static void ProcessShpLightColorG(Primitive prim, XmlReader reader)
        {
            prim.Light.Color.G = reader.ReadElementContentAsFloat("LightColorG", String.Empty);
        }

        private static void ProcessShpLightColorB(Primitive prim, XmlReader reader)
        {
            prim.Light.Color.B = reader.ReadElementContentAsFloat("LightColorB", String.Empty);
        }

        private static void ProcessShpLightColorA(Primitive prim, XmlReader reader)
        {
            prim.Light.Color.A = reader.ReadElementContentAsFloat("LightColorA", String.Empty);
        }

        private static void ProcessShpLightRadius(Primitive prim, XmlReader reader)
        {
            prim.Light.Radius = reader.ReadElementContentAsFloat("LightRadius", String.Empty);
        }

        private static void ProcessShpLightCutoff(Primitive prim, XmlReader reader)
        {
            prim.Light.Cutoff = reader.ReadElementContentAsFloat("LightCutoff", String.Empty);
        }

        private static void ProcessShpLightFalloff(Primitive prim, XmlReader reader)
        {
            prim.Light.Falloff = reader.ReadElementContentAsFloat("LightFalloff", String.Empty);
        }

        private static void ProcessShpLightIntensity(Primitive prim, XmlReader reader)
        {
            prim.Light.Intensity = reader.ReadElementContentAsFloat("LightIntensity", String.Empty);
        }

        #endregion
    }
}
