using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;
using PotamOS.Interfaces;
using log4net;

namespace PotamOS.Controller
{
    public class PotamOSController
    {
        private static readonly ILog m_log = LogManager.GetLogger(
                                             MethodBase.GetCurrentMethod().DeclaringType);

        private static IEngine engine;
        public static IEngine Engine
        {
            get { return engine; }
        }

        private Handshaker handshaker;

        public PotamOSController(IEngine eng)
        {
            engine = eng;
        }

        public bool GoTo(string hppo)
        {
            m_log.DebugFormat("[Controller]: Request to go to {0}", hppo);
            HppoInfo hinfo = new HppoInfo(hppo);
            handshaker = new Handshaker(hinfo);
            bool success = handshaker.Handshake();

            return success;
        }
    }
}
