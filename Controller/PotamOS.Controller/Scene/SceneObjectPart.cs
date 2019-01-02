using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Reflection;
using System.Xml;
using log4net;
using Xenko.Core.Mathematics;

namespace PotamOS.Controller.Scene
{
    public class SceneObjectPart : IEquatable<SceneObjectPart>
    {
        private static readonly ILog m_log = LogManager.GetLogger(
                                             MethodBase.GetCurrentMethod().DeclaringType);

        #region Properties

        public Guid UUID { get; set; }
        public string Name { get; set; }
        public string Description { get; set; }
        public Vector3 GroupPosition { get; set; }
        public Vector3 OffsetPosition { get; set; }
        public Vector3 Scale { get; set; }
        public Quaternion OffsetRotation { get; set; }
        public Material Material { get; set; }

        public PrimFlags Flags { get; set; }

        public Vector3 Velocity { get; set; }
        public Vector3 AngularVelocity { get; set; }
        public Vector3 Acceleration { get; set; }

        //public byte[] textureEntry;
        //TextureEntry defaultTexture;
        //List<TextureEntry> textures;
        public Primitive Primitive { get; set; }

        public Guid GroupID { get; set; }
        public Guid OwnerID { get; set; }
        public Guid LastOwnerID { get; set; }

        bool isPrim;
        bool isSculpt;
        bool isMesh;

        bool ShouldColide;
        bool isPhantom;
        bool isPhysical;

        int sculptType;
        //       LevelDetail maxLod = Highest;

        Guid meshAssetId;

        #endregion

        public SceneObjectPart()
        {
            Primitive = new Primitive();
        }

        public void Load(XmlReader reader)
        {
            SOPSerializer.ReadSOP(reader, this);
        }

        #region IEquitable
        public static bool operator ==(SceneObjectPart lhs, SceneObjectPart rhs)
        {
            if ((Object)lhs == null || (Object)rhs == null)
            {
                return (Object)rhs == (Object)lhs;
            }
            return (lhs.UUID == rhs.UUID);
        }

        public static bool operator !=(SceneObjectPart lhs, SceneObjectPart rhs)
        {
            if ((Object)lhs == null || (Object)rhs == null)
            {
                return (Object)rhs != (Object)lhs;
            }
            return !(lhs.UUID == rhs.UUID);
        }

        public override bool Equals(object obj)
        {
            return (obj is SceneObjectPart) ? this == (SceneObjectPart)obj : false;
        }

        public bool Equals(SceneObjectPart other)
        {
            return this == other;
        }

        #endregion IEquitable
    }

}
