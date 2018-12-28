using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Text;
using System.Net;
using System.Web;

using log4net;

using OpenSim.Framework.Servers.HttpServer;
using OpenSim.Server.Base;
using OpenSim.Services.GridService;
using GridRegion = OpenSim.Services.Interfaces.GridRegion;
using OpenMetaverse;

using OpenSim.PotamOS;

namespace OpenSim.PotamOS.Handlers
{
    public class HppoDefaultHandler : BaseStreamHandler
    {
        private static readonly ILog m_log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);

        private static readonly string index_html_1 = "<html>\n <body>  <form method=\"POST\">\n Your Name: <input name=\"NAME\" type = \"text\" /><br>";
        private static readonly string index_html_2 = "<input name=\"METHOD\" type=\"hidden\" value=\"ENTER\" /><input type = \"submit\" value = \"ENTER\" ></form ></body ></html >\n";

        public HppoDefaultHandler() : base("GET", "/hppo")
        {
            m_log.Debug("[PotamOS]: HppoDefaultHandler");
        }

        public override byte[] Handle(string path, Stream requestData,
                                      IOSHttpRequest httpRequest, IOSHttpResponse httpResponse)
        {
            m_log.DebugFormat("[PotamOS]: HppoDefaultHandler Handle {0}", path);
            // path = /hppo
            //m_log.DebugFormat("[Wifi]: path = {0}", path);
            //m_log.DebugFormat("[Wifi]: ip address = {0}", httpRequest.RemoteIPEndPoint);
            //foreach (object o in httpRequest.Query.Keys)
            //    m_log.DebugFormat("  >> {0}={1}", o, httpRequest.Query[o]);

            string resource = GetParam(path);
            resource = Uri.UnescapeDataString(resource).Trim(PotamOSUtils.DirectorySeparatorChars);
            //m_log.DebugFormat("[Wifi]: resource {0}", resource);

            Request request = CreateRequest(resource, httpRequest);

            string resourcePath = PotamOSUtils.GetPath(resource);

            string type = PotamOSUtils.GetContentType(resource);
            httpResponse.ContentType = type;

            string region = "DEFAULT";
            if (resource != string.Empty)
            {
                string[] parts = Path.Split('/');
                region = parts[0];
            }
            httpResponse.StatusCode = (int)OSHttpStatusCode.SuccessOk;
            httpResponse.ContentType = "text/html"; 
            region = string.Format("Region: <input name=\"REGION\" type=\"text\"value=\"{0}\" /><br>", region);
            return PotamOSUtils.StringToBytes(index_html_1 + region + index_html_2);

        }

        public static Request CreateRequest(string resource, IOSHttpRequest httpRequest)
        {
            Request request = new Request();
            request.Resource = resource;
            request.Cookies = httpRequest.Cookies;
            request.IPEndPoint = httpRequest.RemoteIPEndPoint;
            request.Query = httpRequest.Query;

            return request;
        }

    }

    public class HppoDefaultPostHandler : BaseStreamHandler
    {
        private static readonly ILog m_log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);
        private static uint counter = 0;

        private GridService m_GridService;

        public HppoDefaultPostHandler(GridService gs) :
                base("POST", "/hppo")
        {
            m_GridService = gs;
            m_log.Debug("[PotamOS]: HppoDefaultPostHandler");
        }

        public override byte[] Handle(string path, Stream requestData,
                IOSHttpRequest httpRequest, IOSHttpResponse httpResponse)
        {
            m_log.DebugFormat("[PotamOS]: HppoDefaultPostHandler Handle {0}", path);

            StreamReader sr = new StreamReader(requestData);
            string body = sr.ReadToEnd();
            sr.Close();
            body = body.Trim();

            httpResponse.ContentType = "text/html";

            string resource = GetParam(path);

            string method = string.Empty;
            string region = string.Empty;
            string name = "Anonymous-" + counter++;
            try
            {
                Dictionary<string, object> request =
                        ServerUtils.ParseQueryString(body);

                if (!request.ContainsKey("METHOD") || !request.ContainsKey("REGION"))
                {
                    httpResponse.StatusCode = (int)OSHttpStatusCode.ClientErrorBadRequest;
                    return PotamOSUtils.StringToBytes("");
                }

                method = request["METHOD"].ToString();
                region = request["REGION"].ToString();
                if (request.ContainsKey("NAME"))
                    name = request.ContainsKey("NAME").ToString();

                if (method == "ENTER")
                    return EnterAgent(region, name, httpRequest, request, httpResponse);

                m_log.DebugFormat("[POST HANDLER]: unknown method request: {0}", method);
            }
            catch (Exception e)
            {
                m_log.DebugFormat("[POST HANDLER]: Exception in method {0}: {1}", method, e);
            }

            httpResponse.StatusCode = (int)OSHttpStatusCode.ClientErrorBadRequest;
            return PotamOSUtils.StringToBytes("");

        }

        byte[] EnterAgent(string region, string name, IOSHttpRequest httpRequest, Dictionary<string, object> request, IOSHttpResponse httpResponse)
        {
            // Let's check if the region exists
            GridRegion gregion = null;
            if (string.IsNullOrEmpty(region) || region == "DEFAULT")
            {
                List<GridRegion> regions = m_GridService.GetDefaultRegions(UUID.Zero);
                if (regions.Count > 0)
                    gregion = regions[0];
                else
                    return BadRequest(httpResponse);
            }
            else
            {
                gregion = m_GridService.GetRegionByName(UUID.Zero, region);
                if (gregion == null)
                    return BadRequest(httpResponse);
            }

            // We found the region
            m_log.DebugFormat("[PotamOS]: Found requested region {0} at {1}", region, gregion.ServerURI);
            httpResponse.StatusCode = (int)OSHttpStatusCode.SuccessOk;
            httpResponse.ContentType = "text/plain";
            string simUrl = string.Format("{0}={1}", region, HttpUtility.UrlEncode(gregion.ServerURI));
            return PotamOSUtils.StringToBytes(simUrl);
        }

        private byte[] BadRequest(IOSHttpResponse httpResponse)
        {
            httpResponse.StatusCode = (int)OSHttpStatusCode.ClientErrorBadRequest;
            return Encoding.UTF8.GetBytes("");
        }
    }

    public struct Request
    {
        public string Resource;
        public HttpCookieCollection Cookies;
        public IPEndPoint IPEndPoint;
        public Hashtable Query;
    }


}
