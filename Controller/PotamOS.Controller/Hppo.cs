using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Web;
using Xenko.Core.Mathematics;
using log4net;

namespace PotamOS.Controller
{
    /// <summary>
    /// Main identifier of locations.
    /// hppo://myserver.com/MyScene[/MyChildScene][?x=x&y=y&z=z]
    /// - Root Scenes in hppo are Regions in OpenSim
    /// - Child Scenes are Parcels in OpenSim
    /// </summary>
    public class HppoInfo
    {
        private static readonly ILog m_log = LogManager.GetLogger(
                                     MethodBase.GetCurrentMethod().DeclaringType);

        public static readonly string Scheme = "hppo";

        public Uri Gatekeeper { get; }
        public string PathToScene { get; }
        public string PositionStr { get; } = string.Empty;

        public string Region { get; }
        public string Parcel { get; } = string.Empty;
        public Vector3 Position { get; }

        public HppoInfo(String uriStr)
        {
            Uri hppo = new Uri(uriStr);
            Gatekeeper = new Uri("http://" + hppo.Authority);
            PathToScene = hppo.AbsolutePath;

            string[] parts = PathToScene.Split('/');
            Region = parts[1];
            if (parts.Length > 2)
                Parcel = parts[2];

            float x = 128, y = 128, z = 25;
            if (!string.IsNullOrEmpty(hppo.Query))
            {
                NameValueCollection coords = HttpUtility.ParseQueryString(hppo.Query);
                float.TryParse(coords["x"], out x); float.TryParse(coords["y"], out y); float.TryParse(coords["z"], out z);
            }
            Position = new Vector3(x, y, z);
            PositionStr = string.Format("x={0}&y={1}&z={2}", x, y, z);

            m_log.DebugFormat("[Controller]: Hppo with h={0} r={1} p={2} {3}", Gatekeeper.ToString(), Region, Parcel, PositionStr);
            m_log.InfoFormat("[Controller]: New Hppo {0}", ToString());
        }

        protected HppoInfo(HppoInfo info)
        {
            Gatekeeper = info.Gatekeeper;
            PathToScene = info.PathToScene;
            PositionStr = info.PositionStr;
            Region = info.Region;
            Parcel = info.Parcel;
            Position = info.Position;
        }

        public override String ToString()
        {
            StringBuilder str = new StringBuilder(Gatekeeper.ToString() + "hppo/");
            if (!string.IsNullOrEmpty(Region))
                str.Append(Region);
            if (!string.IsNullOrEmpty(Parcel))
                str.Append("/" + Parcel);
            if (!string.IsNullOrEmpty(PositionStr))
                str.Append("?" + PositionStr);
            return str.ToString();
        }
    }

    public class Hppo : HppoInfo
    {
        private Uri region;
        private Uri assets;
        private Dictionary<string, Uri> services;
        private Guid session;
        private string authToken;

        public Hppo(HppoInfo info) : base(info)
        {
        }
    }

 }
