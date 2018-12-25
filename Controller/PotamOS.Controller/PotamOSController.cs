using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using PotamOS.Interfaces;

namespace PotamOS.Controller
{
    public class PotamOSController
    {
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
            HppoInfo hinfo = new HppoInfo(hppo);
            handshaker = new Handshaker(hinfo);

            return true;
        }
    }
}
