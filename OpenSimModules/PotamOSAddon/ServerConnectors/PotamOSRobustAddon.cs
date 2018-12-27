using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using log4net;
using Nini.Config;
using Mono.Addins;

using OpenSim.Server.Base;
using OpenSim.Framework.Servers.HttpServer;
using OpenSim.Server.Handlers.Base;

using OpenSim.PotamOS;

namespace OpenSim.PotamOS.ServerConnectors
{
    [Extension(Path = "/Robust/Connector")]
    public class PotamOSRobustAddon : ServiceConnector, IRobustConnector
    {
        private static readonly ILog m_log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);
        //private static string CONFIG_FILE = "PotamOS.ini";
        private PotamOSMain m_PotamOSMain;

        private static string AssemblyDirectory
        {
            get
            {
                string location = Assembly.GetExecutingAssembly().Location;
                return Path.GetDirectoryName(location);
            }
        }

        public override string ConfigName
        {
            get { return "PotamOS"; }
        }

        public bool Enabled
        {
            get;
            private set;
        }

        public string PluginPath
        {
            get;
            set;
        }

        // Called from the plugin loader (in ServerUtils)
        public PotamOSRobustAddon()
        {
            m_log.DebugFormat("[PotamOS]: Addin instance created");
        }

        #region IRobustConnector
        public uint Configure(IConfigSource config)
        {
            Config = config;

            //IConfig startconfig = Config.Configs["Startup"];
            //string configdirectory = startconfig.GetString("ConfigDirectory", ".");

            //ConfigFile = Path.Combine(configdirectory, CONFIG_FILE);

            //IConfig potConfig = Config.Configs[ConfigName];

            //if (potConfig == null)
            //{
            //    // No [PotamOS] in the main configuration. We need to read it from its own file
            //    if (!File.Exists(ConfigFile))
            //    {
            //        // We need to copy the one that comes in the package
            //        if (!Directory.Exists(configdirectory))
            //            Directory.CreateDirectory(configdirectory);

            //        string embeddedConfig = Path.Combine(AssemblyDirectory, CONFIG_FILE);
            //        File.Copy(embeddedConfig, ConfigFile);
            //        m_log.ErrorFormat("[PotamOS]: PLEASE EDIT {0} BEFORE RUNNING THIS SERVICE", ConfigFile);
            //        throw new Exception("PotamOS addin must be configured prior to running");
            //    }
            //    else
            //    {
            //        m_log.DebugFormat("[PotamOS]: Configuring from {0}...", ConfigFile);

            //        IConfigSource configsource = new IniConfigSource(ConfigFile);
            //        potConfig = configsource.Configs[ConfigName];

            //        // Merge everything and expand eventual key values used by our config
            //        Config.Merge(configsource);
            //        Config.ExpandKeyValues();
            //    }

            //    if (potConfig == null)
            //        throw new Exception(string.Format("[PotamOS]: Could not load configuration from {0}. Unable to proceed.", ConfigFile));

            //}

            //Enabled = potConfig.GetBoolean("Enabled", false);

            // Let's look for the port in WifiService first, then look elsewhere
            //int port = potConfig.GetInt("ServerPort", -1);
            //if (port > 0)
            //    return (uint)port;

            int port = 8002;
            IConfig section = Config.Configs["Const"];
            if (section != null)
                port = section.GetInt("PublicPort", -1);

            if (port < 0)
                throw new Exception("[PotamOS]: Could not find port in configuration file");

            return (uint)port;
        }

        public void Initialize(IHttpServer server)
        {
            m_log.DebugFormat("[PotamOS]: Initializing. Server at port {0}. Service is {1}", server.Port, Enabled ? "enabled" : "disabled");

            if (!Enabled)
                return;

            //IConfig serverConfig = Config.Configs[ConfigName];
            //if (serverConfig == null)
            //    throw new Exception(String.Format("No section {0} in config file", ConfigName));

            m_PotamOSMain = new PotamOSMain(Config, server, ConfigName);
        }

        public void Unload()
        {
            if (!Enabled)
                return;

            m_PotamOSMain.Unload();
        }

        #endregion IRobustConnector

    }
}
