using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Threading.Tasks;
using System.Reflection;
using Newtonsoft.Json;
using log4net;

namespace PotamOS.Controller.Network
{
    public struct StreamReaderInfo
    {
        public GZipStream Stream;
        public string ContentType;
        public string ContentEncoding;
    }

    public class HttpRequests
    {
        private static readonly ILog m_log = LogManager.GetLogger(
                                             MethodBase.GetCurrentMethod().DeclaringType);

        /// <summary>
        /// Gets a resource given its url. Returns null upon exception.
        /// Caller check for null!
        /// </summary>
        /// <param name="url"></param>
        /// <returns>Typically, html or xml</returns>
        public static string Get(string url, bool auto_decompress)
        {
            try
            {
                HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);
                if (auto_decompress)
                    request.AutomaticDecompression = DecompressionMethods.GZip | DecompressionMethods.Deflate;

                using (HttpWebResponse response = (HttpWebResponse)request.GetResponse())
                using (Stream stream = response.GetResponseStream())
                using (StreamReader reader = new StreamReader(stream))
                {
                    return reader.ReadToEnd();
                }
            }
            catch (Exception e)
            {
                m_log.WarnFormat("[Controller]: Get request to {0} failed. {1}", url, e);
            }

            return null;
        }

        /// <summary>
        /// Gets a stream reader for a resource given its url. Returns null stream upon exception.
        /// Caller check for null stream!
        /// </summary>
        /// <param name="url"></param>
        /// <returns>Typically, xml compressed</returns>
        public static void GetStream(string url, Action<Stream> loadFunction)
        {
            m_log.DebugFormat("[Controller]: GetStream request to {0}", url);
            HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);
            //request.AutomaticDecompression = DecompressionMethods.GZip | DecompressionMethods.Deflate;

            using (HttpWebResponse response = (HttpWebResponse)request.GetResponse())
            {
                if (response.StatusCode != HttpStatusCode.OK)
                {
                    m_log.WarnFormat("[Controller]: GetStream request to {0} failed with status code {1}", url, response.StatusCode);
                    return;
                }
                m_log.DebugFormat("[Controller]: Received response {0} {1} {2}", response.ContentEncoding, response.ContentType, response.ContentLength);
                if (response.ContentEncoding == "gzip")
                    using (GZipStream gzip = new GZipStream(response.GetResponseStream(), CompressionMode.Decompress))
                        loadFunction(gzip);
                else
                    loadFunction(response.GetResponseStream());
                    
            }
        }

        public async static void Post(string url, Dictionary<string, string> values, Action<string> action)
        {
            HttpClient client = new HttpClient();
            var content = new FormUrlEncodedContent(values);
            
            var response = await client.PostAsync(url, content);
            var responseString = await response.Content.ReadAsStringAsync();

            if (response.StatusCode == HttpStatusCode.BadRequest)
            {
                m_log.WarnFormat("[Controller]: Response from post was BadRequest");
                return;
            }

            action(responseString);
        }

        /// <summary>
        /// Gets a resource given its url. Returns null upon exception.
        /// Caller check for null!
        /// </summary>
        /// <param name="url"></param>
        /// <returns>A simple dictionary</returns>
        public static Dictionary<string, string> GetJson(string url)
        {
            try
            {
                HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);
                request.Accept = "application/json";
                request.AutomaticDecompression = DecompressionMethods.GZip | DecompressionMethods.Deflate;

                using (HttpWebResponse response = (HttpWebResponse)request.GetResponse())
                using (Stream stream = response.GetResponseStream())
                using (StreamReader reader = new StreamReader(stream))
                {
                    string resp = reader.ReadToEnd();
                    return JsonConvert.DeserializeObject<Dictionary<string, string>>(resp);
                }
            }
            catch (Exception e)
            {
                m_log.WarnFormat("[Controller]: GetJson request to {0} failed. {1}", url, e);
            }

            return null;
        }

    }
}
