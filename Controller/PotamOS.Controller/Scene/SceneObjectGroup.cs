using System;
using System.Collections.Generic;

namespace PotamOS.Controller.Scene
{
    public class SceneObjectGroup
    {
        private List<SceneObjectPart> m_Parts = new List<SceneObjectPart>();

        public Guid UUID
        {
            get { return m_Parts[0].UUID; }
        }

        public string Name
        {
            get { return m_Parts[0].Name; }
        }

        public int NParts
        {
            get { return m_Parts.Count; }
        }

        public List<SceneObjectPart> Parts
        {
            get { return new List<SceneObjectPart>(m_Parts); }
        }

        public SceneObjectPart RootPart
        {
            get { return m_Parts[0]; }
        }

        public void AddPart(SceneObjectPart sop)
        {
            m_Parts.Add(sop);
        }
    }
}
