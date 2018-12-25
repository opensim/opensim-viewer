///////////////////////////////////////////////////////////////////
//
// (c) 2010 Melanie Thielker and Careminster, Limited
//
// All rights reserved
//
using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using System.Timers;
using System.Data;
using System.IO;
using System.Xml;
using MySql.Data.MySqlClient;
using log4net;
using Nini.Config;
using Nwc.XmlRpc;
using OpenMetaverse;
using OpenSim.Framework;
using OpenSim.Framework.Console;
using OpenSim.Data;
using OpenSim.Data.MySQL;
using OpenSim.Region.Framework.Interfaces;
using OpenSim.Region.Framework.Scenes;
using OpenSim.Services.Interfaces;
using Mono.Addins;
using OpenSim.Region.Framework.Scenes.Serialization;
using Careminster.XStorage;

[assembly: Addin("XStorage.Module", OpenSim.VersionInfo.VersionNumber + "0.1")]
[assembly: AddinDependency("OpenSim.Region.Framework", OpenSim.VersionInfo.VersionNumber)]
[assembly: AddinDescription("Avination file storage addon.")]
[assembly: AddinAuthor("Melanie Thielker")]

namespace Careminster.Modules.XStorage
{
    [Extension(Path = "/OpenSim/RegionModules", NodeName = "RegionModule", Id = "XStorage")]
    public class XStorageModule : ISharedRegionModule
    {
        private static readonly ILog m_log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);

        protected static List<Scene> m_Scenes = new List<Scene>();
        protected Timer m_StorageTimer = new Timer();
        protected bool m_Enabled = false;
        protected static string m_SavePath = "datastore";
        protected static bool m_saveNow = false;
        public static bool UseBackup = true;
        public static bool LoadXStorage = true;
        public static bool LoadBase = true;
        private bool m_loading = false;
        private Dictionary<UUID, bool> m_eligible =
                new Dictionary<UUID, bool>();

        public void Initialise(IConfigSource config)
        {
            IConfig storageConfig = config.Configs["SimulationStorage"];
            if (storageConfig == null)
                return;

            if (storageConfig.GetString("Module", String.Empty) != Name)
                return;

            m_SavePath = storageConfig.GetString("StoragePath", m_SavePath);
            LoadXStorage = storageConfig.GetBoolean("LoadXStorage", LoadXStorage);
            LoadBase = storageConfig.GetBoolean("LoadBase", LoadBase);

            m_Enabled = true;

            IConfig startupConfig = config.Configs["Startup"];
            if (startupConfig != null)
                UseBackup = startupConfig.GetBoolean("UseSceneBackup", UseBackup);
            m_log.Info("[XStorage]: Storage module active");

            m_StorageTimer.AutoReset = true;
            m_StorageTimer.Interval = 600000; // 10 minutes
            m_StorageTimer.Elapsed += OnStorageTimer;
            m_StorageTimer.Start();
        }

        public void PostInitialise()
        {
        }

        public void Close()
        {
        }

        public void AddRegion(Scene scene)
        {
            m_Scenes.Add(scene);

            if (!m_Enabled)
                return;

            scene.EventManager.OnSceneShuttingDown += OnSceneShuttingDown;
            scene.EventManager.OnBackup += OnBackup;

            MainConsole.Instance.Commands.AddCommand("xstorage", false, "xstorage load", "xstorage load", "Load the primsload file", HandleLoad);
            MainConsole.Instance.Commands.AddCommand("xstorage", false, "xstorage save", "xstorage save", "Manually trigger a prim save", HandleSave);
        }

        public void RegionLoaded(Scene scene)
        {
        }

        public void RemoveRegion(Scene scene)
        {
            m_Scenes.Remove(scene);

            if (!m_Enabled)
                return;
        }

        public string Name
        {
            get { return "XStorageModule"; }
        }

        public Type ReplaceableInterface
        {
            get { return null; }
        }

        private void OnStorageTimer(object sender, ElapsedEventArgs args)
        {
            m_saveNow = true;
        }

        private void OnBackup(ISimulationDataService data, bool forced)
        {
            if (!m_saveNow)
                return;

            m_saveNow = false;

            Util.FireAndForget(delegate(object x)
            {
                foreach (Scene s in m_Scenes)
                {
                    int missing = 0;

                    List<UUID> databaseIDs = new List<UUID>(s.SimulationDataService.GetObjectIDs(s.RegionInfo.RegionID));

                    Dictionary<UUID, bool> newEligible =
                            new Dictionary<UUID, bool>();

                    s.ForEachSOG(delegate(SceneObjectGroup sog)
                            {
                                if ((sog.RootPart.Flags & PrimFlags.TemporaryOnRez) != 0)
                                    return;

                                if (sog.IsAttachment)
                                    return;

                                if (!sog.Backup)
                                {
                                    // If it's in the scene and neither temp nor
                                    // attached, it should be backed up.

                                    if (m_eligible.ContainsKey(sog.UUID))
                                    {
                                        sog.AttachToBackup();
                                        sog.HasGroupChanged = true;
                                        missing++;
                                    }
                                    else
                                    {
                                        newEligible[sog.UUID] = true;
                                    }
                                }
                                else if (!databaseIDs.Contains(sog.UUID) && (!sog.HasGroupChanged))
                                {
                                    sog.HasGroupChanged = true;
                                    missing++;
                                }
                            });


                    m_eligible = newEligible;

                    if (missing > 0)
                        m_log.InfoFormat("Found {0} scene objects not in database", missing);
                    StorePrims(s);
                    IScriptModule[] sm = s.RequestModuleInterfaces<IScriptModule>();
                    // MT: Commented because GetXMLState already does this
                    // within the script engine.
//                    foreach (IScriptModule m in sm)
//                        m.SaveAllState();
                }
            });
        }

