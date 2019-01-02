using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using System.Reflection;

using log4net;

namespace PotamOS.Controller.Scene
{
    public class SceneManager
    {
        private static readonly ILog m_log = LogManager.GetLogger(
                                             MethodBase.GetCurrentMethod().DeclaringType);

        private static SceneManager m_Instance;

        public static SceneManager Instance
        {
            get {
                if (m_Instance == null)
                    m_Instance = new SceneManager();
                return m_Instance;
            }
        }

        private List<SceneObjectGroup> m_Objects = new List<SceneObjectGroup>();

        private SceneManager() { }

        public void LoadSceneFromXml(Stream s)
        {
            m_log.InfoFormat("[Controller]: LoadSceneFromXml starting");

            using (XmlTextReader xmlReader = new XmlTextReader(s))
            {
                while (xmlReader.ReadToFollowing("SceneObjectGroup"))
                {
                    try
                    {
                        SceneObjectGroup sog = new SceneObjectGroup();

                        // Read the root sop
                        if (xmlReader.ReadToDescendant("SceneObjectPart"))
                        {
                            SceneObjectPart sop = new SceneObjectPart();
                            sop.Load(xmlReader);
                            sog.AddPart(sop);
                        }

                        using (XmlReader inner = xmlReader.ReadSubtree())
                            // Read the other parts
                            while (inner.ReadToFollowing("SceneObjectPart"))
                            {
                                SceneObjectPart sop = new SceneObjectPart();
                                sop.Load(inner);
                                sog.AddPart(sop);
                            }
                        m_log.DebugFormat("[Controller]: New SOG {0} {1} with {2} parts", sog.UUID, sog.Name, sog.NParts);
                        m_Objects.Add(sog);
                    }
                    catch (Exception e)
                    {
                        m_log.WarnFormat("[Controller]: Problem while parsing {0}: {1}", xmlReader.Name, e);
                    }
                }
            }
            m_log.InfoFormat("[Controller]: LoadSceneFromXml finished. Total SOGs: {0}", m_Objects.Count);
        }
    }
}
