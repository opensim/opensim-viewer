using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;
using PotamOS.Controller.Network;
using log4net;

namespace PotamOS.Controller
{
    public class Handshaker
    {
        private static readonly ILog m_log = LogManager.GetLogger(
                                             MethodBase.GetCurrentMethod().DeclaringType);

        private string url;

        public Handshaker(HppoInfo hinfo)
        {
            url = hinfo.Gatekeeper.ToString() + "/hppo";
        }

        public bool Handshake()
        {
            string response = string.Empty;
            try
            {
                response = HttpRequests.Get(url);
            }
            catch (Exception e)
            {
                m_log.WarnFormat("[Controller]: Hanshake to {0} failed with {1}", url, e);
                return false;
            }

            m_log.DebugFormat("[Controller]: Hanshake to {0} returned {1}", url, response);
            return true;
        }
    }
}
