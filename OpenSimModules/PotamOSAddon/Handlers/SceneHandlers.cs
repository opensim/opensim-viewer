using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Reflection;
using System.Text;
using System.Net;
using System.Web;

using log4net;

using OpenSim.Framework.Servers.HttpServer;
using OpenSim.Server.Base;
using OpenMetaverse;
using OpenSim.Region.Framework.Scenes;
using OpenSim.Region.Framework.Interfaces;
using OpenSim.PotamOS;

namespace OpenSim.PotamOS.Handlers
{
    public class SceneHandler : BaseStreamHandler
    {
        private static readonly ILog m_log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);

        private List<Scene> m_Scenes;

        public SceneHandler(List<Scene> scenes) : base("GET", "/hppo/scene")
        {
            m_Scenes = scenes;
        }

        public override byte[] Handle(string path, Stream requestData,
                                      IOSHttpRequest httpRequest, IOSHttpResponse httpResponse)
        {
            string resource = GetParam(path);
            string region = "DEFAULT";
            if (resource != string.Empty)
            {
                string[] parts = resource.Split('/');
                region = HttpUtility.UrlDecode(parts[1]);
            }
            m_log.DebugFormat("[PotamOS]: region is {0} ({1})", region, resource);

            Scene scene = m_Scenes[0];
            if (region != "DEFAULT")
            {
                scene = m_Scenes.Find(s => s.RegionInfo.RegionName == region);
                if (scene == null)
                {
                    m_log.WarnFormat("[PotamOS]: Requested region {0} not found here. Sending the first region.", region);
                    scene = m_Scenes[0];
                }
            }

            httpResponse.StatusCode = (int)OSHttpStatusCode.SuccessOk;
            httpResponse.ContentType = "application/xml";
            httpResponse.AddHeader("Content-Encoding", "gzip");
            IRegionSerialiserModule serial = scene.RequestModuleInterface<IRegionSerialiserModule>();
            byte[] xml = new byte[0];
            if (serial != null)
            {
                using (MemoryStream mem = new MemoryStream())
                {
                    using (GZipStream zip = new GZipStream(mem, CompressionMode.Compress))
                    using (TextWriter writer = new StreamWriter(zip))
                    {
                        serial.SavePrimsToXml2(scene, writer, new Vector3(0.01f, 0.01f, 0.01f), new Vector3(1000, 1000, 1000));

                    }
                    string xmlstr = Encoding.UTF8.GetString(mem.ToArray());
                    xml = mem.ToArray();
                    m_log.DebugFormat("[PotamOS]: size of xml scene is {0}", xml.Length);
                }
            }
            return xml;

        }

    }

}
