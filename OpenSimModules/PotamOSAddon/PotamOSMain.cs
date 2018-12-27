using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;

using OpenSim.Region.Framework.Scenes;
using OpenSim.Server.Handlers.Base;
using OpenSim.Framework.Servers.HttpServer;
using OpenSim.Services.Interfaces;
using OpenSim.Services.GridService;

using Nini.Config;
using log4net;

using OpenSim.PotamOS.Handlers;

namespace OpenSim.PotamOS
{
    public class PotamOSMain : IServiceConnector
    {
        private static List<Scene> m_Scenes = null;
        private static string m_DocsPath = Path.Combine("..", "potapps");
        public static string DocsPath {
            get { return m_DocsPath; }
        }
        private static readonly ILog m_log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);

        private IHttpServer m_Server;
        private GridService m_GridService;
        private List<IRequestHandler> m_RequestHandlers = new List<IRequestHandler>();

        private IConfigSource m_Config;

        private string ConfigName
        {
            get { return "PotamOS"; }
        }

        // Robust addin calls this
        public PotamOSMain(IConfigSource config, IHttpServer server, string configName) :
            this(config, server, configName, null)
        {
        }

        // Region Module calls this
        public PotamOSMain(IConfigSource config, IHttpServer server, string configName, List<Scene> scenes)
        {
            m_Config = config;
            m_Server = server;
            m_Scenes = scenes;

            m_log.DebugFormat("[PotamOS]: starting with config {0}", ConfigName);

            Initialize(server);

        }

        private void Initialize(IHttpServer server)
        {
            m_log.DebugFormat("[PotamOS]: Initializing. Server at port {0}.", server.Port);

            //IConfig serverConfig = m_Config.Configs[ConfigName];
            //if (serverConfig == null)
            //    throw new Exception(String.Format("No section {0} in config file", ConfigName));

            //m_DocsPath = serverConfig.GetString("DocsPath", m_DocsPath);

            m_GridService = new GridService(m_Config);

            AddStreamHandler(new HppoDefaultHandler());
            AddStreamHandler(new HppoDefaultPostHandler(m_GridService));
            // Register the scene handler with the sim
            if (m_Scenes != null)
                AddStreamHandler(new SceneHandler(m_Scenes));
        }

        private void AddStreamHandler(IRequestHandler rh)
        {
            m_log.DebugFormat("[PotamOS]: Adding {0}", rh.Path);

            m_RequestHandlers.Add(rh);
            m_Server.AddStreamHandler(rh);
        }

        public void Unload()
        {
            foreach (IRequestHandler rh in m_RequestHandlers)
                m_Server.RemoveStreamHandler(rh.HttpMethod, rh.Path);

            // Tell the addons to unload too!
            m_RequestHandlers.Clear();
        }
    }
}
