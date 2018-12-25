using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Reflection;
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
            Region = parts[0];
            if (parts.Length > 1)
                Parcel = parts[1];

            float x = 128, y = 128, z = 25;
            if (!string.IsNullOrEmpty(hppo.Query))
            {
                NameValueCollection coords = HttpUtility.ParseQueryString(hppo.Query);
                float.TryParse(coords["x"], out x); float.TryParse(coords["y"], out y); float.TryParse(coords["z"], out z);
            }
            Position = new Vector3(x, y, z);
            PositionStr = string.Format("x={0}&y={1}&z={2}", x, y, z);
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
            return Scheme + "://" + Gatekeeper + "/" + Region + "/" + Parcel + "?" + PositionStr;
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
