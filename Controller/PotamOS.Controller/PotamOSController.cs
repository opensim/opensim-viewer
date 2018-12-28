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
        private Hppo m_Hppo; // currently connected simulator

        public PotamOSController(IEngine eng)
        {
            engine = eng;
        }

        public void GoToAsync(string hppo)
        {
            m_log.DebugFormat("[Controller]: Request to go to {0}", hppo);
            HppoInfo hinfo = new HppoInfo(hppo);
            handshaker = new Handshaker(hinfo);
            Task.Run(() => handshaker.Handshake());
        }

        public void SubmitFormAsync(Dictionary<string, string> data)
        {
            Task.Run(() => handshaker.SubmitForm(data));

        }
    }
}
