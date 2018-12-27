using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using Mono.Addins;
using log4net;
using Nini.Config;

using OpenSim.Framework;
using OpenSim.Framework.Servers;
using OpenSim.Region.Framework.Scenes;
using OpenSim.Region.Framework.Interfaces;

using OpenSim.PotamOS;

namespace OpenSim.PotamOS.ServerConnectors
{
    [Extension(Path = "/OpenSim/RegionModules", NodeName = "RegionModule", Id = "Hppo")]
    public class PotamOSModule : ISharedRegionModule
    {
        private static readonly ILog m_log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);

        protected bool m_Enabled = false;
        private List<Scene> m_Scenes = new List<Scene>();
        private PotamOSMain m_PotamOSMain;
        IConfigSource m_Config;

        public void Initialise(IConfigSource config)
        {
            m_log.Info("[PotamOS Module]: Initializing...");
            m_Config = config;

            //// We only load the configuration file if the config doesn't know about this module already
            //IConfig potConfig = config.Configs["PotamOS"];
            //if (potConfig == null)
            //    LoadConfiguration(config);

            //potConfig = config.Configs["PotamOS"];
            //if (potConfig == null)
            //    throw new Exception("[PotamOS]: Unable to find configuration. Service disabled.");

            m_Enabled = true; // potConfig.GetBoolean("Enabled", m_Enabled);
        }

        public void PostInitialise()
        {
        }

        public void Close()
        {
        }

        public void AddRegion(Scene scene)
        {
            if (!m_Enabled)
                return;

            m_Scenes.Add(scene);
            if (m_Scenes.Count == 1)
            {
                scene.EventManager.OnRegionReadyStatusChange += EventManager_OnRegionReadyStatusChange; 
            }
        }

        public void RegionLoaded(Scene scene)
        {
        }

        public void RemoveRegion(Scene scene)
        {
            if (!m_Enabled)
                return;

            m_Scenes.Remove(scene);
        }

        public string Name
        {
            get { return "PotamOSModule"; }
        }

        public Type ReplaceableInterface
        {
            get { return null; }
        }

        private void LoadConfiguration(IConfigSource config)
        {
            string configPath = string.Empty;
            bool created;
            if (!Util.MergeConfigurationFile(config, "PotamOS.ini", Path.Combine(PotamOS.Info.AssemblyDirectory, "PotamOS.ini"), out configPath, out created))
            {
                m_log.WarnFormat("[PotamOS]: Configuration file not merged.");
                return;
            }

            if (created)
            {
                m_log.ErrorFormat("[PotamOS]: PLEASE EDIT {0} BEFORE RUNNING THIS SERVICE", configPath);
                throw new Exception("PotamOS addin must be configured prior to running");
            }
        }

        private void EventManager_OnRegionReadyStatusChange(IScene obj)
        {
            try
            {
                if (m_Enabled)
                {
                    m_PotamOSMain = new PotamOSMain(m_Config, MainServer.Instance, string.Empty, m_Scenes);
                    m_log.Debug("[PotamOS]: PotamOS enabled.");
                }
                else
                    m_log.Debug("[PotamOS]: PotamOS disabled.");

            }
            catch (Exception e)
            {
                m_log.ErrorFormat(e.StackTrace);
                m_log.ErrorFormat("[PotamOS]: Could not load PotamOS: {0}. ", e.Message);
                return;
            }

        }

    }
}
