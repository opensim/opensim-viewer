using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Http;
using System.Web;
using System.Reflection;

using PotamOS.Controller.Network;
using PotamOS.Interfaces;
using log4net;

using PotamOS.Controller.Scene;

namespace PotamOS.Controller
{
    public class Handshaker
    {
        private static readonly ILog m_log = LogManager.GetLogger(
                                             MethodBase.GetCurrentMethod().DeclaringType);
        HppoInfo m_HppoInfo;
        private string url;

        public Handshaker(HppoInfo hinfo)
        {
            m_HppoInfo = hinfo;
            url = hinfo.ToString();
        }

        public void Handshake()
        {
            string html = HttpRequests.Get(url, true);
            if (html != null)
                PotamOSController.Engine.NewPage(UIPages.Handshake, html);
        }

        public void SubmitForm(Dictionary<string, string> data)
        {
            try
            {
                HttpRequests.Post(url, data, ResponseFromSubmitForm);
            }
            catch (Exception e)
            {
                m_log.WarnFormat("[Controller]: Problem posting {0}", e);
            }

        }

        private void ResponseFromSubmitForm(string xregion)
        {
            m_log.DebugFormat("[Controller]: ResponseFromSubmitForm {0}", xregion);

            try
            {
                // Check the xregion for info about the sim URL
                // string.Format("{0}={1}", region, HttpUtility.UrlEncode(gregion.ServerURI)

                string[] parts = xregion.Split('=');
                if (parts.Length <= 1)
                    return;

                string name = parts[0];
                string simUrl = HttpUtility.UrlDecode(parts[1]);
                if (!simUrl.EndsWith("/"))
                    simUrl += "/";
                m_log.InfoFormat("[Controller]: Simulator url is {0}, region name is {1}", simUrl, name);

                // Tell the engine we're a-go
                PotamOSController.Engine.NewPage(UIPages.DynamicScene, string.Empty);

                // Finally get the scene
                string url = simUrl + "hppo/scene" + (xregion == "DEFAULT" ? "" : "/" + name);
                try
                {
                    HttpRequests.GetStream(url, SceneManager.Instance.LoadSceneFromXml);
                    m_log.DebugFormat("[Controller]: YIPPIE! Got the scene!");
                }
                catch (Exception e)
                {
                    m_log.WarnFormat("[Controller]: Problem with GetStream request to {0}, {1}", url, e);
                    PotamOSController.Engine.NewPage(UIPages.Splash, string.Empty);
                }
                //Hppo hppo = new Hppo(m_HppoInfo);
                //  ...
                //return hppo;
            }
            catch (Exception e)
            {
                m_log.WarnFormat("[Controller]: Problem with response from post form {0}", e);
                PotamOSController.Engine.NewPage(UIPages.Splash, "");

            }
        }
    }
}