        private void StorePrims(Scene s)
        {
            Directory.CreateDirectory(m_SavePath);

            string dir = Path.Combine(m_SavePath, s.RegionInfo.RegionID.ToString());
            Directory.CreateDirectory(dir);

            string file = Path.Combine(dir, "prims.xml");
            string final = Path.Combine(dir, "primsload.xml");
            string backup = Path.Combine(dir, "primsback.xml");

            File.Delete(file);
            using (StreamWriter sw = File.CreateText(file))
            {
                sw.WriteLine("<SceneStorage>");
                s.ForEachSOG(delegate(SceneObjectGroup sog)
                        {
                            if (sog.IsAttachment)
                                return;
                            if ((sog.RootPart.Flags & PrimFlags.TemporaryOnRez) != 0)
                                return;
                            if (sog.IsDeleted)
                                return;

                            SceneObjectGroup bgr = sog.Copy(false);
                            bgr.RootPart.Velocity = sog.RootPart.Velocity;
                            bgr.RootPart.Acceleration = sog.RootPart.Acceleration;
                            bgr.RootPart.AngularVelocity = sog.RootPart.AngularVelocity;
                            bgr.RootPart.ParticleSystem = sog.RootPart.ParticleSystem;

                            bool isPhysical = bgr.RootPart.PhysActor != null && (!bgr.RootPart.PhysActor.IsPhysical);
                            foreach (SceneObjectPart part in bgr.Parts)
                            {
                                if (isPhysical)
                                    part.GroupPosition = bgr.RootPart.GroupPosition;
                                PrimitiveBaseShape shp = new PrimitiveBaseShape();
                                shp.Scale = part.Shape.Scale;
                                shp.PCode = part.Shape.PCode;
                                shp.PathBegin = part.Shape.PathBegin;
                                shp.PathEnd = part.Shape.PathEnd;
                                shp.PathScaleX = part.Shape.PathScaleX;
                                shp.PathScaleY = part.Shape.PathScaleY;
                                shp.PathShearX = part.Shape.PathShearX;
                                shp.PathShearY = part.Shape.PathShearY;
                                shp.PathSkew = part.Shape.PathSkew;
                                shp.PathCurve = part.Shape.PathCurve;
                                shp.PathRadiusOffset = part.Shape.PathRadiusOffset;
                                shp.PathRevolutions = part.Shape.PathRevolutions;
                                shp.PathTaperX = part.Shape.PathTaperX;
                                shp.PathTaperY = part.Shape.PathTaperY;
                                shp.PathTwist = part.Shape.PathTwist;
                                shp.PathTwistBegin = part.Shape.PathTwistBegin;
                                shp.ProfileBegin = part.Shape.ProfileBegin;
                                shp.ProfileEnd = part.Shape.ProfileEnd;
                                shp.ProfileCurve = part.Shape.ProfileCurve;
                                shp.ProfileHollow = part.Shape.ProfileHollow;
                                shp.TextureEntry = part.Shape.TextureEntry;
                                shp.ExtraParams = part.Shape.ExtraParams;
                                shp.State = part.Shape.State;
                                shp.Media = part.Shape.Media;

                                part.Shape = shp;
                            }

                            string xml = SceneObjectSerializer.ToXml2Format(bgr);
                            bgr = null;

                            sw.Write(xml);
                            sw.Flush();
                        }
                );
                sw.WriteLine("</SceneStorage>");
            }

            File.Delete(backup);
            try
            {
                File.Move(final, backup);
            }
            catch {}

            try
            {
                File.Move(file, final);
            }
            catch {}

            // m_log.InfoFormat("[XSTORAGE]: Persisting region {0} done", s.RegionInfo.RegionName);
        }

