/*
 * Copyright (c) Contributors, http://opensimulator.org/
 * See CONTRIBUTORS.TXT for a full list of copyright holders.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the OpenSimulator Project nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE DEVELOPERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using System.Xml;

using log4net;

namespace OpenSim.PotamOS
{
    public class PotamOSUtils
    {
        private static readonly ILog m_log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);

        public static readonly char[] DirectorySeparatorChars = { System.IO.Path.DirectorySeparatorChar, System.IO.Path.AltDirectorySeparatorChar };
        public static readonly string DocsPath = Path.Combine("..", "potapps");

        public static string GetContentType(string resource)
        {
            resource = resource.ToLower();
            if (resource.EndsWith(".jpg"))
                return "image/jpeg";
            if (resource.EndsWith(".gif"))
                return "image/gif";
            if (resource.EndsWith(".png"))
                return "image/png";
            if (resource.EndsWith(".ico"))
                return "image/x-icon";
            if (resource.EndsWith(".css"))
                return "text/css";
            if (resource.EndsWith(".txt"))
                return "text/plain";
            if (resource.EndsWith(".xml"))
                return "text/xml";
            if (resource.EndsWith(".js"))
                return "application/javascript";
            if (resource.EndsWith(".exe"))
                return "application/exe";
            return "text/html";
        }

        public static byte[] ReadBinaryResource(string resourceName)
        {
            try
            {
                using (BinaryReader sr = new BinaryReader(File.Open(resourceName, FileMode.Open)))
                {
                    byte[] buffer = new byte[32768];
                    using (MemoryStream ms = new MemoryStream())
                    {
                        while (true)
                        {
                            int read = sr.Read(buffer, 0, buffer.Length);
                            if (read <= 0)
                                return ms.ToArray();
                            ms.Write(buffer, 0, read);
                        }
                    }
                }
            }
            catch (Exception e)
            {
                m_log.WarnFormat("[PotamOS]: couldn't read {0} - {1}", resourceName, e); 
            }

            // Let the user know what went wrong.
            m_log.DebugFormat("[PotamOS]: BinaryResource {0} not found", Path.GetFileName(resourceName));
            return new byte[0];
        }

        public static string ReadTextResource(string resourceName, string missingpage)
        {
            return ReadTextResource(resourceName, missingpage, false);
        }

        public static string ReadTextResource(string resourceName, string missingpage, bool keepEndOfLines)
        {
            StringBuilder buffer = new StringBuilder();
            bool found = false;
            try
            {
                using (StreamReader sr = new StreamReader(resourceName))
                {
                    if (keepEndOfLines)
                    {
                        buffer.Append(sr.ReadToEnd());
                    }
                    else
                    {
                        String line;
                        while ((line = sr.ReadLine()) != null)
                        {
                            buffer.Append(line);
                        }
                    }
                    found = true;
                }
            }
            catch (Exception e)
            {
                m_log.WarnFormat("[PotamOS]: couldn't read {0} - {1}", resourceName, e);
            }

            if (!found)
            {
                // Let the user know what went wrong.
                m_log.DebugFormat("[PotamOS]: TextResource {0} not found", Path.GetFileName(resourceName));
                if (missingpage != string.Empty)
                    return ReadTextResource(missingpage, "");
                else
                    return string.Empty;
            }
            return buffer.ToString();
        }

        public static byte[] StringToBytes(string str)
        {
            return Encoding.UTF8.GetBytes(str);
        }

        public static string GetPath(string resource)
        {
            return System.IO.Path.Combine(DocsPath, resource);
        }

        public string ReadFile(string resource)
        {
            string file = GetPath(resource);
            try
            {
                string content = string.Empty;
                using (StreamReader sr = new StreamReader(file))
                {
                    content = sr.ReadToEnd();
                }
                return content;
            }
            catch (Exception e)
            {
                m_log.DebugFormat("[Wifi]: Exception on ReadFile {0}: {1}", resource, e);
                return ReadTextResource("", "");
            }
        }

    }
}
