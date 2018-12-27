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
            httpResponse.StatusCode = (int)OSHttpStatusCode.SuccessOk;
            httpResponse.ContentType = "application/xml";
            httpResponse.AddHeader("Content-Encoding", "gzip");
            IRegionSerialiserModule serial = m_Scenes[0].RequestModuleInterface<IRegionSerialiserModule>();
            byte[] xml = new byte[0];
            if (serial != null)
            {
                using (MemoryStream mem = new MemoryStream())
                using (GZipStream zip = new GZipStream(mem, CompressionMode.Compress, false))
                using (TextWriter writer = new StreamWriter(zip))       
                {
                    serial.SavePrimsToXml2(m_Scenes[0], writer, Vector3.Zero, new Vector3(1000, 1000, 1000));
                    xml = mem.ToArray();
                    m_log.DebugFormat("[PotamOS]: size of compressed xml is {0}", xml.Length);

                }
            }
            return xml;

        }

    }

}
