using System;
using System.Collections.Generic;
using System.Reflection;
using OpenMetaverse;
using OpenSim.Framework;
using OpenSim.Region.Framework.Interfaces;
using OpenSim.Region.Framework.Scenes;
using OpenSim.Data;
using OpenSim.Data.MySQL;
using Careminster.Modules.XStorage;

namespace Careminster.XStorage
{
    /// <summary>
    /// NULL DataStore, do not store anything
    /// </summary>
    public class XStorageSimulationData : MySQLSimulationData
    {
        protected override Assembly Assembly
        {
            get { return base.GetType().Assembly; }
        }

        public XStorageSimulationData()
        {
        }

        public XStorageSimulationData(string connectionString)
        {
            Initialise(connectionString);
        }

        public void Dispose()
        {
        }

        ////////////////////////////////////////////////////////////////////
        // Overrides for database style storage

        public override void StoreObject(SceneObjectGroup obj, UUID regionUUID)
        {
            if (XStorageModule.UseBackup)
                base.StoreObject(obj, regionUUID);
        }

        public override void RemoveObject(UUID obj, UUID regionUUID)
        {
            if (XStorageModule.UseBackup)
                base.RemoveObject(obj, regionUUID);
        }

        public override void StorePrimInventory(UUID primID, ICollection<TaskInventoryItem> items)
        {
            if (XStorageModule.UseBackup)
                base.StorePrimInventory(primID, items);
        }

        public override List<SceneObjectGroup> LoadObjects(UUID regionUUID)
        {
            return XStorageModule.LoadObjects(this, regionUUID);
        }

        public List<SceneObjectGroup> LoadBaseObjects(UUID regionUUID)
        {
            return base.LoadObjects(regionUUID);
        }
    }
}