        public static List<SceneObjectGroup> LoadObjects(XStorageSimulationData dataModule, UUID regionUUID)
        {
            // In this case we always have to load base or the sim will have no persistence
            if (!LoadXStorage)
                return dataModule.LoadBaseObjects(regionUUID);
                
            string dir = Path.Combine(m_SavePath, regionUUID.ToString());
            string file = Path.Combine(dir, "primsload.xml");

            FileInfo info = new FileInfo(file);
            if (info.LastWriteTime < DateTime.Now.AddMinutes(-5))
                file = Path.Combine(dir, "primsback.xml");

            m_log.DebugFormat("[XSTORAGE]: Looking for {0} to load", file);

            if (!File.Exists(file))
            {
                // if this file doesn't exist, load what we can
                m_log.DebugFormat("[XSTORAGE]: File not found, falling back to database", file);
                return dataModule.LoadBaseObjects(regionUUID);
            }

            List<SceneObjectGroup> simData = Load(file);

            if (LoadBase)
            {
                List<SceneObjectGroup> dbContents = dataModule.LoadBaseObjects(regionUUID);

                List<UUID> uuids = new List<UUID>();
                foreach (SceneObjectGroup grp in simData)
                    uuids.Add(grp.UUID);

                foreach(SceneObjectGroup grp in dbContents)
                {
                    if (!uuids.Contains(grp.UUID))
                        simData.Add(grp);
                }
            }

            return simData;
        }

        private static List<SceneObjectGroup> Load(string file)
        {
            XmlDocument doc = new XmlDocument();
            try
            {
                doc.Load(file);
            }
            catch (Exception e)
            {
                m_log.Debug("[XSTORAGE]: File found but not valid Xml", e);
                return new List<SceneObjectGroup>();
            }

            m_log.DebugFormat("[XSTORAGE]: Loading prims from {0}", file);

            XmlNodeList rootL = doc.GetElementsByTagName("SceneStorage");
            if (rootL.Count != 1)
            {
                m_log.Debug("[XSTORAGE]: Document contained no root element");
                return new List<SceneObjectGroup>();
            }

            XmlNode rootN = rootL[0];

            List<SceneObjectGroup> l = new List<SceneObjectGroup>();

            int count = 0;
            for (int i = 0 ; i < rootN.ChildNodes.Count ; i++)
            {
                try
                {
                    SceneObjectGroup g = SceneObjectSerializer.FromXml2Format(rootN.ChildNodes[i].OuterXml);
                    foreach (SceneObjectPart part in g.Parts)
                    {
                        PrimitiveBaseShape shp = new PrimitiveBaseShape();
                        shp.Scale = part.Shape.Scale;
                        shp.PCode = part.Shape.PCode;
                        shp.PathBegin = part.Shape.PathBegin;
                        shp.PathEnd = part.Shape.PathEnd;
                        shp.PathScaleX = part.Shape.PathScaleX;
                        shp.PathScaleY = part.Shape.PathScaleY;
                        shp.PathShearX = part.Shape.PathShearX;
                        shp.PathShearY = part.Shape.PathShearY;
                        shp.PathSkew = part.Shape.PathSkew;
                        shp.PathCurve = part.Shape.PathCurve;
                        shp.PathRadiusOffset = part.Shape.PathRadiusOffset;
                        shp.PathRevolutions = part.Shape.PathRevolutions;
                        shp.PathTaperX = part.Shape.PathTaperX;
                        shp.PathTaperY = part.Shape.PathTaperY;
                        shp.PathTwist = part.Shape.PathTwist;
                        shp.PathTwistBegin = part.Shape.PathTwistBegin;
                        shp.ProfileBegin = part.Shape.ProfileBegin;
                        shp.ProfileEnd = part.Shape.ProfileEnd;
                        shp.ProfileCurve = part.Shape.ProfileCurve;
                        shp.ProfileHollow = part.Shape.ProfileHollow;
                        shp.TextureEntry = part.Shape.TextureEntry;
                        shp.ExtraParams = part.Shape.ExtraParams;
                        shp.State = part.Shape.State;
                        shp.Media = part.Shape.Media;

                        part.Shape = shp;
                    }
                    l.Add(g);
                    count++;

                    if (count % 1000 == 0)
                        m_log.DebugFormat("[XSTORAGE]: Loaded {0} objects", count);
                }
                catch (Exception e)
                {
                    m_log.Debug("[XSTORAGE]: Exception deserializing scene object", e);
                    Environment.Exit(1);
                }
            }
            m_log.DebugFormat("[XSTORAGE]: Read {0} scene objects", count);

            doc = null;
            GC.Collect();
            return l;
        }

        private void OnSceneShuttingDown(Scene s)
        {
            StorePrims(s);
            IScriptModule[] sm = s.RequestModuleInterfaces<IScriptModule>();
            foreach (IScriptModule m in sm)
                m.SaveAllState();
        }

        private void HandleSave(string module, string[] args)
        {
            if (!m_loading)
                m_saveNow = true;
        }

        private void HandleLoad(string module, string[] args)
        {
            try
            {
                m_loading = true;

                Scene s = m_Scenes[0];

                string dir = Path.Combine(m_SavePath, s.RegionInfo.RegionID.ToString());
                string file = Path.Combine(dir, "primsload.xml");

                if (!File.Exists(file))
                {
                    MainConsole.Instance.Output(String.Format("[XSTORAGE]: Can't find {0}", file));
                    return;
                }

                s.DeleteAllSceneObjects();

                List<SceneObjectGroup> l = Load(file);

                foreach (SceneObjectGroup g in l)
                    s.AddRestoredSceneObject(g, true, false, true);
            }
            finally
            {
                m_loading = false;
            }
        }
    }
}
