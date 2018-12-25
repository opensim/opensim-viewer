using System;
using System.Collections;
using System.Collections.Generic;
using System.Net;
using System.Reflection;
using System.Xml.Linq;
using Nwc.XmlRpc;
using log4net;

namespace PotamOS.Controller
{
    public class XmlRpcRequests
    {
        private static readonly ILog m_log = LogManager.GetLogger(
                                             MethodBase.GetCurrentMethod().DeclaringType);

        public static Hashtable XmlRpcCommand(string url, string methodName, params object[] args)
        {
            return SendXmlRpcCommand(url, methodName, args);
        }

        public static Hashtable SendXmlRpcCommand(string url, string methodName, object[] args)
        {
            string responseStr = null;

            try
            {
                XmlRpcRequest client = new XmlRpcRequest(methodName, args);
                XmlRpcResponse Resp = client.Send(url, 6000);

                try
                {
                    responseStr = Resp.ToString();
                    responseStr = XElement.Parse(responseStr).ToString(SaveOptions.DisableFormatting);

                }
                catch (Exception e)
                {
                    m_log.Error("Error parsing XML-RPC response", e);
                }

                if (Resp.IsFault)
                {
                    m_log.DebugFormat(
                        "[LOGHTTP]: XML-RPC request '{0}' to {1} FAILED: FaultCode={2}, FaultMessage={3}",
                         methodName, url, Resp.FaultCode, Resp.FaultString);
                    return null;
                }

                Hashtable respData = (Hashtable)Resp.Value;
                return respData;
            }
            catch (Exception e)
            {
                m_log.DebugFormat("[LOGHTTP]: XML-RPC request '{0}' to {1} FAILED {2}", methodName, url, e);
            }

            return new Hashtable();
        }

    }
}
